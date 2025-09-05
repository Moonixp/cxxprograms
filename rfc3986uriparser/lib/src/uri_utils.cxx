#include "../include/uri_utils.hpp"
#include <iterator>

// Utility functions implementation
namespace uri_utils {
std::string to_lower(const std::string &str) {

  std::string result(str.length(), 0);

  std::transform(str.begin(), str.end(), result.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return result;
}

std::vector<std::string> split(const std::string &str, char delimiter) {
  std::vector<std::string> tokens;
  std::istringstream iss(str);
  std::string token;

  while (std::getline(iss, token, delimiter)) {
    tokens.push_back(token);
  }

  return tokens;
}

std::string trim(const std::string &str) {
  size_t start = str.find_first_not_of(" \t\n\r\f\v");
  if (start == std::string::npos)
    return "";

  size_t end = str.find_last_not_of(" \t\n\r\f\v");
  return str.substr(start, end - start + 1);
}

bool starts_with(const std::string &str, const std::string &prefix) {
  return str.size() >= prefix.size() &&
         str.compare(0, prefix.size(), prefix) == 0;
}
} // namespace uri_utils
