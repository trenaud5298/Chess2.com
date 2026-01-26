#include <asio.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <optional>

static constexpr std::uint16_t SERVER_PORT = 24377;

int main() {
    try {
        asio::io_context io_context;
        std::optional<asio::ip::tcp::socket> socket;

        char readBuffer[2048];

        std::cout << "Client ready.\n";
        std::cout << "Commands: connect <ip>, disconnect, test, quit\n";

        while (true) {
            std::cout << "> ";
            std::string input;
            if (!std::getline(std::cin, input))
                break;

            // -------- quit --------
            if (input == "quit") {
                if (socket && socket->is_open()) {
                    socket->close();
                }
                std::cout << "Exiting client.\n";
                break;
            }

            // -------- connect <ip> --------
            if (input.rfind("connect ", 0) == 0) {
                if (socket && socket->is_open()) {
                    std::cout << "Already connected. Disconnect first.\n";
                    continue;
                }

                std::string ip = input.substr(8);

                try {
                    asio::ip::tcp::endpoint endpoint(
                        asio::ip::make_address(ip),
                        SERVER_PORT
                    );

                    socket.emplace(io_context);
                    socket->connect(endpoint);

                    std::cout << "Connected to " << ip << ":" << SERVER_PORT << "\n";
                }
                catch (const std::exception& e) {
                    socket.reset();
                    std::cerr << "Connect failed: " << e.what() << "\n";
                }
                continue;
            }

            // -------- disconnect --------
            if (input == "disconnect") {
                if (!socket || !socket->is_open()) {
                    std::cout << "Not connected.\n";
                } else {
                    socket->close();
                    socket.reset();
                    std::cout << "Disconnected.\n";
                }
                continue;
            }

            // -------- test --------
            if (input == "test") {
                if (!socket || !socket->is_open()) {
                    std::cout << "Not connected.\n";
                    continue;
                }

                const int N = 1000;
                const int MSG_SIZE = 1000;
                std::vector<double> times;
                times.reserve(N);

                std::string testMsg(MSG_SIZE, 'X');

                std::cout << "Running benchmark (" << N << " messages)...\n";

                for (int i = 0; i < N; ++i) {
                    auto start = std::chrono::high_resolution_clock::now();

                    asio::write(*socket, asio::buffer(testMsg));

                    asio::error_code ec;
                    size_t len = socket->read_some(asio::buffer(readBuffer), ec);
                    if (ec) {
                        std::cerr << "Read error: " << ec.message() << "\n";
                        break;
                    }

                    auto end = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double, std::milli> rtt = end - start;
                    times.push_back(rtt.count());
                }

                double sum = 0.0;
                for (double t : times) sum += t;
                double avg = times.empty() ? 0.0 : sum / times.size();

                std::cout << "Benchmark complete.\n";
                std::cout << "Average RTT: " << avg << " ms\n";
                continue;
            }

            // -------- normal message --------
            if (!socket || !socket->is_open()) {
                std::cout << "Not connected.\n";
                continue;
            }

            std::string msg = input + '\n';

            auto start = std::chrono::high_resolution_clock::now();
            asio::write(*socket, asio::buffer(msg));

            asio::error_code ec;
            size_t len = socket->read_some(asio::buffer(readBuffer), ec);
            if (ec) {
                std::cerr << "Read error: " << ec.message() << "\n";
                socket->close();
                socket.reset();
                continue;
            }

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> rtt = end - start;

            std::cout << "Server replied: ";
            std::cout.write(readBuffer, len);
            std::cout << "\nRTT: " << rtt.count() << " ms\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Client exception: " << e.what() << "\n";
    }

    return 0;
}
