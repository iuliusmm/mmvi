CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(MMVI VERSION 1.0)

# Print version information
MESSAGE(STATUS "CMake version: ${CMAKE_VERSION}")
MESSAGE(STATUS "MMVI version: ${PROJECT_VERSION}")

# C++17 standard is required
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure a header file to pass some of the CMake settings to the source code
CONFIGURE_FILE("${PROJECT_SOURCE_DIR}/mmvi.hpp.in" "${PROJECT_BINARY_DIR}/mmvi.hpp")

# Add the binary tree to the search path for include files so that we will find mmvi.hpp
INCLUDE_DIRECTORIES("${PROJECT_BINARY_DIR}")

# Libraries installed with vcpkg
FIND_PACKAGE(fmt CONFIG REQUIRED)

ADD_SUBDIRECTORY(src)