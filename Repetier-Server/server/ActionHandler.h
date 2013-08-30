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
        static void actionGetPrinterConfig(json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        static void actionSetPrinterConfig(json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        static void actionGetScript(json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        static void actionSetScript(json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        static void actionActivate(json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        static void actionDeactivate(json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        static void actionCommunicationData(json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        static void actionGetEeprom(json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
        static void actionSetEeprom(json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer);
    };
}
#endif /* defined(__Repetier_Server__ActionHandler__) */
