cmake_minimum_required(VERSION 3.21)
cmake_policy(SET CMP0091 NEW)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15 CACHE STRING "Minimum macOS version")

project(signal-noodle VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)

add_library(simde INTERFACE)
target_include_directories(simde INTERFACE libs/simde)

add_subdirectory(libs/fmt)
add_subdirectory(libs/sst-basic-blocks)
add_subdirectory(libs/sst-effects)

add_executable(${PROJECT_NAME} src/main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror)
target_link_libraries(${PROJECT_NAME} PRIVATE simde fmt sst-basic-blocks sst-effects)
