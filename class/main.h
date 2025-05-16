#ifndef MAIN_H
#define MAIN_H

#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <thread>
#include <chrono>
#include <filesystem>
#include <regex>
#include <setupapi.h>
#include <cfgmgr32.h>
#include <fstream>
#include <sstream>

#pragma comment(lib, "setupapi.lib")

using namespace std::chrono_literals;

// Whitelist of allowed USB Volume serial numbers (hex dec)
extern std::unordered_set<DWORD> WHITELIST;
const std::string whitelistFile = "../SavedData/whitelist.txt";

// Get a list of connected removable drives and their serials
struct USBDrive {
    char letter;
    std::string label;
    DWORD serial;
};

// Prompter
void prompt_user_for_action(char driveLetter, DWORD serial, const std::string& whitelistFile);

// Whitelist Check
std::vector<USBDrive> get_connected_drives();
void check_usb_devices();

// Scanner
bool scan_usb_for_threats(char driveLetter);
bool is_file_suspicious(const std::filesystem::path& file);

// Ejection
bool eject_usb_device(char driveLetter);

// Add to whitelist
std::unordered_set<DWORD> load_whitelist(const std::string& filename);
void save_to_whitelist(const std::string& filename, DWORD serial);

#endif //MAIN_H
