# 🔐 kpx — Cross-Platform CLI Password Vault

`kpx` is a lightweight, secure, and portable command-line password vault written in C++.  
It allows you to safely store, retrieve, generate, update, and delete passwords, all locally with encryption and clipboard support.

---

## 📦 Features

- AES-style XOR encryption with SHA-256 derived keys
- Master password-based protection
- Per-entry encryption
- Clipboard integration (copies password on fetch)
- Strong password generator (custom length and character set)
- Cross-platform (Windows & Linux)
- No dependencies on external vault services
- Can even run from pendrives/external drives to carry or share passwords

---

## 📦 Install
- Download the release packages
- Windows:
   - Copy kpx.exe to a location where it can be accessible
      ```C:\kpx\kpx.exe```
   - Add path to environment variable for accessing in terminal
- Linux:
   - Move kpx binary to ```/usr/local/bin```
      
      ```sudo mv /path-to-kpx /usr/local/bin```

   - Make sure its executable ```sudo chmod +x /usr/local/bin/kpx```

- Run ```kpx init``` when installing for first time to set Master Password and vault

---

## 🚀 Setup Instructions

### ✅ Prerequisites

- CMake ≥ 3.16
- C++17-compatible compiler:
  - Windows: MSYS2 (MinGW UCRT64)
  - Linux: g++, clang, etc.

### 📁 Project Structure

```
project_root/
│
├── include/
│   ├── crypto.h
│   ├── sha256.h
│   └── vault.h
|   └── utils.h
│
├── src/
│   ├── crypto.cpp
│   ├── sha256.cpp
│   ├── vault.cpp
│   └── main.cpp
│
├── external/
│   └── clip/         # Clipboard library (https://github.com/dacap/clip)
│
├── CMakeLists.txt
```

### 🔧 Build (Windows/Linux)

```bash
# Clone the repo
git clone https://github.com/S-aurav/SecurePass.git
cd SecurePass

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the executable
cmake --build .
```

---

## 🛠️ Usage

```bash
kpx <command> [arguments...]
```

### 📜 Available Commands

| Command              | Description                                      |
|----------------------|--------------------------------------------------|
| `init`               | Creates vault and sets Master Password           |
| `add`                | Add a new password entry                         |
| `list`               | Show all saved site names                        |
| `get <site>`         | Fetch and copy the password for a site          |
| `update <site>`      | Update username or password for a site          |
| `del <site>`         | Delete an entry for the given site              |
| `genpass`            | Generate a strong password using options        |
| `help`               | Show help/instructions                          |

> All operations require re-entering the master password for decryption.

---

## 🔐 Password Generator

When using `genpass`, you will be prompted to customize the password:

- Include only digits
- Include alphanumeric characters
- Include special characters
- Specify desired password length

---

## 📋 Clipboard Support

After fetching a password using `get <site>`, the password is automatically copied to your clipboard for convenience (powered by `clip`).

---

## 📌 Notes

- All data is stored locally in an encrypted vault file.
- Master password is never stored; hash comparison is used for verification.
- For Linux users, make sure your terminal supports clipboard (`xclip` or `xsel` may be needed).

---
