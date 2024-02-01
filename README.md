# OPEN SOURCE OPC UA CLIENT FOR CAB PRINTER USECASES

These are examples of an OPC UA client written in C++ and Python. Basic functionalities in 
dealing with CAB printers and OPC UA are shown. This includes reading and writing configurations, 
handling and printing labels and replacing data fields in the label using Replace. These examples 
give you an concrete overview how you can control and set up cab printers with OPC UA.

## Dependencies and installation on Linux systems

### Use the cpp-opcua-client

An environment supporting C++17 is necessary. Project dependencies:
 * CMake >= 3.16.3
 * GNU Make >= 4.2.1
 * open62541 >= v1.3.8

The open62541 library version 1.3.8 is required for the c++ and python client. 
Download the open62541 sources using git or as a zipfile from github. 
Newer versions and documentation on:
 * https://www.open62541.org/
 * https://www.open62541.org/doc/0.3/toc.html
 * https://github.com/open62541/open62541

Install the open62541 library:

    $ cd <paths to>/opcua_client
    $ git clone https://github.com/open62541/open62541.git
    $ cd open62541
    $ git checkout tags/v1.3.8
    $ mkdir build
    $ cd build/
    $ cmake -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Debug ..
    $ make -j4
    $ make install

Build and configure the client project:

    $ cd <paths to>/opcua_client/
    $ mkdir build 
    $ cd build/
    $ cmake -DCMAKE_BUILD_TYPE=Debug ..
    $ make

To test or use the client make sure OPC UA is activated in printer settings. 
(/Menu/Setup/Interfaces/Network services/ ...OPC UA)
The client needs printer ip and port for connection. Change the defaults in cab_client.h or enter with arguments.

    $ cd <paths to>/opcua_client/build
    $ chmod +x client
    $ ./client -h

Visit <paths to>/opcua_client/res/usage.txt for usecases and examples or call ./client --help.

### Use the python-opcua-client

Project dependencies:
 * Python >= 3.8.10: https://python.org/downloads

The python client works with the python-opcua library witch is build 
on FreeOpcUa. For more information visit:
 * https://github.com/FreeOpcUa/python-opcua
 * https://python-opcua.readthedocs.io/en/latest/

To use the same functionality in a client based on python the python-opcua library is required.
For istalling the library use following syntax:

    $ sudo apt install python-opcua 
    $ sudo apt install python-opcua-tools
    # pip install opcua

Make sure OPC UA is activated in printer settings. 
(/Menu/Setup/Interfaces/Network services/ ...OPC UA)

    $ cd <paths to>/opcua_client
    $ chmod +x client.py
    $ python3 client.py 
        or
    $ ./client.py --help

The python script automatically generates a build directory with output files, depending on jobs entered.

## Dependencies and installation on Windows systems

### Use the cpp-opcua-client

Use Microsoft Visual Studio 17 2022 working with cpp-client.

Project dependencies:
 * CMake >= 3.28.1: http://www.cmake.org/cmake/resources/software.html
 * Microsoft Visual Studio: https://www.visualstudio.com/products/visual-studio-community-vs
 * Python >= 3.12.1: https://python.org/downloads

The open62541 library version 1.3.8 is required for the client. 
Download the open62541 sources using git or as a zipfile from github.
    
    $ cd <paths to>\opcua_client
    $ git clone https://github.com/open62541/open62541.git
    $ cd open62541
    $ git checkout tags/v1.3.8

Newer versions and documentation on:
 * https://www.open62541.org/
 * https://www.open62541.org/doc/0.3/toc.html
 * https://github.com/open62541/open62541

Open a Windows Power Shell (cmd) with admin rights and run:

    $ cd <paths to>\opcua_client\open62541
    $ mkdir build  
    $ cd build
    # Configure environment 
    $ call "C:\Programm Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64

    # Release build and install
    $ call "<paths to>\cmake.exe" -G "Visual Studio 17 2022" -A x64 -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=..\..\open62541_lib\Release ..
    $ msbuild /m /p:Configuration=Release /p:Platform=x64 open62541.sln
    $ call "<paths to>\cmake.exe" --install .

    # Debug build and install
    $ call "<paths to>\cmake.exe" -G "Visual Studio 17 2022" -A x64 -DBUILD_SHARED_LIBS=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX:PATH=..\..\open62541_lib\Debug ..
    $ msbuild /m /p:Configuration=Debug /p:Platform=x64 open62541.sln
    $ call "<paths to>\cmake.exe" --install .

    $ cd ../../
    $ client.sln .

In <paths to>\opcua_client\open62541_lib\Debug or ..\Release the files fall out.
Next, need to check the include paths for the *.h files. 
If installed the open62541 library as described above, the paths should be OK.
Checking the project settings for Debug and Release is recommended.

| Debug properties | Setting |
|-|-|
| Properties>Configuration Properties>C/C++>General>Additional Include Directories | open62541_lib\Debug\include |
| Properties>Configuration Properties>C/C++>Linker>General>Additional Library Directories | open62541_lib\Debug\lib |
| Properties>Configuration Properties>C/C++>Linker>Input>Additional Dependencies | open62541.lib |

| Release properties | Setting |
|-|-|
| Properties>Configuration Properties>C/C++>General>Additional Include Directories | open62541_lib\Release\include |
| Properties>Configuration Properties>C/C++>Linker>General>Additional Library Directories | open62541_lib\Release\lib |
| Properties>Configuration Properties>C/C++>Linker>Input>Additional Dependencies | open62541.lib |

The client needs the open62541.dll for runtime. To generate x64 folder start build for Debug and Release.
Copy *.dll from <paths to>\opcua_client\open62541_lib\Debug\bin to <paths to>\opcua_client\x64\Debug.
Same procedure for Release build.

Use the client with VS or call the *.exe in <paths to>\opcua_client\x64\Debug or ..\Release.

To test or use the client make sure OPC UA is activated in printer settings. 
(/Menu/Setup/Interfaces/Network services/ ...OPC UA)
The client needs printer ip and port for connection. Change the defaults in cab_client.h or enter with arguments.

    $ cd <paths to>\opcua_client\x64\Debug
    $ .\client.exe "-h" or .\client.exe "--help"

If there are problems with the terminal arguments use brackets.
Visit <paths to>/opcua_client/res/usage.txt for usecases and examples or call .\client.exe "--help".

### Use the python-opcua-client

Project dependencies:
 * Python >= 3.12.1: https://python.org/downloads

The python client works with the python-opcua library witch is build 
on FreeOpcUa. For more information visit:
 * https://github.com/FreeOpcUa/python-opcua
 * https://python-opcua.readthedocs.io/en/latest/

To use the same functionality in a client based on python the python-opcua library is required.
For istalling the library use following syntax:

    $ pip install opcua

Make sure OPC UA is activated in printer settings. 
(/Menu/Setup/Interfaces/Network services/ ...OPC UA)

    $ cd <paths to>\opcua_client
    $ python .\client.py --help

If there are problems with the terminal arguments use brackets.
The python script automatically generates a build directory with output files, depending on jobs entered.

<!-- Eof -->