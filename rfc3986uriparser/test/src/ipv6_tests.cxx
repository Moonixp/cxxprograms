#include "../include/test_common.hpp"

#ifdef IPV6_TESTS
TEST_F(URIParserTest, ParsesUriWithIPv6Host) {
  auto uri = parser_->parse("http://[2001:db8::1]:8080/path");

  EXPECT_EQ(uri->scheme(), "http");
  EXPECT_EQ(uri->host(), "[2001:db8::1]");
  EXPECT_EQ(uri->port().value(), 8080);
  EXPECT_EQ(uri->path(), "/path");
}
#endif
