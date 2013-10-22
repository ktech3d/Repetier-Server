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

#include "User.h"
#include <sqlite3.h>
#include "global_config.h"
#include "RLog.h"
#include "Poco/NumberFormatter.h"
#include "Poco/NumberParser.h"
#include "Poco/Process.h"
#include <vector>
#include <boost/filesystem.hpp>
#include <utils/md5.h>

using namespace std;
using namespace Poco;
using namespace boost;
using namespace boost::filesystem;
using namespace repetier;

namespace repetier {
    sqlite3 *UserDatabase::db = NULL;
    boost::mutex UserDatabase::mutex;
    map<std::string,UserSessionPtr> UserSession::sessions;
    int UserDatabase::numberEntries;
    boost::mutex UserSession::mutex;
    boost::mutex UserSession::sessionmutex;
    
    User::User(int64_t _id,std::string _login,std::string _password,int _permissions) {
        id = _id;
        login = _login;
        permissions = _permissions;
        setPassword(_password);
    }
    
    User::User() {
        login= "";
        password = "";
        id = 0;
        permissions = 1;
    }
    void User::save() {
        UserDatabase::insertIntoDatabase(*this);
    }

    static const char alphanum[] = "0123456789!@#$%^&*ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string User::randomString(int len) {
        string s("");
        for(int i=0;i<len;i++)
            s += alphanum[rand() % (sizeof(alphanum) - 1)];
        return s;
    }
    
    void User::setPassword(std::string pwd) {
        password = pwd; //md5(login+pwd);
    }
    
    bool User::passwordCorrect(std::string pwd,std::string sessionid) {
        return pwd==md5(sessionid+password);
    }

    UserSession::UserSession() {
        boost::mutex::scoped_lock lock(sessionmutex);
        do {
            sessionId = User::randomString(32);
        } while(sessions.find(sessionId)!=sessions.end());
    }
    UserSession::UserSession(std::string _sessionId) {
        sessionId = _sessionId;
    }

    bool UserSession::isExpired() {
        boost::mutex::scoped_lock lock(mutex);
        Poco::Timestamp::TimeDiff diff = lastUsed.elapsed();
        return diff>5*60*1000000; // Expire after 5 minutes.
    }
    
    void UserSession::removeExpiredSessions() {
        boost::mutex::scoped_lock lock(sessionmutex);
        map<string,UserSessionPtr>::iterator it,ie;
        bool found = false;
        do {
            found = false;
            ie = sessions.end();
            for(it = sessions.begin();it!=ie;it++) {
                if(it->second->isExpired()) {
                    sessions.erase(it);
                    found = true;
                    break;
                }
            }
        } while(found);
    }

    UserSessionPtr UserSession::newSession() {
        UserSessionPtr sess(new UserSession());
        boost::mutex::scoped_lock lock(sessionmutex);
        sessions[sess->sessionId] = sess;
        return sess;
    }
    UserSessionPtr UserSession::newSession(string _sessionId) {
        UserSessionPtr sess(new UserSession(_sessionId));
        boost::mutex::scoped_lock lock(sessionmutex);
        sessions[sess->sessionId] = sess;
        return sess;
    }
 
    UserSessionPtr UserSession::getSession(std::string sessionId) {
        removeExpiredSessions();
        boost::mutex::scoped_lock lock(sessionmutex);
        map<string,UserSessionPtr>::iterator it = sessions.find(sessionId);
        if(it == sessions.end()) {
            if(sessionId.length()>20)
                return newSession(sessionId);
            return UserSessionPtr();
        }
        it->second->refreshTimestamp();
        return it->second;
    }

    static int callback(void *NotUsed, int argc, char **argv, char **azColName){
        int i;
        for(i=0; i<argc; i++){
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }
        printf("\n");
        return 0;
    }
    
    static int callbackUser(void *dataPtr, int argc, char **argv, char **azColName){
        User *data = (User*)dataPtr;
        data->id = NumberParser::parse(argv[0]);
        data->login = argv[1];
        data->password = argv[2];
        data->permissions = NumberParser::parse(argv[3]);
        return 0;
    }
    int UserDatabase::callbackNumberEntries(void *dataPtr, int argc, char **argv, char **azColName) {
        numberEntries = NumberParser::parse(argv[0]);
        return 0;
    }

    void UserDatabase::init() {
        if(db!=NULL) return;
        const string &databaseDir = gconfig->getStorageDirectory()+"/database";
        if(!exists(databaseDir)) { // First call - create directory
            if(!create_directories(databaseDir)) {
                cerr << "error: Unable to create database directory " << databaseDir << "." << endl;
                exit(-1);
            }
        }
        string databasePath = databaseDir+"/user.sql";
        int rc;
        char *zErrMsg = 0;
        rc = sqlite3_open(databasePath.c_str(), &db);
        if( rc ){
            RLog::log("Can't open database: @",sqlite3_errmsg(db),true);
            sqlite3_close(db);
            exit(1);
        }
        /* Create SQL statement */
        string createTable = "CREATE TABLE user ("  \
        "id INTEGER PRIMARY KEY     NOT NULL," \
        "login           TEXT    NOT NULL," \
        "password            TEXT     NOT NULL," \
        "permissions         INT     NOT NULL" \
        ");";
        
        /* Execute SQL statement */
        rc = sqlite3_exec(db, createTable.c_str(), callback, 0, &zErrMsg);
        if( rc != SQLITE_OK ){
            // rlog.log("SQL error: @", zErrMsg,true);
            sqlite3_free(zErrMsg);
        } else {
            RLog::log("WorkDispatcher table created successfully");
        }
        updateNumberEntries();
    }
    
    UserPtr UserDatabase::addUser(std::string login,std::string password,int permissions) {
        if(!loginRequired())
            permissions |= 15;
        User *work = new User(0,login,password,permissions);
        insertIntoDatabase(*work);
        return UserPtr(work);
    }
    
    bool UserDatabase::deleteUser(std::string login) {
        UserPtr user = findUserByLogin(login);
        if(user == NULL) return false;
        deleteFromDatabase(*user);
        return true;
    }

    void UserDatabase::deleteFromDatabase(User &d) {
        if(d.id == 0) return;
        char *zErrMsg = 0;
        char* query = sqlite3_mprintf("DELETE FROM user WHERE id=%q;",
                                      NumberFormatter::format((long)d.id).c_str());
        boost::mutex::scoped_lock lock(mutex);
        int rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
        if( rc != SQLITE_OK ){
            RLog::log("SQL error: @", zErrMsg);
            sqlite3_free(zErrMsg);
        } else {
            d.id = sqlite3_last_insert_rowid(db);
        }
        sqlite3_free(query);
        numberEntries--;
    }
    
    void UserDatabase::insertIntoDatabase(User &d) {
        char *zErrMsg = 0;
        if(d.id == 0) {
            char* query = sqlite3_mprintf("INSERT INTO user (id,login,password,permissions) VALUES(NULL,'%q','%q',%q)",
                                          d.login.c_str(),d.password.c_str(),NumberFormatter::format(d.permissions).c_str());
            boost::mutex::scoped_lock lock(mutex);
            int rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
            if( rc != SQLITE_OK ){
                RLog::log("SQL error: @", zErrMsg);
                sqlite3_free(zErrMsg);
            } else {
                d.id = sqlite3_last_insert_rowid(db);
            }
            sqlite3_free(query);
            numberEntries++;
        } else {
            char* query = sqlite3_mprintf("UPDATE user SET login='%q',password='%q',permissions=%q WHERE id=%q",
                                          d.login.c_str(),d.password.c_str(),NumberFormatter::format(d.permissions).c_str(),NumberFormatter::format((long)d.id).c_str());
            boost::mutex::scoped_lock lock(mutex);
            int rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);
            if( rc != SQLITE_OK ){
                RLog::log("SQL error: @", zErrMsg);
                sqlite3_free(zErrMsg);
            }
            sqlite3_free(query);
        }
    }
    void UserDatabase::updateNumberEntries() {
        char *zErrMsg = 0;
        string query = "SELECT count(*) FROM user";
        boost::mutex::scoped_lock lock(mutex);
        UserPtr data(new User());
        int rc = sqlite3_exec(db, query.c_str(), callbackNumberEntries, NULL, &zErrMsg);
        if( rc != SQLITE_OK ){
            RLog::log("SQL error: @", zErrMsg);
            sqlite3_free(zErrMsg);
        }
    }
    UserPtr UserDatabase::findUserByLogin(std::string login) {
        char *zErrMsg = 0;
        char *query = sqlite3_mprintf("SELECT id,login,password,permissions FROM user WHERE login='%q'",login.c_str());
        boost::mutex::scoped_lock lock(mutex);
        UserPtr data(new User());
        int rc = sqlite3_exec(db, query, callbackUser, data.get(), &zErrMsg);
        if( rc != SQLITE_OK ){
            RLog::log("SQL error: @", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        sqlite3_free(query);
        if(data->id == 0)
            data.reset();
        return data;
    }

    int UserDatabase::callbackFillJSON(void *dataPtr, int argc, char **argv, char **azColName){
        json_spirit::mObject &data = *(json_spirit::mObject*)dataPtr;
        json_spirit::mObject user;
        user["id"] = NumberParser::parse(argv[0]);
        user["login"] = argv[1];
        user["permissions"] = NumberParser::parse(argv[2]);
        data["users"].get_array().push_back(user);
        return 0;
    }

    void UserDatabase::fillUserlistJSON(json_spirit::mObject &obj) {
        obj["loginRequired"] = loginRequired();
        obj["users"] = json_spirit::mArray();
        char *zErrMsg = 0;
        char *query = sqlite3_mprintf("SELECT id,login,permissions FROM user ORDER BY login");
        boost::mutex::scoped_lock lock(mutex);
        UserPtr data(new User());
        int rc = sqlite3_exec(db, query, callbackFillJSON, &obj, &zErrMsg);
        if( rc != SQLITE_OK ){
            RLog::log("SQL error: @", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        sqlite3_free(query);
        if(data->id == 0)
            data.reset();
    }
    
    
    void UserDatabase::shutdown() {
        if(db == NULL) return;
        sqlite3_close(db);
        db = NULL;
    }
}
