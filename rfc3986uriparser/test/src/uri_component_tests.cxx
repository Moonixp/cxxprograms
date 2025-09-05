#include "../include/test_common.hpp"
#ifdef URI_COMPONENT_TESTS
// URI class method tests
TEST_F(URIParserTest, URISetterValidation) {
  URI uri;

  // Valid scheme
  EXPECT_NO_THROW(uri.set_scheme("https"));
  EXPECT_EQ(uri.scheme(), "https");

  // Invalid scheme
  EXPECT_THROW(uri.set_scheme("123invalid"), URIParseException);

  // Valid host
  EXPECT_NO_THROW(uri.set_host("example.com"));
  EXPECT_EQ(uri.host(), "example.com");

  // Valid port
  EXPECT_NO_THROW(uri.set_port(8080));
  EXPECT_EQ(uri.port().value(), 8080);
}

TEST_F(URIParserTest, BuildAuthorityMethod) {
  std::string authority =
      URI::build_authority("user:pass", "example.com", 8080);
  EXPECT_EQ(authority, "user:pass@example.com:8080");

  authority = URI::build_authority("", "example.com", std::nullopt);
  EXPECT_EQ(authority, "example.com");

  authority = URI::build_authority("user", "example.com", 80);
  EXPECT_EQ(authority, "user@example.com:80");
}

// ToString tests
TEST_F(URIParserTest, ToStringRoundTrip) {
  std::string original =
      "https://user:pass@example.com:8080/path?query=value#fragment";
  auto uri = parser_->parse(original);
  std::string reconstructed = uri->to_string();

  auto uri_recon = parser_->parse(reconstructed);

  EXPECT_EQ(uri->scheme(), uri_recon->scheme());
  EXPECT_EQ(uri->userinfo(), uri_recon->userinfo());
  EXPECT_EQ(uri->host(), uri_recon->host());
  EXPECT_EQ(uri->port(), uri_recon->port());
  EXPECT_EQ(uri->path(), uri_recon->path());
  EXPECT_EQ(uri->query(), uri_recon->query());
  EXPECT_EQ(uri->fragment(), uri_recon->fragment());
}

TEST_F(URIParserTest, ToStringPreservesStructure) {
  auto uri = parser_->parse("http://example.com/path?query#fragment");
  std::string result = uri->to_string();

  EXPECT_EQ(result, "http://example.com/path?query#fragment");
}

// Stream parsing tests
TEST_F(URIParserTest, ParsesFromStream) {
  std::istringstream iss("http://example.com/path");
  auto uri = parser_->parse(iss);

  EXPECT_EQ(uri->scheme(), "http");
  EXPECT_EQ(uri->host(), "example.com");
  EXPECT_EQ(uri->path(), "/path");
}

TEST_F(URIParserTest, ParsesMultipleFromStream) {
  std::istringstream iss(
      "http://example1.com/path1\nhttps://example2.com/path2");

  std::string line;
  std::getline(iss, line);
  auto uri1 = parser_->parse(line);
  EXPECT_EQ(uri1->scheme(), "http");
  EXPECT_EQ(uri1->host(), "example1.com");

  std::getline(iss, line);
  auto uri2 = parser_->parse(line);
  EXPECT_EQ(uri2->scheme(), "https");
  EXPECT_EQ(uri2->host(), "example2.com");
}
// Character classification tests
TEST_F(URIParserTest, CharacterClassification) {
  EXPECT_TRUE(URIParser::is_unreserved('a'));
  EXPECT_TRUE(URIParser::is_unreserved('Z'));
  EXPECT_TRUE(URIParser::is_unreserved('0'));
  EXPECT_TRUE(URIParser::is_unreserved('-'));
  EXPECT_TRUE(URIParser::is_unreserved('.'));
  EXPECT_TRUE(URIParser::is_unreserved('_'));
  EXPECT_TRUE(URIParser::is_unreserved('~'));
  EXPECT_FALSE(URIParser::is_unreserved('!'));

  EXPECT_TRUE(URIParser::is_scheme_char('a'));
  EXPECT_TRUE(URIParser::is_scheme_char('+'));
  EXPECT_TRUE(URIParser::is_scheme_char('-'));
  EXPECT_TRUE(URIParser::is_scheme_char('.'));
  EXPECT_FALSE(URIParser::is_scheme_char(' '));
}
#endif
