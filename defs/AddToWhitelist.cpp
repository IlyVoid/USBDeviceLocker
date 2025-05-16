#include "../class/main.h"

std::unordered_set<DWORD> load_whitelist(const std::string& filename) {
    std::unordered_set<DWORD> list;
    std::ifstream file(filename);
    DWORD val;

    while (file >> std::hex >> val) {
        list.insert(val);
    }
    return list;
}

void save_to_whitelist(const std::string& filename, DWORD serial) {
    std::ofstream file(filename, std::ios::app); // appending
    file << std::hex << serial << "\n";
}