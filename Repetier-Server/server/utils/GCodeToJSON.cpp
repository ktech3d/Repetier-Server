//
//  GCodeToJSON.cpp
//  Repetier-Server
//
//  Created by Roland Littwin on 31.08.13.
//
//

#include "GCodeToJSON.h"
#include "GCode.h"
#include "PrinterState.h"
#include "printer.h"
#include <fstream>

using namespace std;

GCodeToJSON::GCodeToJSON(PrinterPtr _printer):state(new PrinterState(_printer,10)),printer(_printer) {
    
}
 
void GCodeToJSON::convert(std::string file,std::ostream &out) {
    o = &out;
    std::ifstream data(file.c_str());
    string line;
    out << "[";
    bool first = true;
    while(std::getline(data,line)) {
        GCode gc(printer,line);
        state->analyze(gc);
        if(!state->isG1Move) continue;
        float diff[4] = {(float)(state->x-state->x0),(float)(state->y-state->y0),
            (float)(state->z-state->z0),(float)(state->activeExtruder->lastE-state->activeExtruder->e0)};
        if(diff[0]==0 && diff[1]==0 && diff[2]==0) continue; // pure e move
        if(!first)
            out << ",";
        first = false;
        out << "{\"p\":";
        if(diff[3]>0)
            out << "\"E\"";
        else
            out << "\"M\"";
        out << ",\"x\":" << fixed << setprecision(2) << state->x;
        out << ",\"y\":" << state->y;
        out << ",\"z\":" << state->z;
        out << "}";
    }
    out << "]";
    out.flush();
}
