cmake_minimum_required (VERSION 2.6)

# include
include_directories("../third-party/include/SDL2")
# if (MSVC)
#     include_directories("${PROJECT_SOURCE_DIR}/include/win32")
# endif (MSVC)
# if (CMAKE_COMPILER_IS_GNUCC)
#     include_directories("/usr/local/include/SDL2")
# endif (CMAKE_COMPILER_IS_GNUCC)

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

# link lib
# if (MSVC)
# 	target_link_libraries(testmpeg ${PROJECT_SOURCE_DIR}/lib/win32/SDL2)
# 	target_link_libraries(testmpeg ${PROJECT_SOURCE_DIR}/lib/win32/SDL2main)
# 	target_link_libraries(testmpeg ${PROJECT_SOURCE_DIR}/lib/win32/avformat)
# 	target_link_libraries(testmpeg ${PROJECT_SOURCE_DIR}/lib/win32/avcodec)
# 	target_link_libraries(testmpeg ${PROJECT_SOURCE_DIR}/lib/win32/avutil)
# 	target_link_libraries(testmpeg ${PROJECT_SOURCE_DIR}/lib/win32/swscale)
# 	target_link_libraries(testmpeg ${PROJECT_SOURCE_DIR}/lib/win32/jpeg)
#     link_directories(./lib/windows/x86)
# endif (MSVC)

set(LD_PATH "${PROJECT_SOURCE_DIR}/third-party/lib/linux")
if (CMAKE_COMPILER_IS_GNUCC)
    target_link_libraries(player -lavformat)
    target_link_libraries(player -lavcodec)
    target_link_libraries(player -lavutil)
    target_link_libraries(player -lswscale)
    target_link_libraries(player -lpthread)
    target_link_libraries(player ${LD_PATH}/libSDL2.so)
endif (CMAKE_COMPILER_IS_GNUCC)

