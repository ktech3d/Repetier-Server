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

#include "ActionHandler.h"
#include "PrinterState.h"
#include "global_config.h"
#include "Printjob.h"
#include <vector>
#include "Poco/StreamCopier.h"
#include "GCodeAnalyser.h"
#include "PrinterConfiguration.h"
#include "WorkDispatcher.h"

using namespace json_spirit;
using namespace std;

namespace repetier {
    
    std::map<std::string,actionFunction> ActionHandler::actionMap;
    
    void ActionHandler::registerAction(std::string action,actionFunction func){
        actionMap[action] = func;
    }
    void ActionHandler::dispatch(std::string &action, json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        actionFunction func = actionMap[action];
        if(func == NULL) {
            return;
        }
        try {
            func(obj,out,printer);
        }
        catch(std::exception &e) {}
    }
    
    void ActionHandler::registerStandardActions() {
        registerAction("listPrinter",&actionListPrinter);
        registerAction("messages",&actionMessages);
        registerAction("send",&actionSend);
        registerAction("response",&actionResponse);
        registerAction("move",&actionMove);
        registerAction("removeMessage",&actionRemoveMessage);
        registerAction("removeModel",&actionRemoveModel);
        registerAction("listModels",&actionListModels);
        registerAction("copyModel",&actionCopyModel);
        registerAction("listJobs",&actionListJobs);
        registerAction("startJob",&actionStartJob);
        registerAction("stopJob",&actionStopJob);
        registerAction("removeJob",&actionRemoveJob);
        registerAction("getPrinterConfig",&actionGetPrinterConfig);
        registerAction("setPrinterConfig",&actionSetPrinterConfig);
        registerAction("getScript",&actionGetScript);
        registerAction("setScript",&actionSetScript);
        registerAction("activate",&actionActivate);
        registerAction("deactivate",&actionDeactivate);
        registerAction("communicationData",&actionCommunicationData);
        registerAction("getEeprom",&actionGetEeprom);
        registerAction("setEeprom",&actionSetEeprom);
    }
    
    void ActionHandler::actionListPrinter(mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        mArray ret;
        std::vector<PrinterPtr> *list = &gconfig->getPrinterList();
        for(vector<PrinterPtr>::iterator i=list->begin();i!=list->end();++i) {
            mObject pinfo;
            PrinterPtr p = *i;
            pinfo["name"] = p->config->name;
            pinfo["slug"] = p->config->slug;
            pinfo["online"] = p->getOnlineStatus();
            p->getJobStatus(pinfo);
            pinfo["active"] = p->getActive();
            ret.push_back(pinfo);
        }
        out = ret;
    }
    
    void ActionHandler::actionMessages(mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        mArray ret;
        gconfig->fillJSONMessages(ret);
        out = ret;
    }
    
    void ActionHandler::actionSend(mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        if(printer==NULL) return;
        string cmd = obj["cmd"].get_str();
        printer->injectManualCommand(cmd);
    }

    void ActionHandler::actionResponse(mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
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
    
    void ActionHandler::actionMove(mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        if(printer==NULL) return;
        double x=999999,y=999999,z=999999,e=999999;
        if(obj.find("x")!=obj.end())
            x = obj["x"].get_real();
        if(obj.find("y")!=obj.end())
            y = obj["y"].get_real();
        if(obj.find("z")!=obj.end())
            z = obj["z"].get_real();
        if(obj.find("e")!=obj.end())
            e = obj["e"].get_real();
        bool rel = true;
        if(obj.find("relative")!=obj.end())
            rel = obj["relative"].get_bool();
        printer->move(x, y, z, e,rel);
        mObject o;
        out = o;
    }

    void ActionHandler::actionRemoveMessage(mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        if(printer == NULL) return;
        string a(obj["a"].get_str());
        int id = obj["id"].get_int();
        if(a=="unpause") {
            printer->stopPause();
        }
        if(id) {
            gconfig->removeMessage(id);
//            mArray msg;
//            gconfig->fillJSONMessages(msg);
//            out = msg;
        }

    }

    void ActionHandler::actionRemoveModel(mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        int id = obj["id"].get_int();
        if(printer == NULL) return;
        PrintjobPtr job = printer->getModelManager()->findById(id);
        if(job.get())
            printer->getModelManager()->RemovePrintjob(job);
//        mObject ret;
//        printer->getModelManager()->fillSJONObject("data",ret);
//        out = ret;
    }
    
    void ActionHandler::actionListModels(mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        mObject ret;
        if(printer == NULL) return;
        PrintjobManager* manager = printer->getModelManager();
        if(manager!=NULL)
            manager->fillSJONObject("data",ret);
        out = ret;
    }
    void ActionHandler::actionCopyModel(mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        if(printer == NULL) return;
        mObject ret;
        int id = obj["id"].get_int();
        PrintjobPtr model = printer->getModelManager()->findById(id);
        if(model.get()) {
            PrintjobPtr job = printer->getJobManager()->createNewPrintjob(model->getName());
            job->setLength(model->getLength());
            try {
                    std::ifstream  src(model->getFilename().c_str());
                    std::ofstream  dst(job->getFilename().c_str());
                    Poco::StreamCopier::copyStream(src, dst);
                    dst.close();
                    src.close();
                    printer->getJobManager()->finishPrintjobCreation(job, model->getName(), model->getLength());
                    WorkDispatcherData wd("gcodeInfo","",1000);
                    wd.addParameter(printer->config->slug);
                    wd.addParameter(job->getFilename());
                    WorkDispatcher::addJob(wd);

            } catch(const std::exception& ex) {
                    cerr << "error: Unable to create job file " << job->getFilename() << ":" << ex.what() << endl;
            }
        }
        //  printer->getJobManager()->fillSJONObject("data",ret);
        //  out = ret;
    }
    
    void ActionHandler::actionListJobs(mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        mObject ret;
        if(printer == NULL) return;
        printer->getJobManager()->fillSJONObject("data",ret);
        out = ret;
    }
    
    void ActionHandler::actionStartJob(mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        mObject ret;
        if(printer == NULL) return;
        int id = obj["id"].get_int();
        PrintjobPtr job = printer->getJobManager()->findById(id);
        if(job.get()) {
            PrintjobPtr jobOrig = printer->getModelManager()->findByName(job->getName());
            if(jobOrig!=NULL) {
                shared_ptr<GCodeAnalyser> gca = jobOrig->getInfo(printer);
                gca->printed++;
                gca->safeData();
            }
            printer->getJobManager()->startJob(id);
        }
        //printer->getJobManager()->fillSJONObject("data",ret);
        //out = ret;
    }
    
    void ActionHandler::actionStopJob(mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        mObject ret;
        if(printer == NULL) return;
        int id = obj["id"].get_int();
        PrintjobPtr job = printer->getJobManager()->findById(id);
        if(job.get()) {
            printer->getJobManager()->killJob(id);
            printer->getJobManager()->pushCompleteJob("kill");
        }
        //printer->getJobManager()->fillSJONObject("data",ret);
        //out = ret;
    }
    
    void ActionHandler::actionRemoveJob(mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        mObject ret;
        if(printer == NULL) return;
        int id = obj["id"].get_int();
        PrintjobPtr job = printer->getJobManager()->findById(id);
        if(job.get())
            printer->getJobManager()->RemovePrintjob(job);
        //printer->getJobManager()->fillSJONObject("data",ret);
        //out = ret;
    }
    void ActionHandler::actionGetPrinterConfig(mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        PrinterPtr p = printer;
        if(!obj["printer"].is_null())
            p = gconfig->findPrinterSlug(obj["printer"].get_str());
        if(p == NULL) return;
        out = mObject();
        p->fillJSONConfig(out.get_obj());
    }
    void ActionHandler::actionSetPrinterConfig(mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        if(printer == NULL) return;
        printer->config->fromJSON(obj);
        printer->sendConfigEvent();
    }
    void ActionHandler::actionGetScript(json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        if(printer == NULL) return;
        out = mObject();
        mObject &d = out.get_obj();
        d["name"] = obj["name"];
        d["script"] = printer->config->getScript(obj["name"].get_str());
    }
    void ActionHandler::actionSetScript(json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        if(printer == NULL) return;
        out = mObject();
        mObject &d = out.get_obj();
        d["name"] = obj["name"];
        printer->config->setScript(obj["name"].get_str(),obj["script"].get_str());
        printer->config->saveConfiguration();
    }
    void ActionHandler::actionActivate(json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        printer = gconfig->findPrinterSlug(obj["printer"].get_str());
        if(printer == NULL) return;
        printer->setActive(true);
    }
    void ActionHandler::actionDeactivate(json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        printer = gconfig->findPrinterSlug(obj["printer"].get_str());
        if(printer == NULL) return;
        printer->setActive(false);
    }
    void ActionHandler::actionCommunicationData(json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        if(printer == NULL) return;
        out = mObject();
        printer->fillTransferData(out.get_obj());
    }
    void ActionHandler::actionGetEeprom(json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        if(printer == NULL) return;
        mObject o;
        printer->fireEvent("eepromClear", o);
        printer->injectManualCommand("M205");
    }
    void ActionHandler::actionSetEeprom(json_spirit::mObject &obj,json_spirit::mValue &out,PrinterPtr printer) {
        if(printer == NULL) return;
        mArray &a = obj["eeprom"].get_array();
        mArray::iterator it(a.begin()),ie(a.end());
        for(;it!=ie;++it) {
            mObject &test = (*it).get_obj();
            if(test["value"].get_str()!=test["valueOrig"].get_str()) {
                printer->injectManualCommand("M206 T"+test["type"].get_str()+" P"+test["pos"].get_str()+" X"+test["value"].get_str());
            }
        }
    }

}
