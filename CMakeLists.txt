cmake_minimum_required(VERSION 4.0)
project(uno-game)

set(CMAKE_CXX_STANDARD 26)

find_package(nlohmann_json REQUIRED)
find_package(asio REQUIRED)
find_package(argparse REQUIRED)
find_package(Slint REQUIRED)

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
        src/client/UnoClient.cpp
        src/server/UnoServer.cpp
        src/network/Session.cpp
        src/client/PlayerAction.cpp
        src/ui/GameUI.cpp
)
target_link_libraries(uno-game-lib
        PUBLIC nlohmann_json::nlohmann_json
)
target_link_libraries(uno-game-lib
        PUBLIC asio::asio
)
target_link_libraries(uno-game-lib PUBLIC Slint::Slint)

slint_target_sources(uno-game-lib ui/MainWindow.slint)

add_executable(uno-client src/client/main.cpp)
target_link_libraries(uno-client
        PRIVATE uno-game-lib)

if (WIN32 AND CMAKE_BUILD_TYPE STREQUAL "DEBUG")
    set_target_properties(uno-client PROPERTIES WIN32_EXECUTABLE ON)
endif ()

add_executable(uno-server src/server/main.cpp)
target_link_libraries(uno-server
        PRIVATE uno-game-lib)
target_link_libraries(uno-server
        PRIVATE argparse::argparse
)

add_subdirectory(test)