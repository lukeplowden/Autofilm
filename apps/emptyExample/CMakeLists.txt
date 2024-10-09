cmake_minimum_required(VERSION 3.15)

project(
    emptyExample
    VERSION 0.1.0
    DESCRIPTION "Builds the Autofilm library and example project"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(outputdir "${CMAKE_SRC_DIR}/bin/${CMAKE_BUILD_TYPE}")
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

file(GLOB_RECURSE EMPTYEXAMPLE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_executable(emptyExample ${EMPTYEXAMPLE_SOURCES})

target_include_directories(emptyExample PRIVATE
    ${CMAKE_SOURCE_DIR}/Autofilm/src
)