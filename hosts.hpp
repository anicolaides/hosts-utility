// Author: Alexandros Nicolaides
// Purpose: Class blueprint for hosts.cpp
// File: hosts.hpp 
// Dependencies: hosts.cpp

#ifndef HOSTS_HPP
#define HOSTS_HPP

#include <iostream>
#include <algorithm> /* transform() */
#include <cstdio> /* rename(), remove() */
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <stdexcept>
#include <cstdlib> /* abort, NULL */
using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::fstream;
using std::ios;
using std::string;
using std::stringstream;

typedef std::unordered_set<string> stringset;

class Hosts{
public:
    Hosts(const string &);
    void populateStringset(); /* throws runtime_error */
    void blockDomain(string &); /* throws runtime_error */
    bool appendFile(const string &); /* throws runtime_error */
    inline bool renameFile(const string &, const string &) const;
    bool removeDomain(string &, const bool &); /* throws runtime_error */
    void printDomains() const;
    bool backupHostsFile() const; /* throws runtime_error */
    bool clearHosts(); /* rethrows runtime_error from removeDomain() */
    bool overwriteHosts(const string &);
    string getBackupFileName() const;
private:
    void parseDomain(string &) const; 
    stringset hfContents;
    const static string BACKUP_FILE;
    const static string NON_ROUTABLE;
    const static string UNIX_PATH;
    const string SYS_PATH;
};
#endif