cmake_minimum_required(VERSION 3.10)
project(opcua_cab)

set(CMAKE_CXX_STANDARD 17)

find_package(open62541 REQUIRED)

add_executable(client
               main.cpp
               client_open62541.cpp
               job_dec_jobtype.cpp
               job_dec_nodetype.cpp
               job_dec_datatype.cpp
               job_open62541.cpp
               client.cpp
               logger.cpp
               os_wrappers.cpp)  
               
target_link_libraries(client open62541::open62541)
