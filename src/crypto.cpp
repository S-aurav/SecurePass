#include "../include/crypto.h"

std::string xorEncrypt(const std::string& data, const std::string& key) {
    std::string result = data;
    for (size_t i = 0; i < data.size(); ++i) {
        result[i] ^= key[i % key.size()];
    }
    return result;
}

std::string xorDecrypt(const std::string& data, const std::string& key) {
    return xorEncrypt(data, key);  // XOR is symmetric
}
