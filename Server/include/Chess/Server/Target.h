#ifndef CHESS_SERVER_TARGET_H
#define CHESS_SERVER_TARGET_H
#include <cstdint>
#include <functional>
#include <variant>
#include <vector>
#include <memory>

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

namespace Chess {

// Helper Template For std::visit (See cppreference for details)
template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

class Session;

class Target {
public:
    static Target All();
    static Target Id(std::vector<std::uint32_t> targetIds);
    static Target Predicate(std::function<bool(const Session&)> predicate);

    template <typename Func>
    requires std::invocable<Func&, Session&>
    void forEach(const std::vector<std::uint32_t>& idToSessionIndex, const std::vector<std::shared_ptr<Session>>& sessions, Func&& func) const {
        std::visit(overloaded{

            [&](const AllPayload&) {
                for (const std::shared_ptr<Session>& session : sessions) {
                    std::invoke(func, *session);
                }
            },

            [&](const IdPayload& payload) {
                const std::size_t idMapSize   = idToSessionIndex.size();
                const std::size_t sessionSize = sessions.size();
                for (const std::uint32_t targetId : payload.targetIds) {
                    if (targetId >= idMapSize) continue;

                    const std::uint32_t sessionIndex = idToSessionIndex[targetId];
                    if (sessionIndex >= sessionSize) continue;

                    std::invoke(func, *sessions[sessionIndex]);
                }
            },

            [&](const PredicatePayload& payload) {
                for (const std::shared_ptr<Session>& session : sessions) {
                    Session& sessionRef = *session;
                    if (payload.predicate(sessionRef)) {
                        std::invoke(func, sessionRef);
                    }
                }
            }
        }, m_impl);
    }

private:
    struct AllPayload{};
    struct IdPayload{std::vector<std::uint32_t> targetIds;};
    struct PredicatePayload{std::function<bool(const Session&)> predicate;};

    explicit Target(std::variant<AllPayload, IdPayload, PredicatePayload>);
    std::variant<AllPayload, IdPayload, PredicatePayload> m_impl;
};


}

#endif