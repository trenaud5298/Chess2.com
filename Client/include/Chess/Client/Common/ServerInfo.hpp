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

// TOML Includes
#include <toml++/toml.hpp>

namespace Chess {

struct ServerInfo {
    std::string serverName;
    std::string ip;
    std::string password;
    std::int64_t uuid;

    toml::table toToml() const {
        return toml::table{
            {"server_name", serverName},
            {"ip", ip},
            {"password", password},
            {"uuid", uuid}
        };
    }

    static std::optional<ServerInfo> fromToml(const toml::table& table) {
        auto ip = table["ip"].value<std::string>();
        if (!ip || ip->empty()) { return std::nullopt; }

        return ServerInfo{
            .serverName = table["server_name"].value_or<std::string>(""),
            .ip = *ip,
            .password = table["password"].value_or<std::string>(""),
            .uuid = table["uuid"].value_or<std::int64_t>(0)
        };
    }
};

}

#endif