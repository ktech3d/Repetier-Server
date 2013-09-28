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

#include "global_config.h"
#include <boost/filesystem.hpp>
#include "PrinterState.h"
#include "ServerEvents.h"
#include "PrinterConfiguration.h"

#include "Poco/NumberFormatter.h"
#include "Poco/Process.h"
#include <ctype.h>

using namespace std;
using namespace boost::filesystem;
using Poco::AutoPtr;
using Poco::Util::XMLConfiguration;
using namespace Poco;

GlobalConfig *gconfig;

void ExternalProgram::breakParameter(std::string command,vector<std::string> &params) {
    size_t pos=0;
    char c;
    bool escape = false,quoted = false;
    string param = "";
    while(pos<command.size()) {
        c = command[pos++];
        if(escape) {
            param+=c;
            escape = false;
        } else if(c=='\\') {
            escape = true;
        } else if(c=='"') {
            if(quoted) {
                params.push_back(param);
            }
            quoted=!quoted;
        } else if(!quoted && isspace(c)) {
            if(param.size()>0)
                params.push_back(param);
            param="";
        } else param+=c;
    }
    if(param.size()>0)
        params.push_back(param);
}

void ExternalProgram::runCommand() {
    vector<string> all,args;
    breakParameter(execute, all);
    if(all.size()==0) return;
    string prg = all.front().data();
    if(prg.size()==0) return;
    vector<string>::iterator it = all.begin()++;
    while(it!=all.end()) {
        args.push_back(*it);
        it++;
    }
    ProcessHandle handle = Process::launch(prg,args);
    //handle.wait(); // Wait for the job to finish execution
}

GlobalConfig::GlobalConfig(string filename) {
    daemon = false;
    msgCounter = 0;
    if(!exists(filename)) {
        cerr << "error: configuration file " << filename << " not found." << endl;
		exit(-1);
    }
    conf.assign(new XMLConfiguration(filename));
    string instdir = conf->getString("installation-directory");
    ensureEndsWithSlash(instdir);
    storageDir = conf->getString("storage-directory");
    ensureEndsWithSlash(storageDir);
    wwwDir = instdir+"www/";
    languageDir = instdir+"languages/";
    ports = conf->getString("port");
    logging = conf->getBool("logging");
    printerConfigDir = storageDir+"configs/";
    defaultLanguage = conf->getString("default-language");
    backlogSize = conf->getInt("backlog-size");
    ensureEndsWithSlash(printerConfigDir);
    ensureEndsWithSlash(storageDir);
    ensureEndsWithSlash(wwwDir);
    ensureEndsWithSlash(languageDir);
#ifdef DEBUG
    cout << "Global configuration:" << endl;
    cout << "Web directory: " << wwwDir << endl;
    cout << "Printer config directory: " << printerConfigDir << endl;
    cout << "Storage directory: " << storageDir << endl;
#endif
    string extprgConfigFile = storageDir+"database/extcommands.xml";
    if(exists(extprgConfigFile)) {
        Poco::AutoPtr<Poco::Util::XMLConfiguration> extconf(new XMLConfiguration(extprgConfigFile));
        int i = 0;
        while(extconf->has("command["+NumberFormatter::format(i)+"]")) {
            string base = "command["+NumberFormatter::format(i)+"]";
            ExternalProgramPtr ptr(new ExternalProgram());
            ptr->id = i++;
            ptr->name = extconf->getString(base+".name");
            ptr->execute = extconf->getString(base+".execute");
            externalCommands.push_back(ptr);
        }
    }
}
void GlobalConfig::ensureEndsWithSlash(std::string &path) {
    if(path.size()==0) {
        path = "/";
        return;
    }
    char c = path[path.size()-1];
    if(c=='/' || c=='\\') return;
    path+="/";
}
void GlobalConfig::readPrinterConfigs() {
    printers.clear();
    if ( !exists( printerConfigDir ) ) return;
    directory_iterator end_itr; // default construction yields past-the-end
    for ( directory_iterator itr( printerConfigDir );itr != end_itr;++itr )
    {
        if ( is_regular(itr->status()) )
        {
            if(itr->path().extension()==".xml") {
                cout << "Printer config: " << itr->path() << " extension:" << itr->path().extension() << endl;
                addPrinterFromConfig(itr->path().string());
            }
        }
    }
}
PrinterPtr GlobalConfig::addPrinterFromConfig(std::string configfile) {
    PrinterPtr p(new Printer(configfile));
    p->Init(p);
    printers.push_back(p);
    return p;
}
void GlobalConfig::removePrinter(PrinterPtr printer) {
    printer->stopThread();
    for(vector<PrinterPtr>::iterator it=printers.begin();it!=printers.end();it++) {
        PrinterPtr p = *it;
        if(p->config->slug == printer->config->slug) {
            printers.erase(it);
            break;
        }
    }
    printer->config->remove();
}

void GlobalConfig::startPrinterThreads() {
    vector<PrinterPtr>::iterator pi;
    for(pi=printers.begin();pi!=printers.end();pi++) {
        (*pi)->startThread();
    }
}

void GlobalConfig::stopPrinterThreads() {
    vector<PrinterPtr>::iterator pi;
    for(pi=printers.begin();pi!=printers.end();pi++) {
        (*pi)->stopThread();
    }
}

PrinterPtr GlobalConfig::findPrinterSlug(const std::string& slug) {
    for(vector<PrinterPtr>::iterator it=printers.begin();it!=printers.end();it++) {
        PrinterPtr p = *it;
        if(p->config->slug == slug) return p;
    }
    return PrinterPtr();
}

void GlobalConfig::fillJSONMessages(json_spirit::Array &arr) {
    mutex::scoped_lock l(msgMutex);
    list<RepetierMsgPtr>::iterator it = msgList.begin(),ie = msgList.end();
    for(;it!=ie;++it) {
        using namespace json_spirit;
        Object obj;
        obj.push_back(Pair("id",(*it)->mesgId));
        obj.push_back(Pair("msg",(*it)->message));
        obj.push_back(Pair("link",(*it)->finishLink));
        arr.push_back(obj);
    }
}
void GlobalConfig::fillJSONMessages(json_spirit::mArray &arr) {
    mutex::scoped_lock l(msgMutex);
    list<RepetierMsgPtr>::iterator it = msgList.begin(),ie = msgList.end();
    for(;it!=ie;++it) {
        using namespace json_spirit;
        mObject obj;
        obj["id"] = (*it)->mesgId;
        obj["msg"] = (*it)->message;
        obj["link"] = (*it)->finishLink;
        arr.push_back(obj);
    }
}

void GlobalConfig::createMessage(std::string &msg,std::string &link) {
    mutex::scoped_lock l(msgMutex);
    RepetierMsgPtr p(new RepetierMessage());
    p->mesgId = ++msgCounter;
    p->message = msg;
    p->finishLink = link+"&id="+intToString(p->mesgId);
    msgList.push_back(p);
    json_spirit::mObject data;
    json_spirit::mValue val(data);
    PrinterPtr pptr;
    RepetierEventPtr event(new RepetierEvent(pptr,"messagesChanged",val));
    RepetierEvent::fireEvent(event);
}

void GlobalConfig::removeMessage(int id) {
    mutex::scoped_lock l(msgMutex);
    list<RepetierMsgPtr>::iterator it = msgList.begin(),ie = msgList.end();
    for(;it!=ie;++it) {
        if((*it)->mesgId == id) {
            msgList.remove(*it);
            break;
        }
    }
    json_spirit::mObject data;
    json_spirit::mValue val(data);
    PrinterPtr pptr;
    RepetierEventPtr event(new RepetierEvent(pptr,"messagesChanged",val));
    RepetierEvent::fireEvent(event);
}

std::string intToString(int number) {
    stringstream s;
    s << number;
    return s.str();
}
