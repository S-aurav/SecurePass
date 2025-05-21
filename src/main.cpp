#include <iostream>
#include <string>
#include "../include/vault.h"

void print_usage() {
    std::cout << "Usage:\n"
              << "  vault init                     Initialize the password vault\n"
              << "  vault add <site> <username>    Add a new entry\n"
              << "  vault list                     List all entries\n"
              << "  vault get <site>               Retrieve password for site\n"
              << "  vault genpass                  Generate a strong password\n"
              << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    std::string command = argv[1];

    switch (command[0]) {
        case 'i':
            if (command == "init") {
                vault::handleInit();
            } else {
                std::cerr << "Unknown command: " << command << "\n";
                print_usage();
                return 1;
            }
            break;
        case 'a':
            if (command == "add") {
                vault::handleAdd();
            } else {
                std::cerr << "Unknown command: " << command << "\n";
                print_usage();
                return 1;
            }
            break;
        case 'l':
            if (command == "list") {
                vault::handleList();
            } else {
                std::cerr << "Unknown command: " << command << "\n";
                print_usage();
                return 1;
            }
            break;
        case 'g':
            if (command == "get") {
                if (argc != 3) {
                    std::cerr << "Usage: vault get <site>\n";
                    return 1;
                }
                std::string site = argv[2];
                vault::handleGet(site);
            } else if (command == "genpass") {
                vault::handleGenerate();
            } else {
                std::cerr << "Unknown command: " << command << "\n";
                print_usage();
                return 1;
            }
            break;
        case 'd':
            if (command == "del") {
                if (argc != 3) {
                    std::cerr << "Usage: vault delete <site>\n";
                    return 1;
                }
                std::string site = argv[2];
                vault::handleDelete(site);
            } else {
                std::cerr << "Unknown command: " << command << "\n";
                print_usage();
                return 1;
            }
            break;
        default:
            std::cerr << "Unknown command: " << command << "\n";
            print_usage();
            return 1;
    }

    return 0;
}
