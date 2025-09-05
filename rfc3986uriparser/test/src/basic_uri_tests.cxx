#include "../include/test_common.hpp"

#ifdef BASIC_URI_TESTS
// Basic parsing tests
TEST_F(URIParserTest, ParsesHttpUri) {
  auto uri = parser_->parse(
      "http://www.apple.com/store/products?mychoice=macpro#techspecs");
  EXPECT_EQ(uri->scheme(), "http");
  EXPECT_EQ(uri->host(), "www.apple.com");
  EXPECT_EQ(uri->path(), "/store/products");
  EXPECT_EQ(uri->query(), "mychoice=macpro");
  EXPECT_EQ(uri->fragment(), "techspecs");
  EXPECT_EQ(uri->authority(),
            "www.apple.com"); // Authority is parsed into components
}

TEST_F(URIParserTest, ParsesMailtoUri) {
  auto uri = parser_->parse("mailto:adam.blake@threatspike.com");
  EXPECT_EQ(uri->scheme(), "mailto");
  EXPECT_EQ(uri->path(), "adam.blake@threatspike.com");
  EXPECT_TRUE(uri->query().empty());
  EXPECT_TRUE(uri->fragment().empty());
  EXPECT_TRUE(uri->host().empty());
}

TEST_F(URIParserTest, ParsesUriWithPort) {
  auto uri = parser_->parse("https://example.com:8080/path");
  EXPECT_EQ(uri->scheme(), "https");
  EXPECT_EQ(uri->host(), "example.com");
  EXPECT_EQ(uri->port().value(), 8080);
  EXPECT_EQ(uri->path(), "/path");
}

TEST_F(URIParserTest, ParsesUriWithUserInfo) {
  auto uri = parser_->parse("ftp://user:password@ftp.example.com/files");

  EXPECT_EQ(uri->scheme(), "ftp");
  EXPECT_EQ(uri->userinfo(), "user:password");
  EXPECT_EQ(uri->host(), "ftp.example.com");
  EXPECT_EQ(uri->path(), "/files");
}

TEST_F(URIParserTest, ParsesUriWithEmptyComponents) {
  auto uri = parser_->parse("http://example.com");

  EXPECT_EQ(uri->scheme(), "http");
  EXPECT_EQ(uri->host(), "example.com");
  EXPECT_TRUE(uri->path().empty());
  EXPECT_TRUE(uri->query().empty());
  EXPECT_TRUE(uri->fragment().empty());
}
#endif
