cmake_minimum_required(VERSION 4.0)
find_package(GTest REQUIRED)

set(CMAKE_CXX_STANDARD 26)

project(uno-game-test)

add_executable(uno-game-test
        unit/game/CardTest.cpp
        unit/game/CardTileTest.cpp
        unit/game/PlayerTest.cpp
        unit/game/GameStateTest.cpp
)

target_link_libraries(uno-game-test
        PUBLIC uno-game-lib
)
target_link_libraries(uno-game-test
        PRIVATE GTest::gtest
        PRIVATE GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(uno-game-test)
