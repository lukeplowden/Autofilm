cmake_minimum_required(VERSION 3.15)

project(
    AutofilmDevelopment
    VERSION 0.1.0
    DESCRIPTION "Builds the Autofilm library and example project"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(outputdir "${CMAKE_SOURCE_DIR}/bin")

add_subdirectory(Autofilm)

file(GLOB_RECURSE EMPTYEXAMPLE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/apps/emptyExample/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/apps/emptyExample/src/*.h"
)

add_executable(AutofilmDevelopment ${EMPTYEXAMPLE_SOURCES})

# set_target_properties(Autofilm PROPERTIES
    # RUNTIME_OUTPUT_DIRECTORY ${outputdir}
# )/
set_target_properties(Autofilm PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${outputdir}
)

target_compile_definitions(AutofilmDevelopment PRIVATE
    AF_PLATFORM_WINDOWS
)

target_compile_options(AutofilmDevelopment PRIVATE
    $<$<CONFIG:Debug>:/DDEBUG /Zi>
    $<$<CONFIG:Release>:/O2>
)

target_link_libraries(AutofilmDevelopment PUBLIC Autofilm)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "AutofilmDevelopment")
