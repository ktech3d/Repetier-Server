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

#include "RepetierServerApplication.h"
#include "WebserverAPI.h"

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/URI.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/NumberParser.h"
#include "Poco/format.h"
#include "Poco/NumberFormatter.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include "global_config.h"
#include "WebserverAPI.h"
#include "ActionHandler.h"

using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Timestamp;
using Poco::DateTimeFormatter;
using Poco::DateTimeFormat;
using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::OptionCallback;
using Poco::Util::HelpFormatter;
using namespace Poco::Net;
using namespace Poco;
using namespace std;
using namespace repetier;

class RepetierHTTPRequestHandlerFactory: public HTTPRequestHandlerFactory
{
public:
    RepetierHTTPRequestHandlerFactory()
    {
    }
    
    HTTPRequestHandler* createRequestHandler(
                                             const HTTPServerRequest& request)
    {
            return new repetier::MainRequestHandler();
    }
    
private:
    std::string _format;
};

RepetierServerApplication::RepetierServerApplication(): _helpRequested(false)
{
    configurationFile = "/etc/repetier-server.conf";
}

RepetierServerApplication::~RepetierServerApplication()
{
}
void RepetierServerApplication::initialize(Application& self)
{
    loadConfiguration();
    ServerApplication::initialize(self);
}

void RepetierServerApplication::uninitialize()
{
    ServerApplication::uninitialize();
}

void RepetierServerApplication::defineOptions(Poco::Util::OptionSet& options)
{
    ServerApplication::defineOptions(options);
    
    options.addOption(
                      Option("help", "h", "display argument help information")
                      .required(false)
                      .repeatable(false)
                      .callback(OptionCallback<RepetierServerApplication>(
                                                               this, &RepetierServerApplication::handleHelp)));
    options.addOption(Option("config","c","configuration file").
                      required(false).
                      repeatable(false).
                      argument("config",true).
                      callback(Poco::Util::OptionCallback<RepetierServerApplication>(this,&RepetierServerApplication::handleConfig)));
    options.addOption(Option("port","p","Webserver port").required(false).argument("port",true).
                      callback(Poco::Util::OptionCallback<RepetierServerApplication>(this,&RepetierServerApplication::handlePort)));
}

void RepetierServerApplication::handleHelp(const std::string& name,
                const std::string& value)
{
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader(
                            "A server to handle multiple 3d printer with webserver access.");
    helpFormatter.format(std::cout);
    stopOptionsProcessing();
    _helpRequested = true;
}
void RepetierServerApplication::handleConfig(const std::string& name, const std::string& value) {
    configurationFile = value;
}
void RepetierServerApplication::handlePort(const std::string& name, const std::string& value) {
    if(!Poco::NumberParser::tryParse(value,port))
        port = 8080;
}
int RepetierServerApplication::main(const std::vector<std::string>& args)
{
    if (!_helpRequested)
    {
        boost::filesystem::path cf(configurationFile);
        if(!boost::filesystem::exists(cf) || !boost::filesystem::is_regular_file(cf)) {
            cerr << "Repetier-Server version " << REPETIER_SERVER_VERSION << endl;
            cerr << "Configuration file not found at " << configurationFile << endl;
            cerr << "Please use config option with correct path" << endl;
            handleHelp("","");
            return 2;
        }
        gconfig = new GlobalConfig(configurationFile); // Read global configuration

        std::cout << "Configuration file:" << configurationFile << std::endl;
        std::string format(
                           config().getString("HTTPTimeServer.format",
                                              DateTimeFormat::SORTABLE_FORMAT));
        if(port == 0)
            port = Poco::NumberParser::parse(gconfig->getPorts());
        repetier::MainRequestHandler::registerActionHandler("printer", &repetier::printerRequestHandler);
        repetier::MainRequestHandler::registerActionHandler("socket",&repetier::socketRequestHandler);
        repetier::ActionHandler::registerStandardActions();
        ServerSocket socket(port);
        Poco::Net::HTTPServerParams *params = new Poco::Net::HTTPServerParams();
        params->setServerName("Repetier-Server");
        params->setSoftwareVersion(REPETIER_SERVER_VERSION);
        
        HTTPServer srv(new RepetierHTTPRequestHandlerFactory(),
                       socket,params);
        gconfig->readPrinterConfigs();
        gconfig->startPrinterThreads();
        srv.start();
        waitForTerminationRequest();
        srv.stop();
        ShutdownManager::waitForShutdown();
        cout << "Closing server" << endl;
        gconfig->stopPrinterThreads();
    }
    return Application::EXIT_OK;
}
