#include "../class/main.h"

void prompt_user_for_action(char driveLetter, DWORD serial, const std::string& whitelistFile) {
    std::string eject_message = "Unrecognised USB Drive " + std::string(1, driveLetter) +
        ":\\ contains potentially dangerous files.\n\n"
        "Would you like to EJECT this device?";

    int eject_result = MessageBoxA(
        nullptr,
        eject_message.c_str(),
        "USB Device Locker",
        MB_ICONWARNING | MB_YESNO | MB_TOPMOST | MB_SYSTEMMODAL
        );

    if (eject_result == IDYES) {
        std::cout << "[ACTION] Ejecting USB drive " << driveLetter << "\n";
        eject_usb_device(driveLetter);
    } else if (eject_result == IDNO) {
        std::string whitelist_message = "would you like to ADD this device to the Whitelist?\n";
        int whitelist_result = MessageBoxA(
            nullptr,
            whitelist_message.c_str(),
            "USB Device Locker",
            MB_ICONWARNING | MB_YESNO | MB_TOPMOST | MB_SYSTEMMODAL
            );
        if (whitelist_result == IDYES) {
            std::cout << "[ACTION] Whitelisting USB drive with serial: 0x " << std::hex << serial << std::dec << "\n";
            save_to_whitelist(whitelistFile, serial);
            WHITELIST.insert(serial);
        } else {
            std::cout << "Device will not be added\n" << std::endl;
        }
    }
}