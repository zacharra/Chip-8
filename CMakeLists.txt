cmake_minimum_required(VERSION 3.1)

project(Chip-8)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_SOURCE_DIR}/external/sdl2-cmake-scripts")
set(CMAKE_CXX_STANDARD 17)

include(AddCompilerFlags)
include(FilesystemSupport)

add_subdirectory(external/fmt)

include_directories(
    external/fmt/include
)

global_add_compiler_flags(-Wall -pedantic)

add_subdirectory(src)
add_subdirectory(sdl)

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")
