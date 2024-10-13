cmake_minimum_required(VERSION 3.15)

project(
    Autofilm
    VERSION 0.1.0
    DESCRIPTION "Builds the Autofilm library and example project"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(AUTOFILM_SRC "${CMAKE_SOURCE_DIR}/Autofilm/src")
set(AUTOFILM_EXT "${CMAKE_SOURCE_DIR}/Autofilm/external")
set(outputdir "${CMAKE_SOURCE_DIR}/bin")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# External dependencies
add_subdirectory(${AUTOFILM_EXT}/glfw)
find_package(OpenGL REQUIRED)
add_subdirectory(${AUTOFILM_EXT}/spdlog)
add_library(glad STATIC
    ${AUTOFILM_EXT}/glad/src/glad.c
)
target_include_directories(glad PUBLIC
    ${AUTOFILM_EXT}/glad/include
)

# Source files
add_library(Autofilm SHARED 
    ${AUTOFILM_SRC}/Autofilm.h
    ${AUTOFILM_SRC}/autofilmpch.cpp
    ${AUTOFILM_SRC}/autofilmpch.h
    ${AUTOFILM_SRC}/Core/App.cpp
    ${AUTOFILM_SRC}/Core/App.h
    ${AUTOFILM_SRC}/Core/Core.h
    ${AUTOFILM_SRC}/Core/EntryPoint.h
    ${AUTOFILM_SRC}/Core/Log.cpp
    ${AUTOFILM_SRC}/Core/Log.h
    ${AUTOFILM_SRC}/Core/Window.h
    ${AUTOFILM_SRC}/Events/Event.h
    ${AUTOFILM_SRC}/OpenGL/OpenGLAPI.cpp
    ${AUTOFILM_SRC}/OpenGL/OpenGLAPI.h
    ${AUTOFILM_SRC}/OpenGL/OpenGLWindow.cpp
    ${AUTOFILM_SRC}/OpenGL/OpenGLWindow.h
    ${AUTOFILM_SRC}/Renderer/RenderAPI.h
    ${AUTOFILM_SRC}/Renderer/Renderer.cpp
    ${AUTOFILM_SRC}/Renderer/Renderer.h
)

get_target_property(Autofilm_SOURCES Autofilm SOURCES)
source_group(TREE "${AUTOFILM_SRC}" FILES ${Autofilm_SOURCES})

set_property(TARGET Autofilm PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

target_include_directories(Autofilm PUBLIC
    ${AUTOFILM_EXT}/glad/include
    ${AUTOFILM_EXT}/glfw/include
    ${AUTOFILM_EXT}/spdlog/include
    ${AUTOFILM_EXT}/glm
    ${AUTOFILM_SRC}
)

target_link_libraries(Autofilm PUBLIC 
    OpenGL::GL
    glad
    glfw
    spdlog::spdlog
)

# set_target_properties(Autofilm PROPERTIES
#     LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
# )

set_target_properties(Autofilm PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${outputdir}"   # For DLL files
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${outputdir}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${outputdir}"LIBRARY_OUTPUT_DIRECTORY "${outputdir}"
    ARCHIVE_OUTPUT_DIRECTORY "${outputdir}"  # For static libs and import libs
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${outputdir}"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${outputdir}"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${outputdir}"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${outputdir}"
)

target_compile_definitions(Autofilm PRIVATE
    AF_PLATFORM_WINDOWS
    AF_BUILD_DLL
    $<$<CONFIG:Debug>:AF_ENABLE_ASSERTS>
)

target_precompile_headers(Autofilm PRIVATE ${AUTOFILM_SRC}/autofilmpch.h)

target_compile_options(Autofilm PRIVATE
    $<$<CONFIG:Debug>:/DDEBUG /Zi>
    $<$<CONFIG:Release>:/O2>
)


# EXAMPLE PROJECT
file(GLOB_RECURSE EMPTYEXAMPLE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/apps/emptyExample/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/apps/emptyExample/src/*.h"
)

add_executable(emptyExample ${EMPTYEXAMPLE_SOURCES})

# set_target_properties(emptyExample PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY "${outputdir}"
# )

set_target_properties(emptyExample PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${outputdir}"
    ARCHIVE_OUTPUT_DIRECTORY "${outputdir}"  # For static libs and import libs
    RUNTIME_OUTPUT_DIRECTORY "${outputdir}"   # For DLL files
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${outputdir}"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${outputdir}"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${outputdir}"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${outputdir}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${outputdir}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${outputdir}"
)

set_property(TARGET emptyExample PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

target_compile_definitions(emptyExample PRIVATE
    AF_PLATFORM_WINDOWS
)

target_compile_options(emptyExample PRIVATE
    $<$<CONFIG:Debug>:/DDEBUG /Zi>
    $<$<CONFIG:Release>:/O2>
)

target_link_libraries(emptyExample PUBLIC Autofilm)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "emptyExample")
