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

#ifndef __Repetier_Server__RepetierServerApplication__
#define __Repetier_Server__RepetierServerApplication__

#include <iostream>
#include <Poco/Util/ServerApplication.h>
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"

namespace repetier {
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
}
#endif /* defined(__Repetier_Server__RepetierServerApplication__) */
