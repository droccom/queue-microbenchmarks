cmake_minimum_required(VERSION 2.8.12)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (default Release)" FORCE)
endif()


# No in-tree build allowed.
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR
    "In-source build are not allowed.
Please create a directory directory and run cmake from there, passing the path
to this source directory as the last argumente.
This process created the file `CMakeCache.txt' and the directory `CMakeFiles'.
Please delete them.")
endif()


string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)

if (CMAKE_BUILD_TYPE AND
    NOT uppercase_CMAKE_BUILD_TYPE MATCHES "^(DEBUG|RELEASE|RELWITHDEBINFO|MINSIZEREL)$")
  message(FATAL_ERROR "Invalid value for CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
endif()


set(MAIN_SRC_DIR     ${CMAKE_CURRENT_SOURCE_DIR})
set(INCLUDE_DIR ${MAIN_SRC_DIR}/include)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# check for dependencies
find_package(Threads REQUIRED)

include_directories(${INCLUDE_DIR})
include_directories(${THREADS_PTHREADS_INCLUDE_DIR})

add_subdirectory(src)
