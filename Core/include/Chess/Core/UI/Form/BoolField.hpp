#ifndef CHESS_CORE_UI_FORM_BOOLFIELD_HPP
#define CHESS_CORE_UI_FORM_BOOLFIELD_HPP

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

class BoolField {

public:
    struct Config {
        std::string label;
        bool initialValue{false};
        std::string trueLabel{"Yes"};
        std::string falseLabel{"No"};
    };

public:
    explicit BoolField(Config config);

    [[nodiscard]] ftxui::Component component() const;
    [[nodiscard]] ftxui::Element renderRow() const;

    [[nodiscard]] bool value() const noexcept;
    void setValue(bool value);

    [[nodiscard]] bool isValid() const noexcept;
    [[nodiscard]] const std::string& label() const noexcept;

private:
    [[nodiscard]] std::string stateLabel() const;

private:
    Config m_config{};
    bool m_value{false};

    ftxui::Component m_checkbox;
    ftxui::Component m_component;
};

}

#endif