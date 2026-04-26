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

#include "Chess/Core/Game/Board.hpp"

namespace Chess {

namespace {

toml::array rgbToTomlArray(const RGBColor & color) {
    toml::array arr;
    arr.push_back(color.red);
    arr.push_back(color.green);
    arr.push_back(color.blue);
    return arr;
}

// Toml differentiates const toml::node from toml::node and does not
// convert the non const one to const. Thus there are two versions
// of this function to support both during load time and during runtime
std::optional<RGBColor> rgbFromTomlArray(const toml::node_view<const toml::node>& node) {
    const auto* arr = node.as_array();
    if (!arr || arr->size() != 3) {
        return std::nullopt;
    }

    auto red = (*arr)[0].value<std::uint8_t>();
    auto green = (*arr)[1].value<std::uint8_t>();
    auto blue = (*arr)[2].value<std::uint8_t>();

    if (!red || !green || !blue) {
        return std::nullopt;
    }

    return RGBColor{
        .red = *red,
        .green = *green,
        .blue = *blue
    };
}

std::optional<RGBColor> rgbFromTomlArray(const toml::node_view<toml::node>& node) {
    const auto* arr = node.as_array();
    if (!arr || arr->size() != 3) {
        return std::nullopt;
    }

    auto red = (*arr)[0].value<std::uint8_t>();
    auto green = (*arr)[1].value<std::uint8_t>();
    auto blue = (*arr)[2].value<std::uint8_t>();

    if (!red || !green || !blue) {
        return std::nullopt;
    }

    return RGBColor{
        .red = *red,
        .green = *green,
        .blue = *blue
    };
}

toml::table boardThemeToToml(const BoardTheme& theme) {
    return toml::table{
        {"lightSquare", rgbToTomlArray(theme.lightSquare)},
        {"darkSquare", rgbToTomlArray(theme.darkSquare)},
        {"whitePiece", rgbToTomlArray(theme.whitePiece)},
        {"blackPiece", rgbToTomlArray(theme.blackPiece)},
        {"cursorSquare", rgbToTomlArray(theme.cursorSquare)},
        {"selectedSquare", rgbToTomlArray(theme.selectedSquare)}
    };
}

// Toml differentiates const toml::node from toml::node and does not
// convert the non const one to const. Thus there are two versions
// of this function to support both during load time and during runtime
std::optional<BoardTheme> boardThemeFromToml(const toml::node_view<const toml::node>& node) {
    const auto* table = node.as_table();
    if (!table) {
        return std::nullopt;
    }

    auto lightSquare = rgbFromTomlArray((*table)["lightSquare"]);
    auto darkSquare = rgbFromTomlArray((*table)["darkSquare"]);
    auto whitePiece = rgbFromTomlArray((*table)["whitePiece"]);
    auto blackPiece = rgbFromTomlArray((*table)["blackPiece"]);
    auto cursorSquare = rgbFromTomlArray((*table)["cursorSquare"]);
    auto selectedSquare = rgbFromTomlArray((*table)["selectedSquare"]);

    if (!lightSquare || !darkSquare || !whitePiece || !blackPiece || !cursorSquare || !selectedSquare) {
        return std::nullopt;
    }

    return BoardTheme{
        .lightSquare = *lightSquare,
        .darkSquare = *darkSquare,
        .whitePiece = *whitePiece,
        .blackPiece = *blackPiece,
        .cursorSquare = *cursorSquare,
        .selectedSquare = *selectedSquare
    };
}

std::optional<BoardTheme> boardThemeFromToml(const toml::node_view<toml::node>& node) {
    const auto* table = node.as_table();
    if (!table) {
        return std::nullopt;
    }

    auto lightSquare = rgbFromTomlArray((*table)["lightSquare"]);
    auto darkSquare = rgbFromTomlArray((*table)["darkSquare"]);
    auto whitePiece = rgbFromTomlArray((*table)["whitePiece"]);
    auto blackPiece = rgbFromTomlArray((*table)["blackPiece"]);
    auto cursorSquare = rgbFromTomlArray((*table)["cursorSquare"]);
    auto selectedSquare = rgbFromTomlArray((*table)["selectedSquare"]);

    if (!lightSquare || !darkSquare || !whitePiece || !blackPiece || !cursorSquare || !selectedSquare) {
        return std::nullopt;
    }

    return BoardTheme{
        .lightSquare = *lightSquare,
        .darkSquare = *darkSquare,
        .whitePiece = *whitePiece,
        .blackPiece = *blackPiece,
        .cursorSquare = *cursorSquare,
        .selectedSquare = *selectedSquare
    };
}

}






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

BoardTheme Settings::getBoardTheme() const {
    std::shared_lock lock(m_mutex);

    if (auto theme = boardThemeFromToml(m_displayTable["board_theme"])) {
        return *theme;
    }

    return BoardTheme{};
}

void Settings::setBoardTheme(const BoardTheme& value) {
    {
        std::unique_lock lock(m_mutex);
        m_displayTable.insert_or_assign("board_theme", boardThemeToToml(value));
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
        {"board_theme", boardThemeToToml(BoardTheme{})}
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
    if (auto theme = boardThemeFromToml(file["Display"]["board_theme"]))
        m_displayTable.insert_or_assign("board_theme", boardThemeToToml(*theme));

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
