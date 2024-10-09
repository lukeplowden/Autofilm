cmake_minimum_required(VERSION 3.15)

project(
    ProjectGenerator
    VERSION 0.1.0
    DESCRIPTION "Builds the Autofilm library and example project"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(Autofilm)
target_link_libraries(ProjectGenerator PRIVATE Autofilm)