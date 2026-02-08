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
#include <Chess/Server/Target.h>

// ASIO Includes

// C++ Includes
#include <cstdint>
#include <vector>
#include <functional>
#include <memory>

namespace Chess {

    class Message;
    class Session;
    class SessionInfo;

    class SessionPool {
    public:
        SessionPool();
        ~SessionPool();

        SessionPool(const SessionPool&) = delete;
        SessionPool& operator=(const SessionPool&) = delete;

        void addSession(std::shared_ptr<Session> session);

        void removeSession(const Target& target);

        [[nodiscard]] bool hasSession(const Target& target) const;
        [[nodiscard]] bool empty() const;
        [[nodiscard]] std::uint32_t sessionCount() const;

        // --------------------------------------------------------
        // Template methods for all Target types
        // --------------------------------------------------------

        void message(const Target& target, const Message& msg);

        template <typename Func>
        requires std::invocable<Func&, Session&>
        void post(const Target& target, Func&& fn);

        template <typename Func>
        requires std::invocable<Func&, Session&>
        void visit(const Target& target, Func&& fn);

        std::vector<std::uint32_t> idSnapshot(const Target& target) const;

        std::vector<SessionInfo> infoSnapshot(const Target& target) const;

    private:
        std::vector<std::uint32_t> m_idToIndex;
        std::vector<std::shared_ptr<Session>> m_sessions;
    };

} // namespace Chess

#endif
