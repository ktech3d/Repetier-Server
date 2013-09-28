//
//  PrinterConfigiration.cpp
//  Repetier-Server
//
//  Created by Roland Littwin on 27.07.13.
//
//

#include "PrinterConfiguration.h"
#include "Poco/DOM/Element.h"
#include "Poco/DOM/Text.h"
#include "Poco/DOM/DOMWriter.h"
#include "Poco/XML/XMLWriter.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/Util/XMLConfiguration.h"
#include "Poco/DOM/NodeList.h"
#include "Poco/DOM/NodeIterator.h"
#include "Poco/NumberParser.h"
#include "Poco/NumberFormatter.h"
#include "Poco/DOM/NamedNodeMap.h"
#include "Poco/DOM/Attr.h"
#include "Poco/File.h"
#include "Poco/Path.h"
#include <boost/algorithm/string.hpp>
#include <fstream.h>
#include "Printjob.h"
#include "printer.h"
#include "global_config.h"
#include "ServerEvents.h"

using namespace Poco;
using namespace Poco::XML;
using namespace Poco::Util;
using namespace std;
using namespace json_spirit;

PrimitiveShapeRectangle::PrimitiveShapeRectangle(Poco::XML::Element *node) {
    this->node = node;
    setDefaults();
    try {
        if(node->hasAttribute("xMin"))
            xMin = NumberParser::parseFloat(node->getAttribute("xMin"));
        if(node->hasAttribute("xMax"))
            xMax = NumberParser::parseFloat(node->getAttribute("xMax"));
        if(node->hasAttribute("yMin"))
            yMin = NumberParser::parseFloat(node->getAttribute("yMin"));
        if(node->hasAttribute("xMin"))
            yMax = NumberParser::parseFloat(node->getAttribute("yMax"));
        if(node->hasAttribute("color"))
            color = node->getAttribute("color");
    } catch(Exception &e) {}
}
PrimitiveShapeRectangle::PrimitiveShapeRectangle(json_spirit::mObject &obj) {
    this->node = NULL;
    setDefaults();
    xMin = obj["xMin"].get_real();
    yMin = obj["yMin"].get_real();
    xMax = obj["xMax"].get_real();
    yMax = obj["yMax"].get_real();
    color = obj["color"].get_str();
}

PrimitiveShapeRectangle::PrimitiveShapeRectangle() {
    setDefaults();
    node = NULL;
}
void PrimitiveShapeRectangle::save(Poco::XML::Document *doc, Poco::XML::Element *parent) {
    if(node == NULL) {
        node = doc->createElement("rectangle");
        parent->appendChild(node);
    }
    node->setAttribute("xMin", NumberFormatter::format(xMin));
    node->setAttribute("xMax", NumberFormatter::format(xMax));
    node->setAttribute("yMin", NumberFormatter::format(yMin));
    node->setAttribute("yMax", NumberFormatter::format(yMax));
    node->setAttribute("color", color);
}
void PrimitiveShapeRectangle::setDefaults() {
    xMin = yMin = 0;
    xMax = yMax = 200;
    color = "808080";
}

void PrimitiveShapeRectangle::fillJSON(json_spirit::mObject &obj) {
    obj["shape"] = "rectangle";
    obj["xMin"] = xMin;
    obj["yMin"] = yMin;
    obj["xMax"] = xMax;
    obj["yMax"] = yMax;
    obj["color"] = color;
}
PrimitiveShapeCircle::PrimitiveShapeCircle(Poco::XML::Element *node) {
    this->node = node;
    setDefaults();
    try {
        if(node->hasAttribute("x"))
            x = NumberParser::parseFloat(node->getAttribute("x"));
        if(node->hasAttribute("a"))
            y = NumberParser::parseFloat(node->getAttribute("y"));
        if(node->hasAttribute("radius"))
            radius = NumberParser::parseFloat(node->getAttribute("radius"));
        if(node->hasAttribute("color"))
            color = node->getAttribute("xMin");
    } catch(Exception &e) {}
}
PrimitiveShapeCircle::PrimitiveShapeCircle() {
    node = NULL;
    setDefaults();
}
PrimitiveShapeCircle::PrimitiveShapeCircle(json_spirit::mObject &obj) {
    node = NULL;
    setDefaults();
    x = obj["x"].get_real();
    y = obj["y"].get_real();
    radius = obj["radius"].get_real();
}
void PrimitiveShapeCircle::setDefaults() {
    radius = 100;
    x = y = 0;
}
void PrimitiveShapeCircle::save(Poco::XML::Document *doc, Poco::XML::Element *parent) {
    if(node == NULL) {
        node = doc->createElement("circle");
        parent->appendChild(node);
    }
    node->setAttribute("x", NumberFormatter::format(x));
    node->setAttribute("y", NumberFormatter::format(y));
    node->setAttribute("radius", NumberFormatter::format(radius));
    node->setAttribute("color", color);
}
void PrimitiveShapeCircle::fillJSON(json_spirit::mObject &obj) {
    obj["shape"] = "circle";
    obj["x"] = x;
    obj["y"] = y;
    obj["radius"] = radius;
}
ShapeConfiguration::ShapeConfiguration() {
    node = NULL;
    gridSpacing = 10;
    gridColor = "ffffff";
    basicShape.reset(new PrimitiveShapeRectangle());
}
ShapeConfiguration::ShapeConfiguration(Poco::XML::Element *node) {
    this->node = node;
    gridSpacing = 10;
    gridColor = "ffffff";
    basicShape.reset(new PrimitiveShapeRectangle());
    NodeList *list = node->childNodes();
    size_t n = list->length();
    for(size_t i=0;i<n;i++) {
        Node *node = list->item(i);
        if(node->nodeType() != Node::ELEMENT_NODE) continue;
        Element *e = (Element*) node;
        const string &name = e->nodeName();
        if(name == "grid") {
            if(e->hasAttribute("color"))
                gridColor = e->getAttribute("color");
            if(e->hasAttribute("spacing"))
                gridSpacing = NumberParser::parseFloat(e->getAttribute("spacing"));
        } else if(name == "rectangle") {
            basicShape.reset(new PrimitiveShapeRectangle(e));
        } else if(name == "circle") {
            basicShape.reset(new PrimitiveShapeCircle(e));
        }
    }
}
ShapeConfiguration::~ShapeConfiguration() {
    
}
void ShapeConfiguration::fillJSON(json_spirit::mObject &obj) {
    obj["gridSpacing"] = gridSpacing;
    obj["gridColor"] = gridColor;
    obj["basicShape"] = mObject();
    obj["marker"] = mArray();
    basicShape->fillJSON(obj["basicShape"].get_obj());
    for(vector<PrimitiveShapeConfiguration*>::iterator it = marker.begin();it!=marker.end();++it) {
        mObject m;
        (*it)->fillJSON(m);
        obj["marker"].get_array().push_back(m);
    }
}
void ShapeConfiguration::fromJSON(json_spirit::mObject &obj) {
    gridSpacing = obj["gridSpacing"].get_real();
    gridColor = obj["gridColor"].get_str();
    mObject &bshape = obj["basicShape"].get_obj();
    string shape = bshape["shape"].get_str();
    if(basicShape->node!=NULL && node!=NULL)
        node->removeChild(basicShape->node);
    basicShape.reset((PrimitiveShapeConfiguration*)NULL);
    if(shape == "rectangle")
        basicShape.reset(new PrimitiveShapeRectangle(bshape));
    else if(shape == "circle")
        basicShape.reset(new PrimitiveShapeCircle(bshape));
}
void ShapeConfiguration::save(Poco::XML::Document *doc, Poco::XML::Element *parent) {
    Element *grid = getOrCreateElement(doc,"grid");
    grid->setAttribute("color", gridColor);
    grid->setAttribute("spacing", NumberFormatter::format(gridSpacing));
    basicShape->save(doc,node);
}
Poco::XML::Element *ShapeConfiguration::getOrCreateElement(Poco::XML::Document *doc, const std::string &path) {
    Element *root = node;
    vector <string> fields;
    boost::split(fields, path,boost::is_any_of( "." ) );
    Element *act = NULL;
    for(vector<string>::iterator it=fields.begin();it!=fields.end();++it) {
        act = root->getChildElement(*it);
        if(act == NULL) {
            act = doc->createElement(*it);
            root->appendChild(act);
            act->release();
        }
        root = act;
    }
    return root;
}
NamedTemperature::NamedTemperature() {
    node = NULL;
    temperature = 0;
    name = "unknown";
}
NamedTemperature::NamedTemperature(Poco::XML::Element *node) {
    this->node = node;
    name = node->innerText();
    NamedNodeMap *map = node->attributes();
    if(map!=NULL) {
        Node *temp = map->getNamedItem("value");
        if(temp!=NULL)
            temperature = NumberParser::parse(temp->innerText());
    }
}
NamedTemperature::NamedTemperature(json_spirit::mObject &obj) {
    node = NULL;
    name = obj["name"].get_str();
    temperature = obj["temp"].get_int();
}

void NamedTemperature::save(Poco::XML::Document *doc,Poco::XML::Element *parent) {
    if(node == NULL) {
        node = doc->createElement("temperature");
        parent->appendChild(node);
    }
    node->setAttribute("value", NumberFormatter::format(temperature));
    NodeList *list = node->childNodes();
    if(list->length()==0) {
        AutoPtr<Text> textNode = doc->createTextNode(name);
        node->appendChild(textNode);
        return;
    }
    list->item(0)->setNodeValue(name);

    node->setNodeValue(name);
}
void NamedTemperature::fillJSON(json_spirit::mObject &obj) {
    obj["name"] = name;
    obj["temp"] = temperature;
}
ExtruderConfiguration::ExtruderConfiguration(Poco::XML::Element *node) {
    this->node = node;
    eJerk = 40;
    maxSpeed = 20;
    retractDistance = 20;
    retractSpeed = 20;
    extrudeDistance = 2;
    extrudeSpeed = 2;
    num = 0;
    cooldownPerSecond = 0.5;
    heatupPerSecond = 1.5;
    acceleration = 6000;
    
    lastTemp = 185;
    if(node->hasAttribute("num"))
        num = NumberParser::parse(node->getAttribute("num"));
    if(node->hasAttribute("lastTemp"))
        lastTemp = NumberParser::parse(node->getAttribute("lastTemp"));
    NodeList *data = node->childNodes();
    for(int i=0;i<data->length();i++) {
        Node *item = data->item(i);
        const string &name = item->nodeName();
        if(name == "speed") {
            Element *e = (Element*)item;
            if(e->hasAttribute("max"))
                maxSpeed = NumberParser::parseFloat(e->getAttribute("max"));
            if(e->hasAttribute("jerk"))
                eJerk = NumberParser::parseFloat(e->getAttribute("jerk"));
            if(e->hasAttribute("acceleration"))
                acceleration = NumberParser::parseFloat(e->getAttribute("acceleration"));
        } else if(name == "extrude") {
            Element *e = (Element*)item;
            if(e->hasAttribute("speed"))
                extrudeSpeed = NumberParser::parseFloat(e->getAttribute("speed"));
            if(e->hasAttribute("distance"))
                extrudeDistance = NumberParser::parseFloat(e->getAttribute("distance"));
        } else if(name == "retract") {
            Element *e = (Element*)item;
            if(e->hasAttribute("speed"))
                retractSpeed = NumberParser::parseFloat(e->getAttribute("speed"));
            if(e->hasAttribute("distance"))
                retractDistance = NumberParser::parseFloat(e->getAttribute("distance"));
        } else if(name == "tempChange") {
            Element *e = (Element*)item;
            if(e->hasAttribute("heatupPerSecond"))
                heatupPerSecond = NumberParser::parseFloat(e->getAttribute("heatupPerSecond"));
            if(e->hasAttribute("cooldownPerSecond"))
                cooldownPerSecond = NumberParser::parseFloat(e->getAttribute("cooldownPerSecond"));            
        } else if(name == "temperatures") {
            NodeList *l = item->childNodes();
            for(int j=0;j<l->length();j++) {
                Node *tNode = l->item(j);
                if(tNode->nodeName()=="temperature")
                    temperatures.push_back(NamedTemperature((Element*)tNode));
            }
        }
    }
}

ExtruderConfiguration::ExtruderConfiguration(json_spirit::mObject &obj,int pos) {
    num = pos;
    node = NULL;
    eJerk = obj["eJerk"].get_real();
    maxSpeed = obj["maxSpeed"].get_real();
    acceleration = obj["acceleration"].get_real();
    retractSpeed = obj["retractSpeed"].get_real();
    extrudeSpeed = obj["extrudeSpeed"].get_real();
    retractDistance = obj["retractDistance"].get_real();
    extrudeDistance = obj["extrudeDistance"].get_real();
    heatupPerSecond = obj["heatupPerSecond"].get_real();
    cooldownPerSecond = obj["cooldownPerSecond"].get_real();
    mArray &tempList = obj["temperatures"].get_array();
    lastTemp = obj["lastTemp"].get_int();
    int n = (int)tempList.size();
    temperatures.clear();
    for(int i=0;i<n;i++) {
        temperatures.push_back(NamedTemperature(tempList[i].get_obj()));
    }
}
Poco::XML::Element *ExtruderConfiguration::getOrCreateElement(Poco::XML::Document *doc, const std::string &path) {
    Element *root = node;
    vector <string> fields;
    boost::split(fields, path,boost::is_any_of( "." ) );
    Element *act = NULL;
    for(vector<string>::iterator it=fields.begin();it!=fields.end();++it) {
        act = root->getChildElement(*it);
        if(act == NULL) {
            act = doc->createElement(*it);
            root->appendChild(act);
            act->release();
        }
        root = act;
    }
    return root;
}
void ExtruderConfiguration::save(Poco::XML::Document *doc, Poco::XML::Element *parent) {
    if(node == NULL) {
        node = doc->createElement("extruder");
        parent->appendChild(node);
        node->release();
    }
    node->setAttribute("num", NumberFormatter::format(num));
    node->setAttribute("lastTemp", NumberFormatter::format(lastTemp));
    Element *speed = getOrCreateElement(doc,"maxSpeed");
    speed->setAttribute("acceleration", NumberFormatter::format(acceleration));
    speed->setAttribute("max",NumberFormatter::format(maxSpeed));
    speed->setAttribute("jerk", NumberFormatter::format(eJerk));
    
    Element *extrude = getOrCreateElement(doc,"extrude");
    extrude->setAttribute("speed",NumberFormatter::format(extrudeSpeed));;
    extrude->setAttribute("distance", NumberFormatter::format(extrudeDistance));
    Element *retract = getOrCreateElement(doc, "retract");
    retract->setAttribute("speed",NumberFormatter::format(retractSpeed));
    retract->setAttribute("distance", NumberFormatter::format(retractDistance));
    Element *tempChange = getOrCreateElement(doc, "tempChange");
    tempChange->setAttribute("heatupPerSecond", NumberFormatter::format(heatupPerSecond));
    tempChange->setAttribute("cooldownPerSecond", NumberFormatter::format(cooldownPerSecond));
    Element *temps = getOrCreateElement(doc, "temperatures");
    Node *temp;
    do {
        temp = temps->firstChild();
        if(temp)
            temps->removeChild(temp);
    } while(temp);
    int i,n = (int)temperatures.size();
    for(i=0;i<n;i++) {
        temperatures[i].node = NULL;
        temperatures[i].save(doc,temps);
    }
}
void ExtruderConfiguration::fillJSON(json_spirit::mObject &obj) {
    obj["temperatures"] = mArray();
    for(vector<NamedTemperature>::iterator it = temperatures.begin();it!=temperatures.end();++it) {
        mObject nt;
        it->fillJSON(nt);
        obj["temperatures"].get_array().push_back(nt);
    }
    obj["lastTemp"] = lastTemp;
    obj["eJerk"] = eJerk;
    obj["maxSpeed"] = maxSpeed;
    obj["acceleration"] = acceleration;
    obj["retractSpeed"] = retractSpeed;
    obj["extrudeSpeed"] = extrudeSpeed;
    obj["retractDistance"] = retractDistance;
    obj["extrudeDistance"] = extrudeDistance;
    obj["cooldownPerSecond"] = cooldownPerSecond;
    obj["heatupPerSecond"] = heatupPerSecond;
}

PrinterConfiguration::PrinterConfiguration(string filename) {
    configFilename = filename;
    setDefaults();
    try {
        ifstream in(configFilename.c_str());
        InputSource src(in);
        DOMParser parser;
        config = parser.parse(&src);
        string tag;
        
        NodeList *extruderNodes = config->getElementsByTagName("extruder");
        for(int i=0;i<extruderNodes->length();i++) {
            extruderList.push_back(ExtruderConfigurationPtr(new ExtruderConfiguration((Element*)extruderNodes->item(i))));
        }
        extruderNodes->release();
        
        NodeList *shapeNodes = config->getElementsByTagName("shape");
        if(shapeNodes->length()>0) {
            shape.reset(new ShapeConfiguration((Element*)shapeNodes->item(0)));
        } else shape.reset(new ShapeConfiguration());
        shapeNodes->release();
        
        NodeList *generalNodes = config->getElementsByTagName("general");
        if(generalNodes->length()>0) {
            NodeList *generalNodes2 = generalNodes->item(0)->childNodes();
            for(int i=0;i<generalNodes2->length();i++) {
                Node *n = generalNodes2->item(i);
                tag = n->nodeName();
                if(tag=="#text") continue;
                if(tag == "name")
                    name = n->innerText();
                else if(tag == "printerVariant")
                    printerVariant = n->innerText();
                else if(tag == "slug")
                    slug = n->innerText();
                else if(tag == "active")
                    active = parseBool(n->innerText());
                else if(tag == "fan")
                    fan = parseBool(n->innerText());
                else if(tag == "tempUpdateEvery")
                    tempUpdateEvery = NumberParser::parse(n->innerText());
                else if(tag == "sdcard")
                    sdcard = parseBool(n->innerText());
                else if(tag == "softwarePower")
                    softwarePower = parseBool(n->innerText());
            }
        }
        generalNodes->release();
        
        NodeList *bedNodes = config->getElementsByTagName("heatedBed");
        if(bedNodes->length()>0) {
            Element *bed = (Element*)bedNodes->item(0);
            if(bed->hasAttribute("installed"))
                heatedBed = parseBool(bed->getAttribute("installed"));
            if(bed->hasAttribute("lastTemp"))
                lastBedTemp = parseBool(bed->getAttribute("lastBedTemp"));
            if(bed->hasAttribute("heatupPerSecond"))
                heatedBedHeatupPerSecond = parseBool(bed->getAttribute("heatupPerSecond"));
            if(bed->hasAttribute("cooldownPerSecond"))
                heatedBedCooldownPerSecond = parseBool(bed->getAttribute("cooldownPerSecond"));
            NodeList *bedNodes2 = bed->childNodes();
            for(int i=0;i<bedNodes2->length();i++) {
                Node *n = bedNodes2->item(i);
                tag = n->nodeName();
                if(tag=="#text") continue;
                if(tag == "temperatures") {
                    NodeList *l = n->childNodes();
                    for(int j=0;j<l->length();j++) {
                        Node *tNode = l->item(j);
                        if(tNode->nodeName()=="temperature")
                            bedTemperatures.push_back(NamedTemperature((Element*)tNode));
                    }
                }
            }
        }
        bedNodes->release();
        
        NodeList *connectionNodes = config->getElementsByTagName("serial");
        if(connectionNodes->length()>0) {
            NodeList *connectionNodes2 = connectionNodes->item(0)->childNodes();
            for(int i=0;i<connectionNodes2->length();i++) {
                Node *n = connectionNodes2->item(i);
                tag = n->nodeName();
                if(tag=="#text") continue;
                if(tag == "baudrate")
                    serialBaudrate = NumberParser::parse(n->innerText());
                else if(tag == "device")
                    serialPort = n->innerText();
                else if(tag == "pingPong")
                    serialPingPong = parseBool(n->innerText());
                else if(tag == "inputBufferSize")
                    serialInputBufferSize = NumberParser::parse(n->innerText());
                else if(tag == "protocol")
                    serialProtocol = NumberParser::parse(n->innerText());
                else if(tag == "okAfterResend")
                    serialOkAfterResend = parseBool(n->innerText());
            }
        }
        connectionNodes->release();
        
        NodeList *movementNodes = config->getElementsByTagName("movement");
        if(movementNodes->length()>0) {
            NodeList *movementNodes2 = movementNodes->item(0)->childNodes();
            for(int i=0;i<movementNodes2->length();i++) {
                Node *n = movementNodes2->item(i);
                tag = n->nodeName();
                if(tag=="#text") continue;
                if(tag == "xMin")
                    xMin = NumberParser::parseFloat(n->innerText());
                else if(tag == "xMax")
                    xMax = NumberParser::parseFloat(n->innerText());
                else if(tag == "yMin")
                    yMin = NumberParser::parseFloat(n->innerText());
                else if(tag == "yMax")
                    yMax = NumberParser::parseFloat(n->innerText());
                else if(tag == "zMin")
                    zMin = NumberParser::parseFloat(n->innerText());
                else if(tag == "zMax")
                    zMax = NumberParser::parseFloat(n->innerText());
                else if(tag == "xHome")
                    xHome = NumberParser::parseFloat(n->innerText());
                else if(tag == "yHome")
                    yHome = NumberParser::parseFloat(n->innerText());
                else if(tag == "zHome")
                    zHome = NumberParser::parseFloat(n->innerText());
                else if(tag == "xyJerk")
                    xyJerk = NumberParser::parseFloat(n->innerText());
                else if(tag == "zJerk")
                    zJerk = NumberParser::parseFloat(n->innerText());
                else if(tag == "timeMultiplier")
                    timeMultiplier = NumberParser::parseFloat(n->innerText());
                else if(tag == "movebuffer")
                    movebuffer = NumberParser::parseFloat(n->innerText());
                else if(tag == "xyPrintAcceleration")
                    printAcceleration = NumberParser::parseFloat(n->innerText());
                else if(tag == "xyTravelAcceleration")
                    travelAcceleration = NumberParser::parseFloat(n->innerText());
                else if(tag == "xySpeed") {
                    xySpeed = NumberParser::parseFloat(n->innerText());
                    Element *e = (Element*)n;
                    if(e->hasAttribute("max"))
                        maxXYSpeed = NumberParser::parseFloat(e->getAttribute("max"));
                }
                else if(tag == "zSpeed") {
                    zSpeed = NumberParser::parseFloat(n->innerText());
                    Element *e = (Element*)n;
                    if(e->hasAttribute("max"))
                        maxZSpeed = NumberParser::parseFloat(e->getAttribute("max"));
                } else if(tag == "endstops") {
                    Element *e = (Element*)n;
                    if(e->hasAttribute("x"))
                        hasXHome = parseBool(e->getAttribute("x"));
                    if(e->hasAttribute("y"))
                        hasYHome = parseBool(e->getAttribute("y"));
                    if(e->hasAttribute("z"))
                        hasZHome = parseBool(e->getAttribute("z"));
                    if(e->hasAttribute("all"))
                        hasXHome = parseBool(e->getAttribute("all"));
                    else
                        hasHomeAll = hasXHome & hasYHome & hasZHome;
                }
            }
        }
        movementNodes->release();
    } catch(Exception &ex) {
        cerr << "error:" << ex.message() << endl;
    }
    //root = config->getN
}

PrinterConfiguration::PrinterConfiguration() {
    config = new Document();
    setDefaults();
}

PrinterConfiguration::~PrinterConfiguration() {
    if(config!=NULL)
        config->release();
}

void PrinterConfiguration::createConfiguration(std::string name,std::string slug) {
    this->name = name;
    this->slug = slug;
    configFilename = gconfig->getPrinterConfigDir()+slug+".xml";
    if(shape==NULL)
        shape.reset(new ShapeConfiguration(getOrCreateElement("shape")));
    saveConfiguration();
    PrinterPtr p = gconfig->addPrinterFromConfig(configFilename);
    p->startThread();
}

ExtruderConfigurationPtr PrinterConfiguration::getExtruder(int num) {
    if(extruderList.size()==0) return ExtruderConfigurationPtr((ExtruderConfiguration*)NULL);
    if(num<0) num = 0;
    if(num>=extruderList.size()) num = (int)extruderList.size()-1;
    return extruderList[num];
}

bool PrinterConfiguration::parseBool(const std::string& text) {
    if(text == "false" || text == "0") return false;
    return true;
}

std::string PrinterConfiguration::encodeBool(bool b) {
    if(b) return "true";
    return "false";
}

void PrinterConfiguration::setDefaults() {
    xMin = yMin = zMin = 0;
    xMax = yMax = 200;
    zMax = 100;
    xHome = yHome = zHome = 0;
    hasXHome = hasYHome = hasZHome = hasHomeAll = true;
    xyJerk = 20;
    zJerk = 0.3;
    printAcceleration = 1500;
    travelAcceleration = 2500;
    serialPort = "Select";
    serialBaudrate = 115200;
    serialInputBufferSize = 127;
    serialOkAfterResend = true;
    serialPingPong = false;
    serialProtocol = 0;
    name="unknown";
    slug="unknown";
    printerVariant="cartesian";
    active = true;
    fan = false;
    heatedBed = false;
    lastBedTemp = 60;
    tempUpdateEvery = 1;
    heatedBedCooldownPerSecond = 0.2;
    heatedBedHeatupPerSecond = 0.4;
    timingChanged = false;
    xySpeed = 100;
    zSpeed = 2;
    maxXYSpeed = 200;
    maxZSpeed = 4;
    timeMultiplier = 1;
    movebuffer = 16;
}

Poco::XML::Element *PrinterConfiguration::getOrCreateElement(const std::string &path) {
    NodeList *rootList = config->getElementsByTagName("printer");
    Element *root = NULL;
    if(rootList->length()==0) {
        root = config->createElement("printer");
        config->appendChild(root);
        root->release();
    } else root = (Element*)rootList->item(0);
    rootList->release();
    if(path.length()==0) return root;
    vector <string> fields;
    boost::split(fields, path,boost::is_any_of( "." ) );
    Element *act = NULL;
    for(vector<string>::iterator it=fields.begin();it!=fields.end();++it) {
        act = root->getChildElement(*it);
        if(act == NULL) {
            act = config->createElement(*it);
            root->appendChild(act);
            act->release();
        }
        root = act;
    }
    return root;
}

void PrinterConfiguration::setNodeText(Element* node,string text) {
    NodeList *list = node->childNodes();
    if(list->length()==0) {
        AutoPtr<Text> textNode = config->createTextNode(text);
        node->appendChild(textNode);
        return;
    }
    list->item(0)->setNodeValue(text);
}

void PrinterConfiguration::saveConfiguration() {
    Element *e;
    
    setNodeText(getOrCreateElement("general.name"),name);
    setNodeText(getOrCreateElement("general.slug"),slug);
    setNodeText(getOrCreateElement("general.printerVariant"),printerVariant);
    setNodeText(getOrCreateElement("general.active"),encodeBool(active));
    setNodeText(getOrCreateElement("general.fan"),encodeBool(fan));
    setNodeText(getOrCreateElement("general.tempUpdateEvery"),NumberFormatter::format(tempUpdateEvery));
    setNodeText(getOrCreateElement("general.sdcard"),encodeBool(sdcard));
    setNodeText(getOrCreateElement("general.softwarePower"),encodeBool(softwarePower));
    
    
    setNodeText(getOrCreateElement("connection.serial.baudrate"),NumberFormatter::format(serialBaudrate));
    setNodeText(getOrCreateElement("connection.serial.device"),serialPort);
    setNodeText(getOrCreateElement("connection.serial.pingPong"),encodeBool(serialPingPong));
    setNodeText(getOrCreateElement("connection.serial.inputBufferSize"),NumberFormatter::format(serialInputBufferSize));
    setNodeText(getOrCreateElement("connection.serial.protocol"),NumberFormatter::format(serialProtocol));
    setNodeText(getOrCreateElement("connection.serial.okAfterResend"),encodeBool(serialOkAfterResend));
    
    setNodeText(getOrCreateElement("movement.xMin"),NumberFormatter::format(xMin));
    setNodeText(getOrCreateElement("movement.xMax"),NumberFormatter::format(xMax));
    setNodeText(getOrCreateElement("movement.xHome"),NumberFormatter::format(xHome));
    setNodeText(getOrCreateElement("movement.yMin"),NumberFormatter::format(yMin));
    setNodeText(getOrCreateElement("movement.yMax"),NumberFormatter::format(yMax));
    setNodeText(getOrCreateElement("movement.yHome"),NumberFormatter::format(yHome));
    setNodeText(getOrCreateElement("movement.zMin"),NumberFormatter::format(zMin));
    setNodeText(getOrCreateElement("movement.zMax"),NumberFormatter::format(zMax));
    setNodeText(getOrCreateElement("movement.zHome"),NumberFormatter::format(zHome));
    setNodeText(getOrCreateElement("movement.xyJerk"),NumberFormatter::format(xyJerk));
    setNodeText(getOrCreateElement("movement.zJerk"),NumberFormatter::format(zJerk));
    setNodeText(getOrCreateElement("movement.timeMultiplier"),NumberFormatter::format(timeMultiplier));
    setNodeText(getOrCreateElement("movement.movebuffer"),NumberFormatter::format(movebuffer));
    e = getOrCreateElement("movement.xySpeed");
    setNodeText(e,NumberFormatter::format(xySpeed));
    e->setAttribute("max", NumberFormatter::format(maxXYSpeed));
    e = getOrCreateElement("movement.zSpeed");
    setNodeText(e,NumberFormatter::format(zSpeed));
    e->setAttribute("max", NumberFormatter::format(maxZSpeed));
    setNodeText(getOrCreateElement("movement.xyPrintAcceleration"),NumberFormatter::format(printAcceleration));
    setNodeText(getOrCreateElement("movement.xyTravelAcceleration"),NumberFormatter::format(travelAcceleration));
    e = getOrCreateElement("movement.endstops");
    e->setAttribute("x",encodeBool(hasXHome));
    e->setAttribute("y",encodeBool(hasYHome));
    e->setAttribute("z",encodeBool(hasZHome));
    e->setAttribute("all",encodeBool(hasHomeAll));
    shape->save(config,getOrCreateElement(""));
    e = getOrCreateElement("extruders");
    for(vector<ExtruderConfigurationPtr>::iterator it = extruderList.begin();it!=extruderList.end();++it) {
        (*it)->save(config,e);
    }
    e = getOrCreateElement("heatedBed");
    e->setAttribute("installed",encodeBool(heatedBed));
    e->setAttribute("lastTemp", NumberFormatter::format(lastBedTemp));
    e->setAttribute("heatupPerSecond", NumberFormatter::format(heatedBedHeatupPerSecond));
    e->setAttribute("cooldownPerSecond", NumberFormatter::format(heatedBedCooldownPerSecond));
    Element *temps = getOrCreateElement("heatedBed.temperatures");
    Node *temp;
    do {
        temp = temps->firstChild();
        if(temp)
            temps->removeChild(temp);
    } while(temp);
    int i,n = (int)bedTemperatures.size();
    for(i=0;i<n;i++) {
        bedTemperatures[i].node = NULL;
        bedTemperatures[i].save(config,temps);
    }

    DOMWriter writer;
    ofstream out(configFilename.c_str());
    writer.setNewLine("\n");
    writer.setOptions(XMLWriter::PRETTY_PRINT);
    writer.writeNode(out, config);
    changed = false;
    if(timingChanged) {
        PrinterPtr printer = gconfig->findPrinterSlug(slug);
        if(printer.get()!=NULL)
            printer->getModelManager()->recomputeInfoLazy();
        timingChanged = false;
    }
}

std::string PrinterConfiguration::getScript(std::string name) {
    Element *scripts = getOrCreateElement("scripts");
    NodeList *list = scripts->childNodes();
    for(int i=0;i<list->length();i++) {
        Node *t = list->item(i);
        if(t->nodeType() != Node::ELEMENT_NODE) continue;
        Element *script = (Element*)t;
        if(script->hasAttribute("name") && script->getAttribute("name") == name) {
            return script->innerText();
        }
    }
    return ""; // Default answer
}

void PrinterConfiguration::setScript(std::string name,const std::string &text) {
    Element *scripts = getOrCreateElement("scripts");
    NodeList *list = scripts->childNodes();
    for(int i=0;i<list->length();i++) {
        Node *t = list->item(i);
        if(t->nodeType() != Node::ELEMENT_NODE) continue;
        Element *script = (Element*)t;
        if(script->hasAttribute("name") && script->getAttribute("name") == name) {
            setNodeText(script,text);
            return;
        }
    }
    // new element
    Element *script = config->createElement("script");
    script->setAttribute("name",name);
    setNodeText(script, text);
    scripts->appendChild(script);
}

void PrinterConfiguration::fromJSON(json_spirit::mObject &obj) {
    try {
        mObject &general = obj["general"].get_obj();
        mObject &connection = obj["connection"].get_obj();
        mObject &serial = connection["serial"].get_obj();
        mObject &movement = obj["movement"].get_obj();
        mObject &shape = obj["shape"].get_obj();
        mArray &extruders = obj["extruders"].get_array();
        mObject &hbed = obj["heatedBed"].get_obj();
        
        name = general["name"].get_str();
        if(slug.length()==0)
            slug = general["slug"].get_str();
        printerVariant = general["printerVariant"].get_str();
        active = general["active"].get_bool();
        //heatedBed = general["heatedBed"].get_bool();
        fan = general["fan"].get_bool();
        tempUpdateEvery = general["tempUpdateEvery"].get_int();
        sdcard = general["sdcard"].get_bool();
        softwarePower = general["softwarePower"].get_bool();
        
        serialBaudrate = (serial["baudrate"].type()==json_spirit::int_type ? serial["baudrate"].get_int() : NumberParser::parse(serial["baudrate"].get_str()));
        serialPort = serial["device"].get_str();
        serialPingPong = serial["pingPong"].get_bool();
        serialProtocol = (serial["protocol"].type()==json_spirit::int_type ? serial["protocol"].get_int() :  NumberParser::parse(serial["protocol"].get_str()));
        serialOkAfterResend = serial["okAfterResend"].get_bool();
        serialInputBufferSize = serial["inputBufferSize"].get_int();

        heatedBed = hbed["installed"].get_bool();
        lastBedTemp = hbed["lastTemp"].get_int();
        heatedBedHeatupPerSecond = hbed["heatupPerSecond"].get_real();
        heatedBedCooldownPerSecond = hbed["cooldownPerSecond"].get_real();
        mArray &tempList = hbed["temperatures"].get_array();
        int n = (int)tempList.size();
        bedTemperatures.clear();
        for(int i=0;i<n;i++) {
            bedTemperatures.push_back(NamedTemperature(tempList[i].get_obj()));
        }
        double tmp;
        xMin = movement["xMin"].get_real();
        xMax = movement["xMax"].get_real();
        xHome = movement["xHome"].get_real();
        yMin = movement["yMin"].get_real();
        yMax = movement["yMax"].get_real();
        yHome = movement["yHome"].get_real();
        zMin = movement["zMin"].get_real();
        zMax = movement["zMax"].get_real();
        zHome = movement["zHome"].get_real();
        tmp = movement["xySpeed"].get_real();
        timingChanged |= tmp != xySpeed;
        xySpeed = tmp;
        tmp = movement["zSpeed"].get_real();
        timingChanged |= tmp != zSpeed;
        zSpeed = tmp;
        tmp = movement["maxXYSpeed"].get_real();
        timingChanged |= tmp != maxXYSpeed;
        maxXYSpeed = tmp;
        tmp = movement["maxZSpeed"].get_real();
        timingChanged |= tmp != maxZSpeed;
        maxZSpeed = tmp;
        tmp = movement["timeMultiplier"].get_real();
        timingChanged |= tmp != timeMultiplier;
        timeMultiplier = tmp;
        int itmp = movement["movebuffer"].get_int();
        timingChanged |= itmp != movebuffer;
        movebuffer = itmp;
        tmp = movement["xyJerk"].get_real();
        timingChanged |= tmp != xyJerk;
        xyJerk = tmp;
        tmp = movement["zJerk"].get_real();
        timingChanged |= tmp != zJerk;
        zJerk = tmp;
        tmp = movement["xyPrintAcceleration"].get_real();
        timingChanged |= tmp != printAcceleration;
        printAcceleration = tmp;
        tmp = movement["xyTravelAcceleration"].get_real();
        timingChanged |= tmp != travelAcceleration;
        travelAcceleration = tmp;
        hasXHome = movement["xEndstop"].get_bool();
        hasYHome = movement["yEndstop"].get_bool();
        hasZHome = movement["zEndstop"].get_bool();
        hasHomeAll = movement["allEndstops"].get_bool();

        this->shape->fromJSON(shape);
        
        Element *ex = getOrCreateElement("extruders");
        while(ex->firstChild()) {
            ex->removeChild(ex->firstChild());
        }
        extruderList.clear();
        for(int i=0;i<extruders.size();i++) {
            extruderList.push_back(ExtruderConfigurationPtr(new ExtruderConfiguration(extruders[i].get_obj(),i)));
        }
        saveConfiguration();
    } catch(Exception &e) {}
}

void PrinterConfiguration::fillJSON(json_spirit::mObject &obj) {
    obj["general"] = mObject();
    mObject &general = obj["general"].get_obj();
    general["name"] = name;
    general["slug"] = slug;
    general["printerVariant"] = printerVariant;
    general["active"] = active;
    general["heatedBed"] = heatedBed;
    general["fan"] = fan;
    general["tempUpdateEvery"] = tempUpdateEvery;
    general["sdcard"] = sdcard;
    general["softwarePower"] = softwarePower;
    
    obj["heatedBed"] = mObject();
    mObject &bed = obj["heatedBed"].get_obj();
    bed["installed"] = heatedBed;
    bed["lastTemp"] = lastBedTemp;
    bed["heatupPerSecond"] = heatedBedHeatupPerSecond;
    bed["cooldownPerSecond"] = heatedBedCooldownPerSecond;
    bed["temperatures"] = mArray();
    for(vector<NamedTemperature>::iterator it = bedTemperatures.begin();it!=bedTemperatures.end();++it) {
        mObject nt;
        it->fillJSON(nt);
        bed["temperatures"].get_array().push_back(nt);
    }
    
    obj["connection"] = mObject();
    mObject &connection = obj["connection"].get_obj();
    connection["serial"] = mObject();
    mObject &serial = connection["serial"].get_obj();
    serial["baudrate"] = serialBaudrate;
    serial["device"] = serialPort;
    serial["pingPong"] = serialPingPong;
    serial["protocol"] = serialProtocol;
    serial["okAfterResend"] = serialOkAfterResend;
    serial["inputBufferSize"] = serialInputBufferSize;
    
    obj["movement"] = mObject();
    mObject &movement = obj["movement"].get_obj();
    movement["xMin"] = xMin;
    movement["xMax"] = xMax;
    movement["xHome"] = xHome;
    movement["yMin"] = yMin;
    movement["yMax"] = yMax;
    movement["yHome"] = yHome;
    movement["zMin"] = zMin;
    movement["zMax"] = zMax;
    movement["zHome"] = zHome;
    movement["xySpeed"] = xySpeed;
    movement["zSpeed"] = zSpeed;
    movement["maxXYSpeed"] = maxXYSpeed;
    movement["maxZSpeed"] = maxZSpeed;
    movement["timeMultiplier"] = timeMultiplier;
    movement["movebuffer"] = movebuffer;
    movement["xyJerk"] = xyJerk;
    movement["zJerk"] = zJerk;
    movement["xyPrintAcceleration"] = printAcceleration;
    movement["xyTravelAcceleration"] = travelAcceleration;
    movement["xEndstop"] = hasXHome;
    movement["yEndstop"] = hasYHome;
    movement["zEndstop"] = hasZHome;
    movement["allEndstops"] = hasHomeAll;
    
    obj["shape"] = mObject();
    shape->fillJSON(obj["shape"].get_obj());
    
    obj["extruders"] = mArray();
    for(vector<ExtruderConfigurationPtr>::iterator it = extruderList.begin();it!=extruderList.end();++it) {
        mObject ex;
        (*it)->fillJSON(ex);
        obj["extruders"].get_array().push_back(ex);
    }
}

void PrinterConfiguration::remove() {
    File storageDir(gconfig->getStorageDirectory()+"printer"+Poco::Path::separator()+slug);
    if(storageDir.exists())
        storageDir.remove(true);
    File confFile(configFilename);
    if(confFile.exists())
        confFile.remove();
}

