cmake_minimum_required(VERSION 3.15)

# Set the project name and architecture
project(Autofilm VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the runtime library to use the shared version of the CRT
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MDd")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MDd")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MDd")

# Add GLFW submodule
add_subdirectory(Autofilm/external/glfw)
find_package(OpenGL REQUIRED)

# Add spdlog submodule
add_subdirectory(Autofilm/external/spdlog)

# Add glm headers
include_directories(Autofilm/external/glm)

# Add GLAD submodule
add_library(glad STATIC
    Autofilm/external/glad/src/glad.c
)
target_include_directories(glad PUBLIC
    Autofilm/external/glad/include
)

# Define build configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

# Output directory
set(outputdir "${CMAKE_BUILD_TYPE}")

# Define source files for the Autofilm library
file(GLOB_RECURSE AUTOFILM_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Autofilm/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Autofilm/src/*.h"
)

# Autofilm project
add_library(Autofilm SHARED ${AUTOFILM_SOURCES})
set_property(TARGET Autofilm PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set_property(TARGET Autofilm PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

target_include_directories(Autofilm PUBLIC
    ${CMAKE_SOURCE_DIR}/Autofilm/src
    ${CMAKE_SOURCE_DIR}/Autofilm/external/glfw/include
    ${CMAKE_SOURCE_DIR}/Autofilm/external/glad/include
    ${CMAKE_SOURCE_DIR}/Autofilm/external/spdlog/include
    ${CMAKE_SOURCE_DIR}/Autofilm/external/glm
)

target_link_libraries(Autofilm PUBLIC 
    glfw
    glad
    OpenGL::GL
    spdlog::spdlog
)

set_target_properties(Autofilm PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${outputdir}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${outputdir}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${outputdir}
)

target_compile_definitions(Autofilm PRIVATE
    AF_PLATFORM_WINDOWS
    AF_BUILD_DLL
    $<$<CONFIG:Debug>:AF_ENABLE_ASSERTS>
)


target_compile_options(Autofilm PRIVATE
    $<$<CONFIG:Debug>:/DDEBUG /Zi>
    $<$<CONFIG:Release>:/O2>
)

# Add precompiled header
target_precompile_headers(Autofilm PRIVATE ${CMAKE_SOURCE_DIR}/Autofilm/src/autofilmpch.h)

add_custom_command(TARGET Autofilm POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:Autofilm>
    ${CMAKE_BINARY_DIR}/bin/${outputdir}/emptyExample
)

# emptyExample project
file(GLOB_RECURSE EMPTYEXAMPLE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/apps/emptyExample/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/apps/emptyExample/src/*.h"
)

add_executable(emptyExample ${EMPTYEXAMPLE_SOURCES})

target_include_directories(emptyExample PRIVATE
    ${CMAKE_SOURCE_DIR}/Autofilm/src
)

set_target_properties(emptyExample PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${outputdir}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${outputdir}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${outputdir}
)

target_compile_definitions(emptyExample PRIVATE
    AF_PLATFORM_WINDOWS
)

target_compile_options(emptyExample PRIVATE
    $<$<CONFIG:Debug>:/DDEBUG /Zi>
    $<$<CONFIG:Release>:/O2>
)

target_link_libraries(emptyExample PRIVATE Autofilm)

# Add precompiled header for emptyExample
target_precompile_headers(emptyExample PRIVATE ${CMAKE_SOURCE_DIR}/Autofilm/src/autofilmpch.h)