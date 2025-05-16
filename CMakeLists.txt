cmake_minimum_required(VERSION 3.31)
project(USBDeviceLocker)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(USBDeviceLocker main.cpp
        defs/WhitelistCheck.cpp
        class/main.h
        defs/Scanner.cpp
        defs/prompter.cpp
        defs/Ejection.cpp
        defs/AddToWhitelist.cpp)

# Force MSVC to use latest draft feats.
if (MSVC)
    target_compile_options(USBDeviceLocker PRIVATE /std:c++latest)
endif()

# GCC/Clang
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang | GNU")
    target_compile_options(USBDeviceLocker PRIVATE -std=c++2c)
endif ()

# Linking Win Libs
target_link_libraries(USBDeviceLocker setupapi)