/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/UI/Form/TextField.hpp>

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <algorithm>
#include <utility>

namespace Chess {

TextField::TextField(Config config)
: m_config(std::move(config)), m_text(m_config.initialValue), m_cursorPosition(m_text.size()) {
    ftxui::InputOption option = ftxui::InputOption::Default();
    option.password = m_config.secret;
    option.multiline = false;
    option.cursor_position = &m_cursorPosition;
    option.on_change = [this]() {
        if (m_config.maxLength > 0 && m_text.size() > m_config.maxLength) {
            m_text.resize(m_config.maxLength);
            m_cursorPosition = std::min(m_cursorPosition, static_cast<int>(m_text.size()));
        }
        validate();
    };

    m_input = ftxui::Input(&m_text, m_config.placeholder, option);
    m_component = m_input;

    validate();
}

ftxui::Component TextField::component() const {
    return m_component;
}

ftxui::Element TextField::renderRow() const {
    ftxui::Element message;
    if (m_error.empty()) {
        message = ftxui::text("");
    } else {
        message = ftxui::text(m_error) | ftxui::color(ftxui::Color::Red);
    }

    return ftxui::vbox({
        ftxui::text(m_config.label) | ftxui::bold,
        m_input->Render() | ftxui::border,
        message
    });
}

const std::string& TextField::text() const noexcept {
    return m_text;
}

void TextField::setText(std::string value) {
    if (m_config.maxLength > 0 && value.size() > m_config.maxLength) {
        value.resize(m_config.maxLength);
    }

    m_text = std::move(value);
    m_cursorPosition = m_text.size();
    validate();
}

bool TextField::isValid() const noexcept {
    return m_error.empty();
}

const std::string& TextField::errorText() const noexcept {
    return m_error;
}

const std::string &TextField::label() const noexcept {
    return m_config.label;
}

void TextField::validate() {
    m_error.clear();

    if (!m_config.allowEmpty && m_text.empty()) {
        m_error = "This field is requried";
        return;
    }

    if (m_config.maxLength > 0 && m_text.size() > m_config.maxLength) {
        m_error = "Value exceed maximum length";
        return;
    }

    if (m_config.validator) {
        if (std::optional<std::string> result = m_config.validator(m_text)) {
            m_error = std::move(*result);
        }
    }
}
}
