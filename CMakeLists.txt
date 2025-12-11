cmake_minimum_required(VERSION 4.0)
project(uno-game)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(nlohmann_json REQUIRED)
find_package(asio REQUIRED)
find_package(argparse REQUIRED)
find_package(Slint REQUIRED)
find_package(spdlog REQUIRED)

# Uno Game Library
add_library(uno-game-lib
        src/game/Card.cpp
        src/game/CardTile.cpp
        src/game/Player.cpp
        src/game/GameState.cpp
        src/common/Logger.cpp
        src/common/Utils.cpp
        src/network/Message.cpp
        src/network/MessageSerializer.cpp
        src/network/NetworkServer.cpp
        src/network/NetworkClient.cpp
        src/network/Session.cpp
)
target_link_libraries(uno-game-lib PUBLIC nlohmann_json::nlohmann_json)
target_link_libraries(uno-game-lib PUBLIC asio::asio)
target_link_libraries(uno-game-lib PUBLIC Slint::Slint)
target_link_libraries(uno-game-lib PUBLIC argparse::argparse)
target_link_libraries(uno-game-lib PUBLIC spdlog::spdlog)

slint_target_sources(uno-game-lib ui/MainWindow.slint)

# Uno Client
add_executable(uno-client
        src/client/main.cpp
        src/client/PlayerAction.cpp
        src/client/UnoClient.cpp
        src/ui/GameUI.cpp
        assets/uno.rc
)
target_link_libraries(uno-client PRIVATE uno-game-lib)

if (WIN32 AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(uno-client PROPERTIES WIN32_EXECUTABLE ON)
endif ()


# Uno Server
add_executable(uno-server
        src/server/main.cpp
        src/server/UnoServer.cpp
        assets/uno.rc
)
target_link_libraries(uno-server PRIVATE uno-game-lib)

# Google Test
add_subdirectory(test)

# 复制动态库
foreach (target uno-client uno-server)
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${target}>
            $<TARGET_FILE_DIR:${target}>
            COMMAND_EXPAND_LISTS
    )
endforeach ()