#include <iostream>

#include "Chess/Server/GameServer.hpp"

int main() {
    std::cout<<"Server"<<std::endl;
    Chess::GameServerConfig config;
    Chess::GameServer server(config);

    std::cout << "Running... Press Enter to exit." << std::endl;
    std::cin.get(); // Pauses until Enter is pressed
    return 0;
}
