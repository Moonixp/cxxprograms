/*
 * Author : Hyun Wo
 * Purpose : (For Demo purposes only) This provides utilities for string
 * manipulation for the URI class Date : 5th September 2025
 *
 * This software is under no license and can be used and distributed and
 * modified by any third party as required.
 */
#ifndef URI_UTILS_HPP
#define URI_UTILS_HPP
#include <algorithm>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>

// Utility functions
namespace uri_utils {
std::string to_lower(const std::string &str);
std::vector<std::string> split(const std::string &str, char delimiter);
std::string trim(const std::string &str);
bool starts_with(const std::string &str, const std::string &prefix);
} // namespace uri_utils
#endif
