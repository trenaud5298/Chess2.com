#ifndef CHESS_CLIENT_RUNTIME_PERSISTENCE_SETTINGS_HPP
#define CHESS_CLIENT_RUNTIME_PERSISTENCE_SETTINGS_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/Common/ServerInfo.hpp>
#include <Chess/Core/UI/BoardTheme.hpp>

// TOML Includes
#include <toml++/toml.hpp>

// C++ Includes
#include <filesystem>
#include <shared_mutex>
#include <mutex>
#include <utility>
#include <vector>
#include <atomic>


namespace Chess {
class Settings {
public:
    explicit Settings();

    // Special BatchUpdate Struct
    // Acquire A BatchUpdate Struct
    // To Temporarily Disable Saving.
    // When BatchUpdate Is Destroyed
    // A Save Occurs
    struct BatchUpdate {
        explicit BatchUpdate(Settings& s) : m_settings(s) {
            m_settings.m_batchCount.fetch_add(1, std::memory_order_acquire);
        }
        ~BatchUpdate() {
            if (m_settings.m_batchCount.fetch_sub(1, std::memory_order_release) == 1) {
                m_settings.save();
            }
        }
        BatchUpdate(const BatchUpdate&) = delete;
        BatchUpdate& operator=(const BatchUpdate&) = delete;
    private:
        Settings& m_settings;
    };

    // General
    std::string getUsername() const;
    void setUsername(const std::string& value);

    // Board
    BoardTheme getBoardTheme() const;
    void setBoardTheme(const BoardTheme& value);

    // Network
    std::vector<ServerInfo> getServers() const;
    void setServers(const std::vector<ServerInfo>& value);

private:
    static toml::table makeDefaultTable();
    void load();
    void save();

    std::atomic<std::uint64_t> m_batchCount{0};
    std::filesystem::path m_path;
    mutable std::shared_mutex m_mutex;
    toml::table m_table;
    toml::table& m_generalTable;
    toml::table& m_displayTable;
    toml::table& m_networkTable;
};

}

#endif