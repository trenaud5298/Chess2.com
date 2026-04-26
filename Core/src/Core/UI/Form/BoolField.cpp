/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/UI/Form/BoolField.hpp>

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <utility>

namespace Chess {

BoolField::BoolField(Config config)
: m_config(std::move(config)), m_value(m_config.initialValue) {
    m_checkbox = ftxui::Checkbox(&m_config.label, &m_value);
    m_component = m_checkbox;
}

ftxui::Component BoolField::component() const {
    return m_component;
}

ftxui::Element BoolField::renderRow() const {
    return ftxui::vbox({
        ftxui::text(m_config.label) | ftxui::bold,
        ftxui::hbox({
            m_checkbox->Render(),
            ftxui::text(" "),
            ftxui::text("(" + stateLabel() + ")") | ftxui::dim
        }) | ftxui::border,
        ftxui::separatorEmpty(),
    });
}

bool BoolField::value() const noexcept {
    return m_value;
}

void BoolField::setValue(bool value) {
    m_value = value;
}

bool BoolField::isValid() const noexcept {
    return true;
}

const std::string& BoolField::label() const noexcept {
    return m_config.label;
}

std::string BoolField::stateLabel() const {
    return m_value ? m_config.trueLabel : m_config.falseLabel;
}

}
