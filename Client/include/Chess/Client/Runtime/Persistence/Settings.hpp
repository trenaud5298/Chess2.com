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

// TOML Includes
#include <toml++/toml.hpp>

// C++ Includes
#include <filesystem>
#include <shared_mutex>
#include <mutex>
#include <utility>
#include <vector>

namespace Chess {

class Settings;

template <typename T>
class Setting {

    friend class Settings;

public:
    Setting(Settings& owner, T defaultValue);
    operator T() const;
    Setting& operator=(const T& value);

private:
    const T& getUnsafe() const;
    void setUnsafe(const T& value);

    Settings& m_owner;
    T m_value;
};



class Settings {

    template<typename>
    friend class Setting;

public:
    explicit Settings();
    ~Settings() = default;

    Settings(const Settings&) = delete;
    Settings& operator=(const Settings&) = delete;
    Settings(Settings&&) = delete;
    Settings& operator=(Settings&&) = delete;

    struct General {
        Setting<std::string> username;

        // Defaults
        General(Settings& owner)
        : username(owner, "Chess Server") {}

    }; General general;

    struct Board {
        Setting<std::uint8_t> boardScale;

        Board(Settings& owner)
        : boardScale(owner, 3) {}

    }; Board board;

    struct Network {
        Setting<std::vector<ServerInfo>> servers;

        Network(Settings& owner)
        : servers(owner, {}) {}
    }; Network network;

    void setAllSettings(General& generala, Board& board, Network& network);
    std::tuple<General, Board, Network> getAllSettings();

private:
    void load();
    void save();

private:
    std::filesystem::path m_path;
    mutable std::shared_mutex m_mutex;

    toml::table m_table;
    toml::table m_generalTable;
    toml::table m_boardTable;
    toml::table m_networkTable;
};





template<typename T>
Setting<T>::Setting(Settings& owner, T defaultValue)
: m_owner(owner), m_value(std::move(defaultValue)) {}

template<typename T>
Setting<T>::operator T() const {
    std::shared_lock lock(m_owner.m_mutex);
    return m_value;
}

template<typename T>
Setting<T>& Setting<T>::operator=(const T& value) {
    std::unique_lock lock(m_owner.m_mutex);
    if (m_value == value)
        return *this;

    m_value = value;
    m_owner.save();
    return *this;
}

template<typename T>
const T& Setting<T>::getUnsafe() const {
    return m_value;
}

template<typename T>
void Setting<T>::setUnsafe(const T &value) {
    m_value = value;
}

}

#endif