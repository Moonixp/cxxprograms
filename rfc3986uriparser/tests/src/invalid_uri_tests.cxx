#include "../include/test_common.hpp"

#ifdef INVALID_URI_TESTS
// Invalid URI tests
TEST_F(URIParserTest, ThrowsExceptionForEmptyScheme) {
  EXPECT_THROW(parser_->parse("://example.com"), URIParseException);
}

TEST_F(URIParserTest, HandlesEmptyUriGracefully) {
  auto uri = parser_->parse("");

  EXPECT_TRUE(uri->scheme().empty());
  EXPECT_TRUE(uri->path().empty());
  EXPECT_TRUE(uri->query().empty());
  EXPECT_TRUE(uri->fragment().empty());
}
#endif
