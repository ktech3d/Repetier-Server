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

#ifndef __Repetier_Server__WorkDispatcher__
#define __Repetier_Server__WorkDispatcher__

#include <iostream>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>

struct sqlite3;

typedef void (*WorkDispatcherFunction)( std::string data);

class WorkDispatcherData {
public:
    int64_t id;
    std::string jobType;
    std::string data;
    int priority;
    
    WorkDispatcherData();
    WorkDispatcherData(std::string _jobType,std::string _data,int priority=0);
    inline bool equals(WorkDispatcherData &x) {
        return jobType == x.jobType && data == x.data;
    }
    void addParameter(std::string param);
    static void getParameters(const std::string &data,std::vector<std::string> &params);
};

class WorkDispatcher {
    static sqlite3 *db;
    static WorkDispatcher *dispatcher;
    static boost::mutex mutex;
    static std::map<std::string,WorkDispatcherFunction> actionMap;
    static WorkDispatcher worker;
    volatile bool stopRequested;
    boost::shared_ptr<boost::thread> thread;
    static void deleteFromDatabase(WorkDispatcherData &d);
    static void insertIntoDatabase(WorkDispatcherData &d);
    static boost::shared_ptr<WorkDispatcherData> getNextJob();
    static boost::shared_ptr<WorkDispatcherData> doesJobExist(WorkDispatcherData &d);
public:
    void startThread();
    void stopThread();
    void run();
    
    static void init();
    static void addJob(std::string jobType,std::string data,int priority=0);
    static void addJob(WorkDispatcherData &work);
    static void unqueueJob(std::string jobType,std::string data);
    static void shutdown();
    static void registerDispatcher(std::string jobType,WorkDispatcherFunction func);
    
    // Some standard dispatcher methods

    static void executeCmdDispatcher(std::string data);
    static void generateGCodeStatsDispatcher(std::string data);
};
#endif /* defined(__Repetier_Server__WorkDispatcher__) */
