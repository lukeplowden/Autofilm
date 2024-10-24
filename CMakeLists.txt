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
find_package(Vulkan REQUIRED)
add_subdirectory(${AUTOFILM_EXT}/spdlog)

# GStreamer
if(DEFINED ENV{GSTREAMER_1_0_ROOT_MSVC_X86_64})
    set(GSTREAMER_ROOT $ENV{GSTREAMER_1_0_ROOT_MSVC_X86_64})
else()
    message(FATAL_ERROR "GStreamer is not installed, or Autofilm does not currently support this platform.")
endif()
include_directories(
    ${GSTREAMER_ROOT}include/gstreamer-1.0
    ${GSTREAMER_ROOT}include/glib-2.0
    ${GSTREAMER_ROOT}lib/glib-2.0/include
    ${GSTREAMER_ROOT}lib/gstreamer-1.0/include
    ${GSTREAMER_ROOT}
)
link_directories(
    ${GSTREAMER_ROOT}/lib
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
    ${AUTOFILM_SRC}/Core/File.cpp
    ${AUTOFILM_SRC}/Core/File.h
    ${AUTOFILM_SRC}/Core/Window.h
    ${AUTOFILM_SRC}/Core/WindowManager.cpp
    ${AUTOFILM_SRC}/Events/Event.h
    ${AUTOFILM_SRC}/Core/ThreadPool.h
    ${AUTOFILM_SRC}/Vulkan/VulkanAPI.cpp
    ${AUTOFILM_SRC}/Vulkan/VulkanAPI.h
    ${AUTOFILM_SRC}/Vulkan/VulkanUtils.cpp
    ${AUTOFILM_SRC}/Vulkan/VulkanUtils.h
    ${AUTOFILM_SRC}/Vulkan/VulkanWindow.cpp
    ${AUTOFILM_SRC}/Vulkan/VulkanWindow.h
    ${AUTOFILM_SRC}/Renderer/RenderAPI.h
    ${AUTOFILM_SRC}/Renderer/Renderer.cpp
    ${AUTOFILM_SRC}/Renderer/Renderer.h
    ${AUTOFILM_SRC}/Video/VideoPlayer.h
)

get_target_property(Autofilm_SOURCES Autofilm SOURCES)
source_group(TREE "${AUTOFILM_SRC}" FILES ${Autofilm_SOURCES})

set_property(TARGET Autofilm PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

target_include_directories(Autofilm PUBLIC
    $ENV{VULKAN_SDK}/include
    ${AUTOFILM_EXT}/glfw/include
    ${AUTOFILM_EXT}/spdlog/include
    ${AUTOFILM_EXT}/glm
    ${AUTOFILM_SRC}
)

target_link_libraries(Autofilm PUBLIC 
    Vulkan::Vulkan
    glfw
    spdlog::spdlog
    gstreamer-1.0
    gstbase-1.0
    gstaudio-1.0
    glib-2.0
    gobject-2.0
)    
set_target_properties(Autofilm PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${outputdir}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${outputdir}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${outputdir}"
    LIBRARY_OUTPUT_DIRECTORY "${outputdir}"
    ARCHIVE_OUTPUT_DIRECTORY "${outputdir}"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${outputdir}"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${outputdir}"
    LIBRARY_OUTPUT_DIRECTORY "${outputdir}"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${outputdir}"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${outputdir}"
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

set_target_properties(emptyExample PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${outputdir}"
    ARCHIVE_OUTPUT_DIRECTORY "${outputdir}"
    RUNTIME_OUTPUT_DIRECTORY "${outputdir}"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${outputdir}"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${outputdir}"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${outputdir}"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${outputdir}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${outputdir}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${outputdir}"
)

set_property(TARGET emptyExample PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# target_link_options(emptyExample PRIVATE /NODEFAULTLIB:LIBCMT)

target_compile_definitions(emptyExample PRIVATE
    AF_PLATFORM_WINDOWS
)

target_compile_options(emptyExample PRIVATE
    $<$<CONFIG:Debug>:/DDEBUG /Zi>
    $<$<CONFIG:Release>:/O2>
)

target_link_libraries(emptyExample PUBLIC Autofilm)


set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "emptyExample")