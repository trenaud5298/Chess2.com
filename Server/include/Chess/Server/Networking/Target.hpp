#ifndef CHESS_SERVER_TARGET_H
#define CHESS_SERVER_TARGET_H
/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes

// ASIO Includes

// C++ Includes
#include <cstdint>
#include <functional>
#include <utility>
#include <variant>
#include <vector>
#include <memory>
#include <unordered_map>


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
    static Target All() {return Target(AllPayload());}
    static Target Id(std::vector<std::uint32_t> targetIds) {return Target(IdPayload(std::move(targetIds)));}
    static Target Predicate(std::function<bool(const Session&)> predicate) {return Target(PredicatePayload(std::move(predicate)));}

    template <typename Func>
    requires std::invocable<Func&, Session&>
    void forEach(const std::unordered_map<std::uint32_t, std::uint32_t>& idToSessionIndex, const std::vector<std::shared_ptr<Session>>& sessions, Func&& func) const {
        std::visit(overloaded{

            [&](const AllPayload&) {
                for (const std::shared_ptr<Session>& session : sessions) {
                    std::invoke(std::forward<Func>(func), *session);
                }
            },

            [&](const IdPayload& payload) {
                for (const std::uint32_t targetId : payload.targetIds) {
                    auto sessionIndex = idToSessionIndex.find(targetId);
                    if (sessionIndex == idToSessionIndex.end()) {
                        continue;
                    }
                    std::invoke(std::forward<Func>(func), *sessions[sessionIndex->second]);
                }
            },

            [&](const PredicatePayload& payload) {
                for (const std::shared_ptr<Session>& session : sessions) {
                    Session& sessionRef = *session;
                    if (payload.predicate(sessionRef)) {
                        std::invoke(std::forward<Func>(func), sessionRef);
                    }
                }
            }
        }, m_impl);
    }

private:
    struct AllPayload{};
    struct IdPayload{std::vector<std::uint32_t> targetIds;};
    struct PredicatePayload{std::function<bool(const Session&)> predicate;};

    explicit Target(std::variant<AllPayload, IdPayload, PredicatePayload> impl) : m_impl(std::move(impl)) {}
    std::variant<AllPayload, IdPayload, PredicatePayload> m_impl;
};


}

#endif