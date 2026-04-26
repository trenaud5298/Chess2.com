/*
* Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/UI/Form/RGBField.hpp>

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <utility>

namespace Chess {

namespace {

IntegerField::Config makeChannelConfig(std::string label, int initialValue) {
    return IntegerField::Config{
        .label = std::move(label),
        .initialValue = initialValue,
        .minValue = 0,
        .maxValue = 255,
        .allowEmpty = false
    };
}

}


RGBField::RGBField(Config config)
: m_config(std::move(config)),
m_redField(makeChannelConfig("R", m_config.initialValue.red)),
m_greenField(makeChannelConfig("G", m_config.initialValue.green)),
m_blueField(makeChannelConfig("B", m_config.initialValue.blue)){
    m_container = ftxui::Container::Horizontal({
        m_redField.component(),
        m_greenField.component(),
        m_blueField.component(),
    });
}

ftxui::Component RGBField::component() const {
    return m_container;
}

ftxui::Element RGBField::renderRow() const {
    RGBColor color = value();

    ftxui::Element preview = ftxui::text("");
    if (m_config.showPreview) {
        preview = ftxui::text("    ") | ftxui::bgcolor(color.toFTXUIColor()) | ftxui::border;
    }

    auto channelBlock = [](const IntegerField& field, ftxui::Color labelColor) {
        return ftxui::vbox({
            ftxui::text(field.label()) | ftxui::bold | ftxui::color(labelColor) | ftxui::center,
            field.component()->Render() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 7) | ftxui::border,
        });
    };

    ftxui::Element message;
    if (isValid()) {
        message = ftxui::separatorEmpty();
    } else {
        message = ftxui::text("Each channel must be between 0 and 255") | ftxui::color(ftxui::Color::Red);
    }

    return ftxui::vbox({
        ftxui::text(m_config.label) | ftxui::bold,
        ftxui::hbox({
            channelBlock(m_redField, ftxui::Color::Red),
            ftxui::text(" "),
            channelBlock(m_greenField, ftxui::Color::Green),
            ftxui::text(" "),
            channelBlock(m_blueField, ftxui::Color::Blue),
            m_config.showPreview ? ftxui::text("  ") : ftxui::text(""),
            preview,
        }),
        message
    });
}

RGBColor RGBField::value() const {
    return RGBColor{
        .red = static_cast<std::uint8_t>(m_redField.value()),
        .green = static_cast<std::uint8_t>(m_greenField.value()),
        .blue = static_cast<std::uint8_t>(m_blueField.value()),
    };
}

void RGBField::setValue(RGBColor value) {
    m_redField.setValue(value.red);
    m_greenField.setValue(value.green);
    m_blueField.setValue(value.blue);
}

bool RGBField::isValid() const noexcept {
    return m_redField.isValid() && m_greenField.isValid() && m_blueField.isValid();
}

const std::string& RGBField::label() const noexcept {
    return m_config.label;
}

}
