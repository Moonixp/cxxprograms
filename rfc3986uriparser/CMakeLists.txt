cmake_minimum_required(VERSION 3.5)
project(rfc3986_uri_parser)

# Set the C++ standard for the entire project
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Step 1: Add the 'lib' subdirectory.
# This command tells CMake to process the CMakeLists.txt file inside the 'lib' folder.
# Crucially, it makes the 'uri_parser_lib' target created there visible in this scope.
add_subdirectory(lib)

# Step 2: Add the main executable.
add_executable(uri-parser main.cxx)

# Step 3: Link the main executable to the library.
# The 'uri-parser' executable needs the code from 'uri_parser_lib'.
# CMake now recognizes 'uri_parser_lib' because it was added in the previous step.
# PRIVATE indicates that the 'uri-parser' executable's dependency on the library
# is an internal detail and should not be passed on to other targets.
target_link_libraries(uri-parser PRIVATE uri_parser_lib)


# Define options that will appear in the GUI
option(ENABLE_DEBUG "Enable debug build with -DDEBUG" OFF)
option(ENABLE_VERBOSE "Enable verbose output" OFF)
option(ENABLE_TESTING "Build tests" ON)
option(USE_STATIC_LIBS "Link statically" OFF)

# Set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Apply the debug option
if(ENABLE_DEBUG)
    add_definitions(-DDEBUG)
    set(CMAKE_BUILD_TYPE Debug)
    message(STATUS "Debug mode enabled (-DDEBUG added)")
endif()

if(ENABLE_VERBOSE)
    add_definitions(-DVERBOSE)
    message(STATUS "Verbose mode enabled")
endif()

