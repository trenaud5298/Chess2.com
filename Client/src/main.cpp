#include <asio.hpp>
#include <iostream>
#include <string>
#include <optional>
#include <thread>
#include <vector>
#include <cctype>

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

// Parse tokens from `s` where tokens are separated by spaces, but double-quoted
// substrings are treated as single tokens. Example:
//   connect 127.0.0.1 "my pass" "John Doe"
static std::vector<std::string> splitArgsRespectQuotes(const std::string& s)
{
    std::vector<std::string> out;
    std::size_t i = 0;
    const std::size_t n = s.size();

    while (i < n)
    {
        // skip spaces
        while (i < n && std::isspace(static_cast<unsigned char>(s[i])))
            ++i;
        if (i >= n) break;

        if (s[i] == '"')
        {
            // quoted token
            ++i; // skip opening quote
            std::string token;
            while (i < n)
            {
                if (s[i] == '"')
                {
                    ++i; // skip closing quote
                    break;
                }
                // allow simple backslash escaping of quotes/backslash if desired
                if (s[i] == '\\' && i + 1 < n)
                {
                    ++i;
                    token.push_back(s[i]);
                    ++i;
                }
                else
                {
                    token.push_back(s[i]);
                    ++i;
                }
            }
            out.push_back(std::move(token));
        }
        else
        {
            // unquoted token
            std::string token;
            while (i < n && !std::isspace(static_cast<unsigned char>(s[i])))
            {
                token.push_back(s[i]);
                ++i;
            }
            out.push_back(std::move(token));
        }
    }

    return out;
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
        std::cout << "  connect <ip> <password> <name>   (password/name may be quoted)\n";
        std::cout << "  disconnect\n";
        std::cout << "  quit\n";
        std::cout << "  <any other text sends a chat message>\n";

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

                // parse arguments after "connect "
                const std::string rest = input.substr(8);
                auto tokens = splitArgsRespectQuotes(rest);

                if (tokens.size() < 3)
                {
                    std::cout << "Usage: connect <ip> <password> <name>\n";
                    std::cout << "  Use quotes to include spaces, e.g. connect 127.0.0.1 \"p@ss word\" \"John Doe\"\n";
                    continue;
                }

                std::string ip = tokens[0];
                std::string password = tokens[1];
                std::string name = tokens[2];

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

                    // start async read loop
                    doReadHeader(*socket, incoming);

                    // Immediately send LOGIN message: first message must be login
                    try
                    {
                        Chess::Message loginMsg(Chess::LOGIN);
                        loginMsg.pushString(password);
                        loginMsg.pushString(name);

                        asio::write(*socket, loginMsg.buffers());

                        std::cout << "Sent LOGIN (name: " << name << ")\n";
                    }
                    catch (const std::exception& e)
                    {
                        std::cout << "Failed to send LOGIN: " << e.what() << "\n";
                        socket->close();
                        socket.reset();
                    }
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

            // -------- send chat --------
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