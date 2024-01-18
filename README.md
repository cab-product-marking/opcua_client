## OPEN SOURCE OPC UA CLIENT FOR CAB PRINTER USECASES

These are examples of an OPC UA client written in C++ and Python. Basic functionalities in 
dealing with a CAB printer and OPC UA are shown. This includes reading and writing configurations, 
handling and printing labels and replacing data fields in the label using Replace. These examples 
are intended to give you an idea of how the functionalities of the printer and with the printer 
can be set up.

### Dependencies and installation on Linux systems

An environment supporting C++17 is necessary. Project dependencies:
 * CMake >= 3.16.3
 * GNU Make >= 4.2.1
 * open62541 >= v1.3.8
 * Python3 >= 3.8.10 

The open62541 library version 1.3.8 is required for the c++ and python client. 
Download the open62541 sources (using git or as a zipfile from github) or use the included in the project. 
Newer versions and documentation on:
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

The python client works with the python-opcua library witch is build 
on FreeOpcUa. For more information visit:
 * https://github.com/FreeOpcUa/python-opcua
 * https://python-opcua.readthedocs.io/en/latest/

To use the same functionality in a client based on python the python-opcua library is required.
For istalling the library use following syntax:

    $ sudo apt install python-opcua 
    $ sudo apt install python-opcua-tools
    # pip install opcua

Make sure OPC UA is actiavated in printer settings. 
(/Menu/Setup/Interfaces/Network services/ ...OPC UA)

    $ cd home/user/opcua_cab/
    $ chmod +x client.py
    $ python3 client.py 
        or
    $ ./client.py --help

The python script automatically generates a build directory with output files, depending on jobs entered.

### Dependencies and installation on Windows systems

Use Microsoft Visual Studio 17 2022 working with cpp-client.

Project dependencies:
 * CMake >= 3.28.1: http://www.cmake.org/cmake/resources/software.html
 * Microsoft Visual Studio: https://www.visualstudio.com/products/visual-studio-community-vs
 * Python >= 3.12.1: https://python.org/downloads

The open62541 library version 1.3.8 is required for the c++ and python client. 
Download the open62541 sources (using git or as a zipfile from github) or use the included in the project. 
Newer versions and documentation on:
 * https://www.open62541.org/
 * https://www.open62541.org/doc/0.3/toc.html
 * https://github.com/open62541/open62541

Open a Windows Power Shell (cmd) and run;

    $ cd C:\Users\admin\source\repos\opcua_client\open62541
    $ mkdir build   
    $ cd build
    $ C:\Users\admin\tools\cmake.exe .. -G "Visual Studio 17 2022"
    # You can use cmake-gui for a graphical user-interface to select features.
    # Then open buildopen62541.sln in VS Studio and build as usual.












The python client works with the python-opcua library witch is build 
on FreeOpcUa. For more information visit:
 * https://github.com/FreeOpcUa/python-opcua
 * https://python-opcua.readthedocs.io/en/latest/

To use the same functionality in a client based on python the python-opcua library is required.
For istalling the library use following syntax:

    $ pip install opcua

Make sure OPC UA is actiavated in printer settings. 
(/Menu/Setup/Interfaces/Network services/ ...OPC UA)

    $ cd C:\Users\admin\source\repos\opcua_client\
    $ python .\client.py --help

The python script automatically generates a build directory with output files, depending on jobs entered.


## History
 * 11.12.2023 - Complete testing and write README.md doc
 * 14.12.2023 - Completion sequence added when uploading files and update directory handling
 * 16.12.2023 - Finish tests on windows systems with python and cpp code

<!-- Eof -->