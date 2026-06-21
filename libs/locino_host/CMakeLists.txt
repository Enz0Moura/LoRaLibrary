cmake_minimum_required(VERSION 3.20)

project(locino_host LANGUAGES CXX)

add_library(locino_host
    src/LocINOSerialClient.cpp
    src/LocINOCLI.cpp
)

if(WIN32)
    target_sources(locino_host PRIVATE
        src/SerialPortWindows.cpp
    )

    target_compile_definitions(locino_host PRIVATE
        NOMINMAX
        WIN32_LEAN_AND_MEAN
    )
else()
    target_sources(locino_host PRIVATE
        src/SerialPortPosix.cpp
    )
endif()

target_compile_features(locino_host PUBLIC cxx_std_17)

target_include_directories(locino_host
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../locino_protocol/include
)

add_executable(locino_host_example
    src/example.cpp
)

target_link_libraries(locino_host_example PRIVATE
    locino_host
)