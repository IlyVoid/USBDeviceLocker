# 🔒 USB Device Locker

**USB Device Locker** is a Windows-based security application written in modern C++23 that monitors, scans, and manages connected USB devices. It gives users control over what removable devices are allowed to interact with their system by allowing them to **whitelist trusted USB drives**, **detect unrecognized devices**, and optionally **eject unknown drives**.

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-23-blue?style=for-the-badge" alt="C++26 Badge"/>
  <img src="https://img.shields.io/badge/Platform-Windows-lightgrey?style=for-the-badge" alt="Windows Badge"/>
  <img src="https://img.shields.io/badge/CLion-Project-green?style=for-the-badge" alt="CLion Badge"/>
</p>

---

> The application runs in the terminal and monitors devices every 3 seconds. (when the application is complete a GUI will be released)
> 
> When the file checker works perfectly the optionallity to eject the USB will be removed and the USB will be ejected instantly (Might add advanced settings for special cases)

```
=== USB Device Locker ===

Monitoring extern drives every 3 seconds...
[INFO] USB Drive E:\ [LABEL]: WORKSTICK, Serial: 0xA1B2C3D4 -- ALLOWED
[INFO] USB Drive F:\ [LABEL]: HACKERTOOL, Serial: 0xBADF00D -- !!! BLOCKED (Device not whitelisted)
```

---

## 🔧 Features

✅ **Real-time USB Monitoring**  
✅ **Serial-based Whitelisting**  
✅ **One-time prompt per device connection**  
✅ **Simple device ejection**  
✅ **Threat simulation / scan hook**  
✅ **Clean modular C++ architecture**

---

## 🗂️ Project Structure

```bash
USBDeviceLocker/
│
├── class/
│   └── main.h                # Structs, global declarations
│
├── defs/
│   ├── AddToWhitelist.cpp    # Whitelist load/save logic
│   ├── Scanner.cpp           # Simulated USB threat scan
│   ├── Prompter.cpp          # User prompt (MessageBox, etc.)
│   └── Ejection.cpp          # USB ejection logic
│   └── WhitelistCheck.cpp    # Check if device is already whitelisted
│
├── main.cpp                  # Entry point, main loop
├── SavedData/whitelist.txt   # Whitelisted device serials
├── CMakeLists.txt            # CMake build configuration
└── README.md                 # This file
```

---

## 🛠️ Build & Run

### 📋 Requirements

- Windows 10 or 11
- CLion or any IDE with CMake support
- C++23 capable compiler (e.g., MSVC with C++23 preview)

### 🔧 Building

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

> 💡 Or use CLion’s GUI to configure and build.

### ▶️ Running

```bash
./USBDeviceLocker.exe
```

---

## ⚙️ Configuration

### 🔐 Whitelist File

```txt
# whitelist.txt
A1B2C3D4
BEEF1337
DEADBEEF
```

Each line is a **hexadecimal serial number** from a previously approved USB device. These are read at startup.

---

## 💡 How it Works

1. On each scan, the app detects all removable drives.
2. If the serial number is in the whitelist:
    - Logs `[ALLOWED]` once per device connection.
3. If the device is unknown:
    - Prompts the user via popup:
        - Add to whitelist
        - Eject device
        - Ignore
4. Previously seen devices in the same session are ignored (to avoid spam).

---

## 🚀 Example Prompt

> If a device is unknown, the user sees:

```
[SECURITY WARNING]
Unknown USB device detected on drive E:\
Do you want to trust (whitelist) this device?
```

---

## 📄 Key Code Snippets

### 🔍 Detecting Drives

```cpp
std::vector<USBDrive> get_connected_drives() {
    // Iterates A-Z, checks if DRIVE_REMOVABLE
}
```

### 📜 Whitelist Handling

```cpp
WHITELIST = load_whitelist("whitelist.txt");
```

### ⚠️ Prompt Logic

```cpp
if (!WHITELIST.contains(drive.serial)) {
    prompt_user_for_action(drive.letter, drive.serial, whitelistFile);
}
```

---

## 🧪 Simulated Threat Scan

The `scan_usb_for_threats(char letter)` function can be extended with antivirus APIs or custom logic.

---

## 🚫 Limitations

- Windows-only
- Uses simple MessageBox prompts
- Doesn't yet persist seen devices across sessions
- Ejection may fail if the drive is in use

---

## ✅ To-Do / Roadmap

- [ ] Add GUI (tray app or Qt frontend)
- [ ] Persist seen-device cache across sessions
- [ ] Full antivirus integration
- [ ] Encryption-based trust instead of serials
- [ ] Better logging and alerting system

---

## 📦 License

This project is open-source and available under the MIT License.

---

## 🧠 Credits & Thanks

Created by [Bl4ss]  
C++23 experimental project to explore USB monitoring and security on Windows.

---

> Built with 🦫 energy by 𝖇𝖑4𝖘𝖘
