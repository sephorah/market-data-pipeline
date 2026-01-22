cmake_minimum_required(VERSION 3.20)

project(
    MarketDataSimulatorServer
    VERSION 1.0
    LANGUAGES CXX
)

find_package(cxxopts REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp src/ArgParser.cpp src/ConfigParser.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_link_libraries(${PROJECT_NAME} 
    PRIVATE cxxopts::cxxopts
    PRIVATE nlohmann_json::nlohmann_json)