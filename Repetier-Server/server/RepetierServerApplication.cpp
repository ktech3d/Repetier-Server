//
//  RepetierServerApplication.cpp
//  Repetier-Server
//
//  Created by Roland Littwin on 09.07.13.
//
//

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

class MyRequestHandler : public HTTPRequestHandler
{
public:
    virtual void handleRequest(HTTPServerRequest &req, HTTPServerResponse &resp)
    {
        resp.setStatus(HTTPResponse::HTTP_OK);
        resp.setContentType("text/html");
        Poco::URI uri(req.getURI());
        ostream& out = resp.send();
        out << "<h1>Hello world!</h1>"
        << "<p>Count: "  << ++count         << "</p>"
        << "<p>Host: "   << req.getHost()   << "</p>"
        << "<p>Method: " << req.getMethod() << "</p>"
        << "<p>URI: "    << req.getURI()    << "</p>";
        out << "<p>Path: " << uri.getPath() << "</p>";
        out << "<p>Query: " << uri.getQuery() << "</p>";
        out.flush();
        
        cout << endl
        << "Response sent for count=" << count
        << " and URI=" << req.getURI() << endl;
    }
    
private:
    static int count;
};
int MyRequestHandler::count = 0;

class RepetierHTTPRequestHandlerFactory: public HTTPRequestHandlerFactory
{
public:
    RepetierHTTPRequestHandlerFactory()
    {
    }
    
    HTTPRequestHandler* createRequestHandler(
                                             const HTTPServerRequest& request)
    {
        cout << "URI" << request.getURI();
        //        if (request.getURI() == "/")
            return new repetier::MainRequestHandler();
        //else
        //    return 0;
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
       /* const char *options[] = {"document_root", gconfig->getWebsiteRoot().c_str(),"listening_ports", (port!=0 ? Poco::NumberFormatter::format(port).c_str() : gconfig->getPorts().c_str()), NULL};
        
        ctx = mg_start(&callback, NULL, options);
        //getchar();  // Wait until user hits "enter"
        if(gconfig->daemon) {
            while(1) {
                boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
            }
        }
        while(true) {
            if(getchar()=='x') break;
        }
        mg_stop(ctx);*/
        cout << "Closing server" << endl;
        gconfig->stopPrinterThreads();
    }
    return Application::EXIT_OK;
}
