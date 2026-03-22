#ifndef CHESS_CLIENT_COMMON_SERVERINFO_HPP
#define CHESS_CLIENT_COMMON_SERVERINFO_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// C++ Includes
#include <string>

namespace Chess {

struct ServerInfo {
    std::string serverName;
    std::string ip;
    std::string password;
};

}

#endif