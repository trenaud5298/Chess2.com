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
#include <Chess/Server/Target/TargetConcept.h>

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

        template <TargetConcept TargetType>
        void removeSession(const TargetType& target);

        template <TargetConcept TargetType>
        [[nodiscard]] bool hasSession(const TargetType& target) const;
        [[nodiscard]] bool empty() const;
        [[nodiscard]] std::uint32_t sessionCount() const;

        // --------------------------------------------------------
        // Template methods for all Target types
        // --------------------------------------------------------

        template <TargetConcept TargetType>
        void message(const TargetType& target, const Message& msg);

        template <TargetConcept TargetType, typename Func>
        requires std::invocable<Func, Session&>
        void post(const TargetType& target, Func&& fn);

        template <TargetConcept TargetType, typename Func>
        requires std::invocable<Func, Session&>
        void visit(const TargetType& target, Func&& fn);

        template <TargetConcept TargetType>
        std::vector<std::uint32_t> idSnapshot(const TargetType& target) const;

        template <TargetConcept TargetType>
        std::vector<SessionInfo> infoSnapshot(const TargetType& target) const;

    private:
        std::vector<std::uint32_t> m_idToIndex;
        std::vector<std::shared_ptr<Session>> m_sessions;
    };

} // namespace Chess

#endif
