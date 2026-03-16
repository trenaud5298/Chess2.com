#ifndef CHESS_SERVER_RUNTIME_PERSISTENCE_SETTINGS_HPP
#define CHESS_SERVER_RUNTIME_PERSISTENCE_SETTINGS_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes

// C++ Includes
#include <filesystem>
#include <shared_mutex>
#include <mutex>
#include <utility>

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
        Setting<std::string> serverName;
        Setting<std::string> serverPassword;

        // Defaults
        General(Settings& owner)
        : serverName(owner, "Chess Server"),
        serverPassword(owner, "") {}
    }; General general;

    struct Network {
        Setting<std::uint16_t> port;

        Network(Settings& owner)
        : port(owner, 24377) {}
    }; Network network;

private:
    void load();
    void save();

private:
    std::filesystem::path m_path;
    mutable std::shared_mutex m_mutex;
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