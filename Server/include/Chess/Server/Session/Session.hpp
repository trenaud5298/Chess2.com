#ifndef CHESS_SERVER__SESSION_SESSION_HPP
#define CHESS_SERVER__SESSION_SESSION_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes

// ASIO Includes
#include <asio.hpp>

// C++ Includes

namespace Chess {

class SessionPool;

class Session {

public:
    Session(asio::ip::tcp::socket socket, std::weak_ptr<SessionPool> parent, std::uint32_t id);
    ~Session();

    void message();
    void stop();

    template <typename Func>
    requires std::invocable<Func&, Session&>
    void post(Func&& func);

private:
    asio::strand<asio::io_context::executor_type> m_executor;
    asio::ip::tcp::socket m_socket;
    std::weak_ptr<SessionPool> m_parent;
    uint32_t m_id;
    std::atomic_bool m_stopping;
};


}

#endif