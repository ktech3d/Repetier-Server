//
//  FileUtils.cpp
//  Repetier-Server
//
//  Created by Roland Littwin on 30.08.13.
//
//

#include "FileUtils.h"
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;

namespace RepetierServer {
void filesInDirectory(std::string directory,vector<FileInfo> &list) {
    path fs_path(directory);
    if (!exists(fs_path) || !is_directory(fs_path))
    {
        return ;
    }
    
    directory_iterator end_iter;
    for (directory_iterator dir_itr(fs_path);
         dir_itr != end_iter;
         ++dir_itr )
    {
        try
        {
            if ( is_regular_file( dir_itr->status() ) )
            {
                list.push_back(FileInfo(dir_itr->path().string(),boost::filesystem::last_write_time(dir_itr->path())));
            }
        }
        catch ( const std::exception & ex )
        {
        }
    }

}
    void keepNewestFilesInDirectory(std::string directory,int keep) {
        vector<FileInfo> list;
        filesInDirectory(directory, list);
        if(list.size()<=keep) return;
        while(list.size()>keep) {
            vector<FileInfo>::iterator it(list.begin()),ie(list.end()),oldestpos;
            FileInfo *oldest = &list[0];
            oldestpos = it;
            for(++it;it!=ie;++it) {
                if(oldest->time > (*it).time) {
                    oldest = &(*it);
                    oldestpos = it;
                }
            }
            remove(oldest->filename);
            list.erase(oldestpos);
        }
    }
}