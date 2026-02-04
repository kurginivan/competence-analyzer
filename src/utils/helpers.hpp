#pragma once

#include <string>

namespace Utils {

// String utilities
std::string toUpperCase(const std::string& str);
std::string toLowerCase(const std::string& str);
std::string trim(const std::string& str);
bool isValidEmail(const std::string& email);

// Validation utilities
bool isValidLevel(int level);
bool isPositiveNumber(int num);

// Conversion utilities
std::string levelToDescription(int level);

} // namespace Utils
