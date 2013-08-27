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

#include "ServerEvents.h"

Poco::NotificationCenter RepetierEvent::notificationCenter;

RepetierEvent::RepetierEvent(PrinterPtr prt,std::string _type,json_spirit::mValue _event):printer(prt),type(_type),event(_event) {
}

void RepetierEvent::fireEvent(RepetierEventPtr ptr) {
    notificationCenter.postNotification(new RepetierEventNotification(ptr));
}

RepetierEventQueue::RepetierEventQueue() {
    RepetierEvent::notificationCenter.addObserver(Poco::Observer<RepetierEventQueue,RepetierEventNotification>(*this,&RepetierEventQueue::handleEvent));
}
RepetierEventQueue::~RepetierEventQueue() {
    RepetierEvent::notificationCenter.removeObserver(Poco::Observer<RepetierEventQueue,RepetierEventNotification>(*this,&RepetierEventQueue::handleEvent));
}

void RepetierEventQueue::handleEvent(RepetierEventNotification *note) {
    pushEvent(note->event);
    note->release();
}

bool RepetierEventQueue::hasMessageForPrinter(PrinterPtr &p) {
    boost::mutex::scoped_lock(mutex);
    while(!events.empty()) {
        RepetierEventPtr event(events.back());
        Printer *pptr = event->printer.get();
        if(pptr == NULL || pptr == p.get())
            break;
        events.pop_back();
    }
    return !events.empty();
}

void RepetierEventQueue::pushEvent(RepetierEventPtr event) {
    boost::mutex::scoped_lock(mutex);
    events.push_front(event);
}

RepetierEventPtr RepetierEventQueue::popEvent() {
    boost::mutex::scoped_lock(mutex);
    RepetierEventPtr ep = events.back();
    events.pop_back();
    return ep;
}