/*
 Copyright 2012-2013 Hot-World GmbH & Co. KG
 Author: Roland Littwin (repetier) repetierdev@gmail.com
 Homepage: http://www.repetier.com
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 
 */

#include "GCodeAnalyser.h"
#include "PrinterState.h"
#include "GCode.h"
#include "Poco/Path.h"
#include "Poco/File.h"
#include <fstream>
#include "Poco/Util/PropertyFileConfiguration.h"
#include "Poco/String.h"
#include "Poco/NumberFormatter.h"
#include "Poco/AutoPtr.h"
#include "WorkDispatcher.h"
#include "PrinterConfiguration.h"

using namespace std;
using namespace boost;
using namespace Poco;
using namespace Poco::Util;
using Poco::cat;

void SimulatorLine::updateStepsParameter()
{
    if(areParameterUpToDate()) return;
    float startFactor = startSpeed * invFullSpeed;
    float endFactor   = endSpeed   * invFullSpeed;
    vStart = vMax*startFactor; //starting speed
    vEnd   = vMax*endFactor;
    float vmax2 = vMax*vMax;
    accelSteps = ((vmax2-vStart*vStart)/(accelerationPrim*2)); // Always add 1 for missing precision
    decelSteps = ((vmax2-vEnd*vEnd)  /(accelerationPrim*2));
    if(accelSteps+decelSteps>=stepsRemaining)   // can't reach limit speed
    {
        float red = (accelSteps+decelSteps+2-stepsRemaining)*0.5f;
        accelSteps = accelSteps-PrinterSimulator::min(accelSteps,red);
        decelSteps = decelSteps-PrinterSimulator::min(decelSteps,red);
    }
    setParameterUpToDate();
}
double SimulatorLine::realMoveTime() {
    // s = 1/2 a t^2 + v*t
    // (sqrt(v^2+2*a*s)-v)/a
    double t1 = (sqrt(vStart*vStart+2*accelerationPrim*accelSteps)-vStart)/accelerationPrim;
    double vm = accelerationPrim*t1+vStart;
    double t2 = (stepsRemaining-accelSteps-decelSteps)/vm;
    double t3 = (sqrt(vEnd*vEnd+2*accelerationPrim*decelSteps)-vEnd)/accelerationPrim;
    return t1+t2+t3;
}

// --------------------------------------------------------

PrinterSimulator::PrinterSimulator(PrinterPtr printer) {
    this->printer = printer;
    updateConfig();
    start();
}

PrinterSimulator::~PrinterSimulator() {
    if(buffer!=NULL) delete buffer;
}

void PrinterSimulator::updateConfig() {
    PrinterConfigurationPtr c = printer->config;
    bufferSize = c->movebuffer;
    xyJerk = c->xyJerk;
    zJerk = c->zJerk;
    eJerk = 40.0;
    printAcceleration[0] = c->printAcceleration;
    printAcceleration[1] = c->printAcceleration;
    printAcceleration[2] = c->printAcceleration;
    printAcceleration[3] = 10000;
    travelAcceleration[0] = c->travelAcceleration;
    travelAcceleration[1] = c->travelAcceleration;
    travelAcceleration[2] = c->travelAcceleration;
    travelAcceleration[3] = 10000;
    timeMultiplier = c->timeMultiplier;
    maxXYFeedrate = c->maxXYSpeed;
    maxZFeedrate = c->maxZSpeed;
    maxEFeedrate = 30;
    driveSystem = Cartesian;
    if(c->extruderList.size()>0) {
        printAcceleration[3] = c->extruderList[0]->acceleration;
        eJerk = c->extruderList[0]->eJerk;
        maxEFeedrate = c->extruderList[0]->eJerk;
    }    
    buffer = NULL;
}

void PrinterSimulator::start() {
    updateConfig();
    if(buffer!=NULL) delete buffer;
    buffer = new SimulatorLine[bufferSize];
    bufferReadPos = 0;
    bufferWritePos = 0;
    bufferUsed = 0;
    time = 0;
    state.reset(new PrinterState(printer,10));
}
void PrinterSimulator::sendCommand(GCode *code) {
    state->analyze(*code);
    if(!state->isG1Move) return;
    float diff[4] = {(float)(state->x-state->x0),(float)(state->y-state->y0),
        (float)(state->z-state->z0),(float)(state->activeExtruder->lastE-state->activeExtruder->e0)};
    queueCartesianMove(diff, true);
}
void PrinterSimulator::finishPrint() {
    waitForXFreeLines(bufferSize);
}
void PrinterSimulator::computeMaxJunctionSpeed(SimulatorLine *previous,SimulatorLine *current) {
    // First we compute the normalized jerk for speed 1
    float dx = current->speedX-previous->speedX;
    float dy = current->speedY-previous->speedY;
    float factor=1;
    float jerk = sqrt(dx*dx+dy*dy);
    if(jerk>xyJerk)
        factor = xyJerk/jerk;
    if((previous->dir & current->dir) & 64)
    {
        float dz = fabs(current->speedZ-previous->speedZ);
        if(dz>zJerk)
            factor = min(factor,zJerk/dz);
    }
    float extruderJerk = fabs(current->speedE-previous->speedE);
    if(extruderJerk>eJerk)
        factor = min(factor,eJerk/extruderJerk);
    previous->maxJunctionSpeed = min(previous->fullSpeed*factor,current->fullSpeed);
}

void PrinterSimulator::waitForXFreeLines(int b) {
    while(bufferSize-bufferUsed<b) {
        time+=buffer[bufferReadPos].realMoveTime()*timeMultiplier;
        bufferReadPos = (++bufferReadPos) % bufferSize;
        bufferUsed--;
    }
}
void PrinterSimulator::forwardPlanner(int first) {
    SimulatorLine *act,*next;
    next = &buffer[first];
    float leftSpeed = next->startSpeed;
    while(first!=bufferWritePos)   // All except last segment, which has fixed end speed
    {
        act = next;
        nextPlannerIndex(first);
        next = &buffer[first];
        float vmaxRight;
        // Avoid speed calcs if we know we can accelerate within the line.
        vmaxRight = sqrt(leftSpeed*leftSpeed+act->accelerationDistance2);
        if(vmaxRight>act->endSpeed)   // Could be higher next run?
        {
            act->startSpeed = leftSpeed;
            leftSpeed       = act->endSpeed;
            if(act->endSpeed==act->maxJunctionSpeed)  // Full speed reached, don't compute again!
            {
                act->setEndSpeedFixed(true);
                next->setStartSpeedFixed(true);
            }
            act->invalidateParameter();
        }
        else     // We can accelerate full speed without reaching limit, which is as fast as possible. Fix it!
        {
            act->fixStartAndEndSpeed();
            act->invalidateParameter();
            act->startSpeed = leftSpeed;
            act->endSpeed = leftSpeed = vmaxRight;
            next->setStartSpeedFixed(true);
        }
    }
    next->startSpeed = leftSpeed; // This is the new segment, wgich is updated anyway, no extra flag needed.
  
}
void PrinterSimulator::backwardPlanner(int start,int last) {
    SimulatorLine *act = &buffer[start],*previous;
    float lastJunctionSpeed = act->endSpeed; // Start always with safe speed
    
    //PREVIOUS_PLANNER_INDEX(last); // Last element is already fixed in start speed
    while(start!=last)
    {
        previousPlannerIndex(start);
        previous = &buffer[start];
        // Avoid speed calc once crusing in split delta move
        
            // If you accelerate from end of move to start what speed do you reach?
        lastJunctionSpeed = sqrt(lastJunctionSpeed*lastJunctionSpeed+act->accelerationDistance2); // acceleration is acceleration*distance*2! What can be reached if we try?
        // If that speed is more that the maximum junction speed allowed then ...
        if(lastJunctionSpeed>=previous->maxJunctionSpeed)   // Limit is reached
        {
            // If the previous line's end speed has not been updated to maximum speed then do it now
            if(previous->endSpeed!=previous->maxJunctionSpeed)
            {
                previous->invalidateParameter(); // Needs recomputation
                previous->endSpeed = previous->maxJunctionSpeed; // possibly unneeded???
            }
            // If actual line start speed has not been updated to maximum speed then do it now
            if(act->startSpeed!=previous->maxJunctionSpeed)
            {
                act->startSpeed = previous->maxJunctionSpeed; // possibly unneeded???
                act->invalidateParameter();
            }
            lastJunctionSpeed = previous->maxJunctionSpeed;
        }
        else
        {
            // Block prev end and act start as calculated speed and recalculate plateau speeds (which could move the speed higher again)
            act->startSpeed = previous->endSpeed = lastJunctionSpeed;
            previous->invalidateParameter();
            act->invalidateParameter();
        }
        act = previous;
    } // while loop
}

void PrinterSimulator::updateTrapezoids() {
    int first = bufferWritePos;
    SimulatorLine *firstLine;
    SimulatorLine *act = &buffer[bufferWritePos];
    int maxfirst = bufferReadPos; // first non fixed segment
    if(maxfirst!=bufferWritePos)
        maxfirst = (maxfirst + 1) % bufferSize; // don't touch the line printing
    // Search last fixed element
    while(first!=maxfirst && !buffer[first].isEndSpeedFixed())
        previousPlannerIndex(first);
    if(first!=bufferWritePos && buffer[first].isEndSpeedFixed())
        nextPlannerIndex(first);
    if(first==bufferWritePos)   // Nothing to plan
    {
        act->setStartSpeedFixed(true);
        act->updateStepsParameter();
        return;
    }
    // now we have at least one additional move for optimization
    // that is not a wait move
    // First is now the new element or the first element with non fixed end speed.
    // anyhow, the start speed of first is fixed
    firstLine = &buffer[first];
    int previousIndex = bufferWritePos;
    previousIndex = (previousIndex == 0 ? bufferSize-1 : previousIndex-1);
    SimulatorLine *previous = &buffer[previousIndex];
    if(driveSystem!=Delta) {
    if((previous->primaryAxis == Z_AXIS && act->primaryAxis != Z_AXIS) || (previous->primaryAxis != Z_AXIS && act->primaryAxis == Z_AXIS))
    {
        previous->setEndSpeedFixed(true);
        act->setStartSpeedFixed(true);
        act->updateStepsParameter();
        return;
    }
    }
    
    computeMaxJunctionSpeed(previous,act); // Set maximum junction speed if we have a real move before
    if(previous->isEOnlyMove() != act->isEOnlyMove())
    {
        previous->setEndSpeedFixed(true);
        act->setStartSpeedFixed(true);
        act->updateStepsParameter();
        return;
    }
    backwardPlanner(bufferWritePos,first);
    // Reduce speed to reachable speeds
    forwardPlanner(first);
    
    // Update precomputed data
    do
    {
        buffer[first].updateStepsParameter();
        first = (first + 1) % bufferSize;
    }
    while(first!=bufferWritePos);
    act->updateStepsParameter();
}

void PrinterSimulator::queueCartesianMove(float axis_diff[4],uint8_t pathOptimize)
{
    waitForXFreeLines(1);
    SimulatorLine *p = &buffer[bufferWritePos];
    
    p->flags = 0;
    p->joinFlags = 0;
    if(!pathOptimize) p->setEndSpeedFixed(true);
    p->dir = 0;
    //Find direction
    for(int axis=0; axis < 4; axis++)
    {
        if(axis_diff[axis]<0)
            p->setPositiveDirectionForAxis(axis);
        p->delta[axis] = fabs(axis_diff[axis]);
        if(p->delta[axis]!=0) p->setMoveOfAxis(axis);
    }
    if(p->isNoMove())
    {
        return; // No steps included
    }
    float xydist2;
    
    //Define variables that are needed for the Bresenham algorithm. Please note that  Z is not currently included in the Bresenham algorithm.
    if(p->delta[1] > p->delta[0] && p->delta[1] > p->delta[2] && p->delta[1] > p->delta[3]) p->primaryAxis = 1;
    else if (p->delta[0] > p->delta[2] && p->delta[0] > p->delta[3]) p->primaryAxis = 0;
    else if (p->delta[2] > p->delta[3]) p->primaryAxis = 2;
    else p->primaryAxis = 3;
    p->stepsRemaining = p->delta[p->primaryAxis];
    if(p->isXYZMove())
    {
        xydist2 = axis_diff[0] * axis_diff[0] + axis_diff[1] * axis_diff[1];
        if(p->isZMove())
            p->distance = sqrt(xydist2 + axis_diff[2] * axis_diff[2]);
        else
            p->distance = sqrt(xydist2);
    }
    else
        p->distance = fabs(axis_diff[3]);
    calculateMove(p,axis_diff,pathOptimize);
}
void PrinterSimulator::calculateMove(SimulatorLine *p,float axis_diff[],uint8_t pathOptimize)
{
    float axis_interval[4];
    float time_for_move = p->distance / (state->f/60.0); // time is in ticks
    p->timeInTicks = time_for_move;
               // Compute the solwest allowed interval (ticks/step), so maximum feedrate is not violated
    float limitInterval = time_for_move/p->stepsRemaining; // until not violated by other constraints it is your target speed
    axis_interval[X_AXIS] = fabs(axis_diff[X_AXIS])/(maxXYFeedrate*p->stepsRemaining); // mm/(mm/s*mm) = s/mm
    limitInterval = max(axis_interval[X_AXIS],limitInterval);
    axis_interval[Y_AXIS] = fabs(axis_diff[Y_AXIS])/(maxXYFeedrate*p->stepsRemaining);
    limitInterval = max(axis_interval[Y_AXIS],limitInterval);
    if(p->isZMove())   // normally no move in z direction
    {
        axis_interval[Z_AXIS] = fabs((float)axis_diff[Z_AXIS])/(float)(maxZFeedrate*p->stepsRemaining); // must prevent overflow!
        limitInterval = max(axis_interval[Z_AXIS],limitInterval);
    }
    else axis_interval[Z_AXIS] = 0;
    axis_interval[E_AXIS] = fabs(axis_diff[E_AXIS])/(maxEFeedrate*p->stepsRemaining);
    limitInterval = max(axis_interval[E_AXIS],limitInterval);
    p->fullInterval = limitInterval; // commanded or max. allowed interval = 1/v
    time_for_move = (float)limitInterval*(float)p->stepsRemaining; // for large z-distance this overflows with long computation
    float inv_time_s = 1.0f/time_for_move;
    if(p->isXMove())
    {
        axis_interval[X_AXIS] = time_for_move/p->delta[X_AXIS];
        p->speedX = fabs(axis_diff[X_AXIS]*inv_time_s);
    }
    else p->speedX = 0;
    if(p->isYMove())
    {
        axis_interval[Y_AXIS] = time_for_move/p->delta[Y_AXIS];
        p->speedY = fabs(axis_diff[Y_AXIS]*inv_time_s);
    }
    else p->speedY = 0;
    if(p->isZMove())
    {
        axis_interval[Z_AXIS] = time_for_move/p->delta[Z_AXIS];
        p->speedZ = fabs(axis_diff[Z_AXIS]*inv_time_s);
    }
    else p->speedZ = 0;
    if(p->isEMove())
    {
        axis_interval[E_AXIS] = time_for_move/p->delta[E_AXIS];
        p->speedE = fabs(axis_diff[E_AXIS]*inv_time_s);
    }
    p->fullSpeed = p->distance*inv_time_s; // mm/s in 3d direction
    //long interval = axis_interval[primary_axis]; // time for every step in ticks with full speed
    uint8_t is_print_move = p->isEPositiveMove(); // are we printing
                                            //If acceleration is enabled, do some Bresenham calculations depending on which axis will lead it.
    
    // slowest time to accelerate from v0 to limitInterval determines used acceleration
    // t = (v_end-v_start)/a
    float slowest_axis_plateau_time_repro = 1e20; // repro to reduce division Unit: 1/s
    for(int i=0; i < 4 ; i++)
    {
        // Errors for delta move are initialized in timer
        if(p->isMoveOfAxis(i))
        {
            // v = a * t => t = v/a = F_CPU/(c*a) => 1/t = c*a/F_CPU
            slowest_axis_plateau_time_repro = min(slowest_axis_plateau_time_repro,
                                                         (float)axis_interval[i] * (float)(is_print_move ?  printAcceleration[i] : travelAcceleration[i])); //  steps/s^2 * step/tick  Ticks/s^2
        }
    }
    p->invFullSpeed = 1.0/p->fullSpeed;
    p->accelerationPrim = slowest_axis_plateau_time_repro / axis_interval[p->primaryAxis]; // a = v/t = F_CPU/(c*t): Steps/s^2
                                                                                     //Now we can calculate the new primary axis acceleration, so that the slowest axis max acceleration is not violated
    p->accelerationDistance2 = 2.0*p->distance*slowest_axis_plateau_time_repro*p->fullSpeed; // mm^2/s^2
    p->startSpeed = p->endSpeed = safeSpeed(p);
    
    p->vMax = 1.0f / p->fullInterval; // maximum steps per second, we can reach
                                 // if(p->vMax>46000)  // gets overflow in N computation
                                 //   p->vMax = 46000;
                                 //p->plateauN = (p->vMax*p->vMax/p->accelerationPrim)>>1;
    updateTrapezoids();
    // how much steps on primary axis do we need to reach target feedrate
    //p->plateauSteps = (long) (((float)p->acceleration *0.5f / slowest_axis_plateau_time_repro + p->vMin) *1.01f/slowest_axis_plateau_time_repro);
    bufferWritePos = (bufferWritePos + 1) % bufferSize;
    bufferUsed++;
}
float PrinterSimulator::safeSpeed(SimulatorLine *p)
{
    float safe;
        safe = xyJerk*0.5;
    if(driveSystem!=Delta) {
    if(p->isZMove())
    {
        if(p->primaryAxis==2)
            safe = zJerk*0.5*p->fullSpeed/fabs(p->speedZ);
        else if(fabs(p->speedZ)>zJerk*0.5)
            safe = min(safe,zJerk*0.5*p->fullSpeed/fabs(p->speedZ));
    }
    }
    if(p->isEMove())
    {
        if(p->isXYZMove())
            safe = min(safe,0.5*eJerk*p->fullSpeed/fabs(p->speedE));
        else
            safe = 0.5*eJerk; // This is a retraction move
    }
    return min(safe,p->fullSpeed);
}
// --------------------------------------------------------

void GCodeAnalyser::analyseFile(PrinterPtr printer,std::string file) {
    std::ifstream data(file.c_str());
    string line;
    lines = 0;
    shared_ptr<PrinterSimulator> simulator(new PrinterSimulator(printer));
    simulator->start();
    while(std::getline(data,line)) {
        lines++;
        GCode gc(printer,line);
        simulator->sendCommand(&gc);
    }
    simulator->finishPrint();
    printingTime = simulator->time;
    for(int i=0;i<10;i++)
        filamentUsed.push_back(simulator->state->getExtruder(i).eMax);
    File f(file);
    fileSize = (int)f.getSize();
    layer = simulator->state->layer;
    safeData();
}
GCodeAnalyser::GCodeAnalyser(std::string path) {
    setSourceFile(path);
    Poco::AutoPtr<PropertyFileConfiguration> config(new PropertyFileConfiguration(this->path));
    lines = config->getInt("lines",0);
    printingTime = config->getDouble("printingTime",0);
    totalFilamentUsed = config->getDouble("totalFilament",0);
    for(int i=0;i<10;i++) {
        filamentUsed.push_back(config->getDouble(Poco::cat(string("extruder"),NumberFormatter::format(i)),0));
    }
    fileSize = config->getInt("fileSize", 0);
    printed = config->getInt("printed",0);
    layer = config->getInt("layer",0);
}
GCodeAnalyser::GCodeAnalyser(PrinterPtr printer,std::string path,bool forceRecompute) {
    setSourceFile(path);
    printed = 0;
    File f(this->path);
    if(!f.exists()) {
        if(!forceRecompute) {
            WorkDispatcherData wd("gcodeInfo","",0);
            wd.addParameter(printer->config->slug);
            wd.addParameter(path);
            WorkDispatcher::addJob(wd);
        }
    } else {
    Poco::AutoPtr<PropertyFileConfiguration> config(new PropertyFileConfiguration(this->path));
    lines = config->getInt("lines",0);
    printingTime = config->getDouble("printingTime");
    totalFilamentUsed = config->getDouble("totalFilament");
    for(int i=0;i<10;i++) {
        filamentUsed.push_back(config->getDouble(Poco::cat(string("extruder"),NumberFormatter::format(i)),0));
    }
    fileSize = config->getInt("fileSize", 0);
    printed = config->getInt("printed",0);
    layer = config->getInt("layer",0);
    }
    if(forceRecompute)
        analyseFile(printer, path);
}

void GCodeAnalyser::removeData() {
    File f(this->path);
    if(f.exists() && f.isFile())
        f.remove();
}
void GCodeAnalyser::setSourceFile(std::string source) {
    Path p(source);
    p.setExtension("gin");
    path = p.toString();
}
void GCodeAnalyser::safeData() {
    Poco::AutoPtr<PropertyFileConfiguration> config(new PropertyFileConfiguration());
    config->setInt("lines",lines);
    config->setDouble("printingTime", printingTime);
    int i = 0;
    vector<double>::iterator it = filamentUsed.begin(),ie = filamentUsed.end();
    totalFilamentUsed = 0;
    for(;it!=ie;it++,i++) {
        totalFilamentUsed += *it;
        string exname = Poco::cat(string("extruder"),NumberFormatter::format(i));
        config->setDouble(exname, *it);
    }
    config->setDouble("totalFilament", totalFilamentUsed);
    config->setInt("fileSize",fileSize);
    config->setInt("printed",printed);
    config->setInt("layer",layer);
    config->save(path);
}
