#pragma once

#ifndef VAULT_H
#define VAULT_H

#include <string>

namespace vault {
    std::string getVaultPath();
    bool vaultExists(const std::string& path);
    void createVaultFile(const std::string& path);
    bool authenticateUser();
    bool authenticateUser(const std::string& masterPass);
    void handleInit();
    void handleUpdate(const std::string& site);
    void handleAdd();
    void handleList();
    void handleGet(const std::string& site);
    void handleGenerate();
    void handleDelete(const std::string& site);
}

#endif // VAULT_H