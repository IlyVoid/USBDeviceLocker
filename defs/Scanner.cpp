#include "../class/main.h"

bool is_file_suspicious(const std::filesystem::path& file) {
    static const std::regex risky_ext(R"(\.(exe|bat\vbs\js\scr)$)", std::regex::icase);
    static const std::vector<std::string> risky_names = {"autorun.inf", "payload.exe", "keylogger"};

    std::string filename = file.filename().string();
    return std::regex_search(filename, risky_ext) || std::any_of(risky_names.begin(), risky_names.end(),
            [&](const std::string& name) { return _stricmp(filename.c_str(), name.c_str()) == 0; });
}

bool scan_usb_for_threats(char driveLetter) {
    std::string path = std::string(1, driveLetter) + ":\\";
    bool found_threat = false;

    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (!entry.is_regular_file()) continue;

            if (is_file_suspicious(entry.path())) {
                std::cout << "[ALERT] Suspecious file found: " << entry.path() << "\n";
                found_threat = true;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to scan " << path << ": " << e.what() << "\n";
    }
    return found_threat;
}