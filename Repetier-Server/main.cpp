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
 

To reister as windows service call with /registerService
 Unregister windows service with /unregisterService
 Combine with userfriendly name /displayName Name
 
 Once registered the service can be started or stopped using the
 Services MMC applet, or using the NET START and NET STOP
 commands on the command line.
 
 On unix use --daemon to start as daemon.
 
 Debug parameter:
 --config=/Users/littwin/Documents/Projekte/Repetier-Server/Repetier-Server/repetier-server.xml
*/


#include <iostream>
#include <fstream>
#include "printer.h"
#include "global_config.h"
#include "WebserverAPI.h"
#include "RLog.h"
#if defined(__APPLE__) || defined(__linux)
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#endif
#include "RepetierServerApplication.h"

using namespace std;

#if defined(__APPLE__) || defined(__linux)

void Signal_Handler(int sig) /* signal handler function */
{
	switch(sig){
case SIGHUP:
	break;
case SIGTERM:
            //mg_stop(ctx);
	gconfig->stopPrinterThreads();
	exit(0); // Terminate server
	break;		
	}	
}
#endif

int main(int argc, char * argv[])
{
    RepetierServerApplication app;
    return app.run(argc, argv);
}

