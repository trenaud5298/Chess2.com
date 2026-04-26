/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/UI/ConditionalButton.hpp>

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <utility>

namespace Chess {

ConditionalButton::ConditionalButton(Config config)
: m_config(std::move(config)) {}

ftxui::Element ConditionalButton::OnRender() {
    ftxui::Element element = ftxui::text(m_config.label);

    element = applyCommonStyle(element);
    element |= ftxui::border;

    if (!enabled()) {
        element |= ftxui::color(m_config.disabledColor);
        element |= ftxui::dim;
    } else if (Focused()) {
        element |= ftxui::inverted;
        element |= ftxui::bold;
    } else {
        element |= ftxui::bold;
    }

    return element | ftxui::reflect(m_box);
}


bool ConditionalButton::OnEvent(ftxui::Event event) {
    if (event == ftxui::Event::Return || event == ftxui::Event::Character(' ')) {
        if (!enabled()) {
            return true;
        }

        if (m_config.onPress) {
            m_config.onPress();
        }
        return true;
    }

    if (!event.is_mouse()) {
        return false;
    }

    auto& mouse = event.mouse();
    bool inside = contains(mouse.x, mouse.y);

    if (!inside) {
        return false;
    }

    TakeFocus();

    if (!enabled()) {
        return true;
    }

    if (mouse.button == ftxui::Mouse::Left && mouse.motion == ftxui::Mouse::Pressed) {
        if (m_config.onPress) {
            m_config.onPress();
        }
        return true;
    }

    return false;
}

bool ConditionalButton::Focusable() const {
    return enabled();
}


bool ConditionalButton::enabled() const {
    if (!m_config.isEnabled) {
        return true;
    }
    return m_config.isEnabled();
}

bool ConditionalButton::contains(int x, int y) const noexcept {
    return x >= m_box.x_min && x <= m_box.x_max && y >= m_box.y_min && y <= m_box.y_max;
}


ftxui::Element ConditionalButton::applyCommonStyle(ftxui::Element element) const {
    if (m_config.centerLabel) {
        element |= ftxui::center;
    }

    if (m_config.width > 0) {
        element |= ftxui::size(ftxui::WIDTH, ftxui::EQUAL, m_config.width);
    }

    return element;
}

ftxui::Component MakeConditionalButton(ConditionalButton::Config config) {
    return ftxui::Make<ConditionalButton>(std::move(config));
}
}
