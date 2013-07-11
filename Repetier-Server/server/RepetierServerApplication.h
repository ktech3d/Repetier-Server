//
//  RepetierServerApplication.h
//  Repetier-Server
//
//  Created by Roland Littwin on 09.07.13.
//
//

#ifndef __Repetier_Server__RepetierServerApplication__
#define __Repetier_Server__RepetierServerApplication__

#include <iostream>
#include <Poco/Util/ServerApplication.h>
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"

class RepetierServerApplication : public Poco::Util::ServerApplication {
public:
    RepetierServerApplication();
    ~RepetierServerApplication();
    std::string configurationFile;
    int port;
protected:
    void initialize(Application& self);
    void uninitialize();
    void defineOptions(Poco::Util::OptionSet& options);
    void handleHelp(const std::string& name,
                    const std::string& value);
    void handleConfig(const std::string& name, const std::string& value);
    void handlePort(const std::string& name, const std::string& value);
    int main(const std::vector<std::string>& args);
    
private:
    bool _helpRequested;
    struct mg_context *ctx;
};
#endif /* defined(__Repetier_Server__RepetierServerApplication__) */
