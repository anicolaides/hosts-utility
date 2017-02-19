// Name: Alexandros Nicolaides
// Purpose: Cross-platform utility to redirect domain name/s to a non-routable address.
// File: main.cpp (Application Interface)
// Dependencies: hosts.hpp, hosts.cpp, dirtywork.hpp

#include "hosts.hpp"
#include "hosts.cpp" /* needed to compile in cloud9 IDE */
#include "dirtywork.hpp"

void menu() {
    cout << "\n-------------------System Administrator's Utility-------------------\n"
        << "1 - Block a specific domain name.\n"
        << "2 - Block domain name/s specified in a file.\n"
        << "3 - Display domain name/s currently being blocked.\n"
        << "4 - Remove a specific domain name from hosts file.\n"
        << "5 - Create a backup of the hosts file to the current directory.\n"
        << "6 - Unblock all domains names.\n"
        << "R - Restore from a backup.\n"
        << "Q - To Quit.\n" << endl;
}

char userChoice() {
    char choice;
    string input;
    cout << "Please enter one of the above: ";
    getline(cin, input);
    stringstream(input) >> choice;
    return choice;
}

int main(int argc, char** argv) {
    Dirtywork helper(argv);
    Hosts file(helper.getHostsPath());
    menu();
    char choice = userChoice();
    while (choice != 'q' && choice != 'Q') {
        string domain, filePath;
        stringstream ss;
        switch (choice) {
        case '1':
            cout << "Enter a domain name to block (e.g facebook.com): ";
            getline(cin, domain);
            // assuming smallest domain name is of length 4 (e.g "a.co")
            while (domain.length() < 4) {
                cerr << "\nERROR: The domain name is too small, try again (e.g facebook.com): ";
                domain.clear();
                getline(cin, domain);
            }
            try { 
                if (file.backupHostsFile()) file.blockDomain(domain);
            }
            catch(const std::runtime_error &error) { 
                cerr << error.what() << endl;}
            break;
        case '2':
            cout << "\nPlease enter the path to the file to extract data from: (file.txt): ";
            getline(cin, filePath);
            while (!filePath.length()) {
                cout << "\nPlease enter a valid file path or file name: ";
                filePath.clear();
                getline(cin, filePath);
            }
            try {
                if(file.backupHostsFile()) file.appendFile(filePath);
            }
            catch (const std::runtime_error &error) {
                cerr << error.what() << endl;
            }
            break;
        case '3':
            file.printDomains();
            break;
        case '4':
            cout << "Enter the domain name to be removed in the form (e.g facebook.com): ";
            getline(cin, domain);
            while (!domain.length()) {
                cerr << "\nERROR: Your domain is too small, try again (e.g facebook.com): ";
                domain.clear();
                getline(cin, domain);
            }
            try {
                if (file.backupHostsFile() && file.removeDomain(domain, false))
                    cout << "\nThe domain name " + domain + " has been removed successfully." << endl;
                else
                    cerr << "\nERROR: Couldn't remove the domain from hosts file." << endl;
            }
            catch (const std::runtime_error &error) {
                cerr << error.what() << endl;
            }
            break;
        case '5':
            try {
                if (file.backupHostsFile()) cout << "\nThe backup file can be found at: " << helper.getCurDir()
                    << '/' << file.getBackupFileName() << endl;
            }
            catch (const std::runtime_error &error) {
                cout << error.what() << endl;
            }
            break;
        case '6':
            try {
                if (file.backupHostsFile() && file.clearHosts())
                    cout << "All domain names have been removed. You are all set!" << endl;
                else cerr << "\nThere are no domains currently being blocked. You are all set!" << endl;
            }
            catch (const std::runtime_error &error) {
                cout << error.what() << endl;
            }
            break;
        case 'r':
        case 'R':
            cout << "\nEnter the file name or path of the file that's to replace the hosts file (e.g hosts.bak): ";
            filePath.clear();
            getline(cin, filePath);
            while (!filePath.length()) {
                cerr << "ERROR: Your file name or path is too small, try again (e.g hosts.bak): ";
                filePath.clear();
                getline(cin, filePath);
            }
            if (file.overwriteHosts(filePath))
                cout << "\nThe file has been replaced successfully!" << endl;
            else
                cerr << "ERROR: Could not replace the hosts file." << endl;
            break;
        default:
            cerr << "\nERROR: Wrong input." << endl;
            break;
        }
        menu();
        choice = userChoice();
    }
    return 0;
}
