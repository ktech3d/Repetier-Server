//
//  PrinterConfigiration.h
//  Repetier-Server
//
//  Created by Roland Littwin on 27.07.13.
//
//

#ifndef __Repetier_Server__PrinterConfigiration__
#define __Repetier_Server__PrinterConfigiration__

#include <iostream>
#include <vector.h>
#include <map.h>
#include "Poco/DOM/Document.h"
#include "Poco/DOM/Element.h"
#include "Poco/DOM/AutoPtr.h"
#include <boost/shared_ptr.hpp>
#include "json_spirit.h"

class PrimitiveShapeConfiguration {
public:
    Poco::XML::Element *node;
    std::string color;
    
    virtual void save(Poco::XML::Document *doc, Poco::XML::Element *parent) = 0;
    virtual void fillJSON(json_spirit::mObject &obj) = 0;
};
class PrimitiveShapeRectangle: public PrimitiveShapeConfiguration {
    void setDefaults();
public:
    double xMin,yMin,xMax,yMax;
    PrimitiveShapeRectangle(Poco::XML::Element *node);
    PrimitiveShapeRectangle(json_spirit::mObject &obj);
    PrimitiveShapeRectangle();
    virtual void save(Poco::XML::Document *doc, Poco::XML::Element *parent);
    virtual void fillJSON(json_spirit::mObject &obj);
};
class PrimitiveShapeCircle:public PrimitiveShapeConfiguration {
    void setDefaults();
public:
    double radius;
    double x,y;
    PrimitiveShapeCircle(Poco::XML::Element *node);
    PrimitiveShapeCircle(json_spirit::mObject &obj);
    PrimitiveShapeCircle();
    virtual void save(Poco::XML::Document *doc, Poco::XML::Element *parent);
    virtual void fillJSON(json_spirit::mObject &obj);
};
class ShapeConfiguration {
    Poco::XML::Element *node;
public:
    std::string gridColor;
    double gridSpacing;
    boost::shared_ptr<PrimitiveShapeConfiguration> basicShape;
    std::vector<PrimitiveShapeConfiguration*> marker;
    
    ShapeConfiguration();
    ShapeConfiguration(Poco::XML::Element *node);
    ~ShapeConfiguration();
    void fillJSON(json_spirit::mObject &obj);
    void fromJSON(json_spirit::mObject &obj);
};
class NamedTemperature {
public:
    std::string name;
    int temperature;
    Poco::XML::Element *node;
    
    NamedTemperature();
    NamedTemperature(Poco::XML::Element *node);
    NamedTemperature(json_spirit::mObject &obj);
    void save(Poco::XML::Document *doc, Poco::XML::Element *parent);
    void fillJSON(json_spirit::mObject &obj);
};
class ExtruderConfiguration {
    Poco::XML::Element *getOrCreateElement(Poco::XML::Document *doc, const std::string &path);
public:
    int num;
    double eJerk;
    double maxSpeed;
    double retractSpeed,retractDistance;
    double extrudeSpeed,extrudeDistance;
    std::vector<NamedTemperature> temperatures;
    Poco::XML::Element *node;
    
    ExtruderConfiguration(Poco::XML::Element *node);
    ExtruderConfiguration(json_spirit::mObject &obj,int pos);
    void save(Poco::XML::Document *doc, Poco::XML::Element *parent);
    void fillJSON(json_spirit::mObject &obj);
};
typedef boost::shared_ptr<ExtruderConfiguration> ExtruderConfigurationPtr;

class PrinterConfiguration {
    
    std::string configFilename;
    Poco::XML::Document *config;
    bool changed;
    void setDefaults();
public:
    PrinterConfiguration(std::string filename);
    PrinterConfiguration();
    ~PrinterConfiguration();
    void saveConfiguration();
    static bool parseBool(const std::string &text);
    static std::string encodeBool(bool b);
    Poco::XML::Element *getOrCreateElement(const std::string &path);
    void setNodeText(Poco::XML::Element* node,std::string text);
    void fillJSON(json_spirit::mObject &obj);
    void fromJSON(json_spirit::mObject &obj);
    ExtruderConfigurationPtr getExtruder(int num);
    inline int getExtruderCount() {return (int)extruderList.size();}
    std::map<std::string,std::string> scriptList;
    std::vector<ExtruderConfigurationPtr> extruderList;
    boost::shared_ptr<ShapeConfiguration> shape;
    double xMin,yMin,zMin,xMax,yMax,zMax;
    double xHome,yHome,zHome;
    double xyJerk,zJerk,printAcceleration,travelAcceleration;
    double xySpeed,zSpeed;
    bool hasXHome,hasYHome,hasZHome,hasHomeAll;
    std::string serialPort;
    int serialBaudrate;
    bool serialPingPong;
    int serialInputBufferSize;
    int serialProtocol;
    bool serialOkAfterResend;
    
    std::string name;
    std::string slug;
    std::string printerVariant;
    bool active;
    bool fan;
    bool heatedBed;
    int tempUpdateEvery;
    bool sdcard;
    bool softwarePower;
    std::vector<NamedTemperature> bedTemperatures;
};

#endif /* defined(__Repetier_Server__PrinterConfigiration__) */
