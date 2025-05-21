#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <locale>
#include <sstream>

#ifdef _WIN32
#include <conio.h>
#endif

std::string getMaskedInput(const std::string& prompt) {
    std::string input;
    char ch;

    std::cout << prompt;

#ifdef _WIN32
    while ((ch = _getch()) != '\r') { // '\r' is Enter key
        if (ch == '\b') { // Backspace
            if (!input.empty()) {
                input.pop_back();
                std::cout << "\b \b";
            }
        } else if (isprint(ch)) {
            input.push_back(ch);
            std::cout << '*';
        }
    }
#else
    system("stty -echo"); // Turn off echo (Unix)
    std::getline(std::cin, input);
    system("stty echo");  // Turn on echo
#endif

    std::cout << std::endl;
    return input;
}

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

// Trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        [](unsigned char ch) { return !std::isspace(ch); }));
}

// Trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
        [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
}

// Trim from both ends (copying version)
static inline std::string trim(const std::string &s) {
    std::string result = s;
    ltrim(result);
    rtrim(result);
    return result;
}

static inline std::string toLower(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}