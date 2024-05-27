cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

set(PROJECT_NAME pascal)
project(${PROJECT_NAME})

set(PROJ_LIBRARY "parser")
set(PROJ_TESTS   "tests_main")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include" gtest)

add_subdirectory(sources)
add_subdirectory(sample)
add_subdirectory(gtest)
add_subdirectory(tests)