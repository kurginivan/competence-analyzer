#include "helpers.hpp"
#include <algorithm>
#include <cctype>
#include <regex>

namespace Utils {

std::string toUpperCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

std::string toLowerCase(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

bool isValidEmail(const std::string& email) {
    const std::regex pattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return std::regex_match(email, pattern);
}

bool isValidLevel(int level) {
    return level >= 1 && level <= 5;
}

bool isPositiveNumber(int num) {
    return num > 0;
}

std::string levelToDescription(int level) {
    switch (level) {
        case 1: return "Basic";
        case 2: return "Intermediate";
        case 3: return "Advanced";
        case 4: return "Expert";
        case 5: return "Master";
        default: return "Unknown";
    }
}

} // namespace Utils
