cmake_minimum_required(VERSION 3.9)

project(renoir LANGUAGES CXX C)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(RENOIR_BUILD_EXAMPLES "Build example applications that showcase the renoir libraries." ON)
option(RENOIR_UNITY_BUILD "Combine all renoir source files into one jumbo build." ON)
option(RENOIR_USE_LOCAL_MN "Uses the local mn submodule in renoir" ON)

if (RENOIR_USE_LOCAL_MN)
	add_subdirectory(external/mn EXCLUDE_FROM_ALL)
endif()
add_subdirectory(external/glew EXCLUDE_FROM_ALL)

add_subdirectory(external/glslang-8.13.3743 EXCLUDE_FROM_ALL)
add_subdirectory(renoir-window)
add_subdirectory(renoir-gl450)

add_library(renoir INTERFACE)
target_include_directories(renoir INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (RENOIR_BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()
