cmake_minimum_required (VERSION 2.6)

include_directories("../third-party/include")

# source files
FILE (GLOB SRC *.cpp)
add_executable (test-player ${SRC})

# linked library
target_link_libraries (test-player LINK_PUBLIC player)
