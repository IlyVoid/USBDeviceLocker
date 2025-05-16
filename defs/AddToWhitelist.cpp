#include "../class/main.h"

std::unordered_set<DWORD> load_whitelist(const std::string& filename) {
    std::unordered_set<DWORD> list;
    std::ifstream file(filename);
    DWORD val;

    if (!file) {
        std::cerr << "[ERROR] Cannot open whitelist file: " << filename << "\n";
        return list;
    }

    while (file >> std::hex >> val) {
        list.insert(val);
        std::cout << "[LOADED] Whitelist serial: 0x" << std::hex << val << std::dec << "\n";
    }
    return list;
}

void save_to_whitelist(const std::string& filename, DWORD serial) {
    std::ofstream file(filename, std::ios::app); // appending
    file << std::hex << serial << "\n";
}