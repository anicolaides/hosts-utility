// Author: Alexandros Nicolaides
// Purpose: Functions to work with the Hosts file.
// File: hosts.cpp
// Dependencies: hosts.hpp

#include "hosts.hpp"

const string Hosts::NON_ROUTABLE = "0.0.0.0";
const string Hosts::UNIX_PATH = "/etc/hosts";
const string Hosts::BACKUP_FILE = "hosts.bak";

Hosts::Hosts(const string & filePath) :SYS_PATH(filePath) { 
    try {
        populateStringset();
    }
    catch (const std::runtime_error &error) {
        cerr << error.what() << endl;
        abort();
    }
}

void Hosts::populateStringset() {
    fstream hostsFile(SYS_PATH, ios::in);
    // populates hfContents with domain_name names being redirected to NON-ROUTABLE
    if (hostsFile) {
        string line;
        while (getline(hostsFile, line)) {
            if (line.substr(0, 7) == NON_ROUTABLE)
                hfContents.insert(line.substr(line.find_first_of(" \t") + 1));
        }
        hostsFile.close();
    }
    else 
        throw std::runtime_error("\nERROR: Cannot open \'" + SYS_PATH + '\'');
}

void Hosts::parseDomain(string & domain_name) const {
    stringstream ss;
    string dummy;
    // converts domain_name name to lowercase and removes "www." including anything preciding it
    transform(domain_name.begin(), domain_name.end(), domain_name.begin(), ::tolower);
    if (ss << domain_name) {
        domain_name.clear();
        ss >> domain_name >> dummy;
    }
    size_t found_www = domain_name.find("www.");
    if(found_www != std::string::npos && found_www < domain_name.length()) domain_name.erase(found_www, 4);
}

void Hosts::blockDomain(string & domain_name) {
    parseDomain(domain_name);
    fstream hostsFile(SYS_PATH, ios::in | ios::app | ios::out);
    if (hostsFile) {
        string line;
        stringset::const_iterator hasDomain(hfContents.find(domain_name)),
            hasWeb(hfContents.find("www." + domain_name));
        hostsFile.clear();
        // if domain_name name about to be blocked is not already blocked
        if (hasDomain != hfContents.end() && hasWeb != hfContents.end())
            cerr << "\nERROR: " << domain_name << " is already being blocked." << endl;
        else {
            if (hasDomain == hfContents.end()) {
                cout << NON_ROUTABLE << ' ' << domain_name << endl;
                hostsFile << NON_ROUTABLE << ' ' << domain_name << endl;
                hfContents.insert(domain_name);
            }
            if (hasWeb == hfContents.end()) {
                cout << NON_ROUTABLE << ' ' << "www." << domain_name << endl;
                hostsFile << NON_ROUTABLE << ' ' << "www." << domain_name << endl;
                hfContents.insert("www." + domain_name);
            }
        }
        hostsFile.close();
    }
    else throw std::runtime_error("\nERROR: Cannot open hosts file.\n");
}

bool Hosts::appendFile(const string & filePath) {
    bool opSucceded = false;
    string line;
    fstream fin(filePath, ios::in);
    if (fin) {
        while (getline(fin, line)) blockDomain(line);
        fin.close();
        opSucceded = true;
    } 
    else 
        throw std::runtime_error("\nERROR: Could not open the file specified.\n");
    return opSucceded;
}

bool Hosts::renameFile(const string & oldFilePath, const string & newFilePath) const {
    return (!rename(oldFilePath.c_str(), newFilePath.c_str()));
}

bool Hosts::removeDomain(string & domain_name, const bool & in_loop) {
    // removes "www." from string, for the rest of the function to succeed
    parseDomain(domain_name); 
    string line;
    bool succeded = false;
    fstream hostsFile(SYS_PATH, ios::in);
    fstream tempOut(SYS_PATH + ".tmp", std::ios::trunc | std::ios::out);

    if (hostsFile && tempOut) {
        // makes a copy of hosts file without any domain names
        while (getline(hostsFile, line)) {
            std::size_t found = line.find_first_of(" \t") + 1;
            if (line.substr(found) != domain_name && line.substr(found) != "www." + domain_name)
                tempOut << line << endl;
        }
        hostsFile.close();
        tempOut.close();

        // makes backup of hosts file, removes and replaces the hosts file with an updated one
        if (!std::remove(SYS_PATH.c_str()) && renameFile(SYS_PATH + ".tmp", SYS_PATH)) {
            if (!hfContents.empty()) {
                if (!in_loop) {
                    hfContents.erase(domain_name);
                    hfContents.erase("www." + domain_name);
                }
                succeded = true;
            }
        }
        else {
            std::remove((SYS_PATH + ".tmp").c_str());
            throw std::runtime_error("\nERROR: Cannot open hosts file.\n");
        }
    }
    return succeded;
}

void Hosts::printDomains() const {
    if (!hfContents.empty()) {
        cout << "\nThe following addresses are being redirected to a non-routable address:" << endl;
        for (const auto & domain_name : hfContents) 
            cout << domain_name << endl;
    } 
    else 
        cerr << "\nNo domain names are being blocked." << endl;
}

bool Hosts::backupHostsFile() const {
    bool backupSucceded = false;
    fstream hostsFile(SYS_PATH, ios::in);
    if (hostsFile) {
        string line;
        fstream fout("hosts.bak", ios::out);
        if (fout) {
            while (getline(hostsFile, line)) fout << line << endl;
            fout.close();
            backupSucceded = true;
        } 
        else 
            throw std::runtime_error("\nERROR: Cannot create hosts.bak file.\n");
        hostsFile.close();
    }
    else 
        throw std::runtime_error("\nERROR: Cannot open hosts file.\n");
    return backupSucceded;
}

bool Hosts::clearHosts() {
    bool opSucceded = false;
    if (!hfContents.empty()) {
        for (auto it = hfContents.begin(); it != hfContents.end();) {
            string domain_name = *it;
            auto toDelete = it++;
            try {
                if(removeDomain(domain_name, true)) hfContents.erase(toDelete);
            }
            catch (const std::runtime_error &error) {
                throw;
            }
        }
        hfContents.clear();
        opSucceded = true;
    } 
    return opSucceded;
}

bool Hosts::overwriteHosts(const string & filePath) {
    bool opSucceded = false;
    string line;
    fstream hostsFile(SYS_PATH, ios::out | ios::trunc), replacerFile(filePath, ios::in);
    if (hostsFile && replacerFile) {
        while (getline(replacerFile, line)) 
            hostsFile << line << endl;
        opSucceded = true;
        hostsFile.close();
        replacerFile.close();
        hfContents.clear();
        try {
            populateStringset();
        }
        catch (const std::runtime_error &error) {
            cerr << error.what() << endl;
            abort();
        }
    }
    return opSucceded;
}

string Hosts::getBackupFileName() const { return BACKUP_FILE; }