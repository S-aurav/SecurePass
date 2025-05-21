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
#include "../include/crypto.h"

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

    bool authenticateUser(const std::string& masterPass) {
        std::ifstream file(getVaultPath());
        std::string savedHash;
        std::getline(file, savedHash);
        file.close();

        if (sha256(masterPass) == savedHash) {
            return true;
        } else {
            std::cout << "[!] Wrong password.\n";
            return false;
        }
    }


    void handleInit() {
        std::cout << "[+] Initializing vault...\n";
        std::string vaultPath = getVaultPath();


        std::cout << "== CLI PASSWORD VAULT ==\n";
        if (vaultExists(vaultPath)) {
            std::cout << "[+] Vault found at: " << vaultPath << "\n";
            return;
        } else {
            std::cout << "[!] Vault file not found. Creating new vault...\n";
            createVaultFile(vaultPath);
        }

        std::ifstream infile(vaultPath);
        std::string existingHash;
        std::getline(infile, existingHash);
        infile.close();

        if (!existingHash.empty()) {
            std::cout << "[!] Vault is already initialized.\n";
            return;
        }

        std::string masterPass, confirmPass;
        // std::cout << "Enter a master password: ";
        masterPass = getMaskedInput("Enter a master password: ");
        // std::cout << "Confirm master password: ";
        confirmPass = getMaskedInput("Confirm master password: ");
        if (masterPass.length() < 3) {
            std::cerr << "[!] Password must be at least 4 characters long.\n";
            return;
        }

        if (masterPass != confirmPass) {
            std::cerr << "[!] Passwords do not match.\n";
            return;
        }

        std::string hashed = sha256(masterPass);

        std::ofstream outfile(vaultPath);
        if (outfile.is_open()) {
            outfile << hashed << "\n";  // Store hashed password
            outfile.close();
            std::cout << "[+] Master password set and vault initialized!\n";
        } else {
            std::cerr << "[!] Could not write to vault.\n";
        }
        
    }

    void handleAdd() {
        std::cout << "[+] Adding new entry...\n";
        if (!authenticateUser()) return;

        std::string site, username, password, confirmMasterPass;
        std::cout << "Enter site name: ";
        std::cin >> site;
        std::cout << "Enter username: ";
        std::getline(std::cin >> std::ws, username); // Read full line for username
        std::cout << "Enter password: ";
        std::cin >> password;

        confirmMasterPass = getMaskedInput("Re-enter master password (for encryption): ");

        if (confirmMasterPass.length() < 3) {
            std::cerr << "[!] Password must be at least 4 characters long.\n";
            return;
        }

        if (!authenticateUser(confirmMasterPass)) {
            std::cerr << "[!] Authentication failed.\n";
            return;
        }

        std::string keyHash = sha256(confirmMasterPass + site); // Per-entry encryption key
        std::string encryptedUsername = xorEncrypt(username, keyHash);
        std::string encryptedPassword = xorEncrypt(password, keyHash);

        std::ofstream vault(getVaultPath(), std::ios::app | std::ios::binary);
        if (!vault) {
            std::cerr << "[!] Failed to open vault file.\n";
            return;
        }

        vault << site << "|" << encryptedUsername << "|" << encryptedPassword << "\n";
        vault.close();

        std::cout << "[+] Entry added securely for site: " << site << "\n";
        
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