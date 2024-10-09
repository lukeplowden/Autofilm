cmake_minimum_required(VERSION 3.15)

project(
    Autofilm 
    VERSION 0.1.0
    DESCRIPTION "A C++ creative coding library"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(outputdir "${CMAKE_SOURCE_DIR}/bin")

add_subdirectory(external/glfw)
find_package(OpenGL REQUIRED)
add_subdirectory(external/spdlog)
add_library(glad STATIC
    external/glad/src/glad.c
)
target_include_directories(glad PUBLIC
    external/glad/include
)


file(GLOB_RECURSE AUTOFILM_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

add_library(Autofilm SHARED ${AUTOFILM_SOURCES})
set_property(TARGET Autofilm PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

target_include_directories(Autofilm PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Autofilm PUBLIC 
    OpenGL::GL
    glad
    glfw
    spdlog::spdlog
)

set_target_properties(Autofilm PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${outputdir}
)

target_compile_definitions(Autofilm PRIVATE
    AF_PLATFORM_WINDOWS
    AF_BUILD_DLL
    $<$<CONFIG:Debug>:AF_ENABLE_ASSERTS>
)

target_precompile_headers(Autofilm PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/autofilmpch.h)

target_compile_options(Autofilm PRIVATE
    $<$<CONFIG:Debug>:/DDEBUG /Zi>
    $<$<CONFIG:Release>:/O2>
)

