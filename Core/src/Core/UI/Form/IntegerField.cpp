/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/UI/Form/IntegerField.hpp>

// FTXUI Includes
#include <ftxui/component/event.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <algorithm>
#include <utility>
#include <charconv>



namespace Chess {

IntegerField::IntegerField(Config config)
: m_config(std::move(config)), m_value(m_config.initialValue) {
    syncTextFromValue();
    m_cursorPosition = m_text.size();

    ftxui::InputOption option = ftxui::InputOption::Default();
    option.multiline = false;
    option.cursor_position = &m_cursorPosition;
    option.on_change = [this]() {
        validate();
    };

    m_input = ftxui::Input(&m_text, "", option);
    m_input |= ftxui::CatchEvent([this](ftxui::Event event) {
        if (!event.is_character()) {
            return false;
        }

        std::string character = event.character();
        if (character.empty()) {
            return false;
        }

        char c = character[0];
        if (c >= '0' && c <= '9') {
            return false;
        }

        if (c == '-' && m_config.minValue < 0 && m_cursorPosition == 0 && m_text.find('-') == std::string::npos) {
            return false;
        }

        return true;
    });

    m_component = m_input;
    validate();
}

ftxui::Component IntegerField::component() const {
    return m_component;
}

ftxui::Element IntegerField::renderRow() const {
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

int IntegerField::value() const noexcept {
    return m_value;
}

void IntegerField::setValue(int value) {
    m_value = std::clamp(value, m_config.minValue, m_config.maxValue);
    syncTextFromValue();
    m_cursorPosition = m_text.size();
    validate();
}

bool IntegerField::isValid() const noexcept {
    return m_error.empty();
}

const std::string& IntegerField::errorText() const noexcept {
    return m_error;
}

const std::string &IntegerField::label() const noexcept {
    return m_config.label;
}

void IntegerField::validate() {
    m_error.clear();

    if (!m_config.allowEmpty && m_text.empty()) {
        m_error = "This field is required";
        return;
    }

    int parsedValue = 0;
    auto result = std::from_chars(m_text.data(), m_text.data() + m_text.size(), parsedValue);

    if (result.ec != std::errc{} || result.ptr != (m_text.data()+m_text.size())) {
        m_error = "Enter a valid integer";
        return;
    }

    if (parsedValue < m_config.minValue || parsedValue > m_config.maxValue) {
        m_error = "Value must be between " + std::to_string(m_config.minValue) + " and " + std::to_string(m_config.maxValue);
        return;
    }

    m_value = parsedValue;
}

void IntegerField::syncTextFromValue() {
    m_text = std::to_string(m_value);
}


}
