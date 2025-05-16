#include "../class/main.h"

std::unordered_set<DWORD> WHITELIST;
std::unordered_set<DWORD> SEEN_THIS_SESSION;

std::vector<USBDrive> get_connected_drives() {
    std::vector<USBDrive> drives;

    DWORD driveMask = GetLogicalDrives();
    for (char letter = 'A'; letter <= 'Z'; ++letter) {
        if (!(driveMask & (1 << (letter - 'A')))) continue;

        std::string rootPath = std::string(1, letter) + ":\\";
        UINT type = GetDriveTypeA(rootPath.c_str());

        if (type == DRIVE_REMOVABLE) {
            char volumeName[MAX_PATH] = {};
            DWORD serial = 0;

            if (GetVolumeInformationA(rootPath.c_str(),
                volumeName, sizeof(volumeName),
                &serial, nullptr,
                nullptr, nullptr,
                0)) {
                drives.push_back({letter, volumeName, serial});
                }
        }
    }
    return drives;
}

// Check for any non-whitelisted USBs
void check_usb_devices() {
    auto drives = get_connected_drives();

    for (const auto& drive : drives) {
        if (SEEN_THIS_SESSION.contains(drive.serial)) {
            continue; // Already seen, skip
        }

        // Mark as seen now
        SEEN_THIS_SESSION.insert(drive.serial);

        std::cout << "[INFO] USB Drive " << drive.letter << ":\\"
                  << "[LABEL]: " << drive.label << ", Serial: 0x"
                  << std::hex << drive.serial << std::dec;

        if (WHITELIST.contains(drive.serial)) {
            std::cout << " -- ALLOWED\n";
        } else {
            std::cout << " -- !!! BLOCKED (Device not whitelisted)\n";

            if (scan_usb_for_threats(drive.letter)) {
                prompt_user_for_action(drive.letter, drive.serial, whitelistFile);
            } else {
                std::cout << "No obvious threats found on " << drive.letter << ":\\ -- [SAFE?] Up to user.\n";
                prompt_user_for_action(drive.letter, drive.serial, whitelistFile);
            }
        }
    }
}