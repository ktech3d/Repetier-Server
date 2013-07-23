//
//  GCodeAnalyser.h
//  Repetier-Server
//
//  Created by Roland Littwin on 15.07.13.
//
//

#ifndef __Repetier_Server__GCodeAnalyser__
#define __Repetier_Server__GCodeAnalyser__

#include <iostream>
#include <vector>
#include "printer.h"

class GCode;
class PrinterState;
class Printer;

#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2
#define E_AXIS 3

/** Are the step parameter computed */
#define FLAG_JOIN_STEPPARAMS_COMPUTED 1
/** The right speed is fixed. Don't check this block or any block to the left. */
#define FLAG_JOIN_END_FIXED 2
/** The left speed is fixed. Don't check left block. */
#define FLAG_JOIN_START_FIXED 4
/** Start filament retraction at move start */
#define FLAG_JOIN_START_RETRACT 8
/** Wait for filament pushback, before ending move */
#define FLAG_JOIN_END_RETRACT 16
/** Disable retract for this line */
#define FLAG_JOIN_NO_RETRACT 32
/** Wait for the extruder to finish it's up movement */
#define FLAG_JOIN_WAIT_EXTRUDER_UP 64
/** Wait for the extruder to finish it's down movement */
#define FLAG_JOIN_WAIT_EXTRUDER_DOWN 128

class SimulatorLine {
public:
    uint8_t primaryAxis;
    uint8_t flags;
    float timeInTicks;
    uint8_t joinFlags;
    uint8_t dir;                       ///< Direction of movement. 1 = X+, 2 = Y+, 4= Z+, values can be combined.
    float delta[4];                  ///< Steps we want to move.
    float speedX;                   ///< Speed in x direction at fullInterval in mm/s
    float speedY;                   ///< Speed in y direction at fullInterval in mm/s
    float speedZ;                   ///< Speed in z direction at fullInterval in mm/s
    float speedE;                   ///< Speed in E direction at fullInterval in mm/s
    float fullSpeed;                ///< Desired speed mm/s
    float invFullSpeed;             ///< 1.0/fullSpeed for fatser computation
    float accelerationDistance2;             ///< Real 2.0*distanceÜacceleration mm²/s²
    float maxJunctionSpeed;         ///< Max. junction speed between this and next segment
    float startSpeed;               ///< Staring speed in mm/s
    float endSpeed;                 ///< Exit speed in mm/s
    float distance;
    float fullInterval;     ///< interval at full speed in ticks/step.
    float accelSteps;        ///< How much steps does it take, to reach the plateau.
    float decelSteps;        ///< How much steps does it take, to reach the end speed.
    float accelerationPrim; ///< Acceleration along primary axis
    float vMax;              ///< Maximum reached speed in steps/s.
    float vStart;            ///< Starting speed in steps/s.
    float vEnd;              ///< End speed in steps/s
    float stepsRemaining;
    
    double realMoveTime();
    inline bool areParameterUpToDate()
    {
        return joinFlags & FLAG_JOIN_STEPPARAMS_COMPUTED;
    }
    inline void invalidateParameter()
    {
        joinFlags &= ~FLAG_JOIN_STEPPARAMS_COMPUTED;
    }
    inline void setParameterUpToDate()
    {
        joinFlags |= FLAG_JOIN_STEPPARAMS_COMPUTED;
    }
    inline bool isStartSpeedFixed()
    {
        return joinFlags & FLAG_JOIN_START_FIXED;
    }
    inline void setStartSpeedFixed(bool newState)
    {
        joinFlags = (newState ? joinFlags | FLAG_JOIN_START_FIXED : (joinFlags & (~FLAG_JOIN_START_FIXED)));
    }
    inline void fixStartAndEndSpeed()
    {
        joinFlags |= FLAG_JOIN_END_FIXED | FLAG_JOIN_START_FIXED;
    }
    inline bool isEndSpeedFixed()
    {
        return joinFlags & FLAG_JOIN_END_FIXED;
    }
    inline void setEndSpeedFixed(bool newState)
    {
        joinFlags = (newState ? joinFlags | FLAG_JOIN_END_FIXED : joinFlags & ~FLAG_JOIN_END_FIXED);
    }
    inline bool isExtruderForwardMove()
    {
        return (dir & 136)==136;
    }
    inline void setXMoveFinished()
    {
        dir&=~16;
    }
    inline void setYMoveFinished()
    {
        dir&=~32;
    }
    inline void setZMoveFinished()
    {
        dir&=~64;
    }
    inline void setXYMoveFinished()
    {
        dir&=~48;
    }
    inline bool isXPositiveMove()
    {
        return (dir & 17)==17;
    }
    inline bool isXNegativeMove()
    {
        return (dir & 17)==16;
    }
    inline bool isYPositiveMove()
    {
        return (dir & 34)==34;
    }
    inline bool isYNegativeMove()
    {
        return (dir & 34)==32;
    }
    inline bool isZPositiveMove()
    {
        return (dir & 68)==68;
    }
    inline bool isZNegativeMove()
    {
        return (dir & 68)==64;
    }
    inline bool isEPositiveMove()
    {
        return (dir & 136)==136;
    }
    inline bool isENegativeMove()
    {
        return (dir & 136)==128;
    }
    inline bool isXMove()
    {
        return (dir & 16);
    }
    inline bool isYMove()
    {
        return (dir & 32);
    }
    inline bool isXOrYMove()
    {
        return dir & 48;
    }
    inline bool isZMove()
    {
        return (dir & 64);
    }
    inline bool isEMove()
    {
        return (dir & 128);
    }
    inline bool isEOnlyMove()
    {
        return (dir & 240)==128;
    }
    inline bool isNoMove()
    {
        return (dir & 240)==0;
    }
    inline bool isXYZMove()
    {
        return dir & 112;
    }
    inline bool isMoveOfAxis(uint8_t axis)
    {
        return (dir & (16<<axis));
    }
    inline void setMoveOfAxis(uint8_t axis)
    {
        dir |= 16<<axis;
    }
    inline void setPositiveDirectionForAxis(uint8_t axis)
    {
        dir |= 1<<axis;
    }
    
    void updateStepsParameter();
};
enum DriveSystem {Cartesian = 1,Delta};
class PrinterSimulator {
public:
    double xyJerk;
    double zJerk;
    double eJerk;
    double maxXYFeedrate;
    double maxZFeedrate;
    double maxEFeedrate;
    double printAcceleration[4];
    double travelAcceleration[4];
    double time;
    double timeMultiplier;
    int bufferSize;
    int bufferReadPos;
    int bufferWritePos;
    int bufferUsed;
    SimulatorLine *buffer;
    DriveSystem driveSystem;
    PrinterSimulator(PrinterPtr printer);
    ~PrinterSimulator();
    void start();
    void sendCommand(GCode *code);
    void finishPrint();
    
    static inline float min(float a,float b) {if(a<b) return a;return b;}
    static inline float max(float a,float b) {if(a>b) return a;return b;}
    boost::shared_ptr<PrinterState> state;
private:
    PrinterPtr printer;
    void computeMaxJunctionSpeed(SimulatorLine *previous,SimulatorLine *current);
    void waitForXFreeLines(int b=1);
    void forwardPlanner(int p);
    void backwardPlanner(int p,int last);
    void updateTrapezoids();
    inline void previousPlannerIndex(int &p)
    {
        p = (p ? p-1 : bufferSize-1);
    }
    inline void nextPlannerIndex(int& p)
    {
        p = (p == bufferSize-1 ? 0 : p+1);
    }
    void calculate_move(SimulatorLine *p,float axis_diff[],uint8_t pathOptimize);
    void queueCartesianMove(float axis_diff[4],uint8_t pathOptimize);
    float safeSpeed(SimulatorLine *p);
    
};

class GCodeAnalyser {
public:
    GCodeAnalyser(std::string file);
    GCodeAnalyser(PrinterPtr printer,std::string path);
    void safeData();
    void removeData();
    double printingTime;
    double totalFilamentUsed;
    int lines;
    int fileSize;
    int printed;
    int layer;
    std::vector<double> filamentUsed;
private:
    void setSourceFile(std::string source);
    void analyseFile(PrinterPtr printer,std::string file);
    std::string path;
};

#endif /* defined(__Repetier_Server__GCodeAnalyser__) */
