cmake_minimum_required(VERSION 3.31)
project(USBDeviceLocker)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# Set Qt6 directory
set(Qt6_DIR "C:/Qt/6.9.0/mingw_64/lib/cmake/Qt6")

# Add Qt packages
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui Network)

# Define executable
add_executable(USBDeviceLocker
        main.cpp
        defs/WhitelistCheck.cpp
        class/main.h
        defs/Scanner.cpp
        defs/prompter.cpp
        defs/Ejection.cpp
        defs/AddToWhitelist.cpp
        gui/MainWindow.cpp
        gui/MainWindow.h
)

# Use latest draft for MSVC
if (MSVC)
    target_compile_options(USBDeviceLocker PRIVATE /std:c++latest)
endif()

# Use C++2c for GCC/Clang
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(USBDeviceLocker PRIVATE -std=c++23)
endif()

# Link libraries with correct keyword style
target_link_libraries(USBDeviceLocker
        PRIVATE
        Qt6::Widgets
        Qt6::Core
        Qt6::Gui
        Qt6::Network
        setupapi
)