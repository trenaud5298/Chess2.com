#ifndef CHESS_CORE_UI_CONDITIONALBUTTON_HPP
#define CHESS_CORE_UI_CONDITIONALBUTTON_HPP

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
#include <functional>

namespace Chess {

class ConditionalButton : public ftxui::ComponentBase {

public:
    struct Config {
        std::string label;
        std::function<void()> onPress;
        std::function<bool()> isEnabled;

        int width{0};
        bool centerLabel{true};
        ftxui::Color disabledColor{ftxui::Color::GrayDark};
    };

public:
    explicit ConditionalButton(Config config);

    ftxui::Element OnRender() override;
    bool OnEvent(ftxui::Event event) override;
    bool Focusable() const override;

private:
    [[nodiscard]] bool enabled() const;
    [[nodiscard]] bool contains(int x, int y) const noexcept;
    [[nodiscard]] ftxui::Element applyCommonStyle(ftxui::Element element) const;

private:
    Config m_config{};
    ftxui::Box m_box{};
};

ftxui::Component MakeConditionalButton(ConditionalButton::Config config);

}

#endif