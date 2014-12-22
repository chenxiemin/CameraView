cmake_minimum_required (VERSION 2.6)
project (CameraView)

# build type
set(CMAKE_BUILD_TYPE Debug CACHE STRING
		"Choose the type of build, options are: Debug Release")

# output directory
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/output)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/output)

add_subdirectory (Player)
add_subdirectory (TestPlayer)
