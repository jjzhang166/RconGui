cmake_minimum_required(VERSION 2.6)
set(EXECUTABLE rcongui)
project(${EXECUTABLE} CXX)

find_package(Qt4 REQUIRED)
# find_package(Qt5Widgets REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# For some reason the above flags don't really work...
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR CMAKE_COMPILER_IS_GNUCXX)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(--std=c++${CMAKE_CXX_STANDARD} STD_CXX)
    if(STD_CXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++${CMAKE_CXX_STANDARD}")
    else()
        message(SEND_ERROR "Requires C++${CMAKE_CXX_STANDARD} or better")
    endif()
else()
    message(WARNING "Unrecognized compiler: ${CMAKE_CXX_COMPILER_ID}, make sure it supports C++${CMAKE_CXX_STANDARD}")
endif()

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
    src/main.cpp
    src/create_server_dialog.cpp
    src/rcon_window.cpp
    src/server_widget.cpp
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})


add_executable(${EXECUTABLE} ${SOURCES})
# target_link_libraries(${EXECUTABLE} Qt5::Widgets Qt5::DBus)
target_link_libraries(${EXECUTABLE} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

install(TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)
