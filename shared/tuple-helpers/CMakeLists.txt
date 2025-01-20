project(${PROJECT_NAME} CXX)

include_directories(include)

add_library(tuple-helpers INTERFACE)

target_include_directories(tuple-helpers INTERFACE include)