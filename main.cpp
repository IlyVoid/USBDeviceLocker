#include "./class/main.h"

int main() {
    std::cout <<"=== USB Device Locker ===\n" << std::endl;
    std::cout << "Monitoring extern drives every 3 seconds...\n\n" << std::endl;

    WHITELIST = load_whitelist(whitelistFile);

    while (true) {
        check_usb_devices();
        std::this_thread::sleep_for(3s);
    }
    return 0;
}