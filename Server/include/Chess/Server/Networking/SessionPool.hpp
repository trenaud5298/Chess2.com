#ifndef CHESS_SERVER_SESSIONPOOL_H
#define CHESS_SERVER_SESSIONPOOL_H

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Networking/Target.hpp>

// ASIO Includes
#include <asio.hpp>

// C++ Includes
#include <cstdint>
#include <vector>
#include <functional>
#include <memory>
#include <shared_mutex>
#include <unordered_map>


namespace Chess {

    class GameServer;
    class Target;
    class Message;
    class Session;
    class SessionInfo;

    class SessionPool {
    public:
        SessionPool(GameServer* gameServer);
        ~SessionPool();

        SessionPool(const SessionPool&) = delete;
        SessionPool& operator=(const SessionPool&) = delete;

        std::uint32_t addSession(asio::ip::tcp::socket socket);
        void removeSession(const Target& target);

        void message(const Target& target, const Message& msg);

        template <typename Func>
        requires std::invocable<Func&, Session&>
        void post(const Target& target, Func&& func);

        template <typename Func>
        requires std::invocable<Func&, Session&>
        void visit(const Target& target, Func&& func);

        [[nodiscard]] bool hasSession(const Target& target) const;
        [[nodiscard]] bool empty() const;
        [[nodiscard]] std::uint32_t sessionCount() const;

        std::vector<std::uint32_t> idSnapshot(const Target& target) const;

        std::vector<SessionInfo> infoSnapshot(const Target& target) const;

    private:
        mutable std::shared_mutex m_sessionMutex;
        std::uint32_t m_nextSessionId;
        std::unordered_map<std::uint32_t, std::uint32_t> m_idToIndex;
        std::vector<std::shared_ptr<Session>> m_sessions;

        GameServer* m_gameServer;
    };

} // namespace Chess

#endif
