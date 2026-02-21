#include <asio.hpp>
#include <iostream>
#include <string>
#include <optional>
#include <thread>

// Chess
#include <Chess/Core/Networking/Message.hpp>

static constexpr std::uint16_t SERVER_PORT = 24377;

void doReadHeader(
    asio::ip::tcp::socket& socket,
    Chess::Message& msg);

void doReadBody(
    asio::ip::tcp::socket& socket,
    Chess::Message& msg);

void doReadHeader(
    asio::ip::tcp::socket& socket,
    Chess::Message& msg)
{
    msg.clear();

    asio::async_read(
        socket,
        msg.headerBuffer(),
        [&](std::error_code ec, std::size_t)
        {
            if (ec)
            {
                std::cout << "Disconnected from server.\n";
                return;
            }

            if (!msg.validateHeader())
            {
                std::cout << "Invalid header from server.\n";
                return;
            }

            msg.resize(msg.header().bodyLength);

            doReadBody(socket, msg);
        });
}

void doReadBody(
    asio::ip::tcp::socket& socket,
    Chess::Message& msg)
{
    asio::async_read(
        socket,
        msg.bodyBuffer(),
        [&](std::error_code ec, std::size_t)
        {
            if (ec)
            {
                std::cout << "Disconnected from server.\n";
                return;
            }

            try
            {
                std::string text = msg.readString();
                std::cout << "\n[Server] " << text << "\n> " << std::flush;
            }
            catch (...)
            {
                std::cout << "Failed to parse message\n";
            }

            doReadHeader(socket, msg);
        });
}

int main()
{
    try
    {
        asio::io_context io_context;
        auto work = asio::make_work_guard(io_context);
        std::optional<asio::ip::tcp::socket> socket;

        Chess::Message incoming(Chess::NONE);

        std::thread ioThread([&]()
        {
            io_context.run();
        });

        std::cout << "Client ready.\n";
        std::cout << "Commands:\n";
        std::cout << "  connect <ip>\n";
        std::cout << "  disconnect\n";
        std::cout << "  quit\n";
        std::cout << "  <any other text sends a message>\n";

        while (true)
        {
            std::cout << "> ";

            std::string input;
            if (!std::getline(std::cin, input))
                break;

            // -------- quit --------
            if (input == "quit")
            {
                if (socket && socket->is_open())
                    socket->close();

                io_context.stop();
                break;
            }

            // -------- connect --------
            if (input.rfind("connect ", 0) == 0)
            {
                if (socket && socket->is_open())
                {
                    std::cout << "Already connected.\n";
                    continue;
                }

                std::string ip = input.substr(8);

                try
                {
                    asio::ip::tcp::endpoint endpoint(
                        asio::ip::make_address(ip),
                        SERVER_PORT
                    );

                    socket.emplace(io_context);
                    socket->connect(endpoint);

                    std::cout << "Connected to "
                              << ip << ":" << SERVER_PORT << "\n";

                    doReadHeader(*socket, incoming);
                }
                catch (const std::exception& e)
                {
                    socket.reset();
                    std::cout << "Connection failed: " << e.what() << "\n";
                }

                continue;
            }

            // -------- disconnect --------
            if (input == "disconnect")
            {
                if (!socket || !socket->is_open())
                {
                    std::cout << "Not connected.\n";
                    continue;
                }

                socket->close();
                socket.reset();

                std::cout << "Disconnected.\n";
                continue;
            }

            // -------- send --------
            if (!socket || !socket->is_open())
            {
                std::cout << "Not connected.\n";
                continue;
            }

            try
            {
                Chess::Message msg(Chess::CHAT);
                msg.pushString(input);

                asio::write(*socket, msg.buffers());

                std::cout << "Sent message (" << msg.bodySize() << " bytes)\n";
            }
            catch (const std::exception& e)
            {
                std::cout << "Send failed: " << e.what() << "\n";

                if (socket && socket->is_open())
                    socket->close();

                socket.reset();
            }
        }

        work.reset();
        io_context.stop();
        ioThread.join();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Client exception: " << e.what() << "\n";
    }

    return 0;
}