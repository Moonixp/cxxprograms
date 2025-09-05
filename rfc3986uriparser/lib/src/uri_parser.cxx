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
 * build authority string from userinfo, host and port
 *
 * if userinfo is not empty, it is prepended to host with an "@"
 * if port is not empty, it is appended to host with a ":"
 *
 * @param userinfo userinfo part (can be empty)
 * @param host host part
 * @param port port part (can be empty)
 * @return constructed authority string
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
 * parses a uri from a given string
 *
 * @param uri_string string representing the uri to parse
 * @return a pointer to the parsed uri object
 */
std::unique_ptr<URI> URIParser::parse(const std::string &uri_string) const {
  std::istringstream iss(uri_string);
  return parse(iss);
}

/**
 * reads a uri from a given input stream and returns a pointer to the parsed
 * uri object
 *
 * @param input the input stream to read from
 * @return a pointer to the parsed uri object
 * @throws URIParseException if the input stream is empty or the uri is invalid
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
 * reads a uri scheme from an input stream
 *
 * @param input input stream with the uri to parse
 * @param uri uri object to store the parsed scheme
 * @return true if a scheme was parsed, false otherwise
 * @throws URIParseException if the input stream is empty or a uri component
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
 * parses a uri authority from an input stream
 *
 * @param input input stream with the uri to parse
 * @param uri uri object to store the parsed authority
 * @return true if an authority was parsed, false otherwise
 * @throws URIParseException if the input stream is empty or a uri component
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
 * extracts userinfo, host and port from the authority
 *
 * the format is [userinfo@]host[:port]
 *
 * @param authority the authority string to extract from
 * @param uri the uri object to store the parsed userinfo, host and port
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

/**
 * read the path part of a uri from an input stream
 *
 * read the path part from an input stream and store it in the given uri
 * object. stops when a '?' or '#' character is encountered or when the end of
 * the stream is reached.
 *
 * @param input the input stream to read from
 * @param uri the uri object to store the parsed path in
 * @return true if the path part was successfully parsed, false otherwise
 */
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

/**
 * parses a uri query from an input stream
 *
 * this function reads the query part from an input stream and stores it in the
 * given uri object. it stops when a '#' character is encountered or when the
 * end of the stream is reached.
 *
 * @param input the input stream to read from
 * @param uri the uri object to store the parsed query in
 * @return true if the query part was successfully parsed, false otherwise
 */
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

/**
 * parses a uri fragment from an input stream
 *
 * this function reads the fragment part from an input stream and stores it
 * in the given uri object. it stops when the end of the stream is reached.
 *
 * @param input the input stream to read from
 * @param uri the uri object to store the parsed fragment in
 * @return true if the fragment was successfully parsed, false otherwise
 */
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
/** 
  * @brief convert a pair of hexadecimal characters into a single unsigned char
  * @details The function takes two hexadecimal characters, converts them into a
  * single unsigned char and returns it.
  * @param high The first hexadecimal character, which will be used as the high
  * nibble of the resulting byte.
  * @param low The second hexadecimal character, which will be used as the low
  * nibble of the resulting byte.
  * @return The unsigned char value of the two hexadecimal characters.
  */
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
