/*
 Copyright 2012 Roland Littwin (repetier) repetierdev@gmail.com
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


#ifndef __Repetier_Server__WebserverAPI__
#define __Repetier_Server__WebserverAPI__

#include <iostream>
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include <map>
#include "json_spirit.h"
#include <boost/thread.hpp>

class Printer;
namespace repetier {
    
    class MainRequestHandler : public Poco::Net::HTTPRequestHandler
    {
    public:
        virtual void handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp);
        static void StaticResponse(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp);
        static void PHPResponse(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp);
        static void serverError(Poco::Net::HTTPServerResponse &resp);
        static void notFoundError(Poco::Net::HTTPServerResponse &resp);
        static void registerActionHandler(std::string name,Poco::Net::HTTPRequestHandler* action);
    private:
        static std::map<std::string, Poco::Net::HTTPRequestHandler*> actionMap;
    };
    class PrinterRequestHandler : public Poco::Net::HTTPRequestHandler
    {
    public:
        virtual void handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp);
    };
    class WebSocketRequestHandler: public Poco::Net::HTTPRequestHandler
	/// Handle a WebSocket connection.
    {
    public:
        void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
    private:
        Printer *printer;
        boost::mutex sendMutex;
    };
    extern PrinterRequestHandler printerRequestHandler;
    extern WebSocketRequestHandler socketRequestHandler;
    
    /** Load the given file and translate contents. Store the translated file
     in a string. Function is thread safe.
     @param filename File to load and translate.
     @param lang Language code for translation.
     @param result File content as translated string.
     */
    extern void TranslateFile(const std::string &filename,const std::string &lang,std::string& result);
;
    extern bool doesLanguageExist(std::string lang);
    extern std::string JSONValueAsString(const json_spirit::Value &v);
}

#endif /* defined(__Repetier_Server__WebserverAPI__) */
