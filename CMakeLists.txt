cmake_minimum_required(VERSION 4.0)
project(uno-game)

set(CMAKE_CXX_STANDARD 26)

find_package(ftxui CONFIG REQUIRED)

add_library(uno-game-lib
        src/game/Card.cpp
        src/game/CardTile.cpp
        src/game/Player.cpp
        src/game/GameState.cpp
        src/common/Utils.cpp
        src/network/Message.cpp
)

add_executable(uno-game src/main.cpp)
target_link_libraries(uno-game
        PRIVATE uno-game-lib)
target_link_libraries(uno-game
        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component
)

add_subdirectory(test)