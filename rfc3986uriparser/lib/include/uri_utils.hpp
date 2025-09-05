/*
 * Author : Hyun Wo
 * Purpose : This software is free for use and modification by any third party.
 * No warranty is provided and the user is responsible for any consequences of
 * its use.
 * Date : 5th September 2025
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
