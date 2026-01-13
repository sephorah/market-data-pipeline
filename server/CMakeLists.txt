cmake_minimum_required(VERSION 3.20)

project(
    MarketDataSimulatorServer
    VERSION 1.0
    LANGUAGES CXX
)

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)