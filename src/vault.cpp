#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <limits>
#include <string>
#include <vector>
#include <iomanip>

#include "../include/vault.h"
#include "../include/sha256.h"
#include "../include/utils.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <random>


const std::string VAULT_FILE = "kpx_data.enc.jpg";


namespace vault{

    std::string getVaultPath() {
        std::string cwd = std::filesystem::current_path().string();
        return cwd + "/" + VAULT_FILE;
    }

    bool vaultExists(const std::string& path) {
        return std::filesystem::exists(path);
    }

    void createVaultFile(const std::string& path) {
        std::ofstream vaultFile(path, std::ios::binary);
        if (vaultFile) {
            std::cout << "[+] Created new vault file at: " << path << "\n";
            vaultFile.close();
        } else {
            std::cerr << "[!] Failed to create vault file.\n";
            exit(1);
        }
    }

    bool authenticateUser() {
        std::ifstream file(getVaultPath());
        std::string savedHash;
        std::getline(file, savedHash);
        file.close();

        for (int attempts = 0; attempts < 3; ++attempts) {
            std::string inputPass;

            while(true) {
                inputPass = getMaskedInput("Enter master password: ");

                if(inputPass.empty()) {
                    std::cout << "[!] Password cannot be empty.\n";
                } else if (inputPass.length() < 3) {
                    std::cout << "[!] Password must be at least 4 characters long.\n";
                } else {
                    break;
                }
            }

            
            if (sha256(inputPass) == savedHash) {
                return true;
            } else {
                std::cout << "[!] Wrong password (" << 2 - attempts << " tries left).\n";
            }
        }

        std::cout << "[!] Maximum attempts reached. Exiting.\n";
        return false;
    }


    void handleInit() {
        std::cout << "Initializing password vault..." << std::endl;
        
    }

    void handleAdd() {
        std::cout << "Adding a new entry..." << std::endl;
        
    }

    void handleList() {
        std::cout << "Listing all entries..." << std::endl;
        
    }

    void handleGet(const std::string& site) {
        std::cout << "Retrieving password for site: " << site << std::endl;
        
    }

    void handleGenerate() {
        std::cout << "Generating a strong password..." << std::endl;
        
    }

    void handleDelete(const std::string& site) {
        std::cout << "Deleting entry for site: " << site << std::endl;
        
    }
}