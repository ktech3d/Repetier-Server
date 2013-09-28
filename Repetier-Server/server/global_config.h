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


#ifndef __Repetier_Server__global_config__
#define __Repetier_Server__global_config__

#define REPETIER_SERVER_VERSION "0.24"

#include <iostream>
#include "printer.h"
#include <vector>
#include <list>
#include "Poco/Util/XMLConfiguration.h"

class RepetierMessage {
public:
    std::string message; ///< The message itself.
    std::string finishLink; ///< Link to remove the message
    int mesgId;  ///< Message id
};
typedef shared_ptr<RepetierMessage> RepetierMsgPtr;

class ExternalProgram {
public:
    int id;
    std::string name;
    std::string execute;
    static void breakParameter(std::string command,std::vector<std::string> &params);
    void runCommand();
};
typedef shared_ptr<ExternalProgram> ExternalProgramPtr;

/**
This class contains in first instance the variables from repetier-server.con.
All variables can be read threadsafe through the getter functions.
*/
class GlobalConfig {
    Poco::AutoPtr<Poco::Util::XMLConfiguration> conf;
    std::string wwwDir; ///< Website root
    std::string printerConfigDir; ///< Printer config directory
    std::string storageDir; ///< Data storage directory.
    std::string languageDir; ///< Dir with translation text.
    std::string ports; ///< Ports the server should listen to.
    std::string defaultLanguage; ///< Default language if no language is detected
    std::vector<PrinterPtr> printers;
    std::vector<ExternalProgramPtr> externalCommands;
    int backlogSize;
    mutex msgMutex; ///< Mutex for thread safety of message system.
    int msgCounter; ///< Last used message id.
    std::list<RepetierMsgPtr> msgList; ///< List with active messages.
    bool logging;
public:
    bool daemon;
    inline const std::string& getWebsiteRoot() {return wwwDir;}
    inline const std::string& getPrinterConfigDir() {return printerConfigDir;}
    inline const std::string& getStorageDirectory() {return storageDir;}
    inline const int getBacklogSize() {return backlogSize;}
    inline const std::string& getPorts() {return ports;}
    inline const std::string& getLanguageDir() {return languageDir;}
    inline const std::string& getDefaultLanguage() {return defaultLanguage;}
    inline const bool getLogging() {return logging;}
    /** Load the global configuration file ans set variables accordingly. */
    GlobalConfig(std::string filename);
    /** Walk through all printer configuration files and load them. */
    void readPrinterConfigs();
    /** Start a thread for each printer to watch. */
    void startPrinterThreads();
    /** Stop all running printer threads. */
    void stopPrinterThreads();
    /** Find a printer by its slug name 
     @param slug slug name of the printer you search
     @return found printer or 0 if printer does not exist.
     */
    PrinterPtr findPrinterSlug(const std::string& slug);
    inline std::vector<PrinterPtr> &getPrinterList() {return printers;}
    /** Fill the array with all open messages.
     @param obj Array to fill. */ 
    void fillJSONMessages(json_spirit::Array &obj);
    void fillJSONMessages(json_spirit::mArray &obj);
    /** Create a new message to show. Threadsafe!
     @param msg Message to show.
     @param link Link to remove the message. */
    void createMessage(std::string &msg,std::string &link);
    /** Remove a message from the messages list. Threadsafe.
     @param id Message id. */
    void removeMessage(int id);
    void ensureEndsWithSlash(std::string &path);
    std::vector<ExternalProgramPtr> &getExternalCommands() {return externalCommands;}
    PrinterPtr addPrinterFromConfig(std::string configfile);
    void removePrinter(PrinterPtr printer);
};
extern GlobalConfig *gconfig;
extern std::string intToString(int number);
#endif /* defined(__Repetier_Server__global_config__) */
