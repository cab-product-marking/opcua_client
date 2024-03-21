cmake_minimum_required(VERSION 3.10)
project(opcua_cab)

set(CMAKE_CXX_STANDARD 17)

find_package(open62541 REQUIRED)

add_executable(client
               main.cpp
               client.cpp
               client_open62541.cpp
               client_freeopcua.cpp
               job_open62541.cpp
               job_freeopcua.cpp
               job_open62541_node.cpp
               data_open62541.cpp
               data_freeopcua.cpp
               logger.cpp
               node_tree.cpp
               os_wrappers.cpp)  
               
target_link_libraries(client open62541::open62541)
