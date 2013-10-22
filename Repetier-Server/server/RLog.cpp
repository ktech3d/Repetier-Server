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

#ifdef __unix
#include <syslog.h>
#endif
#include "RLog.h"
#include "global_config.h"
#if defined(_WIN32) && !defined(__SYMBIAN32__)
#include <windows.h>
#endif
#include "productData.h"
#include "Poco/SimpleFileChannel.h"
#include "Poco/AutoPtr.h"

using namespace std;
using namespace Poco;

Poco::Logger *logger = NULL;

void RLog::initLogger() {
    AutoPtr<SimpleFileChannel> channel(new SimpleFileChannel);
    channel->setProperty("path", gconfig->getLoggingDirectory()+"server.log");
    channel->setProperty("rotation","2 M");
    Logger::root().setChannel(channel);
    logger = &Poco::Logger::get(PROJECT_NAME);
    logger->information("Start logging...");
}
void RLog::log(const std::string &line,bool err) {
    if(err)
        logger->error(line);
    else
        logger->information(line);
    if(gconfig->daemon == false) {
        cout << line << endl;
        return;
    }
}
void RLog::log(const std::string &line,int val,bool err) {
    string res;
    size_t p = line.find('@');
    if(p==string::npos) p = line.length();
    res = line.substr(0,p)+intToString(val)+line.substr(min(p+1,line.length()));
    log(res,err);
}
void RLog::log(const std::string &line,double val,bool err) {
    string res;
    size_t p = line.find('@');
    char buf[40];
    sprintf(buf,"%f",val);
    if(p==string::npos) p = line.length();
    res = line.substr(0,p)+static_cast<string>(buf)+line.substr(min(p+1,line.length()));
    log(res,err);
}
void RLog::log(const std::string &line,const std::string& val,bool err) {
    string res;
    size_t p = line.find('@');
    if(p==string::npos) p = line.length();
    res = line.substr(0,p)+val+line.substr(min(p+1,line.length()));
    log(res,err);
}
void RLog::log(const std::string &line,char *val,bool err) {
    log(line,string(val),err);
}