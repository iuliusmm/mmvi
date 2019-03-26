# Parse argtable version
FILE(STRINGS "version.txt" ARGTABLE_VERSION)

# All symbols will be automatically exported
SET(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Add shared library
ADD_LIBRARY(argtable SHARED argtable3.c)

# Set argtable version
SET_TARGET_PROPERTIES(argtable PROPERTIES VERSION ${ARGTABLE_VERSION})

TARGET_INCLUDE_DIRECTORIES(argtable PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})