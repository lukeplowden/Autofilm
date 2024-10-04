cmake_minimum_required(VERSION 3.10)

# Set the project name and architecture
project(Autofilm)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_subdirectory(Autofilm/external/glfw)
find_package(OpenGL REQUIRED)

# Define build configurations
set(CMAKE_CONFIGURATION_TYPES "Debug;" CACHE STRING "" FORCE)

# Output directory
set(outputdir "")

# Define source files for the Autofilm library
file(GLOB AUTOFILM_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Autofilm/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Autofilm/src/*.h"
)

# Autofilm project
add_library(Autofilm SHARED ${AUTOFILM_SOURCES})

target_include_directories(Autofilm PUBLIC
    ${CMAKE_SOURCE_DIR}/Autofilm/src
    ${CMAKE_SOURCE_DIR}/Autofilm/src
)

set_target_properties(Autofilm PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${outputdir}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${outputdir}
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${outputdir}
)

target_compile_definitions(Autofilm PRIVATE
    AF_PLATFORM_WINDOWS
    AF_BUILD_DLL
)

target_compile_options(Autofilm PRIVATE
    $<$<CONFIG:Debug>:-DAF_DEBUG>
    $<$<CONFIG:Release>:-DAF_RELEASE>
    $<$<CONFIG:Dist>:-DAF_DIST>
)

target_compile_options(Autofilm PRIVATE
    $<$<CONFIG:Debug>:/DDEBUG /Zi>
    $<$<CONFIG:Release>:/O2>
    $<$<CONFIG:Dist>:/O2>
)

add_custom_command(TARGET Autofilm POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:Autofilm>
    ${CMAKE_BINARY_DIR}/bin/${outputdir}/emptyExample
)

# emptyExample project
file(GLOB EMPTYEXAMPLE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/apps/emptyExample/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/apps/emptyExample/src/*.h"
)

add_executable(emptyExample ${EMPTYEXAMPLE_SOURCES})

target_include_directories(emptyExample PRIVATE
${CMAKE_SOURCE_DIR}/Autofilm/src/
${CMAKE_SOURCE_DIR}/Autofilm/src/
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
    $<$<CONFIG:Debug>:-DAF_DEBUG>
    $<$<CONFIG:Release>:-DAF_RELEASE>
    $<$<CONFIG:Dist>:-DAF_DIST>
)

target_compile_options(emptyExample PRIVATE
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Dist>:-O2>
)

target_link_libraries(emptyExample PRIVATE Autofilm)