add_library(uri_parser_lib STATIC
    src/uri_parser.cxx
    src/uri_utils.cxx
)

target_include_directories(uri_parser_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

