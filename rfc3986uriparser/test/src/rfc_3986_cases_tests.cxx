#include "../include/test_common.hpp"
#ifdef RFC_3986_CASES_TESTS
// Complex URI examples from RFC 3986
TEST_F(URIParserTest, RFC3986Examples) {
  // Example from RFC 3986 section 1.1.2
  auto uri = parser_->parse("ftp://ftp.is.co.za/rfc/rfc1808.txt");
  EXPECT_EQ(uri->scheme(), "ftp");
  EXPECT_EQ(uri->host(), "ftp.is.co.za");
  EXPECT_EQ(uri->path(), "/rfc/rfc1808.txt");

  // Another example
  uri = parser_->parse("http://www.ietf.org/rfc/rfc2396.txt");
  EXPECT_EQ(uri->scheme(), "http");
  EXPECT_EQ(uri->host(), "www.ietf.org");
  EXPECT_EQ(uri->path(), "/rfc/rfc2396.txt");

  // URI with query parameters
  uri = parser_->parse("ldap://[2001:db8::7]/c=GB?objectClass?one");
  EXPECT_EQ(uri->scheme(), "ldap");
  EXPECT_EQ(uri->host(), "[2001:db8::7]");
  EXPECT_EQ(uri->path(), "/c=GB");
  EXPECT_EQ(uri->query(), "objectClass?one");
}
#endif
