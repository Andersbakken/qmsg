cmake_minimum_required(VERSION 2.8)
project(qmsg)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
add_executable(qmsg main.cpp qargumentparser.h qswitchargument.cpp qargumentparser.cpp qswitchargument.h)
target_link_libraries(qmsg Qt5::Widgets)

