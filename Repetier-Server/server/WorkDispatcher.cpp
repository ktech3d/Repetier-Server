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
 
 The WorkDipatcher handles background jobs in a way, that only one
 job is running at a time. That prevent high background loads especially
 on small or old machines, where it could influence print throughput.
 
 Jobs are stored inside a database, so open jobs survive a restart.
 
 If a job gets added, that is already inside the queue with the same job type and data,
 it is not added a second time.
 */

#include "WorkDispatcher.h"
#include <sqlite3.h>
#include "global_config.h"
#include "RLog.h"
#include "Poco/NumberFormatter.h"
#include "Poco/NumberParser.h"
#include "Poco/Process.h"
#include <vector>
#include "GCodeAnalyser.h"
#include "Printjob.h"
#include <boost/filesystem.hpp>

using namespace std;
using namespace Poco;
using namespace boost;
using namespace boost::filesystem;

sqlite3 *WorkDispatcher::db = NULL;
WorkDispatcher *WorkDispatcher::dispatcher = NULL;
boost::mutex WorkDispatcher::mutex;
WorkDispatcher WorkDispatcher::worker;
std::map<std::string,WorkDispatcherFunction> WorkDispatcher::actionMap;

WorkDispatcherData::WorkDispatcherData(std::string _jobType,std::string _data,int _priority) {
    jobType = _jobType;
    data = _data;
    priority = _priority;
    id = 0;
}
WorkDispatcherData::WorkDispatcherData() {
    jobType= "";
    data = "";
    priority = -1;
    id = 0;
}
void WorkDispatcherData::addParameter(string param) {
    if(data.length()>0)
        data += "#";
    if(param.find('#')!=string::npos) { // escape #
        string newParam = "";
        size_t lastP = 0,curP;
        do {
            curP = param.find('#',lastP);
            if(curP!=string::npos)
                newParam+=param.substr(lastP,curP-lastP)+"\\#";
            else
                newParam+=param.substr(lastP);
            lastP = curP+1;
        } while(curP!=string::npos);
        param = newParam;
    }
    data += param;
}
void WorkDispatcherData::getParameters(const string &data,std::vector<std::string> &params) {
    params.clear();
    size_t lastP = 0,curP,pos;
    int cnt;
    string param = "";
    do {
        curP = data.find('#',lastP);
        if(curP == string::npos) { // finished
            param += data.substr(lastP,data.length()-lastP);
            params.push_back(param);
        } else {
            cnt = 0;
            if(curP>0) {
                pos = curP-1;
                while(data[pos]=='\\') {
                    cnt++;
                    if(pos==0) break;
                    pos--;
                }
            }
            if((cnt & 1)==0) { // no backslash - new param
                param += data.substr(lastP,curP-lastP);
                params.push_back(param);
                param = "";
            } else { // escaped # from parameter
                param += data.substr(lastP,curP-lastP-1)+"#";
            }
            lastP = curP+1;
        }
    } while(curP!=string::npos);
}

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

static int callbackData(void *dataPtr, int argc, char **argv, char **azColName){
    WorkDispatcherData *data = (WorkDispatcherData*)dataPtr;
    data->id = NumberParser::parse(argv[0]);
    data->jobType = argv[1];
    data->data = argv[2];
    data->priority = NumberParser::parse(argv[3]);
    return 0;
}

void WorkDispatcher::init() {
    if(db!=NULL) return;
    const string &databaseDir = gconfig->getStorageDirectory()+"/database";
    if(!exists(databaseDir)) { // First call - create directory
        if(!create_directories(databaseDir)) {
            cerr << "error: Unable to create database directory " << databaseDir << "." << endl;
            exit(-1);
        }
    }
    string databasePath = databaseDir+"/workdispatcher.sql";
    int rc;
    char *zErrMsg = 0;
    rc = sqlite3_open(databasePath.c_str(), &db);
    if( rc ){
        rlog.log("Can't open database: @",sqlite3_errmsg(db),true);
        sqlite3_close(db);
        exit(1);
    }
    /* Create SQL statement */
    string createTable = "CREATE TABLE dispatcher ("  \
    "id INTEGER PRIMARY KEY     NOT NULL," \
    "jobType           TEXT    NOT NULL," \
    "data            TEXT     NOT NULL," \
    "priority            INT     NOT NULL" \
    ");";
    
    /* Execute SQL statement */
    rc = sqlite3_exec(db, createTable.c_str(), callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        // rlog.log("SQL error: @", zErrMsg,true);
        sqlite3_free(zErrMsg);
    } else {
        rlog.log("WorkDispatcher table created successfully");
    }
    registerDispatcher("gcodeInfo", generateGCodeStatsDispatcher);
    registerDispatcher("executeCmd", executeCmdDispatcher);
   /* WorkDispatcherData w1("kann","sein");
    WorkDispatcherData w2("oder","nicht");
    insertIntoDatabase(w1);
    insertIntoDatabase(w2);
    cout << "w1 id " << w1.id << " w2 = " << w2.id << endl;
    deleteFromDatabase(w1);*/
    worker.startThread();
}

void WorkDispatcher::addJob(std::string jobType,std::string data,int priority) {
    WorkDispatcherData work(jobType,data,priority);
    shared_ptr<WorkDispatcherData> existingData = doesJobExist(work);
    if(existingData->jobType.length()==0) return;
    insertIntoDatabase(work);
}
void WorkDispatcher::addJob(WorkDispatcherData &work) {
    shared_ptr<WorkDispatcherData> existingData = doesJobExist(work);
    if(existingData->jobType.length()>0) return;
    insertIntoDatabase(work);
}
void WorkDispatcher::unqueueJob(std::string jobType,std::string data) {
    WorkDispatcherData work(jobType,data,0);
    shared_ptr<WorkDispatcherData> existingData = doesJobExist(work);
    if(existingData->jobType.length()==0) return;
    deleteFromDatabase(*existingData);
}

void WorkDispatcher::deleteFromDatabase(WorkDispatcherData &d) {
    if(d.id == 0) return;
    char *zErrMsg = 0;
    char* query = sqlite3_mprintf("DELETE FROM dispatcher WHERE id=%q;",
                                  NumberFormatter::format((long)d.id).c_str());
    boost::mutex::scoped_lock lock(mutex);
    int rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        rlog.log("SQL error: @", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        d.id = sqlite3_last_insert_rowid(db);
    }
    sqlite3_free(query);
}
void WorkDispatcher::insertIntoDatabase(WorkDispatcherData &d) {
    char *zErrMsg = 0;
    char* query = sqlite3_mprintf("INSERT INTO dispatcher(id,jobType,data,priority) VALUES(NULL,'%q','%q',%q);",
                                  d.jobType.c_str(),d.data.c_str(),NumberFormatter::format(d.priority).c_str());
    boost::mutex::scoped_lock lock(mutex);  
    int rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        rlog.log("SQL error: @", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        d.id = sqlite3_last_insert_rowid(db);
    }
    sqlite3_free(query);
}

boost::shared_ptr<WorkDispatcherData> WorkDispatcher::getNextJob() {
    char *zErrMsg = 0;
    string query = "SELECT id,jobType,data,priority FROM dispatcher ORDER BY priority DESC LIMIT 1;";
    boost::mutex::scoped_lock lock(mutex);
    shared_ptr<WorkDispatcherData> data(new WorkDispatcherData());
    int rc = sqlite3_exec(db, query.c_str(), callbackData, data.get(), &zErrMsg);
    if( rc != SQLITE_OK ){
        rlog.log("SQL error: @", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    return data;
}

boost::shared_ptr<WorkDispatcherData> WorkDispatcher::doesJobExist(WorkDispatcherData &d) {
    char *zErrMsg = 0;
    char* query = sqlite3_mprintf("SELECT id,jobType,data,priority FROM dispatcher WHERE jobType=%Q and data=%Q ORDER BY priority DESC LIMIT 1;",
                                 d.jobType.c_str(),d.data.c_str() );
    boost::mutex::scoped_lock lock(mutex);
    shared_ptr<WorkDispatcherData> data(new WorkDispatcherData());
    int rc = sqlite3_exec(db, query, callbackData, data.get(), &zErrMsg);
    if( rc != SQLITE_OK ){
        rlog.log("SQL error: @", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    sqlite3_free(query);
    return data;    
}

void WorkDispatcher::shutdown() {
    worker.stopThread();
    if(db == NULL) return;
    sqlite3_close(db);
    db = NULL;
}

void WorkDispatcher::registerDispatcher(std::string jobType,WorkDispatcherFunction func) {
    actionMap[jobType] = func;
}

void WorkDispatcher::startThread() {
    assert(!thread);
    thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&WorkDispatcher::run, this)));
    
}

void WorkDispatcher::stopThread() {
    thread->interrupt();
    thread->join();
}

void WorkDispatcher::run() {
    while (!stopRequested)
    {
        try {
            shared_ptr<WorkDispatcherData> data = getNextJob();
            if(data->id == 0) { // job queue is empty
                boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
                continue;
            }
            WorkDispatcherFunction func = actionMap[data->jobType];
            if(func == NULL) { // unknown type - skip
                deleteFromDatabase(*data);
                continue;
            }
            try {
                func(data->data);
            }
            catch(std::exception &e) {}
            deleteFromDatabase(*data);
        }
        catch( boost::thread_interrupted) {
            stopRequested = true;
        }
    }
}

void WorkDispatcher::executeCmdDispatcher(std::string data) {
    vector<string> args,all;
    WorkDispatcherData::getParameters(data, all);
    if(args.size()==0) return;
    string prg = args.front().data();
    vector<string>::iterator it = all.begin()++;
    while(it!=all.end()) {
        args.push_back(*it);
        it++;
    }
    ProcessHandle handle = Process::launch(prg,args);
    handle.wait(); // Fair for the job to finish execution
}

void WorkDispatcher::generateGCodeStatsDispatcher(std::string data) {
    vector<string> args;
    WorkDispatcherData::getParameters(data, args);
    PrinterPtr p = gconfig->findPrinterSlug(args[0]);
    shared_ptr<GCodeAnalyser> gc(new GCodeAnalyser(p,args[1],true));
    PrintjobPtr model = p->getModelManager()->findByFilename(args[1]);
    if(model.get() != NULL) {
        model->setInfo(gc);
    }
    model = p->getJobManager()->findByFilename(args[1]);
    if(model.get() != NULL) {
        model->setInfo(gc);
    }
    json_spirit::mObject obj;
    p->fireEvent("jobsChanged", obj);
}

