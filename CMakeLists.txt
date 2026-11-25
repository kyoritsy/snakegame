cmake_minimum_required(VERSION 3.16)
project(SnakeTurnBased LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

add_executable(snake
    main.cpp
    src/Snake.cpp
    src/Game.cpp
)

target_link_libraries(snake PRIVATE SFML::Graphics SFML::Window SFML::System)