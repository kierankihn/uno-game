cmake_minimum_required(VERSION 4.0)
project(uno-game)

set(CMAKE_CXX_STANDARD 26)

find_package(ftxui CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(asio REQUIRED)

add_library(uno-game-lib
        src/game/Card.cpp
        src/game/CardTile.cpp
        src/game/Player.cpp
        src/game/GameState.cpp
        src/common/Utils.cpp
        src/network/Message.cpp
        src/network/MessageSerializer.cpp
        src/network/NetworkServer.cpp
        src/network/NetworkClient.cpp
)
target_link_libraries(uno-game-lib
        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component
)
target_link_libraries(uno-game-lib
        PRIVATE nlohmann_json::nlohmann_json
)
target_link_libraries(uno-game-lib
        PRIVATE asio::asio
)

add_executable(uno-game src/main.cpp)
target_link_libraries(uno-game
        PRIVATE uno-game-lib)

add_subdirectory(test)