cmake_minimum_required(VERSION 4.0)
project(uno-game)

set(CMAKE_CXX_STANDARD 26)

find_package(ftxui CONFIG REQUIRED)

add_executable(uno-game src/main.cpp)
target_link_libraries(uno-game
        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component
)