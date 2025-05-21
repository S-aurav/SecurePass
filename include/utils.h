#include <iostream>
#include <string>

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