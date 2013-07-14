/*
 Copyright 2012 Roland Littwin (repetier) repetierdev@gmail.com
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

#include "WebserverAPI.h"
#include "global_config.h"
#include "printer.h"
#include "json_spirit.h"
#include "Poco/URI.h"
#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/DynamicAny.h"
#include "Poco/StreamCopier.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/PartHandler.h"
#include <map>
#include <set>
#include "moFileReader.h"
#include <boost/bind.hpp>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include "PrinterState.h"
#include "Printjob.h"
#include "RLog.h"
#include "Poco/CountingStream.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/NetException.h"
#include "Poco/Util/ServerApplication.h"
#include "ActionHandler.h"

using namespace std;
using namespace json_spirit;
using namespace boost;
using namespace Poco;
using namespace Poco::Net;
using namespace Poco::Util;

using Poco::CountingInputStream;

#if defined(_WIN32) && !defined(__SYMBIAN32__) // Windows specific
#pragma warning( disable : 4996 )
// Visual Studio 6 does not know __func__ or __FUNCTION__
// The rest of MS compilers use __FUNCTION__, not C99 __func__
// Also use _strtoui64 on modern M$ compilers
#if defined(_MSC_VER) && _MSC_VER < 1300
#define STRX(x) #x
#define STR(x) STRX(x)
#define __func__ "line " STR(__LINE__)
#define strtoull(x, y, z) strtoul(x, y, z)
#define strtoll(x, y, z) strtol(x, y, z)
#else
#define __func__  __FUNCTION__
#define strtoull(x, y, z) _strtoui64(x, y, z)
#define strtoll(x, y, z) _strtoi64(x, y, z)
#endif // _MSC_VER

#define snprintf _snprintf
#define strnstr _strnstr
#define fseeko(x, y, z) _lseeki64(_fileno(x), (y), (z))
#define fdopen(x, y) _fdopen((x), (y))
//#define read(x, y, z) _read((x), (y), (unsigned) z)
#else
#endif
#ifndef O_EXLOCK
#define O_EXLOCK 0
#endif

namespace repetier {
    
    PrinterRequestHandler printerRequestHandler;
    WebSocketRequestHandler socketRequestHandler;
    
    class MyPartHandler: public Poco::Net::PartHandler
    {
        static int counter;
    public:
        MyPartHandler():
		_length(0)
        {
        }
        ~MyPartHandler() {
            if(_storage.isFile()) {
                File file(_storage);
                if(file.exists() && file.isFile())
                    file.remove(); // Cleanup if nobody used the file
            }
        }
        void handlePart(const MessageHeader& header, std::istream& stream)
        {
            _type = header.get("Content-Type", "(unspecified)");
            if (header.has("Content-Disposition"))
            {
                std::string disp;
                NameValueCollection params;
                MessageHeader::splitParameters(header["Content-Disposition"], disp, params);
                _name = params.get("name", "(unnamed)");
                _fileName = params.get("filename", "(unnamed)");
            }
            counter++;
            _storage = Path(Path::temp(), std::string("upload")+NumberFormatter::format(counter)+".bin");
            
            CountingInputStream istr(stream);
            ofstream ostr(_storage.toString().c_str());
            StreamCopier::copyStream(istr, ostr);
            _length = istr.chars();
        }
        
        int length() const
        {
            return _length;
        }
        
        const std::string& name() const
        {
            return _name;
        }
        
        const std::string& fileName() const
        {
            return _fileName;
        }
        
        const std::string& contentType() const
        {
            return _type;
        }
        const Path& storage() const {
            return _storage;
        }
    private:
        int _length;
        std::string _type;
        std::string _name;
        std::string _fileName;
        Path _storage;
    };
    int MyPartHandler::counter = 0;
    
    std::map<std::string, Poco::Net::HTTPRequestHandler*> MainRequestHandler::actionMap;
    
    void MainRequestHandler::handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp) {
        string uriString = req.getURI();
        size_t endp = uriString.find_first_of('?');
        if(endp!=string::npos)
            uriString = uriString.substr(0,endp);
        Path path(uriString);
        string extension = path.getExtension();
        if(extension == "php" || req.getURI().length()<2) {
            PHPResponse(req, resp);
            return;
        }
        Poco::URI uri(req.getURI());
        vector<string>::iterator it,end;
        vector<string> segments;
        uri.getPathSegments(segments);
        string &base =segments[0];
        map<string,HTTPRequestHandler*>::iterator itHandler = actionMap.find(base);
        if(itHandler != actionMap.end())
            (*itHandler).second->handleRequest(req,resp);
        else
            StaticResponse(req, resp);
    }
    void MainRequestHandler::serverError(Poco::Net::HTTPServerResponse &resp) {
        
    }
    void MainRequestHandler::notFoundError(Poco::Net::HTTPServerResponse &resp) {
        ostream& out = resp.send();
        resp.setStatus(HTTPResponse::HTTP_NOT_FOUND);
        resp.setContentType("text/plain");
        out << "Requested file not found.";
        out.flush();
    }
    void MainRequestHandler::registerActionHandler(std::string name,Poco::Net::HTTPRequestHandler* action) {
        actionMap[name] = action;
    }
    
    void MainRequestHandler::StaticResponse(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp) {
        Poco::URI uri(req.getURI());
        vector<string>::iterator it,end;
        vector<string> segments;
        uri.getPathSegments(segments);
        it = segments.begin();
        end = segments.end();
        string path = uri.getPath();
        Path filePath = Path(gconfig->getWebsiteRoot()).append(path);
        File file(filePath);
        if(!file.exists() || !file.isFile()) {
            notFoundError(resp);
            return;
        }
        //cout << "File " << filePath.toString() << endl;
        size_t extPos = path.find_last_of('.');
        string extension = "php";
        if(extPos!=string::npos) extension = path.substr(extPos+1);
        resp.setChunkedTransferEncoding(true);
        if(extension=="php" || extension=="html")
            resp.setContentType("text/html; charset=utf-8");
        else if(extension=="jpg")
            resp.setContentType("image/jpeg");
        else if(extension=="png")
            resp.setContentType("image/png");
        else if(extension=="gif")
            resp.setContentType("image/gif");
        else if(extension=="css")
            resp.setContentType("text/css");
        else if(extension=="js")
            resp.setContentType("application/javascript");
        else if(extension=="woff")
            resp.setContentType("application/font-woff");
        else if(extension=="otf")
            resp.setContentType("font/otf");
        else if(extension=="eot")
            resp.setContentType("application/vnd.ms-fontobject");
        else if(extension=="ttf")
            resp.setContentType("font/ttf");
        else if(extension=="svg")
            resp.setContentType("image/svg+xml");
        else
            resp.setContentType("text/plain");
        resp.setContentLength(file.getSize());
        resp.setStatus(HTTPResponse::HTTP_OK);
        
        ostream& out = resp.send();
        ifstream in(filePath.toString().c_str());
        StreamCopier::copyStream(in,out);
        out.flush();
    }
    
    void WebSocketRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
    {
        Application& app = Application::instance();
        try
        {
            printer = NULL;
            WebSocket ws(request, response);
            //app.logger().information("WebSocket connection established.");
            char buffer[8192];
            int flags;
            int n;
            do
            {
                n = ws.receiveFrame(buffer, sizeof(buffer), flags);
                //app.logger().information(Poco::format("Frame received (length=%d, flags=0x%x).", n, unsigned(flags)));
                mValue cmd;
                if(read(buffer,cmd)) {
                    mObject &oCmd = cmd.get_obj();
                    string action = oCmd["action"].get_str();
                    mObject &data = oCmd["data"].get_obj();
                    mValue rObj;
                    if(oCmd.find("printer") != oCmd.end()) {
                        string p = oCmd["printer"].get_str();
                        cout << "action " << action << " printer " << p << endl;
                        if(printer == NULL || printer->slugName != p)
                            printer = gconfig->findPrinterSlug(p);
                    }
                    ActionHandler::dispatch(action, data,rObj,printer);
                    mObject wsResponse;
                    wsResponse["data"] = rObj;
                    wsResponse["callback_id"] = oCmd["callback_id"];
                    string ret = write(wsResponse,json_spirit::raw_utf8);
                    mutex::scoped_lock l(sendMutex);
                    ws.sendFrame(ret.c_str(), (int)ret.length(), flags);
                }
            }
            while (n > 0 || (flags & WebSocket::FRAME_OP_BITMASK) != WebSocket::FRAME_OP_CLOSE);
            //app.logger().information("WebSocket connection closed.");
        }
        catch (WebSocketException& exc)
        {
            app.logger().log(exc);
            switch (exc.code())
            {
                case WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
                    response.set("Sec-WebSocket-Version", WebSocket::WEBSOCKET_VERSION);
                    // fallthrough
                case WebSocket::WS_ERR_NO_HANDSHAKE:
                case WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
                case WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
                    response.setStatusAndReason(HTTPResponse::HTTP_BAD_REQUEST);
                    response.setContentLength(0);
                    response.send();
                    break;
            }
        } 
    }
    
    
    void listPrinter(Object &ret) {
        Array parr;
        std::vector<Printer*> *list = &gconfig->getPrinterList();
        for(vector<Printer*>::iterator i=list->begin();i!=list->end();++i) {
            Object pinfo;
            Printer *p = *i;
            pinfo.push_back(Pair("name",p->name));
            pinfo.push_back(Pair("slug",p->slugName));
            pinfo.push_back(Pair("online",(p->getOnlineStatus())));
            p->getJobStatus(pinfo);
            pinfo.push_back(Pair("active",p->getActive()));
            parr.push_back(pinfo);
        }
        ret.push_back(Pair("data",parr));
        Array msg;
        gconfig->fillJSONMessages(msg);
        ret.push_back(Pair("messages",msg));
    }
    void PrinterRequestHandler::handleRequest(HTTPServerRequest &req, HTTPServerResponse &resp) {
        MyPartHandler partHandler;
        HTMLForm form(req,req.stream(),partHandler);
        resp.setStatus(HTTPResponse::HTTP_OK);
        resp.setContentType("Content-Type: text/html; charset=utf-8");
        resp.set("Cache-Control", "public, max-age=0");
        ostream& out = resp.send();
        Poco::URI uri(req.getURI());
        vector<string> segments;
        uri.getPathSegments(segments);
        
        // First check the path for its parts
        string error;
        Printer *printer = NULL;
        string cmdgroup = (segments.size()>1 ? segments[1] : "");
        if(segments.size()>2) { // Read printer string
            printer = gconfig->findPrinterSlug(segments[2]);
        }
        Object ret;
        if(cmdgroup=="list") {
            listPrinter(ret);
        } else if(printer==NULL) {
            error = "Unknown printer";
        } else if(cmdgroup=="job") {
            string a = form.get("a","");
            if(a=="list") {
                printer->getJobManager()->fillSJONObject("data",ret);
                Array msg;
                gconfig->fillJSONMessages(msg);
                ret.push_back(Pair("messages",msg));
            } else if(a=="upload") {
#ifdef DEBUG
                cout << "Upload job" << endl;
#endif
                string name,jobname = form.get("name");
                PrintjobPtr job = printer->getJobManager()->createNewPrintjob(jobname);
                File file(partHandler.storage());
                file.renameTo(job->getFilename());
                long size=partHandler.length();
                printer->getJobManager()->finishPrintjobCreation(job,name,size);
                printer->getJobManager()->fillSJONObject("data",ret);
#ifdef DEBUG
                cout << "Name:" << name << " Size:" << size << endl;
#endif
            } else if(a=="remove") {
                DynamicAny sid(form.get("id",""));
                if(!sid.isNumeric()) {
                    int id = sid.convert<int>();
                    PrintjobPtr job = printer->getJobManager()->findById(id);
                    if(job.get())
                        printer->getJobManager()->RemovePrintjob(job);
                }
                printer->getJobManager()->fillSJONObject("data",ret);
            } else if(a=="start") {
                DynamicAny sid(form.get("id",""));
                if(!sid.isNumeric()) {
                    int id = sid.convert<int>();
                    PrintjobPtr job = printer->getJobManager()->findById(id);
                    if(job.get()) {
                        printer->getJobManager()->startJob(id);
                    }
                }
                printer->getJobManager()->fillSJONObject("data",ret);
            } else if(a=="stop") {
                DynamicAny sid(form.get("id",""));
                if(!sid.isNumeric()) {
                    int id = sid.convert<int>();
                    PrintjobPtr job = printer->getJobManager()->findById(id);
                    if(job.get()) {
                        printer->getJobManager()->killJob(id);
                        printer->getScriptManager()->pushCompleteJob("Kill");
                    }
                }
                printer->getJobManager()->fillSJONObject("data",ret);
            }
        } else if(cmdgroup=="model") {
            string a(form.get("a",""));
            if(a=="list") {
                printer->getModelManager()->fillSJONObject("data",ret);
            } else if(a=="upload") {
#ifdef DEBUG
                cout << "Upload model" << endl;
#endif
                string name,jobname(form.get("name",""));
                PrintjobPtr job = printer->getModelManager()->createNewPrintjob(jobname);
                File file(partHandler.storage());
                file.renameTo(job->getFilename());
                long size=partHandler.length();
                printer->getModelManager()->finishPrintjobCreation(job,name,size);
                printer->getModelManager()->fillSJONObject("data",ret);
#ifdef DEBUG
                cout << "Name:" << name << " Size:" << size << endl;
#endif
            } else if(a=="remove") {
                DynamicAny sid(form.get("id",""));
                if(!sid.isNumeric()) {
                    int id = sid.convert<int>();
                    PrintjobPtr job = printer->getModelManager()->findById(id);
                    if(job.get())
                        printer->getModelManager()->RemovePrintjob(job);
                }
                printer->getModelManager()->fillSJONObject("data",ret);
            } else if(a=="copy") {
                DynamicAny sid(form.get("id",""));
                if(!sid.isNumeric()) {
                    int id = sid.convert<int>();
                    PrintjobPtr model = printer->getModelManager()->findById(id);
                    if(model.get()) {
                        PrintjobPtr job = printer->getJobManager()->createNewPrintjob(model->getName());
                        job->setLength(model->getLength());
                        try {
                            std::ifstream  src(model->getFilename().c_str());
                            std::ofstream  dst(job->getFilename().c_str());
                            dst << src.rdbuf();
                            dst.close();
                            src.close();
                            printer->getJobManager()->finishPrintjobCreation(job, model->getName(), model->getLength());
                        } catch(const std::exception& ex)
                        {
                            cerr << "error: Unable to create job file " << job->getFilename() << ":" << ex.what() << endl;
                        }
                    }
                }
                printer->getJobManager()->fillSJONObject("data",ret);
            }
        } else if(cmdgroup=="script") {
            string a(form.get("a",""));
            if(a=="list") {
                printer->getScriptManager()->fillSJONObject("data",ret);
            } else if(a=="save") {
                string name,jobname(form.get("f",""));
                PrintjobPtr job = printer->getScriptManager()->findByName(jobname);
                string text(form.get("text",""));
                try {
                    ofstream out;
                    out.open(job->getFilename().c_str());
                    out << text;
                    out.close();
                } catch(std::exception &ex) {
                    RLog::log("Error writing script: @",static_cast<const string>(ex.what()));
                }
            } else if(a=="load") {
                string name(form.get("f",""));
                PrintjobPtr job = printer->getScriptManager()->findByName(name);
                if(job.get()) {
                    try {
                        ifstream in;
                        in.open(job->getFilename().c_str());
                        StreamCopier::copyStream(in,out);
                    } catch(std::exception &ex) {
                        RLog::log("Error reading script: @",static_cast<string>(ex.what()));
                    }
                }
                return;
            }
        } else if(cmdgroup=="msg") {
            string a(form.get("a",""));
            DynamicAny sid(form.get("id",""));
            int id = 0;
            if(sid.isNumeric()) id = sid.convert<int>();
            if(a=="unpause") {
                printer->stopPause();
            }
            if(id) {
                gconfig->removeMessage(id);
                Array msg;
                gconfig->fillJSONMessages(msg);
                ret.push_back(Pair("messages",msg));
            }
        } else if(cmdgroup == "pconfig") {
            string a(form.get("a",""));
            if(a=="active") {
                string mode(form.get("mode",""));
                if(!mode.empty()) {
                    printer->setActive(mode=="1");
                }
                listPrinter(ret);
            }
        } else if(printer->getOnlineStatus()==0) {
            error = "Printer offline";
            // ============ ONLINE COMMANDS FROM HERE ==============
        } else if(cmdgroup=="send") {
            string cmd(form.get("cmd",""));
            if(!cmd.empty()) {
                printer->injectManualCommand(cmd);
            }
        } else if(cmdgroup=="response") { // Return log
            DynamicAny sfilter(form.get("filter","")),sstart(form.get("start",""));
            uint8_t filter=0;
            uint32_t start=0;
            if(!sfilter.isNumeric())
                filter = sfilter.convert<uint8_t>();
            if(!sstart.isNumeric())
                start = sstart.convert<uint32_t>();
            boost::shared_ptr<list<boost::shared_ptr<PrinterResponse> > > rlist = printer->getResponsesSince(start,filter, start);
            Object lobj;
            lobj.push_back(Pair("lastid",(int)start));
            Array a;
            list<boost::shared_ptr<PrinterResponse> >::iterator it = rlist->begin(),end=rlist->end();
            for(;it!=end;++it) {
                PrinterResponse *resp = (*it).get();
                Object o;
                o.push_back(Pair("id",(int)resp->responseId));
                o.push_back(Pair("time",resp->getTimeString()));
                o.push_back(Pair("text",resp->message));
                o.push_back(Pair("type",resp->logtype));
                a.push_back(o);
            }
            lobj.push_back(Pair("lines",a));
            Object state;
            printer->state->fillJSONObject(state);
            lobj.push_back(Pair("state",state));
            ret.push_back(Pair("data",lobj));
        } else if(cmdgroup=="move") {
            DynamicAny sx(form.get("x","")),sy(form.get("y","")),sz(form.get("z","")),se(form.get("e",""));
            double x=0,y=0,z=0,e=0;
            if(sx.isNumeric()) x = sx.convert<double>();
            if(sy.isNumeric()) y = sy.convert<double>();
            if(sz.isNumeric()) z = sz.convert<double>();
            if(se.isNumeric()) e = se.convert<double>();
            printer->move(x, y, z, e);
        }
        ret.push_back(Pair("error",error));
        
        // Print result
		out << write(ret,json_spirit::raw_utf8);
        out.flush();
    }
    string JSONValueAsString(const Value &v) {
        switch(v.type()) {
            case str_type:
                return v.get_str();
            case int_type:
            {
                char b[40];
                sprintf(b,"%d",v.get_int());
                return string(b);
            }
            case real_type: {
                char b[40];
                sprintf(b,"%f",v.get_real());
                return string(b);
            }
            case bool_type:
                if(v.get_bool()) return string("true");
                return string("false");
            case array_type:
                return "array";
            case obj_type:
                return "object";
            case null_type:
                return "null";
        }
        return("Unsupported type");
    }
    Value* findVariable(list<Value> &vars,const string& name) {
        list<Value>::iterator istart = vars.begin(),iend = vars.end();
        bool found = false;
        for(;istart!=iend && !found;istart++) {
            Value &v = *istart;
            if(v.type()==obj_type) {
                Object &obj = v.get_obj();
                vector<Pair>::iterator oit = obj.begin(),oend = obj.end();
                for(;oit!=oend;oit++) {
                    Pair &p = *oit;
                    if(p.name_ == name) {
                        return &p.value_;
                    }
                }
            }
        }
        return NULL;
    }
    void FillTemplateRecursive(string& text,string& result,list<Value>& vars,size_t start,size_t end) {
        size_t pos(start),pos2,posclose;
        while(pos<end) {
            pos2 = text.find("{{",pos);
            if(pos2==string::npos || pos2+3>=end) { // Finished, no more vars etc
                result.append(text,pos,end-pos);
                return;
            }
            pos2+=2;
            posclose = text.find("}}",pos2);
            if(posclose==string::npos) { // Finished, no more vars etc
                result.append(text,pos,end-pos);
                return;
            }
            
            result.append(text,pos,pos2-pos-2);
            char tp = text[pos2];
            
            if(tp == '#') { // foreach loop
                string name = text.substr(pos2+1,posclose-pos2-1);
                size_t spacePos = name.find(' ');
                string cmd = "";
                if(spacePos!=string::npos) {
                    cmd = name.substr(0,spacePos);
                    name = name.substr(spacePos+1);
                    pos2+=cmd.length()+1;
                }
                string ename = "{{/"+name+"}}";
                size_t epos = text.find(ename,posclose);
                pos2+=name.length()+3;
                posclose = epos+ename.length()-2; // Continue after block
                Value *v = findVariable(vars,name);
                if(v!=NULL) {
                    if(cmd.length()==0) {
                        if(v!=NULL && v->type()==array_type) {
                            Array &a = v->get_array();
                            vector<Value>::iterator it = a.begin(),iend = a.end();
                            for(;it!=iend;++it) {
                                vars.push_front(*it);
                                FillTemplateRecursive(text, result, vars, pos2, epos);
                                vars.pop_front();
                            }
                        }
                    } else if(cmd=="if" && v->type()==bool_type) {
                        if(v->get_bool()) {
                            FillTemplateRecursive(text, result, vars, pos2, epos);
                        }
                    } else if(cmd=="ifnot" && v->type()==bool_type) {
                        if(!v->get_bool()) {
                            FillTemplateRecursive(text, result, vars, pos2, epos);
                        }
                    }
                }
            } else if(tp=='!') { // Comment, simply ignore it
            } else { // Variable
                string name = text.substr(pos2,posclose-pos2);
                Value *v = findVariable(vars,name);
                if(v!=NULL)
                    result.append(JSONValueAsString(*v));
            }
            pos = posclose+2;
        }
    }
    void FillTemplate(string &text,string& result,Object& data) {
        result.clear();
        size_t start(0),end(text.length());
        result.reserve(end*2);
        list<Value> vars;
        vars.push_front(Value(data));
        FillTemplateRecursive(text,result,vars,start,end);
    }
    void MainRequestHandler::PHPResponse(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp) {
        string uri(req.getURI());
        if(uri.length()<=1) uri="/index.php";
        size_t p = uri.find_first_of('?');
        if(p!=string::npos)
            uri = uri.substr(0,p);
        HTMLForm form(req);
        //if(uri.length()<5 || uri.substr(uri.length()-4,4)!=".php") return NULL;
        
        // Step 1: Find translation file
        
        string alang = req.get("Accept-Language", "");
        string lang = "";
        if(!alang.empty()) {
            size_t n = alang.length();
            int mode = 0;
            size_t lstart;
            for(size_t i=0;i<n;i++) {
                char c = alang[i];
                if(mode==0 && isspace(c)) continue;
                if(mode == 0) {
                    lstart = i;
                    mode = 1;
                    continue;
                }
                if(mode ==1 && i==lstart+2) {
                    mode = 2;
                    alang[i]=0;
                    string tlang(&alang[lstart]);
                    alang[i] = c; // restore const value
                    if(doesLanguageExist(tlang)) {
                        lang = tlang;
                        break;
                    }
                }
                if(mode==2 && c==',') mode=0; // next acceptable language
            }
        }
        if(lang == "")
            lang = gconfig->getDefaultLanguage();
        string content;
        TranslateFile(gconfig->getWebsiteRoot()+uri,lang,content);
        // Step 2: Fill template parameter
        Object obj;
        string param = form.get("pn","");
        if(!param.empty()) {
            Printer *p = gconfig->findPrinterSlug(param);
            if(p) p->fillJSONObject(obj);
        }
        obj.push_back(Pair("version",string(REPETIER_SERVER_VERSION)));
        // Step 3: Run template
        string content2;
        //FillTemplate(content, content2, obj);
        ostream &out = resp.send();
        resp.setContentType("text/html; charset=utf-8");
        resp.setContentLength(content.length());
        resp.setStatus(HTTPResponse::HTTP_OK);
        out << content;
        out.flush();
    }
    static map<string,boost::shared_ptr<moFileLib::moFileReader> > rmap;
    static set<string> langNotExist;
    static boost::mutex langMutex;
    bool doesLanguageExist(string lang) {
        moFileLib::moFileReader *r = NULL;
        if((rmap[lang].get()) == NULL) {
            boost::mutex::scoped_lock l(langMutex);
            if(langNotExist.size()>0 && langNotExist.find(lang)==langNotExist.end())
                return false;
            string mofile = gconfig->getLanguageDir()+lang+".mo";
            if(!boost::filesystem::exists(mofile)) {
                langNotExist.insert(lang);
                return false;
            }
            r = new moFileLib::moFileReader();
            r->ReadFile(mofile.c_str());
            rmap[lang].reset(r);
            return true;
        } else return true;
        
    }
    void TranslateFile(const std::string &filename,const std::string &lang,std::string& result) {
        result.clear();
        
        // read mo file if not cached
        moFileLib::moFileReader *r = NULL;
        if((rmap[lang].get()) == NULL) {
            boost::mutex::scoped_lock l(langMutex);
            string mofile = gconfig->getLanguageDir()+lang+".mo";
            if(!boost::filesystem::exists(mofile))
                mofile = gconfig->getLanguageDir()+gconfig->getDefaultLanguage()+".mo";
            if(!boost::filesystem::exists(mofile)) return;
            r = new moFileLib::moFileReader();
            r->ReadFile(mofile.c_str());
            rmap[lang].reset(r);
        } else r = rmap[lang].get();
        // Read file contents
        string contents;
		std::ifstream in(filename.c_str(), ios::in | ios::binary);
		if (in.good())
        {
            in.seekg(0, std::ios::end);
            contents.resize((size_t)in.tellg());
            in.seekg(0, std::ios::beg);
			in.read(&contents[0], contents.size());
			in.close();
        } else {
            result.clear();
            return;
        }
        // Replace translations
        size_t start(0),pos(0),end(contents.length()),pos2,tstart,tend;
        result.reserve(end+end/10); // Reserve some extra space to prevent realloc
        while(pos<end) {
            pos = contents.find("<?php",start);
            if(pos==string::npos) { // End reached, copy rest of file
                result.append(contents,start,end-start);
                break;
            }
            pos2 = contents.find("?>",pos+5);
            if(pos2==string::npos) { // format error, copy rest of file
                result.append(contents,start,end-start);
                break;
            }
            result.append(contents,start,pos-start);
            start = pos2+2;
            tstart = contents.find("_(\"",pos);
            tend = contents.rfind("\")",pos2);
            if(tstart<tend && tend!=string::npos) {
                string key = contents.substr(tstart+3,tend-tstart-3);
                result.append(r->Lookup(key.c_str()));
            }
        }
    }
} // repetier namespace

