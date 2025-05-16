#include "../class/main.h"

bool eject_usb_device(char driveLetter) {
    std::wstring volumePath = L"\\\\. \\" + std::wstring(1, driveLetter) + L":";

    HANDLE volume = CreateFileW(
        volumePath.c_str(),
        GENERIC_READ | GENERIC_WRITE | FILE_SHARE_READ, FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr
        );

    if (volume == INVALID_HANDLE_VALUE) {
        std::cerr << "[ERROR] Unable to open drive for eject: " << driveLetter << "\n";
        return false;
    }

    // Lock volume
    DWORD bytesReturned;
    if (!DeviceIoControl(volume, FSCTL_LOCK_VOLUME, nullptr, 0, nullptr, 0, &bytesReturned, nullptr)) {
        std::cerr << "[ERROR] Unable to lock volume\n";
        CloseHandle(volume);
        return false;
    }

    // Dismount the volume
    if (!DeviceIoControl(volume, FSCTL_DISMOUNT_VOLUME, nullptr, 0, nullptr, 0, &bytesReturned, nullptr)) {
        std::cerr << "[ERROR] Unable to dismount volume\n";
        CloseHandle(volume);
        return false;
    }
    // Try eject
    if (!DeviceIoControl(volume, IOCTL_STORAGE_EJECT_MEDIA, nullptr, 0, nullptr, 0, &bytesReturned, nullptr)) {
        std::cerr << "[ERROR] Unable to eject device\n";
        CloseHandle(volume);
        return false;
    }

    if (!eject_usb_device(driveLetter)) {
        MessageBoxA(nullptr, "[ATTENTION] Ejection failed. Please relome the device manually.", "USB Locker", MB_ICONERROR);
    }
    CloseHandle(volume);
    std::cout << "[INFO] USB drive " << driveLetter << ":\\ successfully ejected\n";
    return true;
}