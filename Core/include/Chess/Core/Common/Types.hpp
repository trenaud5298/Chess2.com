#ifndef CHESS_SERVER_RUNTIME_COMMON_TYPES_HPP
#define CHESS_SERVER_RUNTIME_COMMON_TYPES_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

#include <cstdint>

namespace Chess {

using SessionID = std::uint64_t;
using RoomID = std::uint64_t;

}

#endif