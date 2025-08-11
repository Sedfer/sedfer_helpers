message(VERBOSE "Configuring ${CMAKE_CURRENT_LIST_FILE}")

cmake_minimum_required(VERSION 3.21)
project(sedfer_helpers)

set(CMAKE_CXX_STANDARD 23)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(tests)
