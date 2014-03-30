/*
Copyright 2012 Roland Littwin (repetier) repetierdev@gmail.com

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


#include <iostream>
#include <fstream>
#include "mongoose.h"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
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

namespace po = boost::program_options;
using namespace std;

struct mg_context *ctx;

#if defined(_WIN32) && !defined(__SYMBIAN32__)
#include <windows.h>

// Internal name of the service 
#define SERVICE_NAME             "Repetier-Server" 
// Displayed name of the service 
#define SERVICE_DISPLAY_NAME     "Repetier-Server Service" 
// Service start options. 
#define SERVICE_START_TYPE       SERVICE_DEMAND_START 
// List of service dependencies - "dep1\0dep2\0\0" 
#define SERVICE_DEPENDENCIES     ""  
// The name of the account under which the service should run 
#define SERVICE_ACCOUNT          "NT AUTHORITY\\LocalService" 
// The password to the service account name 
#define SERVICE_PASSWORD         NULL 
#endif

static void *callback(enum mg_event event,
struct mg_connection *conn) {
	const struct mg_request_info *ri = mg_get_request_info(conn);

	if (event == MG_NEW_REQUEST) {
	
		cerr << ri->uri << "\n";
		if(strncmp(ri->uri,"/image/",7)==0) return repetier::HandleImage(conn);
		if(strncmp(ri->uri,"/tools/",7)==0) return repetier::HandleTools(conn);
		if(strncmp(ri->uri,"/printer/",9)!=0) return repetier::HandlePagerequest(conn);
		repetier::HandleWebrequest(conn);
		
		// Mark as processed
		return (void*)"";
	} else {
		return NULL; //repetier::HandlePagerequest(conn);;
	}
}
#if defined(__APPLE__) || defined(__linux)

void Signal_Handler(int sig) /* signal handler function */
{
	switch(sig){
case SIGHUP:
	break;
case SIGTERM:
	mg_stop(ctx);
	gconfig->stopPrinterThreads();
	exit(0); // Terminate server
	break;		
	}	
}
#endif

int main(int argc, const char * argv[])
{
	// Declare the supported options.
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "produce help message")
		("config,c", po::value<string>(), "Configuration file")
		("daemon","Start as daemon")
		("pidfile,p",po::value<string>(),"PID file")
		("userid,u",po::value<string>(),"User id for daemon")
#if defined(_WIN32) && !defined(__SYMBIAN32__)
		("install","Install printer service")
		("remove","Remove printer service")
#endif
		;
	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
	} catch(std::exception &ex) {
        cerr << "Repetier-Server version " << REPETIER_SERVER_VERSION << endl;
		cerr << "error: Error parsing command line: " << ex.what() << endl;
	}
	po::notify(vm);

	if (vm.count("help")) {
        cout << "Repetier-Server version " << REPETIER_SERVER_VERSION << endl;
		cout << desc << "\n";
		return 1;
	}
	string confFile;
	if(vm.count("config")) {
		confFile = vm["config"].as<string>();
	}
	if(confFile.length() == 0) confFile = string("/etc/repetier-server.conf");
	boost::filesystem::path cf(confFile);
	if(!boost::filesystem::exists(cf) || !boost::filesystem::is_regular_file(cf)) {
        cerr << "Repetier-Server version " << REPETIER_SERVER_VERSION << endl;
		cerr << "Configuration file not found at " << confFile << endl;
		cerr << "Please use config option with correct path" << endl;
		cerr << desc << endl;
		return 2;
	}
	gconfig = new GlobalConfig(confFile); // Read global configuration

#if defined(_WIN32) && !defined(__SYMBIAN32__)
	if(vm.count("install")) {
		char szPath[MAX_PATH];
		SC_HANDLE schSCManager = NULL;
		SC_HANDLE schService = NULL;

		if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)) == 0) {
			cerr << "GetModuleFileName failed w/err " <<  GetLastError() << endl;
			goto Cleanup;
		}
		// Open the local default service control manager database
		schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
		if (schSCManager == NULL)
		{
			cerr << "OpenSCManager failed w/err " << GetLastError() << endl;
			goto Cleanup;
		}

		// Install the service into SCM by calling CreateService
		schService = CreateService(
			schSCManager,                   // SCManager database
			SERVICE_NAME,                 // Name of service
			SERVICE_DISPLAY_NAME,                 // Name to display
			SERVICE_QUERY_STATUS,           // Desired access
			SERVICE_WIN32_OWN_PROCESS,      // Service type
			SERVICE_START_TYPE,                    // Service start type
			SERVICE_ERROR_NORMAL,           // Error control type
			szPath,                         // Service's binary
			NULL,                           // No load ordering group
			NULL,                           // No tag identifier
			SERVICE_DEPENDENCIES,                // Dependencies
			SERVICE_ACCOUNT,                     // Service running account
			SERVICE_PASSWORD                     // Password of the account
			);
		if (schService == NULL)
		{
			cerr << "CreateService failed w/err " <<  GetLastError() << endl;
			goto Cleanup;
		}

		cout << "Repetier-Server service is installed." << endl;
Cleanup:
		// Centralized cleanup for all allocated resources.
		if (schSCManager)
		{
			CloseServiceHandle(schSCManager);
			schSCManager = NULL;
		}
		if (schService)
		{
			CloseServiceHandle(schService);
			schService = NULL;
		}
		return 0;
	}
	if(vm.count("remove")) {
		SC_HANDLE schSCManager = NULL;
		SC_HANDLE schService = NULL;
		SERVICE_STATUS ssSvcStatus = {};

		// Open the local default service control manager database
		schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
		if (schSCManager == NULL)
		{
			cout << "OpenSCManager failed w/err " << GetLastError() << endl;;
			goto Cleanup2;
		}

		// Open the service with delete, stop, and query status permissions
		schService = OpenService(schSCManager, SERVICE_NAME, SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE);
		if (schService == NULL)
		{
			cerr << "OpenService failed w/err " << GetLastError() << endl;
			goto Cleanup2;
		}

		// Try to stop the service
		if (ControlService(schService, SERVICE_CONTROL_STOP, &ssSvcStatus))
		{
			cout << "Stopping Repetier-Server service." << endl;
			Sleep(1000);

			while (QueryServiceStatus(schService, &ssSvcStatus))
			{
				if (ssSvcStatus.dwCurrentState == SERVICE_STOP_PENDING)
				{
					cout << ".";
					Sleep(1000);
				}
				else break;
			}

			if (ssSvcStatus.dwCurrentState == SERVICE_STOPPED)
			{
				cerr << "Repetier-Server service is stopped." << endl;
			}
			else
			{
				cerr << "Repetier-Server service failed to stop." << endl;
			}
		}

		// Now remove the service by calling DeleteService.
		if (!DeleteService(schService))
			cerr << "DeleteService failed w/err " << GetLastError() << endl;
		else
			cout << "Repetier-Server service is removed." << endl;

Cleanup2:
		// Centralized cleanup for all allocated resources.
		if (schSCManager)
		{
			CloseServiceHandle(schSCManager);
			schSCManager = NULL;
		}
		if (schService)
		{
			CloseServiceHandle(schService);
			schService = NULL;
		}
		return 0;
	}

#endif
	if(vm.count("daemon")) {
		gconfig->daemon = true;
#ifdef DEBUG
		cout << "Running as daemon" << endl;
#endif
#if defined(__APPLE__) || defined(__linux)
		pid_t pid, sid;

		//Fork the Parent Process
		pid = fork();

		if (pid < 0) { exit(EXIT_FAILURE); }

		//We got a good pid, Close the Parent Process
		if (pid > 0) { exit(EXIT_SUCCESS); }

		//Change File Mask
		umask(0);

		//Create a new Signature Id for our child
		sid = setsid();
		if (sid < 0) { exit(EXIT_FAILURE); }

		//Change Directory
		//If we cant find the directory we exit with failure.
		if ((chdir("/")) < 0) { exit(EXIT_FAILURE); }

		//Close Standard File Descriptors
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		signal(SIGHUP,Signal_Handler); /* hangup signal */
		signal(SIGTERM,Signal_Handler); /* software termination signal from kill */
		if(vm.count("pidfile")) {
			try {
				pid = getpid();
				ofstream out(vm["pidfile"].as<string>().c_str());
				out << pid;
			} catch(std::exception &ex) {

			}
		}
		if(vm.count("userid")) {
			string suid = vm["userid"].as<string>();
			int uid = 0;
			sscanf(suid.c_str(),"%i",&uid);
			RLog::log("Switching permissions to user id @",uid);
			setuid(uid);
		}
#endif
	}

	gconfig->readPrinterConfigs();
	gconfig->startPrinterThreads();
	const char *options[] = {"document_root", gconfig->getWebsiteRoot().c_str(),"listening_ports", gconfig->getPorts().c_str(), NULL};

	ctx = mg_start(&callback, NULL, options);
	//getchar();  // Wait until user hits "enter"
	if(gconfig->daemon) {
		while(1) {
			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		}
	}
	while(true) {
		if(getchar()=='x') break;
	}
	mg_stop(ctx);
	cout << "Closing server" << endl;
	gconfig->stopPrinterThreads();
	return 0;
}

