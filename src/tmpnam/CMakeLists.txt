ADD_EXECUTABLE(tmpnam tmpnam.cpp)
TARGET_LINK_LIBRARIES(tmpnam PRIVATE argtable fmt::fmt)