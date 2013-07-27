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
using namespace std;

RLog rlog;

RLog::RLog() {
#ifdef __unix
   openlog ("Repetier-Server", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL0);
#endif
}
RLog::~RLog() {
#ifdef __linux
   closelog ();
#endif
}
void RLog::log(const std::string &line,bool err) {
    if(gconfig->daemon == false) {
        cout << line << endl;
        return;
    }
#if defined(_WIN32) && !defined(__SYMBIAN32__)
	//if(err)
		//WriteEventLogEntry(line.c_str(), EVENTLOG_ERROR_TYPE);
	//else
		//WriteEventLogEntry(line.c_str(),  EVENTLOG_INFORMATION_TYPE);
#endif
#ifdef __unix
    if(err)
        syslog (LOG_ERR, "%s",line.c_str());
    else
        syslog (LOG_INFO,"%s", line.c_str());
#endif
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