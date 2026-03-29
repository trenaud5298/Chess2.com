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
#include <utility>

namespace Chess {

Settings::Settings() : m_path(std::filesystem::current_path() / "Client" / "Settings.toml"),
m_table(makeDefaultTable()), m_generalTable(*m_table["General"].as_table()), m_displayTable(*m_table["Display"].as_table()), m_networkTable(*m_table["Network"].as_table()){
    load();
    save();
}

std::string Settings::getUsername() const {
    std::shared_lock lock(m_mutex);
    return m_generalTable["username"].value<std::string>().value();
}

void Settings::setUsername(const std::string& value) {
    {
        std::unique_lock lock(m_mutex);
        m_generalTable.insert_or_assign("username", value);
    }
    if (m_batchCount.load(std::memory_order_relaxed) == 0) save();
}

std::uint8_t Settings::getBoardScale() const {
    std::shared_lock lock(m_mutex);
    return m_displayTable["board_scale"].value<std::uint8_t>().value();
}

void Settings::setBoardScale(std::uint8_t value) {
    {
        std::unique_lock lock(m_mutex);
        m_displayTable.insert_or_assign("board_scale", static_cast<int64_t>(value));
    }
    if (m_batchCount.load(std::memory_order_relaxed) == 0) save();
}

std::vector<ServerInfo> Settings::getServers() const {
    std::shared_lock lock(m_mutex);
    std::vector<ServerInfo> result;
    const auto* arr = m_networkTable["servers"].as_array();
    if (!arr) return result;
    // Iterates Through Array, Tries To Make Item Into Table, Tries To Make Table Into ServerInfo
    for (auto& entry : *arr) {
        if (const auto* table = entry.as_table()) {
            if (auto info = ServerInfo::fromToml(*table)) {
                result.push_back(std::move(*info));
            }
        }
    }
    return result;
}

void Settings::setServers(const std::vector<ServerInfo>& servers) {
    {
        std::unique_lock lock(m_mutex);
        toml::array arr;
        for (const auto& s : servers)
            arr.push_back(s.toToml());
        m_networkTable.insert_or_assign("servers", std::move(arr));
    }
    if (m_batchCount.load(std::memory_order_relaxed) == 0) save();
}

toml::table Settings::makeDefaultTable() {
    // General Settings
    toml::table generalTable{
        {"username", std::string("")}
    };

    // Display Settings
    toml::table displayTable{
        {"board_scale", int64_t{3}}
    };

    // Network Settings
    toml::table networkTable{
        {"servers", toml::array{}}
    };

    return toml::table{
        {"General", std::move(generalTable)},
        {"Display", std::move(displayTable)},
        {"Network", std::move(networkTable)}
    };
}

void Settings::load() {
    std::unique_lock lock(m_mutex);
    if (!std::filesystem::exists(m_path)) return;

    auto result = toml::parse_file(m_path.string());
    if (!result) return;

    const toml::table& file = result.table();

    // General
    if (auto v = file["General"]["username"].value<std::string>())
        m_generalTable.insert_or_assign("username", *v);

    // Display
    if (auto v = file["Display"]["board_scale"].value<int64_t>())
        m_displayTable.insert_or_assign("board_scale", *v);

    // Network
    if (const auto* arr = file["Network"]["servers"].as_array()) {
        toml::array validated;
        for (const auto& entry : *arr) {
            if (const auto* table = entry.as_table()) {
                if (auto info = ServerInfo::fromToml(*table)) {
                    validated.push_back(info->toToml());
                }
            }
        }
        m_networkTable.insert_or_assign("servers", std::move(validated));
    }
}

void Settings::save() {
    std::shared_lock lock(m_mutex);
    std::filesystem::create_directories(m_path.parent_path());
    std::ofstream file(m_path);
    if (file) file << m_table;
}

}
