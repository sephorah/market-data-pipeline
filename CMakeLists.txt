cmake_minimum_required(VERSION 3.20)

project(
    MarketDataSimulator
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

add_subdirectory(server)