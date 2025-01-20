cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME csv-parser)

project(${PROJECT_NAME} CXX)

add_executable(${PROJECT_NAME} src/main.cpp)

add_subdirectory(shared/tuple-helpers)
add_subdirectory(lib/csv-parser)

target_link_libraries(${PROJECT_NAME} CSVParser)