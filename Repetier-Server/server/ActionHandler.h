//
//  ActionHandler.h
//  Repetier-Server
//
//  Created by Roland Littwin on 13.07.13.
//
//

#ifndef __Repetier_Server__ActionHandler__
#define __Repetier_Server__ActionHandler__

#include <iostream>
#include<map>
#include "json_spirit.h"
#include "printer.h"

class Printer;
namespace repetier {
    typedef void (*actionFunction)( json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
    
    class ActionHandler {
        static std::map<std::string,actionFunction> actionMap;
    public:
        static void registerAction(std::string action,actionFunction func);
        static void dispatch(std::string &action, json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        
        static void registerStandardActions();
        static void actionListPrinter( json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        static void actionMessages( json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        static void actionSend( json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        static void actionResponse( json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        static void actionMove( json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        static void actionRemoveMessage( json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        static void actionRemoveModel( json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        static void actionListModels( json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        static void actionCopyModel( json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        static void actionListJobs( json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        static void actionStartJob( json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        static void actionStopJob( json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        static void actionRemoveJob( json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
    };
}
#endif /* defined(__Repetier_Server__ActionHandler__) */
