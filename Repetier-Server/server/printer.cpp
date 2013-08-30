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

#define _CRT_SECURE_NO_WARNINGS // Disable deprecation warning in VS2005
#define _CRT_SECURE_NO_DEPRECATE 
#define _SCL_SECURE_NO_DEPRECATE

#include "printer.h"
#include "Printjob.h"
#include "GCode.h"
#include "PrinterSerial.h"
#include "PrinterState.h"
#include "global_config.h"
#include <boost/filesystem.hpp>
#include "json_spirit.h"
#include "RLog.h"
#include "ServerEvents.h"
#include "PrinterConfiguration.h"
#include "utils/FileUtils.h"

using namespace std;
using namespace boost::filesystem;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost;

PrinterResponse::PrinterResponse(const string& mes,uint32_t id,uint8_t tp):message(mes) {
    responseId = id;
    logtype = tp;
    time = boost::posix_time::microsec_clock::local_time();
}

std::string PrinterResponse::getTimeString() {
    tm tm = boost::posix_time::to_tm(time);
    char buf[40];
    sprintf(buf,"%2d:%02d:%02d",tm.tm_hour,tm.tm_min,tm.tm_sec);
    return string(buf);
}
Printer::Printer(string conf):config(new PrinterConfiguration(conf)) {
    stopRequested = false;
    lastResponseId = 0;
    state = NULL;
    serial = new PrinterSerial(*this);
    resendError = 0;
    errorsReceived = 0;
    bytesReceived = 0;
    linesSend = 0;
    bytesSend = 0;
    paused = false;
}
Printer::~Printer() {
    serial->close();
    rotateLogTo("");
    if(state!=NULL)
        delete state;
    delete serial;
    delete modelManager;
    delete jobManager;
    // delete scriptManager;
}
void Printer::Init(PrinterPtr ptr) {
    thisPtr = ptr;
    state = new PrinterState(ptr); // done at creation
    state->setReal();
    jobManager = new PrintjobManager(gconfig->getStorageDirectory()+"printer/"+config->slug+"/jobs",ptr);
    modelManager = new PrintjobManager(gconfig->getStorageDirectory()+"printer/"+config->slug+"/models",ptr);
    //scriptManager = new PrintjobManager(gconfig->getStorageDirectory()+"printer/"+config->slug+"/scripts",ptr,true);
    logDirectory = gconfig->getStorageDirectory()+"printer/"+config->slug+"/logs";
    if(!exists(logDirectory)) { // First call - create directory
        if(!create_directories(logDirectory)) {
            cerr << "error: Unable to create log directory " << logDirectory << "." << endl;
            exit(-1);
        }
    }
    logStream = NULL;
    rotateLogTo("connected");
}

void Printer::rotateLogTo(std::string name) {
    mutex::scoped_lock l(logMutex);
    if(logStream!=NULL) {
        logStream->close();
        delete logStream;
    }
    if(name.size()==0) {
        logStream = NULL;
        return;
    }
    RepetierServer::keepNewestFilesInDirectory(logDirectory, 5);
    string filename = logDirectory+"/"+name+".log";
    logStream = new ofstream();
    logStream->open(filename.c_str());
}
void Printer::logLine(std::string line) {
    mutex::scoped_lock l(logMutex);
    if(logStream != NULL)
        *logStream << line << endl;
}

void Printer::startThread() {
    assert(!thread);
    thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&Printer::run, this)));
    
}
void Printer::updateLastTempMutex() {
    mutex::scoped_lock l(lastTempMutex);
    lastTemp = boost::posix_time::microsec_clock::local_time();
}

void Printer::run() {
    while (!stopRequested)
    {
        try {
            boost::this_thread::sleep(boost::posix_time::milliseconds(10));
            if(!config->active) {
                if(serial->isConnected())
                    serial->close();
                boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
                continue; // skip normal usage
            }
            if(!serial->isConnected()) {
                boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
                cacheSize = config->serialInputBufferSize;
                serial->tryConnect();
            } else {
                {
                    time_duration td;
                    {
                        mutex::scoped_lock l(lastTempMutex);
                        posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
                        td = now-lastTemp;
                    } // Must close mutex to prevent deadlock!
                    if(manualCommands.size()<5 && config->tempUpdateEvery>0 && td.seconds()>=config->tempUpdateEvery) {
                        injectManualCommand("M105");
                        lastTemp = microsec_clock::local_time();
                    }
                }
                jobManager->manageJobs(); // refill job queue
            }
            trySendNextLine();
        }
        catch( boost::thread_interrupted) {
            stopRequested = true;
        }
    }
}
void Printer::stopThread() {
    thread->interrupt();
    thread->join();
#ifdef DEBUG
    cout << "Thread for printer " << config->name << " finished" << endl;
#endif
}
void Printer::connectionClosed() {
    jobManager->undoCurrentJob();
    mutex::scoped_lock l(sendMutex);
    manualCommands.clear();
}

void Printer::addResponse(const std::string& msg,uint8_t rtype) {
    mutex::scoped_lock l(responseMutex);
    shared_ptr<PrinterResponse> newres(new PrinterResponse(msg,++lastResponseId,rtype));
    responses.push_back(newres);
    if(responses.size()>(size_t)gconfig->getBacklogSize())
        responses.pop_front();
    l.unlock();
    logLine((rtype == 1? "< " : "> ")+newres->getTimeString()+": "+newres->message);
    json_spirit::mObject o;
    o["id"] = (int)newres->responseId;
    o["time"] = newres->getTimeString();
    o["text"] = newres->message;
    o["type"] = newres->logtype;
    RepetierEventPtr event(new RepetierEvent(thisPtr,"log",json_spirit::mValue(o)));
    RepetierEvent::fireEvent(event);
    
}
bool Printer::shouldInjectCommand(const std::string& cmd) {
    if(cmd=="@kill") {
        serial->resetPrinter();
        return false;
    }
    if(cmd.length()<2) return false; // Don't waste time with empty lines
    size_t p = cmd.find(';');
    if(p<2) return false;
    return true;
}
void Printer::injectManualCommand(const std::string& cmd) {
    if(!shouldInjectCommand(cmd)) return;
    {
        mutex::scoped_lock l(sendMutex);
        manualCommands.push_back(cmd);
    } // need parantheses to prevent deadlock with trySendNextLine
    trySendNextLine(); // Check if we need to send the command immediately
}
void Printer::injectJobCommand(const std::string& cmd) {
    if(!shouldInjectCommand(cmd)) return;
    mutex::scoped_lock l(sendMutex);
    jobCommands.push_back(cmd);
    // No need to trigger job commands early. There will most probably follow more very soon
    // and the job should already run.
}
void Printer::move(double x,double y,double z,double e,bool relative) {
    if(x!=999999)
        injectManualCommand(state->getMoveXCmd(x, config->xySpeed*60.0,relative));
    if(y!=999999)
        injectManualCommand(state->getMoveYCmd(y, config->xySpeed*60.0,relative));
    if(z!=999999)
        injectManualCommand(state->getMoveZCmd(z, config->zSpeed*60.0,relative));
    if(e!=999999) {
        ExtruderConfigurationPtr ex = config->getExtruder(state->activeExtruder->id);
        injectManualCommand(state->getMoveECmd(e,60.0 * (e>0 ? ex->extrudeSpeed : ex->retractSpeed),relative));
    }
}

size_t Printer::jobCommandsStored() {
    mutex::scoped_lock l(sendMutex);
    return jobCommands.size();
}

boost::shared_ptr<list<boost::shared_ptr<PrinterResponse> > > Printer::getResponsesSince(uint32_t resId,uint8_t filter,uint32_t &lastid) {
    lastid = resId;
    shared_ptr<list<shared_ptr<PrinterResponse> > > retList(new list<shared_ptr<PrinterResponse> >());
    mutex::scoped_lock l(responseMutex);
    deque<shared_ptr<PrinterResponse> >::iterator act = responses.begin();
    deque<shared_ptr<PrinterResponse> >::iterator e = responses.end();
    while(act!=e) {
        if((*act)->responseId>resId && ((*act)->logtype & filter)!=0) {
            retList->push_back(*act);
            lastid = (*act)->responseId;
        }
        act++;
    }
    return retList;
}
void Printer::close() {
    serial->close();
}
void Printer::resendLine(size_t line)
{
    {
        mutex::scoped_lock l(sendMutex);
        ignoreNextOk = config->serialOkAfterResend;
        resendError++;
        errorsReceived++;
        if(!config->serialPingPong && errorsReceived==3 && cacheSize>63) {
            cacheSize = 63;
        }
        if (config->serialPingPong)
            readyForNextSend = true;
        else  {
            nackLines.clear();
            receiveCacheFill = 0;
        }
    
        if (resendError > 8) {
            string msg = "Receiving only error messages. Reset communication.";
            string url = "/printer/msg/"+config->slug+"?a=close";
            gconfig->createMessage(msg, url);
            serial->resetPrinter();
        }
        line &=65535;
        resendLines.clear();
        bool addLines = false;
        for(deque<shared_ptr<GCode> >::iterator it=history.begin();it!=history.end();++it) {
            GCode &gc = **it;
            if (gc.hasN() && (gc.getN() & 65535) == line)
                addLines = true;
            if(addLines) resendLines.push_back(*it);
        }
        if (binaryProtocol != 0) {
            boost::this_thread::sleep(boost::posix_time::milliseconds(320000/config->serialBaudrate));
            uint8_t  buf[32];
            for (int i = 0; i < 32; i++) buf[i] = 0;
            serial->writeBytes(buf,32);
            boost::this_thread::sleep(boost::posix_time::milliseconds(320000/config->serialBaudrate));
        } else {
            boost::this_thread::sleep(boost::posix_time::milliseconds(cacheSize*10000/config->serialBaudrate)); // Wait for buffer to empty
        }
    } // unlock mutex or we get deadlock!
    trySendNextLine();
}
// manageHOstCmmands is called with sendMutex locked!
void Printer::manageHostCommand(boost::shared_ptr<GCode> &cmd) {
    string c = cmd->hostCommandPart();
    if(c=="@pause") {
        string msg= "Printer "+config->name+" paused:"+cmd->hostParameter();
        string answer = "/printer/msg/"+config->slug+"?a=unpause";
        gconfig->createMessage(msg,answer);
        paused = true;
        state->storePause();
        jobManager->pushCompleteJobNoBlock("pause",true);
    } else if(c=="@isathome") {
        state->setIsathome();
    } else if(c=="@kill") {
        serial->resetPrinter();
    } else if(c=="@test") {
        json_spirit::mObject data;
        json_spirit::mValue val(data);
        PrinterPtr pptr;
        RepetierEventPtr event(new RepetierEvent(pptr,"messagesChanged",val));
        RepetierEvent::fireEvent(event);
    }
}

void Printer::stopPause() {
    state->injectUnpause();
    mutex::scoped_lock l(sendMutex);
    paused = false;
}

void Printer::fireEvent(string eventName,json_spirit::mObject &data) {
    RepetierEventPtr event(new RepetierEvent(thisPtr,eventName,json_spirit::mValue(data)));
    RepetierEvent::fireEvent(event);
}

void Printer::fireEvent(string eventName,json_spirit::mArray &data) {
    RepetierEventPtr event(new RepetierEvent(thisPtr,eventName,json_spirit::mValue(data)));
    RepetierEvent::fireEvent(event);
}

bool Printer::trySendPacket(GCodeDataPacketPtr &dp,shared_ptr<GCode> &gc) {
    if((config->serialPingPong && readyForNextSend) || (!config->serialPingPong && cacheSize>receiveCacheFill+dp->length)) {
        serial->writeBytes(dp->data,dp->length);
        if(!config->serialPingPong) {
            receiveCacheFill += dp->length;
            nackLines.push_back(dp->length);
        } else readyForNextSend = false;
        history.push_back(gc);
        if(history.size()>MAX_HISTORY_SIZE)
            history.pop_front();
        lastCommandSend = boost::posix_time::microsec_clock::local_time();
        bytesSend+=dp->length;
        linesSend++;
        addResponse(gc->getOriginal(), 1);        
        return true;
    }
    return false;
}

// Extract the value following a identifier ident until the next space or line end.
bool Printer::extract(const string& source,const string& ident,string &result)
{
    size_t pos = 0; //source.find(ident);
    size_t len = source.length();
    do
    {
        if(pos>0) pos++;
        pos = source.find(ident, pos);
        if (pos == string::npos) return false;
        if(pos==0) break;
    } while (source[pos-1]!=' ');
    size_t start = pos + ident.length();
    while(start<len && source[pos]==' ') start++;
    size_t end = start;
    while (end < len && source[end] != ' ') end++;
    result = source.substr(start,end-start);
    return true;
}
void Printer::trySendNextLine() {
    if (!garbageCleared) return;
    mutex::scoped_lock l(sendMutex);
    if (config->serialPingPong && !readyForNextSend) {return;}
    if (!serial->isConnected()) {return;} // Not ready yet
    shared_ptr<GCode> gc;
    GCodeDataPacketPtr dp;
    // first resolve old communication problems
    if (resendLines.size()>0) {
        gc = resendLines.front();
        if (binaryProtocol == 0 || gc->forceASCII)
            dp = gc->getAscii(true,true);
        else
            dp = gc->getBinary();
        if(trySendPacket(dp,gc))
        {
            //[rhlog addText:[@"Resend: " stringByAppendingString:[gc getAsciiWithLine:YES withChecksum:YES]]];
        }
        return;
    }
    if (resendError > 0) resendError--; // Drop error counter
                                        // then check for manual commands
    if (manualCommands.size() > 0)  {
        gc = shared_ptr<GCode>(new GCode(thisPtr,manualCommands.front()));
        if (gc->hostCommand)
        {
            manageHostCommand(gc);
            manualCommands.pop_front();
            return;
        }
        if(gc->m!=117)
            gc->setN(state->increaseLastline());
        if (binaryProtocol == 0 || gc->forceASCII)
            dp = gc->getAscii(true,true);
        else
            dp = gc->getBinary();
        if(trySendPacket(dp,gc)) {
            manualCommands.pop_front();
            state->analyze(*gc);
        } else if(gc->hasN() && !(gc->hasM() && gc->getM()==110)) state->decreaseLastline();
        return;
    }
    // do we have a printing job?
    if (jobCommands.size()>0 && !paused)  {
        gc = shared_ptr<GCode>(new GCode(thisPtr,jobCommands.front()));
        if (gc->hostCommand)
        {
            manageHostCommand(gc);
            jobCommands.pop_front();
            return;
        }
        if(gc->m!=117)
            gc->setN(state->increaseLastline());
        if (binaryProtocol == 0 || gc->forceASCII)
            dp = gc->getAscii(true,true);
        else
            dp = gc->getBinary();
        if(trySendPacket(dp,gc)) {
            jobCommands.pop_front();
            state->analyze(*gc);
        } else if(gc->hasN() && !(gc->hasM() && gc->getM()==110)) state->decreaseLastline();
        return;
    }
}

void Printer::analyseResponse(string &res) {
#ifdef DEBUG
    //   cout << "Response:" << res << endl;
#endif
    uint8_t rtype = 4;
    while(res.length()>0 && res[0]<32)
        res = res.substr(1);
    string h;
    
    state->analyseResponse(res,rtype); // Update state variables
    size_t fpos = res.find("start");
    if (fpos==0 ||
        (garbageCleared==false && fpos!=string::npos))
    {
        {
            mutex::scoped_lock l(sendMutex);
            state->reset();
            // [job killJob]; // continuing the old job makes no sense, better save the plastic
            history.clear();
            readyForNextSend = true;
            nackLines.clear();
            receiveCacheFill = 0;
            garbageCleared = true;
            manualCommands.clear();
            jobManager->undoCurrentJob();
        }
        injectManualCommand("M110 N0");
        injectManualCommand("M115");
   }
    if (extract(res,"Resend:",h))
    {
        size_t line = atoi(h.c_str());
        resendLine(line);
    }
    else if (res.find("ok")==0)
    {
        rtype = 2;
        garbageCleared = true;
        //if(Main.main.logView.toolACK.Checked)
        //    log(res, true, level);
        if (!ignoreNextOk)  // ok in response of resend?
        {
            if (config->serialPingPong) readyForNextSend = true;
            else
            {
                mutex::scoped_lock l(sendMutex);
                if (nackLines.size() > 0) {
                    receiveCacheFill-= nackLines.front();
                    nackLines.pop_front();
                }
            }
            resendError = 0;
            trySendNextLine();
        } else
            ignoreNextOk = false;
    }
    else if (res=="wait") 
    {
        rtype = 2;
        mutex::scoped_lock l(sendMutex);
        boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
        time_duration td(now-lastCommandSend);
        if(td.total_seconds()>5) {
            if (config->serialPingPong) readyForNextSend = true;
            else
            {
                nackLines.clear();
                receiveCacheFill = 0;
            }
        }
        resendError = 0;
    } else if(extract(res,"EPR:",h)) { // EPR:3 39 20.00 Max. jerk [mm/s]
        cout << res << endl;
        size_t p1 = res.find(' ',0);
        if(p1!=string::npos) {
            size_t p2 = res.find(' ',p1+1);
            if(p2!=string::npos) {
                size_t p3 = res.find(' ',p2+1);
                if(p3!=string::npos) {
                    json_spirit::mArray list;
                    json_spirit::mObject epr;
                    epr["type"] = res.substr(4,p1-4);
                    epr["pos"] = res.substr(p1+1,p2-p1-1);
                    epr["value"] = res.substr(p2+1,p3-p2-1);
                    epr["valueOrig"] = res.substr(p2+1,p3-p2-1);
                    epr["text"] = res.substr(p3+1);
                    list.push_back(epr);
                    fireEvent("eepromData", list);
                }
            }
        }
    }
    addResponse(res,rtype);
    trySendNextLine();
}

int Printer::getOnlineStatus() {
    if(serial->isConnected()) return 1;
    return 0;
}
bool Printer::getActive() {
    return config->active;
}
void Printer::setActive(bool v) {
    config->active = v;
    sendConfigEvent();
}
void Printer::getJobStatus(json_spirit::Object &obj) {
    jobManager->getJobStatus(obj);
}
void Printer::getJobStatus(json_spirit::mObject &obj) {
    jobManager->getJobStatus(obj);
}
void Printer::sendStateEvent() {
    json_spirit::mObject o;
    state->fillJSONObject(o);
    RepetierEventPtr event(new RepetierEvent(thisPtr,"state",json_spirit::mValue(o)));
    RepetierEvent::fireEvent(event);
}
void Printer::sendConfigEvent() {
    json_spirit::mObject o;
    config->fillJSON(o);
    RepetierEventPtr event(new RepetierEvent(thisPtr,"config",json_spirit::mValue(o)));
    RepetierEvent::fireEvent(event);    
}

void Printer::fillJSONConfig(json_spirit::mObject &obj) {
    config->fillJSON(obj);
}
void Printer::fillTransferData(json_spirit::mObject &obj) {
    obj["bytesSend"] = bytesSend;
    obj["bytesReceived"] = bytesReceived;
    obj["resendErrors"] = errorsReceived;
    obj["linesSend"] = linesSend;
}
