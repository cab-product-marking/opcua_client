# Linux Systems
## Project dependencies 
 * cmake >= 3.16.3
 * GNU Make >= 4.2.1
 * open62541 >= v1.3.8
 * (Python3 >= 3.8.10 for python example)
## Cpp
### Install library 
    $ cd home/user/opcua_cab/open62541-1.3.8/
    $ mkdir build
    $ cd build/
    $ cmake ..
    $ make
### Build and configure 
    $ cd home/user/opcua_cab/
    $ mkdir build 
    $ cd build/
    $ cmake ..
    $ make
### Run client 
Make sure OPC UA is actiavated in printer settings (/Menu/Setup/Interfaces/Network services/ ...OPC UA)
Adapt HOST in the setter.h file to your printer (Menu/Setup/Interfaces/Ethernet/ ...IP address)
Adapt PASW in the setter.h file to your printer (/Menu/Setup/Security/ ...Password OPC UA)

    $ cd home/user/opcua_cab/build
    $ chmod +x client
    $ ./client -h

Visit home/user/opcua_cab/res/usage.txt for usecases and examples or call ./client --help.
## Python3
### Install library
    $ sudo apt install python-opcua # Library
    $ sudo apt install python-opcua-tools # Command-line tools
### Run client
Make sure OPC UA is actiavated in printer settings (/Menu/Setup/Interfaces/Network services/ ...OPC UA)
Adapt HOST in the setter.h file to your printer (Menu/Setup/Interfaces/Ethernet/ ...IP address)
Adapt PASW in the setter.h file to your printer (/Menu/Setup/Security/ ...Password OPC UA)

    $ cd home/user/opcua_cab/
    $ chmod +x client.py
    $ python3 client.py 
        or
    $ ./client.py --help

The python script automatically generates a build directory with output files, depending on jobs entered.

# Windows
## Project dependencies 
## Cpp
## Ptyhon3

## History
 * 11.12.2023 - Complete testing and write README.md doc
 * 14.12.2023 - Completion sequence added when uploading files and update directory handling

<!-- Eof -->