#include <iostream>
#include <atomic>
#include "server.h"

int main() {
    try {
        std::atomic_bool running = true;
        auto server = Server(running);
        server.run();
        std::string input;
        do {
            std::cin >> input;
        } while (input != "exit");
        running = false;
        server.join();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}