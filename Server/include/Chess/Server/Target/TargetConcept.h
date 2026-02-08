#ifndef CHESS_SERVER_TARGET_TARGETCONCEPT_H
#define CHESS_SERVER_TARGET_TARGETCONCEPT_H

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
#include <vector>
#include <memory>
#include <cstdint>
#include <concepts>
#include <functional>

namespace Chess {

class Session;

template <typename R>
concept SessionRangeConcept = requires(R r) {
    { *r.begin() } -> std::same_as<Session&>;
};

template <typename T>
concept TargetConcept = requires (
    const T& target,
    const std::vector<std::uint32_t>& idToSessionIndex,
    const std::vector<std::shared_ptr<Session>>& sessions
)
{
    target.forEach(idToSessionIndex, sessions, std::declval<std::function<void(Session&)>>());
    { target.range(idToSessionIndex, sessions) } -> SessionRangeConcept;
};

}

#endif