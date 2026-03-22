/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/Runtime/Persistence/Settings.hpp>

// TOML Includes
#include <toml++/toml.hpp>

// C++ Includes
#include <fstream>
#include <iostream>
#include <utility>

namespace Chess {

Settings::Settings() : general(*this), board(*this), network(*this), m_path(std::filesystem::current_path() / "Client" / "Settings.toml") {
    load();
    save();
}

void Settings::load() {
    std::unique_lock lock(m_mutex);
    if (!std::filesystem::exists(m_path)) {
        return;
    }

    toml::parse_result result = toml::parse_file(m_path.string());
    if (!result) {
        return;
    }

    toml::table& table = result.table();

    if (auto v = table["General"]["username"].value<std::string>())
        general.username.setUnsafe(*v);

    if (auto v = table["Board"]["board_scale"].value<std::uint8_t>())
        board.boardScale.setUnsafe(*v);

    if (auto* array = table["Network"]["servers"].as_array()) {
        std::vector<ServerInfo> parsed;
        for (auto& entry : *array) {
            if (auto* tbl = entry.as_table()) {
                ServerInfo info;
                info.serverName = tbl->get_as<std::string>("server_name")->value_or("");
                info.ip = tbl->get_as<std::string>("ip")->value_or("");
                info.password = tbl->get_as<std::string>("password")->value_or("");
                if (info.ip != "") {
                    parsed.push_back(std::move(info));
                }
            }
        }
        network.servers.setUnsafe(std::move(parsed));
    }

}

void Settings::save() {
    std::shared_lock lock(m_mutex);

    toml::table table;

    // General Settings
    toml::table generalTable;
    generalTable.insert_or_assign("username", general.username.getUnsafe());

    table.insert_or_assign("General", std::move(generalTable));

    // Board Settings
    toml::table boardTable;
    boardTable.insert_or_assign("board_scale", board.boardScale.getUnsafe());

    table.insert_or_assign("Board", std::move(boardTable));

    // Network Settings
    toml::table networkTable;
    toml::array serverArray = toml::array{};
    for (const auto& server : network.servers.getUnsafe()) {
        serverArray.push_back(toml::table{
            {"server_name", server.serverName},
            {"ip", server.ip},
            {"password", server.password},
        });
    }
    // Assuming you build a toml::table per category:
    networkTable.insert("servers", std::move(serverArray));

    table.insert_or_assign("Network", std::move(networkTable));

    std::filesystem::create_directories(m_path.parent_path());
    std::ofstream file(m_path);
    if (!file) return;
    file << table;
}
}
