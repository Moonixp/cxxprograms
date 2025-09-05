/*
 * Author : Hyun Wo
 * Purpose : This software is free for use and modification by any third party.
 * No warranty is provided and the user is responsible for any consequences of
 * its use.
 * Date : 5th September 2025
 */

#ifndef URI_PARSER_H
#define URI_PARSER_H

#include "uri_utils.hpp"
#include <algorithm>
#include <cctype>
#include <cinttypes>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class URIParseException : public std::runtime_error {
public:
  explicit URIParseException(const std::string &message)
      : std::runtime_error(message) {}
};

class URI {
public:
  URI() = default;
  ~URI() = default;

  // Getters
  const std::string &scheme() const { return scheme_; }
  const std::string &authority() const { return authority_; }
  const std::string &userinfo() const { return userinfo_; }
  const std::string &host() const { return host_; }
  const std::optional<uint16_t> &port() const { return port_; }
  const std::string &path() const { return path_; }
  const std::string &query() const { return query_; }
  const std::string &fragment() const { return fragment_; }

  // Setters with validation
  void set_scheme(const std::string &scheme);
  void set_authority(const std::string &authority);
  void set_userinfo(const std::string &userinfo);
  void set_host(const std::string &host);
  void set_port(const std::optional<uint16_t> &port);
  void set_path(const std::string &path);
  void set_query(const std::string &query);
  void set_fragment(const std::string &fragment);

  // String representation
  std::string to_string() const;

  // Component builders
  static std::string build_authority(const std::string &userinfo,
                                     const std::string &host,
                                     const std::optional<uint16_t> &port);

  // Validation helpers
  static bool is_valid_scheme(const std::string &scheme);
  static bool is_valid_host(const std::string &host);
  static bool is_valid_port(uint16_t port);

private:
  std::string scheme_;
  std::string authority_;
  std::string userinfo_;
  std::string host_;
  std::optional<uint16_t> port_;
  std::string path_;
  std::string query_;
  std::string fragment_;

  // Validation helpers
  // static bool is_valid_scheme(const std::string &scheme);
  // static bool is_valid_host(const std::string &host);
  // static bool is_valid_port(uint16_t port);
};

class URIParser {

public:
  URIParser() = default;
  ~URIParser() = default;

  // Parse from string
  std::unique_ptr<URI> parse(const std::string &uri_string) const;

  // Parse from stream
  std::unique_ptr<URI> parse(std::istream &input) const;

  // Character classification
  static bool is_unreserved(char c);
  static bool is_gen_delim(char c);
  static bool is_sub_delim(char c);
  static bool is_reserved(char c);
  static bool is_scheme_char(char c);
  static bool is_userinfo_char(char c);
  static bool is_host_char(char c);
  static bool is_port_char(char c);
  static bool is_path_char(char c);
  static bool is_query_char(char c);
  static bool is_fragment_char(char c);

  // Percent decoding
  static std::string percent_decode(const std::string &input);
  static unsigned char hex_to_char(char high, char low);

  // Parsing helpers
  bool parse_scheme(std::istream &input, URI &uri) const;
  bool parse_host(std::istream &input, URI &uri) const;
  bool parse_authority(std::istream &input, URI &uri) const;
  void parse_userinfo_host_port(const std::string &authority, URI &uri) const;
  bool parse_path(std::istream &input, URI &uri) const;
  bool parse_query(std::istream &input, URI &uri) const;
  bool parse_fragment(std::istream &input, URI &uri) const;

private:
  // Parsing helpers
  // bool parse_scheme(std::istream &input, URI &uri) const;
  // bool parse_authority(std::istream &input, URI &uri) const;
  // void parse_userinfo_host_port(const std::string &authority, URI &uri)
  // const; bool parse_path(std::istream &input, URI &uri) const; bool
  // parse_query(std::istream &input, URI &uri) const; bool
  // parse_fragment(std::istream &input, URI &uri) const;

  // Character classification
  // static bool is_unreserved(char c);
  // static bool is_gen_delim(char c);
  // static bool is_sub_delim(char c);
  // static bool is_reserved(char c);
  // static bool is_scheme_char(char c);
  // static bool is_userinfo_char(char c);
  // static bool is_host_char(char c);
  // static bool is_port_char(char c);
  // static bool is_path_char(char c);
  // static bool is_query_char(char c);
  // static bool is_fragment_char(char c);
  //
  // // Percent decoding
  // static std::string percent_decode(const std::string &input);
  // static unsigned char hex_to_char(char high, char low);
};

#endif // URI_PARSER_H
