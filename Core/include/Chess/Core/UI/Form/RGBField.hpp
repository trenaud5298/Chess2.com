#ifndef CHESS_CORE_UI_FORM_RGBFIELD_HPP
#define CHESS_CORE_UI_FORM_RGBFIELD_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/UI/BoardTheme.hpp>
#include <Chess/Core/UI/Form//IntegerField.hpp>

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <string>

namespace Chess {

class RGBField {

public:
    struct Config {
        std::string label;
        RGBColor initialValue{};
        bool showPreview{true};
    };

public:
    explicit RGBField(Config config);

    [[nodiscard]] ftxui::Component component() const;
    [[nodiscard]] ftxui::Element renderRow() const;

    [[nodiscard]] RGBColor value() const;
    void setValue(RGBColor value);

    [[nodiscard]] bool isValid() const noexcept;
    [[nodiscard]] const std::string& label() const noexcept;

private:
    Config m_config{};

    IntegerField m_redField;
    IntegerField m_greenField;
    IntegerField m_blueField;

    ftxui::Component m_container;
};

}

#endif