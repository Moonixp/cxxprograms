#include "../../lib/include/uri_parser.hpp"
#include <gtest/gtest.h>
#include <sstream>
#define ALL_TESTS
#ifdef ALL_TESTS
#define BASIC_URI_TESTS
#define INVALID_URI_TESTS
#define IPV6_TESTS
#define PARSING_EDGE_CASE_TESTS
#define RFC_3986_CASES_TESTS
#define URI_COMPONENT_TESTS
#endif

class URIParserTest : public ::testing::Test {
protected:
  void SetUp() override { parser_ = std::make_unique<URIParser>(); }
  void TearDown() override { parser_.reset(); }
  std::unique_ptr<URIParser> parser_;
};

#include "basic_uri_tests.cxx"
#include "invalid_uri_tests.cxx"
#include "ipv6_tests.cxx"
#include "parsing_edge_case_tests.cxx"
#include "rfc_3986_cases_tests.cxx"
#include "uri_component_tests.cxx"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
