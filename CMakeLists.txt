cmake_minimum_required(VERSION 3.15)
project(BattleshipGame VERSION 1.0 LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)


find_package(SFML 2.5 COMPONENTS graphics window system network audio QUIET)
if(NOT SFML_FOUND)
    message(STATUS "SFML not found, fetching from GitHub…")
    FetchContent_Declare(
        sfml
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        2.6.x
    )
    set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
    set(SFML_BUILD_DOC      OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(sfml)
endif()


set(BATTLESHIP_SRC
    src/Game.cpp
    src/Board.cpp
    src/Ship.cpp
    src/Player.cpp
    src/AI.cpp
    src/Network.cpp
)

add_library(battleship STATIC ${BATTLESHIP_SRC})
target_include_directories(battleship PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(battleship PUBLIC
    sfml-graphics sfml-window sfml-system sfml-network sfml-audio
)


add_executable(${PROJECT_NAME} src/main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE battleship)


include(CTest)
if(BUILD_TESTING)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)  # нужно для MSVC
    FetchContent_MakeAvailable(googletest)

    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS test/*_test.cpp)
    add_executable(BattleshipTests ${TEST_SOURCES})
    target_link_libraries(BattleshipTests PRIVATE battleship GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(BattleshipTests)
endif()
