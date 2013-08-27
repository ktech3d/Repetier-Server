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


#ifndef __Repetier_Server__PrinterState__
#define __Repetier_Server__PrinterState__

#include <iostream>
#include <boost/thread.hpp>
#include "json_spirit_value.h"
#include <boost/cstdint.hpp>
#include "printer.h"

using namespace boost;

struct ExtruderStatus {
    int id;
    uint64_t time;
    double tempSet;
    double tempRead;
    int8_t output;
    double ePos;
    double eOffset;
    double eMax;
    double ePrinter;
    double lastE;
    double e0;
    ExtruderStatus();
    void resetPosition();
};
class Printer;
class GCode;
/**
 The PrinterState stores variable values which are
 changed by sending commands or measured by external sensors
 in the firmware.
 The class is thread safe.
 */
class PrinterState {
    bool realPrinter;
public:
    int extruderCount;
    PrinterPtr printer;
    boost::mutex mutex; // Used for thread safety
    ExtruderStatus *activeExtruder;
    //float extruderTemp;
    bool uploading;
    ExtruderStatus bed;
    ExtruderStatus* extruder;
    double x, y, z,f; //, e,emax,f,eprinter;
    double x0,y0,z0;
    double lastX,lastY,lastZ; //,lastE;
    double xOffset, yOffset, zOffset, eOffset;
    double lastZPrint;
    bool fanOn;
    int fanVoltage;
    bool powerOn;
    bool relative;
    bool eRelative;
    int debugLevel;
    uint32_t lastline;
    bool hasXHome, hasYHome, hasZHome;
    double printerWidth, printerHeight, printerDepth;
    int tempMonitor;
    bool drawing;
    int layer;
    bool isG1Move;
    bool eChanged;
    double printingTime;
    bool sdcardMounted;
    
    std::string firmware;
    std::string firmwareURL;
    std::string protocol;
    std::string machine;
    int binaryVersion;
    bool isRepetier;
    bool isMarlin;
    int extruderCountSend;
    int speedMultiply;
    int flowMultiply;
    ExtruderStatus& getExtruder(int extruderId);
    
    double pauseX,pauseY,pauseZ,pauseE,pauseF;
    bool pauseRelative;
    
    PrinterState(PrinterPtr p,int minExtruder=1);
    ~PrinterState();
    void reset();
    void setReal() {realPrinter = true;}
    /** Returns the extruder temperature structure. 
     @param extruderId Id of the extruder. -1 for active extruder.
     @returns Temperature state of selected extruder.
     */
    const ExtruderStatus& getExtruder(int extruderId) const;
    /** Analyses the gcode and changes the status variables accordingly. */
    void analyze(GCode &code);
    /** Analyse the response */
    void analyseResponse(const std::string &res,uint8_t &rtype);
    bool extract(const std::string& source,const std::string& ident,std::string &result);
    /** Increases the line counter.
     @returns Increased line number. */
    uint32_t increaseLastline();
    uint32_t decreaseLastline();
    void setIsathome();
    uint32_t getLastline() {boost::mutex::scoped_lock l(mutex);return lastline;}
    void fillJSONObject(json_spirit::Object &obj);
    void fillJSONObject(json_spirit::mObject &obj);
    std::string getMoveXCmd(double dx,double f,bool relative=true);
    std::string getMoveYCmd(double dy,double f,bool relative=true);
    std::string getMoveZCmd(double dz,double f,bool relative=true);
    std::string getMoveECmd(double de,double f,bool relative=true);
    void storePause();
    void injectUnpause();
};
#endif /* defined(__Repetier_Server__PrinterState__) */
