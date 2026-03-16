/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Runtime/Persistence/Settings.hpp>

// TOML Includes
#include <toml++/toml.hpp>

// C++ Includes
#include <fstream>
#include <iostream>
#include <utility>

namespace Chess {

Settings::Settings() : general(*this), network(*this), m_path(std::filesystem::current_path() / "Server" / "Settings.toml") {
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

    if (auto v = table["General"]["server_name"].value<std::string>())
        general.serverName.setUnsafe(*v);
    if (auto v = table["General"]["server_password"].value<std::string>())
        general.serverPassword.setUnsafe(*v);

    if (auto v = table["Network"]["port"].value<std::uint16_t>())
        network.port.setUnsafe(*v);

}

void Settings::save() {
    std::shared_lock lock(m_mutex);

    toml::table table;

    // General Settings
    toml::table generalTable;
    generalTable.insert_or_assign("server_name", general.serverName.getUnsafe());
    generalTable.insert_or_assign("server_password", general.serverPassword.getUnsafe());

    table.insert_or_assign("General", std::move(generalTable));

    // Network Settings
    toml::table networkTable;
    networkTable.insert_or_assign("port", network.port.getUnsafe());

    table.insert_or_assign("Network", std::move(networkTable));

    std::filesystem::create_directories(m_path.parent_path());
    std::ofstream file(m_path);
    if (!file) return;
    file << table;
}
}
