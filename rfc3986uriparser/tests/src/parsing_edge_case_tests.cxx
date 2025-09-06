#include "../include/test_common.hpp"
#ifdef PARSING_EDGE_CASE_TESTS
TEST_F(URIParserTest, ParsesUriWithComplexQuery) {
  auto uri =
      parser_->parse("https://example.com/search?q=test&page=1&sort=desc");

  EXPECT_EQ(uri->scheme(), "https");
  EXPECT_EQ(uri->host(), "example.com");
  EXPECT_EQ(uri->query(), "q=test&page=1&sort=desc");
}

// Percent decoding tests
TEST_F(URIParserTest, PercentDecoding) {
  std::string encoded = "hello%20world%21";
  std::string decoded = URIParser::percent_decode(encoded);
  EXPECT_EQ(decoded, "hello world!");

  encoded = "%41%42%43";
  decoded = URIParser::percent_decode(encoded);
  EXPECT_EQ(decoded, "ABC");

  encoded = "normal%20text%2Fwith%2Fslashes";
  decoded = URIParser::percent_decode(encoded);
  EXPECT_EQ(decoded, "normal text/with/slashes");
}

// Test URI with special characters
TEST_F(URIParserTest, UriWithSpecialCharacters) {
  auto uri = parser_->parse(
      "http://example.com/path%20with%20spaces?key=value%26with%3Dequals");

  EXPECT_EQ(uri->scheme(), "http");
  EXPECT_EQ(uri->host(), "example.com");
  EXPECT_EQ(uri->path(), "/path%20with%20spaces");
  EXPECT_EQ(uri->query(), "key=value%26with%3Dequals");
}

// Test multiple consecutive parsings
TEST_F(URIParserTest, MultipleParsings) {
  std::vector<std::string> test_uris = {
      "http://example.com", "https://user:pass@host:8080/path",
      "mailto:test@example.com", "ftp://ftp.example.com/files"};

  for (const auto &uri_str : test_uris) {
    EXPECT_NO_THROW({
      auto uri = parser_->parse(uri_str);
      EXPECT_FALSE(uri->to_string().empty());
    });
  }
}
#endif
