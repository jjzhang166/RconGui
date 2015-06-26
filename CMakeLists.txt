cmake_minimum_required(VERSION 2.6)
set(EXECUTABLE rcongui)
project(${EXECUTABLE})

find_package(Qt4 REQUIRED)
# find_package(Qt5Widgets REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES src/rcon_window.cpp src/main.cpp)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})


add_executable(${EXECUTABLE} ${SOURCES})
# target_link_libraries(${EXECUTABLE} Qt5::Widgets Qt5::DBus)
target_link_libraries(${EXECUTABLE} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

install(TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin)
