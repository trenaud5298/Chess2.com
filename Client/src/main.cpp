// simple_client_test.cpp
#include <asio.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>

int main() {
    try {
        asio::io_context io_context;

        // Connect to server
        asio::ip::tcp::socket socket(io_context);
        asio::ip::tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1"), 12345);
        socket.connect(endpoint);
        std::cout << "Connected to server!" << std::endl;

        char readBuffer[2048]; // make sure buffer is big enough for reply

        while (true) {
            std::cout << "Enter message (or 'quit'/'test'): ";
            std::string msg;
            std::getline(std::cin, msg);

            if (msg == "quit") break;

            if (msg == "test") {
                const int N = 1000;
                const int MSG_SIZE = 1000;
                std::vector<double> times;
                times.reserve(N);

                std::string testMsg(MSG_SIZE, 'X');

                std::cout << "Running benchmark: sending " << N << " messages..." << std::endl;

                for (int i = 0; i < N; ++i) {
                    auto start = std::chrono::high_resolution_clock::now();

                    asio::write(socket, asio::buffer(testMsg));

                    asio::error_code ec;
                    size_t len = socket.read_some(asio::buffer(readBuffer), ec);
                    if (ec) {
                        std::cerr << "Read error on iteration " << i << ": " << ec.message() << std::endl;
                        break;
                    }

                    auto end = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double, std::milli> rtt = end - start;
                    times.push_back(rtt.count());
                }

                // Compute average
                double sum = 0;
                for (double t : times) sum += t;
                double avg = times.empty() ? 0.0 : sum / times.size();

                std::cout << "Benchmark complete." << std::endl;
                std::cout << "Average round-trip time: " << avg << " ms" << std::endl;
                continue;
            }

            // Normal message
            msg += '\n'; // make sure server can parse it
            auto start = std::chrono::high_resolution_clock::now();
            asio::write(socket, asio::buffer(msg));

            asio::error_code ec;
            size_t len = socket.read_some(asio::buffer(readBuffer), ec);
            if (ec) {
                std::cerr << "Read error: " << ec.message() << std::endl;
                break;
            }
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> rtt = end - start;

            std::cout << "Server replied: ";
            std::cout.write(readBuffer, len);
            std::cout << std::endl;
            std::cout << "Round-trip time: " << rtt.count() << " ms" << std::endl;
        }

        socket.close();
        std::cout << "Disconnected from server." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Client exception: " << e.what() << std::endl;
    }

    return 0;
}
