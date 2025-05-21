#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <limits>
#include <string>
#include <vector>
#include <iomanip>
#include <clip.h> // For clipboard operations

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
        std::ifstream vault(getVaultPath());
        std::string line;
        bool any = false;

        // Skip the first line (password hash)
        std::getline(vault, line);

        std::cout << "== Saved Sites ==\n";
        while (std::getline(vault, line)) {
            std::istringstream iss(line);
            std::string site;
            if (std::getline(iss, site, '|')) {
                std::cout << "- " << site << "\n";
                any = true;
            }
        }

        if (!any) std::cout << "[*] No entries found.\n";
        
    }

    void handleGet(const std::string& targetSite) {
        if (!authenticateUser()) return;

        std::string confirmMasterPass;
        confirmMasterPass = getMaskedInput("Re-enter master password (for decryption): ");

        if (confirmMasterPass.length() < 3) {
            std::cerr << "[!] Password must be at least 4 characters long.\n";
            return;
        }

        if (!authenticateUser(confirmMasterPass)) {
            std::cerr << "[!] Authentication failed.\n";
            return;
        }
        

        std::ifstream vault(getVaultPath());
        std::string line;
        bool found = false;

        // Skip password hash line
        std::getline(vault, line);

        while (std::getline(vault, line)) {
            std::istringstream iss(line);
            std::string site, encryptedUsername, encryptedPassword;

            std::getline(iss, site, '|');
            std::getline(iss, encryptedUsername, '|');
            std::getline(iss, encryptedPassword);

            if (site.empty() || encryptedUsername.empty() || encryptedPassword.empty()) {
                std::cerr << "[!] Corrupted entry in vault.\n";
                return;
            }

            if (site == targetSite) {
                std::string keyHash = sha256(confirmMasterPass + site);
                std::string username = xorDecrypt(encryptedUsername, keyHash);
                std::string password = xorDecrypt(encryptedPassword, keyHash);

                std::cout << "== " << site << " ==\n";
                std::cout << "Username: " << username << "\n";
                std::cout << "Password: " << password << "\n";


                if(clip::set_text(password)) {
                    std::cout << "[+] Password copied to clipboard.\n";
                } else {
                    std::cerr << "[!] Failed to copy password to clipboard.\n";
                }
                
                found = true;
                break;
            }
        }

        if (!found) {
            std::cout << "[!] No entry found for site: " << targetSite << "\n";
        }
        
    }

    void handleUpdate(const std::string& targetSite) {
        std::cout << "[*] Updating entry for site: " << targetSite << "\n";

        std::ifstream vaultIn(getVaultPath());
        if (!vaultIn) {
            std::cerr << "[!] Could not open vault file.\n";
            return;
        }

        std::vector<std::string> lines;
        std::string line;

        while (std::getline(vaultIn, line)) {
            lines.push_back(line);
        }
        vaultIn.close();

        if (lines.empty()) {
            std::cerr << "[!] Vault file is empty.\n";
            return;
        }

        std::string masterPass;
        // std::cout << "Enter master password: ";
        masterPass = getMaskedInput("Enter master password: ");

        std::string keyHash = sha256(masterPass + targetSite);

        bool updated = false;
        std::vector<std::string> updatedLines;
        updatedLines.push_back(lines[0]); // Preserve master password hash

        for (size_t i = 1; i < lines.size(); ++i) {
            auto parts = split(lines[i], '|');
            if (parts.size() != 3) {
                updatedLines.push_back(lines[i]);
                continue;
            }

            std::string Site = parts[0];
            if (Site == targetSite) {
                std::string newUsername, newPassword;
                std::cout << "Enter new username: ";
                std::cin >> newUsername;
                std::cout << "Enter new password: ";
                std::cin >> newPassword;

                // std::string encSite = xorEncrypt(targetSite, keyHash);
                std::string encUser = xorEncrypt(newUsername, keyHash);
                std::string encPass = xorEncrypt(newPassword, keyHash);

                updatedLines.push_back(Site + "|" + encUser + "|" + encPass);
                updated = true;
            } else {
                updatedLines.push_back(lines[i]);
            }
        }

        if (!updated) {
            std::cout << "[!] No entry found for site: " << targetSite << "\n";
            return;
        }

        std::ofstream vaultOut(getVaultPath(), std::ios::trunc);
        for (const auto& l : updatedLines) {
            vaultOut << l << "\n";
        }
        vaultOut.close();

        std::cout << "[+] Entry updated successfully.\n";

    }

    void handleGenerate() {
        int choice, length;
        std::string charset;

        std::cout << "[+] Password Generator\n";
        std::cout << "Choose type of password:\n";
        std::cout << "1. Numbers only\n";
        std::cout << "2. Alphanumeric\n";
        std::cout << "3. Alphanumeric + Special characters\n";
        std::cout << "Enter your choice (1-3): ";
        std::cin >> choice;

        std::cout << "Enter desired password length: ";
        std::cin >> length;

        if (length <= 0) {
            std::cout << "[!] Invalid length.\n";
            return;
        }

        switch (choice) {
            case 1:
                charset = "0123456789";
                break;
            case 2:
                charset = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
                break;
            case 3:
                charset = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()-_=+[]{}|;:',.<>?/";
                break;
            default:
                std::cout << "[!] Invalid choice.\n";
                return;
        }

        std::string password;
        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<> dist(0, charset.size() - 1);

        for (int i = 0; i < length; ++i) {
            password += charset[dist(generator)];
        }

        std::cout << "[+] Generated password: " << password << "\n";

        char saveChoice;
        std::cout << "Do you want to save this password in the vault? (y/n): ";
        std::cin >> saveChoice;

        if (saveChoice == 'y' || saveChoice == 'Y') {
            if (!authenticateUser()) return;

            std::string site, username, confirmMasterPass;
            std::cout << "Enter site name: ";
            std::cin >> site;
            std::cout << "Enter username: ";
            std::cin >> username;
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
            std::string encryptedSite = site; // Stored in plaintext
            std::string encryptedUsername = xorEncrypt(username, keyHash);
            std::string encryptedPassword = xorEncrypt(password, keyHash);

            std::ofstream vault(getVaultPath(), std::ios::app | std::ios::binary);
            
            if (!vault) {
                std::cerr << "[!] Failed to open vault file.\n";
                return;
            }

            vault << encryptedSite << "|" << encryptedUsername << "|" << encryptedPassword << "\n";
            vault.close();

            std::cout << "[+] Entry saved securely for site: " << site << "\n";
        }else {
            std::cout << "[*] Password not saved.\n";
        }
        
    }

    void handleDelete(const std::string& site) {
        std::cout << "Deleting entry for site: " << site << std::endl;
        
    }
}