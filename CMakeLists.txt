cmake_minimum_required(VERSION 4.0)
project(uno-game)

set(CMAKE_CXX_STANDARD 26)

find_package(ftxui CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(asio REQUIRED)
find_package(argparse REQUIRED)

# 移除 /utf-8 选项
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT CMAKE_CXX_COMPILER_FRONTEND_VARIANT MATCHES "MSVC")
    foreach (component screen dom component)
        get_target_property(FTXUI_COMPILE_OPTIONS ftxui::${component} INTERFACE_COMPILE_OPTIONS)
        if (FTXUI_COMPILE_OPTIONS)
            list(REMOVE_ITEM FTXUI_COMPILE_OPTIONS "/utf-8")
            set_target_properties(ftxui::${component} PROPERTIES INTERFACE_COMPILE_OPTIONS "${FTXUI_COMPILE_OPTIONS}")
        endif ()
    endforeach ()
endif ()

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
)
target_link_libraries(uno-game-lib
        PUBLIC ftxui::screen
        PUBLIC ftxui::dom
        PUBLIC ftxui::component
)
target_link_libraries(uno-game-lib
        PUBLIC nlohmann_json::nlohmann_json
)
target_link_libraries(uno-game-lib
        PUBLIC asio::asio
)

add_executable(uno-client src/client/main.cpp)
target_link_libraries(uno-client
        PRIVATE uno-game-lib)

add_executable(uno-server src/server/main.cpp)
target_link_libraries(uno-server
        PRIVATE uno-game-lib)
target_link_libraries(uno-server
        PRIVATE argparse::argparse
)

add_subdirectory(test)