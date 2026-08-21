cmake_minimum_required(VERSION 3.13...4.3)

project(Brickview VERSION 1.0)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Core system
add_subdirectory(Soup)
add_subdirectory(Brickview)

# GUI Application
add_subdirectory(BrickviewApp)
