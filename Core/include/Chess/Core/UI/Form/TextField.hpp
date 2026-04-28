#ifndef CHESS_CORE_UI_FORM_TEXTFIELD_HPP
#define CHESS_CORE_UI_FORM_TEXTFIELD_HPP

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
#include <cstddef>
#include <functional>
#include <optional>
#include <string>
#include <string_view>

namespace Chess {

class TextField {

public:
    struct Config {
        std::string label;
        std::string initialValue;
        std::string placeholder;
        std::size_t maxLength{0};
        bool allowEmpty{true};
        bool secret{false};
        std::function<std::optional<std::string>(std::string_view)> validator;
    };

public:
    explicit TextField(Config config);

    [[nodiscard]] ftxui::Component component() const;
    [[nodiscard]] ftxui::Element renderRow() const;

    [[nodiscard]] const std::string& text() const noexcept;
    void setText(std::string value);

    [[nodiscard]] bool isValid() const noexcept;
    [[nodiscard]] const std::string& errorText() const noexcept;
    [[nodiscard]] const std::string& label() const noexcept;

private:
    void validate();

private:
    Config m_config{};
    std::string m_text;
    std::string m_error;
    int m_cursorPosition{0};

    ftxui::Component m_input;
    ftxui::Component m_component;
};

}

#endif