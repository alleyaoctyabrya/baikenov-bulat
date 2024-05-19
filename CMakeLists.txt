cmake_minimum_required(VERSION 3.25)
project(tp_project)

set(CMAKE_CXX_STANDARD 17)

add_executable(tp_project Game.cpp)
find_package(Curses REQUIRED)
include(FetchContent)

FetchContent_Declare(asio-cmake
        GIT_REPOSITORY https://github.com/kingsamchen/asio-cmake.git
        GIT_TAG        origin/master
)

set(ASIO_CMAKE_ASIO_TAG asio-1-12-2)
FetchContent_MakeAvailable(asio-cmake)


target_link_libraries(tp_project
        PRIVATE asio
)
add_library(BLOCK_LIBRARY Block.cpp Block.h)
add_library(BOT_LIBRARY Bot.cpp Bot.h)
add_library(MAP_LIBRARY Map.cpp Map.h)
target_link_libraries(tp_project PUBLIC ${CURSES_LIBRARIES})
target_link_libraries(tp_project PUBLIC BLOCK_LIBRARY MAP_LIBRARY BOT_LIBRARY)
target_include_directories(tp_project PUBLIC ${CURSES_INCLUDE_DIRS})