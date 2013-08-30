//
//  FileUtils.h
//  Repetier-Server
//
//  Created by Roland Littwin on 30.08.13.
//
//

#ifndef __Repetier_Server__FileUtils__
#define __Repetier_Server__FileUtils__

#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <vector>

namespace RepetierServer {

    struct FileInfo {
        std::string filename;
        std::time_t time;
        
        FileInfo(std::string name,std::time_t _time) {
            time = _time;
            filename = name;
        }
    };
    void filesInDirectory(std::string directory,std::vector<FileInfo> &list);
    void keepNewestFilesInDirectory(std::string directory,int keep);
};
#endif /* defined(__Repetier_Server__FileUtils__) */
