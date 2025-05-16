#include "../class/main.h"

void log_to_file(const std::string& msg) {
    namespace fs = std::filesystem;

    try {
        fs::path logDir = fs::current_path() / "../SavedData";
        if (!fs::exists(logDir)) {
            fs::create_directory(logDir);
        }
        fs::path logFile = logDir / "usb_eject_log.txt";

        std::ofstream log(logFile, std::ios::app);
        if (log.is_open()) {
            log << msg << std::endl;
            log.flush();
        } else {
            std::cerr << "[ERROR] Could not open log file: " << logFile << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception during logging: " << e.what() << "\n";
    }
}

void print_last_error(const std::string& msg) {
    DWORD errorCode = GetLastError();
    LPWSTR errorMsg = nullptr;

    FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, errorCode, 0, (LPWSTR)&errorMsg, 0, nullptr);

    std::wstring wmsg(msg.begin(), msg.end());
    std::wstring fullMsg = L"[ERROR] " + wmsg + L": " + (errorMsg ? errorMsg : L"Unknown error");
    log_to_file(std::string(fullMsg.begin(), fullMsg.end()));

    if (errorMsg)
        LocalFree(errorMsg);
}

bool eject_usb_device(char driveLetter) {
    log_to_file("Starting eject_usb_device for drive " + std::string(1, driveLetter));

    std::wstring volumePath = L"\\\\.\\" + std::wstring(1, driveLetter) + L":";

    HANDLE volume = CreateFileW(
        volumePath.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr
    );

    if (volume == INVALID_HANDLE_VALUE) {
        print_last_error("Unable to open drive for eject");
        return false;
    }

    DWORD bytesReturned;

    // Lock volume with retry
    const int maxLockAttempts = 5;
    bool locked = false;
    for (int i = 0; i < maxLockAttempts; ++i) {
        if (DeviceIoControl(volume, FSCTL_LOCK_VOLUME, nullptr, 0, nullptr, 0, &bytesReturned, nullptr)) {
            locked = true;
            log_to_file("Volume locked successfully.");
            break;
        } else {
            print_last_error("Unable to lock volume, attempt " + std::to_string(i + 1));
        }
        Sleep(500);
    }
    if (!locked) {
        log_to_file("Failed to lock volume after retries.");
        CloseHandle(volume);
        return false;
    }

    // Dismount
    if (!DeviceIoControl(volume, FSCTL_DISMOUNT_VOLUME, nullptr, 0, nullptr, 0, &bytesReturned, nullptr)) {
        print_last_error("Unable to dismount volume");
        CloseHandle(volume);
        return false;
    }
    log_to_file("Volume dismounted successfully.");

    // Eject
    if (!DeviceIoControl(volume, IOCTL_STORAGE_EJECT_MEDIA, nullptr, 0, nullptr, 0, &bytesReturned, nullptr)) {
        print_last_error("Unable to eject device");
        CloseHandle(volume);
        return false;
    }

    log_to_file("USB drive " + std::string(1, driveLetter) + ":\\ successfully ejected.");

    CloseHandle(volume);
    return true;
}