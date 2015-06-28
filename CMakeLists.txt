cmake_minimum_required(VERSION 2.6)
set(EXECUTABLE rcongui)
project(${EXECUTABLE} CXX)

# C++11
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

# Debug
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -pedantic -Werror")
# Enable Debug by default, can be changed with -D CMAKE_BUILD_TYPE=Release
if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
    set(DEBUG 1)
else()
    set(DEBUG 0)
endif()

# Enable extra Qt tools
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

include_directories("${CMAKE_SOURCE_DIR}/src")

set(SOURCES
    src/main.cpp
    src/ui/create_server_dialog.cpp
    src/ui/rcon_window.cpp
    src/ui/server_widget.cpp
    src/ui/history_line_edit.cpp
    src/settings.cpp
)


add_executable(${EXECUTABLE} ${SOURCES})

# Libraries

# Qt
find_package(Qt5Widgets REQUIRED)
target_link_libraries(${EXECUTABLE} Qt5::Widgets)
# find_package(Qt4 REQUIRED)
# target_link_libraries(${EXECUTABLE} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Boost
find_package (Boost COMPONENTS system thread REQUIRED)
target_link_libraries(${EXECUTABLE} ${Boost_LIBRARIES})
include_directories(${Boost_INCLUDE_DIRS})

# Install
install(TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)
