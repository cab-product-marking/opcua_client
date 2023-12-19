## OPEN SOURCE OPC UA CLIENT FOR CAB PRINTER USECASES

These are examples for an OPC UA client written in C++ and Python.

## Installation 

An environment supporting C++17 is necessary. Project dependencies:
 * cmake >= 3.16.3
 * GNU Make >= 4.2.1
 * open62541 >= v1.3.8
 * (Python3 >= 3.8.10 for python example)

### Linux 

The open62541 library version 1.3.8 is required for the c++ client. 
This is included in the project. Newer versions and documentation on:
 * https://www.open62541.org/
 * https://www.open62541.org/doc/0.3/toc.html
 * https://github.com/open62541/open62541

Install the open62541 library:

    $ cd home/user/opcua_client/open62541-1.3.8/
    $ mkdir build
    $ cd build/
    $ cmake ..
    $ make

Build and configure the client project:

    $ cd home/user/opcua_client/
    $ mkdir build 
    $ cd build/
    $ cmake ..
    $ make

To test or use the client make sure OPC UA is actiavated in printer settings. 
(/Menu/Setup/Interfaces/Network services/ ...OPC UA)

    $ cd home/user/opcua_client/build
    $ chmod +x client
    $ ./client -h

Visit home/user/opcua_client/res/usage.txt for usecases and examples or call ./client --help.

To use the same functionality in a client based on python:

    $ sudo apt install python-opcua 
    $ sudo apt install python-opcua-tools

Make sure OPC UA is actiavated in printer settings. 
(/Menu/Setup/Interfaces/Network services/ ...OPC UA)

    $ cd home/user/opcua_cab/
    $ chmod +x client.py
    $ python3 client.py 
        or
    $ ./client.py --help

The python script automatically generates a build directory with output files, depending on jobs entered.

### Windows


## History
 * 11.12.2023 - Complete testing and write README.md doc
 * 14.12.2023 - Completion sequence added when uploading files and update directory handling

<!-- Eof -->