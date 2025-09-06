cmake_minimum_required(VERSION 3.5)
project(rfc3986_uri_parser)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib)

add_executable(uri-parser src/main.cxx)
target_link_libraries(uri-parser PRIVATE uri_parser_lib)

option(ENABLE_DEBUG "Enable debug build with -DDEBUG" OFF)
option(ENABLE_VERBOSE "Enable verbose output" OFF)
option(ENABLE_TESTING "Build tests" ON)
option(USE_STATIC_LIBS "Link statically" OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

if(ENABLE_DEBUG)
    add_definitions(-DDEBUG)
    set(CMAKE_BUILD_TYPE Debug)
    message(STATUS "Debug mode enabled (-DDEBUG added)")
endif()

if(ENABLE_VERBOSE)
    add_definitions(-DVERBOSE)
    message(STATUS "Verbose mode enabled")
endif()

