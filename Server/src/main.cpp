// main_server.cpp
#include <Chess/Server/GameServer.h>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    try {
        Chess::GameServer server("0.0.0.0", 24377, 5);

        std::cout << "Starting server..." << std::endl;
        server.start();

        std::cout << "Press Enter to stop server..." << std::endl;
        std::cin.get();

        server.stop();
        std::cout << "Server stopped." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Server exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
