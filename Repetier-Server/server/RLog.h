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

#ifndef __Repetier_Server__RLog__
#define __Repetier_Server__RLog__

#include <iostream>

class RLog {
public:
    RLog();
    ~RLog();
    static void log(const std::string &line,bool err=false);
    static void log(const std::string &line,int val,bool err=false);
    static void log(const std::string &line,double val,bool err=false);
    static void log(const std::string &line,const std::string& val,bool err=false);
};
extern RLog rlog;

#endif /* defined(__Repetier_Server__RLog__) */
