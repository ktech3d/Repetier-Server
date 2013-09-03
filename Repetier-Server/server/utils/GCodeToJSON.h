//
//  GCodeToJSON.h
//  Repetier-Server
//
//  Created by Roland Littwin on 31.08.13.
//
//

#ifndef __Repetier_Server__GCodeToJSON__
#define __Repetier_Server__GCodeToJSON__

#include <iostream>
#include <boost/shared_ptr.hpp>

class PrinterState;
class Printer;

class GCodeToJSON {
    std::ostream *o;
    boost::shared_ptr<PrinterState> state;
    boost::shared_ptr<Printer> printer;
public:
    GCodeToJSON(boost::shared_ptr<Printer> _printer);
    void convert(std::string file,std::ostream &out);
};
#endif /* defined(__Repetier_Server__GCodeToJSON__) */
