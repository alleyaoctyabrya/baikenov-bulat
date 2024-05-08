cmake_minimum_required(VERSION 3.27)
project(tp_project)

set(CMAKE_CXX_STANDARD 20)

add_executable(tp_project Game.cpp)
find_package(Curses REQUIRED)
add_library(BLOCK_LIBRARY Block.cpp Block.h)
add_library(BOT_LIBRARY Bot.cpp Bot.h)
add_library(MAP_LIBRARY Map.cpp Map.h)
target_link_libraries(tp_project ${CURSES_LIBRARIES})
target_link_libraries(tp_project BLOCK_LIBRARY MAP_LIBRARY BOT_LIBRARY)
target_include_directories(tp_project PUBLIC ${CURSES_INCLUDE_DIRS})