#ifndef CRYPTO_H
#define CRYPTO_H

#include <string>

std::string xorEncrypt(const std::string& data, const std::string& key);
std::string xorDecrypt(const std::string& data, const std::string& key);

#endif
