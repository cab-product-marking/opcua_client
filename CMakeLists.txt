cmake_minimum_required(VERSION 3.10)
project(opcua_cab)

set(CMAKE_CXX_STANDARD 17)

find_package(open62541 REQUIRED)

add_executable(client
               main.cpp
               opcua_client.cpp
               cab_client.cpp
               tree_node.cpp)  
               
target_link_libraries(client open62541::open62541)
