/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Modal/SettingsModal.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>
#include <Chess/Client/Runtime/Persistence/Settings.hpp>
#include <Chess/Core/UI/ConditionalButton.hpp>

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <utility>

namespace Chess {

namespace {

TextField::Config makeUsernameConfig(const std::string& username) {
    return TextField::Config{
        .label = "Username",
        .initialValue = username,
        .placeholder = "Enter username",
        .maxLength = 64,
        .allowEmpty = false,
        .secret = false,
    };
}

RGBField::Config makeRGBConfig(std::string label, RGBColor initialValue) {
    return RGBField::Config{
        .label = std::move(label),
        .initialValue = initialValue,
        .showPreview = true,
    };
}

}

SettingsModal::SettingsModal(ClientPanel& panel)
: ModalInterface(panel),
m_usernameField(makeUsernameConfig(m_clientPanel.gameClient().persistenceManager().settings().getUsername())),
m_lightSquareField(makeRGBConfig("Light Square",m_clientPanel.gameClient().persistenceManager().settings().getBoardTheme().lightSquare)),
m_darkSquareField(makeRGBConfig("Dark Square",m_clientPanel.gameClient().persistenceManager().settings().getBoardTheme().darkSquare)),
m_whitePieceField(makeRGBConfig("White Piece",m_clientPanel.gameClient().persistenceManager().settings().getBoardTheme().whitePiece)),
m_blackPieceField(makeRGBConfig("Black Piece",m_clientPanel.gameClient().persistenceManager().settings().getBoardTheme().blackPiece)),
m_cursorSquareField(makeRGBConfig("Cursor Square",m_clientPanel.gameClient().persistenceManager().settings().getBoardTheme().cursorSquare)),
m_selectedSquareField(makeRGBConfig("Selected Square",m_clientPanel.gameClient().persistenceManager().settings().getBoardTheme().selectedSquare)),
m_applyButton(MakeConditionalButton({
  .label = "Apply",
  .onPress = std::bind_front(&SettingsModal::apply, this),
  .isEnabled = std::bind_front(&SettingsModal::canApply, this),
  .width = 18,
  .centerLabel = true,
})),
m_saveButton(MakeConditionalButton({
  .label = "Save",
  .onPress = std::bind_front(&SettingsModal::saveAndClose, this),
  .isEnabled = std::bind_front(&SettingsModal::canApply, this),
  .width = 18,
  .centerLabel = true,
})),
m_cancelButton(MakeConditionalButton({
  .label = "Cancel",
  .onPress = std::bind_front(&SettingsModal::requestDismiss, this),
  .width = 18,
  .centerLabel = true,
})) {

    auto buttonRow = ftxui::Container::Horizontal({
        m_applyButton,
        m_saveButton,
        m_cancelButton,
    });

    m_form = ftxui::Container::Vertical({
        m_usernameField.component(),
        m_lightSquareField.component(),
        m_darkSquareField.component(),
        m_whitePieceField.component(),
        m_blackPieceField.component(),
        m_cursorSquareField.component(),
        m_selectedSquareField.component()
    });

    auto scrollableForm = ftxui::CatchEvent(m_form, [this](ftxui::Event event) {
        if (!event.is_mouse()) {
            return false;
        }

        int x = event.mouse().x;
        int y = event.mouse().y;
        if (x < m_formBox.x_min || x > m_formBox.x_max ||
            y < m_formBox.y_min || y > m_formBox.y_max) {
            return false;
        }

        if (event.mouse().button == ftxui::Mouse::WheelUp) {
            return m_form->OnEvent(ftxui::Event::ArrowUp);
        }

        if (event.mouse().button == ftxui::Mouse::WheelDown) {
            return m_form->OnEvent(ftxui::Event::ArrowDown);
        }

        return false;
    });

    auto layout = ftxui::Container::Vertical({
        scrollableForm,
        buttonRow,
    });

    m_component = ftxui::Renderer(layout, [this]() {
        return ftxui::vbox({
            ftxui::text("Settings") | ftxui::bold | ftxui::center,
            ftxui::separator(),
            ftxui::vbox({
                ftxui::text("General") | ftxui::bold,
                m_usernameField.renderRow(),
                ftxui::separator(),
                ftxui::text("Board Theme") | ftxui::bold,
                m_lightSquareField.renderRow(),
                m_darkSquareField.renderRow(),
                m_whitePieceField.renderRow(),
                m_blackPieceField.renderRow(),
                m_cursorSquareField.renderRow(),
                m_selectedSquareField.renderRow(),
            }) | ftxui::frame | ftxui::vscroll_indicator | ftxui::reflect(m_formBox) | ftxui::flex | ftxui::yflex,
            ftxui::separator(),
            ftxui::hbox({
                m_applyButton->Render(),
                ftxui::text("  "),
                m_saveButton->Render(),
                ftxui::text("  "),
                m_cancelButton->Render(),
            }) | ftxui::center
        }) | ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 64) | ftxui::size(ftxui::HEIGHT, ftxui::GREATER_THAN, 24);
    });
}


SettingsModal::~SettingsModal() {}

ftxui::Component SettingsModal::getComponent() {
    return m_component;
}

void SettingsModal::onEnter() {}

void SettingsModal::onLeave() {}

bool SettingsModal::canApply() const {
    return m_usernameField.isValid() &&
           m_lightSquareField.isValid() &&
           m_darkSquareField.isValid() &&
           m_whitePieceField.isValid() &&
           m_blackPieceField.isValid() &&
           m_cursorSquareField.isValid() &&
           m_selectedSquareField.isValid();
}

BoardTheme SettingsModal::buildBoardTheme() const {
    return BoardTheme{
        .lightSquare = m_lightSquareField.value(),
        .darkSquare = m_darkSquareField.value(),
        .whitePiece = m_whitePieceField.value(),
        .blackPiece = m_blackPieceField.value(),
        .cursorSquare = m_cursorSquareField.value(),
        .selectedSquare = m_selectedSquareField.value()
    };
}

void SettingsModal::apply() {
    if (!canApply()) {
        return;
    }

    Settings& settings = m_clientPanel.gameClient().persistenceManager().settings();
    Settings::BatchUpdate batch(settings);

    settings.setUsername(m_usernameField.text());
    settings.setBoardTheme(buildBoardTheme());
}

void SettingsModal::saveAndClose() {
    if (!canApply()) {
        return;
    }

    apply();
    requestDismiss();
}
} // namespace Chess