cmake_minimum_required(VERSION 3.20)

project(locino_cpu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(locino_cpu
    src/main.cpp
    src/SerialPort.cpp
    src/LocINOSerialClient.cpp
)

target_include_directories(locino_cpu PRIVATE
    include
    ../shared
)