// main_server.cpp
#include <Chess/Server/GameServer.h>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    try {
        // Create server on localhost:12345 with default thread count
        Chess::GameServer server("127.0.0.1", 12345, 5);

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
