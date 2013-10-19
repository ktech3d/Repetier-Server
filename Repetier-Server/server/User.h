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

#ifndef __Repetier_Server__User__
#define __Repetier_Server__User__

#include <iostream>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>
#include <Poco/Timestamp.h>
#include <json_spirit.h>
struct sqlite3;

namespace repetier {
    
    
    class User {
    public:
        int64_t id;
        std::string login;
        std::string password;
        int permissions; // 1 = print, 2 = add files, 4 = delete files, 8 = configure printer
        
        User();
        User(int64_t id,std::string _login,std::string _password,int _permissions);
        
        bool canPrint() {return (permissions & 1)==1;}
        bool canAddFiles() {return (permissions & 2)==2;}
        bool canDeleteFiles() {return (permissions & 4)==4;}
        bool canConfigure() {return (permissions & 8)==8;}
        void setPassword(std::string pwd);
        bool passwordCorrect(std::string pwd,std::string psalt);
        void save();
        static std::string randomString(int len);
    };
    typedef boost::shared_ptr<User> UserPtr;
    
    class UserSession;
    typedef boost::shared_ptr<UserSession> UserSessionPtr;
    class UserSession {
        Poco::Timestamp lastUsed;
        static boost::mutex mutex;
        static boost::mutex sessionmutex;
        static std::map<std::string,UserSessionPtr> sessions;
    public:
        std::string sessionId;
        UserPtr user;
        
        UserSession();
        UserSession(std::string sessionId);
        bool isExpired();
        inline bool isRegistered() {return user!=NULL;}
        inline void refreshTimestamp() {boost::mutex::scoped_lock lock(mutex); lastUsed.update();}
        
        static void removeExpiredSessions();
        static UserSessionPtr newSession();
        static UserSessionPtr newSession(std::string sessionId);
        static UserSessionPtr getSession(std::string sessionId);
    };
    
    class UserDatabase {
        friend class User;
        static sqlite3 *db;
        static boost::mutex mutex;
        static int numberEntries;
        static void updateNumberEntries();
        static int callbackNumberEntries(void *dataPtr, int argc, char **argv, char **azColName);
        static int callbackFillJSON(void *dataPtr, int argc, char **argv, char **azColName);
    protected:
        static void deleteFromDatabase(User &d);
        static void insertIntoDatabase(User &d);
    public:
        static void init();
        static void shutdown();
        static bool loginRequired() {return numberEntries>0;}
        static UserPtr addUser(std::string login,std::string password,int permissions=0);
        static bool deleteUser(std::string login);
        static UserPtr findUserByLogin(std::string login);
        static void fillUserlistJSON(json_spirit::mObject &obj);
        static int getNumberOfEntries() {return numberEntries;}
    };
    
}
#endif /* defined(__Repetier_Server__WorkDispatcher__) */
