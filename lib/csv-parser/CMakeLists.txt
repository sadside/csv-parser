project(${PROJECT_NAME} CXX)

include_directories(include)

add_library(CSVParser INTERFACE)

target_include_directories(CSVParser INTERFACE include)
target_link_libraries(CSVParser INTERFACE tuple-helpers)