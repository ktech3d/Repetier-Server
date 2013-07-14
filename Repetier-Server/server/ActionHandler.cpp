//
//  ActionHandler.cpp
//  Repetier-Server
//
//  Created by Roland Littwin on 13.07.13.
//
//

#include "ActionHandler.h"
#include "printer.h"
#include "PrinterState.h"
#include "global_config.h"
#include "Printjob.h"
#include <vector>

using namespace json_spirit;
using namespace std;

namespace repetier {
    
    std::map<std::string,actionFunction> ActionHandler::actionMap;
    
    void ActionHandler::registerAction(std::string action,actionFunction func){
        actionMap[action] = func;
    }
    void ActionHandler::dispatch(std::string &action, json_spirit::mObject &obj,json_spirit::mValue &out,Printer *printer) {
        actionFunction func = actionMap[action];
        if(func == NULL) {
            return;
        }
        func(obj,out,printer);
    }
    
    void ActionHandler::registerStandardActions() {
        registerAction("listPrinter",&actionListPrinter);
        registerAction("messages",&actionMessages);
        registerAction("send",&actionSend);
        registerAction("response",&actionResponse);
        registerAction("move",&actionMove);
        registerAction("removeMessage",&actionRemoveMessage);
        registerAction("removeModel",&actionRemoveModel);
    }
    
    void ActionHandler::actionListPrinter(mObject &obj,json_spirit::mValue &out,Printer *printer) {
        mArray ret;
        std::vector<Printer*> *list = &gconfig->getPrinterList();
        for(vector<Printer*>::iterator i=list->begin();i!=list->end();++i) {
            mObject pinfo;
            Printer *p = *i;
            pinfo["name"] = p->name;
            pinfo["slug"] = p->slugName;
            pinfo["online"] = p->getOnlineStatus();
            p->getJobStatus(pinfo);
            pinfo["active"] = p->getActive();
            ret.push_back(pinfo);
        }
        out = ret;
    }
    
    void ActionHandler::actionMessages(mObject &obj,json_spirit::mValue &out,Printer *printer) {
        mArray ret;
        gconfig->fillJSONMessages(ret);
        out = ret;
    }
    
    void ActionHandler::actionSend(mObject &obj,json_spirit::mValue &out,Printer *printer) {
        if(printer==NULL) return;
        string cmd = obj["cmd"].get_str();
        printer->injectManualCommand(cmd);
    }

    void ActionHandler::actionResponse(mObject &obj,json_spirit::mValue &out,Printer *printer) {
        if(printer==NULL) return;
        mObject lobj;
        uint8_t filter=obj["filter"].get_int();
        uint32_t start=obj["start"].get_int();
        boost::shared_ptr<list<boost::shared_ptr<PrinterResponse> > > rlist = printer->getResponsesSince(start,filter, start);
        lobj["lastid"] = (int)start;
        mArray a;
        list<boost::shared_ptr<PrinterResponse> >::iterator it = rlist->begin(),end=rlist->end();
        for(;it!=end;++it) {
            PrinterResponse *resp = (*it).get();
            mObject o;
            o["id"] = (int)resp->responseId;
            o["time"] = resp->getTimeString();
            o["text"] = resp->message;
            o["type"] = resp->logtype;
            a.push_back(o);
        }
        lobj["lines"] = a;
        mObject pstate;
        printer->state->fillJSONObject(pstate);
        lobj["state"] = pstate;
        out = lobj;
    }
    
    void ActionHandler::actionMove(mObject &obj,json_spirit::mValue &out,Printer *printer) {
        if(printer==NULL) return;
        double x=0,y=0,z=0,e=0;
        if(obj.find("x")!=obj.end())
            x = obj["x"].get_real();
        if(obj.find("y")!=obj.end())
            y = obj["y"].get_real();
        if(obj.find("z")!=obj.end())
            z = obj["z"].get_real();
        if(obj.find("e")!=obj.end())
            e = obj["e"].get_real();
        printer->move(x, y, z, e);
    }

    void ActionHandler::actionRemoveMessage(mObject &obj,json_spirit::mValue &out,Printer *printer) {
        string a(obj["a"].get_str());
        int id = obj["id"].get_int();
        if(a=="unpause") {
            printer->stopPause();
        }
        if(id) {
            gconfig->removeMessage(id);
            mArray msg;
            gconfig->fillJSONMessages(msg);
            out = msg;
        }

    }

    void ActionHandler::actionRemoveModel(mObject &obj,json_spirit::mValue &out,Printer *printer) {
        int id = obj["id"].get_int();
        PrintjobPtr job = printer->getModelManager()->findById(id);
        if(job.get())
            printer->getModelManager()->RemovePrintjob(job);
        mObject ret;
        printer->getModelManager()->fillSJONObject("data",ret);
        out = ret;
    }
}