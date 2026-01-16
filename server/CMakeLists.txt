cmake_minimum_required(VERSION 3.20)

project(
    MarketDataSimulatorServer
    VERSION 1.0
    LANGUAGES CXX
)

find_package(cxxopts REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp src/ArgParser.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_link_libraries(${PROJECT_NAME} cxxopts::cxxopts)