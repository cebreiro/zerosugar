cmake_minimum_required (VERSION 3.27)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(zerosugar)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/msvc.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/find_packages.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/protobuf.cmake)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(zerosugar)
add_subdirectory(tests)
