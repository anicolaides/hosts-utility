# hosts-utility
Cross-platform utility to facilitate blocking domain names using the hosts file.

## Usage
For option #2 of the menu, please provide a file in the following format:

```
www.yahoo.com
facebook.com
...
```

## How to compile
Using Unix-based Operating Systems:
```
g++ -std=c++14 main.cpp dirtywork.hpp hosts.hpp hosts.cpp
```

Using Microsoft Windows:

1. Create a Visual C++ Win32 Project.

2. Choose Console application.

3. Add main.cpp, dirtywork.hpp, hosts.hpp, and hosts.cpp to the project.

4. Ctrl + F5.


## Dealing with Microsoft Windows' UAC:

1. VS 2015 --> Project --> Properties --> Linker --> Manifest file --> UAC Execution Level

2. Set UAC Execution Level to **"requireAdministrator (/level='requireAdministrator')"**
