#ifndef CHESS_CORE_UI_FORM_INTEGERFIELD_HPP
#define CHESS_CORE_UI_FORM_INTEGERFIELD_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */


// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <string>

namespace Chess {

class IntegerField {

public:
    struct Config {
        std::string label;
        int initialValue{0};
        int minValue{0};
        int maxValue{100};
        bool allowEmpty{false};
    };

public:
    explicit IntegerField(Config config);

    [[nodiscard]] ftxui::Component component() const;
    [[nodiscard]] ftxui::Element renderRow() const;

    [[nodiscard]] int value() const noexcept;
    void setValue(int value);

    [[nodiscard]] bool isValid() const noexcept;
    [[nodiscard]] const std::string& errorText() const noexcept;
    [[nodiscard]] const std::string& label() const noexcept;

private:
    void validate();
    void syncTextFromValue();

private:
    Config m_config{};
    std::string m_text;
    std::string m_error;
    int m_value{0};
    int m_cursorPosition{0};

    ftxui::Component m_input;
    ftxui::Component m_component;
};

}

#endif