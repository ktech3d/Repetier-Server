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

#ifndef __Repetier_Server__ServerEvents__
#define __Repetier_Server__ServerEvents__

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <deque>
#include "Poco/NotificationCenter.h"
#include "Poco/Notification.h"
#include "Poco/Observer.h"
#include "json_spirit.h"

class Printer;
class RepetierEventQueue;
typedef boost::shared_ptr<Printer> PrinterPtr;
class RepetierEvent;
typedef boost::shared_ptr<RepetierEvent> RepetierEventPtr;

class RepetierEvent {
    friend class RepetierEventQueue;
    static Poco::NotificationCenter notificationCenter;
public:
    PrinterPtr printer;
    std::string type;
    json_spirit::mValue event;
    
    RepetierEvent(PrinterPtr prt,std::string _type,json_spirit::mValue _event);
    static void fireEvent(RepetierEventPtr ptr);
};

class RepetierEventNotification : public Poco::Notification {
public:
    RepetierEventPtr event;
    RepetierEventNotification(RepetierEventPtr _event):event(_event) {};
};

class RepetierEventQueue {
    std::deque<RepetierEventPtr> events;
    boost::mutex mutex;
    void handleEvent(RepetierEventNotification *note);
public:
    RepetierEventQueue();
    ~RepetierEventQueue();
    bool hasMessageForPrinter(PrinterPtr &p);
    void pushEvent(RepetierEventPtr event);
    RepetierEventPtr popEvent();
};

#endif /* defined(__Repetier_Server__ServerEvents__) */
