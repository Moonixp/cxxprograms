#include "../include/uri_parser.hpp"
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>

void URI::set_scheme(const std::string &scheme) {
  if (!is_valid_scheme(scheme)) {
    throw URIParseException("Invalid scheme: " + scheme);
  }
  scheme_ = uri_utils::to_lower(scheme);
}

void URI::set_authority(const std::string &authority) {
  authority_ = authority;
}

void URI::set_userinfo(const std::string &userinfo) { userinfo_ = userinfo; }

void URI::set_host(const std::string &host) {
  if (!is_valid_host(host)) {
    throw URIParseException("Invalid host: " + host);
  }
  host_ = host;
}

void URI::set_port(const std::optional<uint16_t> &port) {
  if (port.has_value() && !is_valid_port(port.value())) {
    throw URIParseException("Invalid port: " + std::to_string(port.value()));
  }
  port_ = port;
}

void URI::set_path(const std::string &path) { path_ = path; }

void URI::set_query(const std::string &query) { query_ = query; }

void URI::set_fragment(const std::string &fragment) { fragment_ = fragment; }

/**
 * String representation of the URI.
 *
 * The string representation is defined as follows:
 *   scheme "://" authority path "?" query "#" fragment
 * Where:
 *   - scheme is the URI scheme (e.g., http, https, ftp, etc.)
 *   - authority is the URI authority (e.g., hostname, port, etc.)
 *   - path is the URI path
 *   - query is the URI query
 *   - fragment is the URI fragment
 *
 * Examples:
 *   http://example.com/path?query#fragment
 *   https://example.com:8080/path?query#fragment
 *   ftp://example.com/path?query#fragment
 *   //example.com:8080/path?query#fragment
 *
 * @return A string representation of the URI.
 */
std::string URI::to_string() const {
  std::ostringstream oss;

  if (!scheme_.empty()) {
    oss << scheme_ << ":";
  }

  if (!authority_.empty()) {
    oss << "//" << authority_;
  }

  oss << path_;

  if (!query_.empty()) {
    oss << "?" << query_;
  }

  if (!fragment_.empty()) {
    oss << "#" << fragment_;
  }

  return oss.str();
}

/**
 * Construct an authority string from the given userinfo, host and port.
 *
 * If userinfo is not empty, it is prepended to the host with an "@" separator.
 * If port is not empty, it is appended to the host with a ":" separator.
 *
 * \param userinfo The userinfo part (can be empty).
 * \param host The host part.
 * \param port The port part (can be empty).
 * \return The constructed authority string.
 */
std::string URI::build_authority(const std::string &userinfo,
                                 const std::string &host,
                                 const std::optional<uint16_t> &port) {
  std::ostringstream oss;

  if (!userinfo.empty()) {
    oss << userinfo << "@";
  }

  oss << host;

  if (port.has_value()) {
    oss << ":" << port.value();
  }

  return oss.str();
}

bool URI::is_valid_scheme(const std::string &scheme) {
  if (scheme.empty())
    return false;
  if (!std::isalpha(scheme[0]))
    return false;

  for (char c : scheme) {
    if (!URIParser::is_scheme_char(c)) {
      return false;
    }
  }
  return true;
}

bool URI::is_valid_host(const std::string &host) { return !host.empty(); }

bool URI::is_valid_port(uint16_t port) { return port > 0; }

/**
 * Parses a URI from a string.
 *
 * @param uri_string URI string to parse
 * @return A pointer to the parsed URI object
 */
std::unique_ptr<URI> URIParser::parse(const std::string &uri_string) const {
  std::istringstream iss(uri_string);
  return parse(iss);
}

/**
 * Parses a URI from an input stream.
 *
 * @param input Input stream with the URI to parse
 * @return A pointer to the parsed URI object
 * @throws URIParseException If the input stream is empty or a URI component
 * is invalid
 */
std::unique_ptr<URI> URIParser::parse(std::istream &input) const {
  auto uri = std::make_unique<URI>();
  std::istream::pos_type start_pos = input.tellg();

  try {
    // Parse scheme
    if (!parse_scheme(input, *uri)) {
      input.seekg(start_pos);
    }

    // Check for authority / host after (//)
    if (input.peek() == '/') {
      input.get();

      if (input.peek() == '/') {
        input.get();
        // input.unget(); // Put back the first '/'
        parse_authority(input, *uri);
      } else {
        input.seekg(-1, std::ios_base::cur); // Put back the character
      }
    }

    // Parse path
    parse_path(input, *uri);

    // Parse query
    if (input.peek() == '?') {
      input.get(); // Consume '?'
      parse_query(input, *uri);
    }

    // Parse fragment
    if (input.peek() == '#') {
      input.get(); // Consume '#'
      parse_fragment(input, *uri);
    }
  } catch (const std::exception &e) {
    throw URIParseException(std::string("Failed to parse URI: ") + e.what());
  }

  return uri;
}

/**
 * Parses a URI scheme from an input stream.
 *
 * @param input Input stream with the URI to parse
 * @param uri URI object to store the parsed scheme
 * @return true if a scheme was parsed, false otherwise
 * @throws URIParseException If the input stream is empty or a URI component
 * is invalid
 */
bool URIParser::parse_scheme(std::istream &input, URI &uri) const {

  std::string scheme;
  char c;

  // rfc3986 needs first char to be alpha
  bool first_char_trip = true;

  while (input.get(c)) {
    if (first_char_trip) {
      if (!std::isalpha(c)) {
        input.unget();
        throw URIParseException("Invalid scheme");
        return false;
      }
      scheme += c;
      first_char_trip = false;
    } else {
      if (c == ':') {
        uri.set_scheme(scheme);
        return true;
      } else if (is_scheme_char(c)) {
        scheme += c;
      } else {
        // Not a scheme character, rewind
        for (int i = 0; i < static_cast<int>(scheme.size()) + 1; ++i) {
          input.unget();
        }
        return false;
      }
    }
  }

  // End of stream before ':'
  for (int i = 0; i < static_cast<int>(scheme.size()); ++i) {
    input.unget();
  }

  return false;
}

/**
 * Parses a URI authority from an input stream.
 *
 * @param input Input stream with the URI to parse
 * @param uri URI object to store the parsed authority
 * @return true if an authority was parsed, false otherwise
 * @throws URIParseException If the input stream is empty or a URI component
 * is invalid
 */
bool URIParser::parse_authority(std::istream &input, URI &uri) const {

  std::string authority;
  char c;

  while (input.get(c)) {
    if (c == '/' || c == '?' || c == '#' || c == '\0') {
      input.unget(); // Put back the delimiter
      break;
    }
    authority += c;
  }

  if (!authority.empty()) {
    uri.set_authority(authority);
    parse_userinfo_host_port(authority, uri);
    return true;
  }

  return false;
}

/**
 * Extracts userinfo, host and port from the authority.
 *
 * authority format is [userinfo@]host[:port]
 *
 * @param authority The authority string to extract from
 * @param uri The URI object to store the parsed userinfo, host and port
 */
void URIParser::parse_userinfo_host_port(const std::string &authority,
                                         URI &uri) const {
  std::string userinfo, host;
  std::optional<uint16_t> port;

  size_t at_pos = authority.find('@');

  if (at_pos != std::string::npos) {
    userinfo = authority.substr(0, at_pos);
    host = authority.substr(at_pos + 1);
  } else {
    host = authority;
  }

  size_t port_colon_pos = std::string::npos;

  // if [ipv6] the end is ] else the end is :
  if (host[0] == '[') {
    port_colon_pos = host.find(']') + 1;
  } else {
    port_colon_pos = host.find(':');
  }

  if (port_colon_pos < host.size() && host[host.size()] != ':') {
    if (port_colon_pos != std::string::npos) {

      std::string port_str = host.substr(port_colon_pos + 1);
      host = host.substr(0, port_colon_pos);

      try {
        int port_num = std::stoi(port_str);
        if (port_num > 0 && port_num <= 65535) {
          port = static_cast<uint16_t>(port_num);
        }
      } catch (...) {
        // Invalid port, ignore
      }
    }
  }

  uri.set_userinfo(userinfo);
  uri.set_host(host);
  uri.set_port(port);
}

bool URIParser::parse_path(std::istream &input, URI &uri) const {
  std::string path;
  char c;

  while (input.get(c)) {
    if (c == '?' || c == '#' || c == '\0') {
      input.unget(); // Put back the delimiter
      break;
    }
    if (is_path_char(c)) {
      path += c;
    }
  }

  uri.set_path(path);
  return !path.empty();
}

bool URIParser::parse_query(std::istream &input, URI &uri) const {
  std::string query;
  char c;

  while (input.get(c)) {
    if (c == '#' || c == '\0') {
      input.unget(); // Put back the delimiter
      break;
    }
    if (is_query_char(c)) {
      query += c;
    }
  }

  uri.set_query(query);
  return !query.empty();
}

bool URIParser::parse_fragment(std::istream &input, URI &uri) const {
  std::string fragment;
  char c;

  while (input.get(c)) {
    if (c == '\0') {
      break;
    }
    if (is_fragment_char(c)) {
      fragment += c;
    }
  }

  uri.set_fragment(fragment);
  return !fragment.empty();
}

// Character classification methods
bool URIParser::is_unreserved(char c) {
  return std::isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~';
}

bool URIParser::is_gen_delim(char c) {
  return c == ':' || c == '/' || c == '?' || c == '#' || c == '[' || c == ']' ||
         c == '@';
}

bool URIParser::is_sub_delim(char c) {
  return c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' ||
         c == ')' || c == '*' || c == '+' || c == ',' || c == ';' || c == '=';
}

bool URIParser::is_reserved(char c) {
  return is_gen_delim(c) || is_sub_delim(c);
}

bool URIParser::is_scheme_char(char c) {
  return std::isalnum(c) || c == '+' || c == '-' || c == '.';
}

bool URIParser::is_userinfo_char(char c) {
  return is_unreserved(c) || is_sub_delim(c) || c == ':' || c == '%';
}

bool URIParser::is_host_char(char c) {
  return is_unreserved(c) || is_sub_delim(c) || c == ':' || c == '[' ||
         c == ']' || c == '%';
}

bool URIParser::is_port_char(char c) { return std::isdigit(c); }

bool URIParser::is_path_char(char c) {
  return is_unreserved(c) || is_sub_delim(c) || c == ':' || c == '@' ||
         c == '/' || c == '%';
}

bool URIParser::is_query_char(char c) {
  return is_unreserved(c) || is_sub_delim(c) || c == ':' || c == '@' ||
         c == '/' || c == '?' || c == '%';
}

bool URIParser::is_fragment_char(char c) {
  return is_unreserved(c) || is_sub_delim(c) || c == ':' || c == '@' ||
         c == '/' || c == '?' || c == '%';
}

std::string URIParser::percent_decode(const std::string &input) {
  std::string output;

  for (size_t i = 0; i < input.size(); ++i) {
    if (input[i] == '%' && i + 2 < input.size()) {
      char high = input[i + 1];
      char low = input[i + 2];
      if (std::isxdigit(high) && std::isxdigit(low)) {
        output += hex_to_char(high, low);
        i += 2;
        continue;
      }
    }
    output += input[i];
  }

  return output;
}

unsigned char URIParser::hex_to_char(char high, char low) {
  auto hex_to_int = [](char c) -> int {
    if (c >= '0' && c <= '9')
      return c - '0';
    if (c >= 'a' && c <= 'f')
      return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
      return c - 'A' + 10;
    return 0;
  };

  return static_cast<unsigned char>((hex_to_int(high) << 4) | hex_to_int(low));
}
