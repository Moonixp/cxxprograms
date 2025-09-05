cmake_minimum_required(VERSION 3.5)

project(rfc3986_uri_parser_tests)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Test options
option(ENABLE_ALL_TESTS "Enable all test suites" ON)
option(ENABLE_BASIC_URI_TESTS "Enable basic URI tests" OFF)
option(ENABLE_INVALID_URI_TESTS "Enable invalid URI tests" OFF)
option(ENABLE_IPV6_TESTS "Enable IPv6 tests" OFF)
option(ENABLE_PARSING_EDGE_CASE_TESTS "Enable parsing edge case tests" OFF)
option(ENABLE_RFC_3986_CASES_TESTS "Enable RFC 3986 cases tests" OFF)
option(ENABLE_URI_COMPONENT_TESTS "Enable URI component tests" OFF)

if(ENABLE_ALL_TESTS)
    add_definitions(-DALL_TESTS)
    message(STATUS "All tests enabled")
else()
    if(ENABLE_BASIC_URI_TESTS)
        add_definitions(-DBASIC_URI_TESTS)
        message(STATUS "Basic URI tests enabled")
    endif()
    
    if(ENABLE_INVALID_URI_TESTS)
        add_definitions(-DINVALID_URI_TESTS)
        message(STATUS "Invalid URI tests enabled")
    endif()
    
    if(ENABLE_IPV6_TESTS)
        add_definitions(-DIPV6_TESTS)
        message(STATUS "IPv6 tests enabled")
    endif()
    
    if(ENABLE_PARSING_EDGE_CASE_TESTS)
        add_definitions(-DPARSING_EDGE_CASE_TESTS)
        message(STATUS "Parsing edge case tests enabled")
    endif()
    
    if(ENABLE_RFC_3986_CASES_TESTS)
        add_definitions(-DRFC_3986_CASES_TESTS)
        message(STATUS "RFC 3986 cases tests enabled")
    endif()
    
    if(ENABLE_URI_COMPONENT_TESTS)
        add_definitions(-DURI_COMPONENT_TESTS)
        message(STATUS "URI component tests enabled")
    endif()
endif()


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB TEST_SOURCES "src/*.cxx")
file(GLOB URI_SOURCES "../lib/src/*.cxx")

add_executable(
  uri_parser_tests
  ${URI_SOURCES}
  ${TEST_SOURCES}
)

target_include_directories(uri_parser_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(uri_parser_tests PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../lib/include)

target_link_libraries(
  uri_parser_tests
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(uri_parser_tests)

#
# if(CONFIG_MY_OPTION)
#     message("My option is enabled!")
#     add_definitions(-DMY_FEATURE_ENABLED) # Define a preprocessor macro
# endif()
# # config cmake autoconf
# option(ALL_TESTS "Enable all tests" ON)
# if(ALL_TESTS)
#     set(ALL_TESTS_STRING 1)
# endif()
# configure_file(config.h.in config.h @ONLY)
# include_directories(${CMAKE_CURRENT_BINARY_DIR})
#
