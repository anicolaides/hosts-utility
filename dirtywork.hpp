// Author: Alexandros Nicolaides
// Purpose: Elevate application privileges(UNIX only), detect host OS, determine app dependencies' file paths
// File: dirtywork.hpp 
// Dependencies: None

#ifndef DIRTYWORK_HPP
#define DIRTYWORK_HPP

#if defined(__linux__) || defined(__APPLE__)
#include <limits.h> /* PATH_MAX defined there */
#include <unistd.h> /* getuid(), getcwd() */
#include <cstdlib> /* exit, EXIT_FAILURE */
#elif _WIN32
#undef UNICODE
#include <windows.h>
#endif

#include <string>
#include <iostream>
using std::string;
using std::cout;
using std::endl;

class Dirtywork {
public:
    Dirtywork(char ** argv) :argv(argv) {
        #if defined(__linux__) || defined(__APPLE__)
            hostsPath = UNIX_PATH;
            elevateUnix();
            curDir = static_cast<string>(getcwd(buff, PATH_MAX + 1));
        #elif _WIN32
            // determines system directory for Microsoft Windows
            if (GetSystemDirectory(infoBuf, INFO_BUFFER_SIZE)) 
                hostsPath = static_cast<string>(infoBuf) + "\\drivers\\etc\\hosts";

            // determines current directory of the binary(.exe)
            if (GetCurrentDirectory(_MAX_PATH, infoBuf))
                curDir = static_cast<string>(infoBuf);
        #else
            cout << "\nYour OS is not currently supported.The program is about to terminate." << endl;
            exit(EXIT_FAILURE);
        #endif
    }
    #if defined(__linux__) || defined(__APPLE__)
        bool isRoot() const { return (!getuid()); }
        void elevateUnix() const {
            // attempts to run bin file with sudo
            if (!isRoot()) { 
                system(("sudo " + static_cast<string>(argv[0])).c_str()); 
                std::exit(EXIT_SUCCESS); 
            }
            else 
                cout << "\nSuccess, the program has root access!" << endl;
        }
    #endif
    string getCurDir() const { return curDir; }
    string getHostsPath() const { return hostsPath; }
    
private:
    const string UNIX_PATH = "/etc/hosts";
    string hostsPath, curDir;
    #if defined(__linux__) || defined(__APPLE__)
        char buff[PATH_MAX + 1];
    #elif _WIN32
        const static unsigned short INFO_BUFFER_SIZE = 32767;
        char infoBuf[INFO_BUFFER_SIZE];
    #endif
    char ** argv;
};
#endif
