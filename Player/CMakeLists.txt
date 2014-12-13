cmake_minimum_required (VERSION 2.6)

MESSAGE( STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE} )

# include
include_directories("../third-party/include")
if (MSVC)
	include_directories("../third-party/include/SDL2-win32")
else (MSVC)
	include_directories("../third-party/include/SDL2")
endif (MSVC)

# cflags
# set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}")
if (CMAKE_COMPILER_IS_GNUCC)
    # set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -ggdb3 -O0 -std=gnu++11 -D__STDC_CONSTANT_MACROS")
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -O3 -std=gnu++11 -D__STDC_CONSTANT_MACROS")
endif (CMAKE_COMPILER_IS_GNUCC)

# source file
FILE (GLOB SRC *.cpp)
add_library (player ${SRC})

# for other includes this module
target_include_directories (player PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# link library
if (MSVC)
	set(LD_PATH "${PROJECT_SOURCE_DIR}/third-party/lib/win32")
else (MSVC)
	set(LD_PATH "${PROJECT_SOURCE_DIR}/third-party/lib/linux")
endif (MSVC)
target_link_libraries(player ${LD_PATH}/avformat)
target_link_libraries(player ${LD_PATH}/avcodec)
target_link_libraries(player ${LD_PATH}/avutil)
target_link_libraries(player ${LD_PATH}/swscale)
target_link_libraries(player ${LD_PATH}/SDL2)
if (MSVC)
    target_link_libraries(player ${LD_PATH}/vld.lib)
endif (MSVC)
if (CMAKE_COMPILER_IS_GNUCC)
	target_link_libraries(player ${LD_PATH}/pthread)
endif (CMAKE_COMPILER_IS_GNUCC)

add_custom_command(TARGET player POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/third-party/lib/win32/avcodec-55.dll"
        "${PROJECT_BINARY_DIR}/output/${CMAKE_BUILD_TYPE}/")
add_custom_command(TARGET player POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/third-party/lib/win32/avformat-55.dll"
        "${PROJECT_BINARY_DIR}/output/${CMAKE_BUILD_TYPE}/")
add_custom_command(TARGET player POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/third-party/lib/win32/avutil-52.dll"
        "${PROJECT_BINARY_DIR}/output/${CMAKE_BUILD_TYPE}/")
add_custom_command(TARGET player POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/third-party/lib/win32/SDL2.dll"
        "${PROJECT_BINARY_DIR}/output/${CMAKE_BUILD_TYPE}/")
add_custom_command(TARGET player POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/third-party/lib/win32/swscale-2.dll"
        "${PROJECT_BINARY_DIR}/output/${CMAKE_BUILD_TYPE}/")
