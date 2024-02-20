cmake_minimum_required(VERSION 3.10)
project(opcua_cab)

set(CMAKE_CXX_STANDARD 17)

find_package(open62541 REQUIRED)

add_executable(client
                src/main.cpp
               src/client_freeopcua.cpp
               src/client_open62541.cpp
               src/job_freeopcua.cpp
               src/job_open62541.cpp
               src/client.cpp
               src/logger.cpp
               src/os_wrappers.cpp)  
               
target_link_libraries(client open62541::open62541)
