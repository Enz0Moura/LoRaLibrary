cmake_minimum_required(VERSION 3.28)
project(LoRaLibrary.cpp)

set(CMAKE_CXX_STANDARD 17)

add_library(LoRaLibrary SHARED LoRaLibrary.cpp
        LoRaLibrary.cpp)
