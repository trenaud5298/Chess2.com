#ifndef CHESS_CLIENT_UI_MODAL_SETTINGSMODAL_HPP
#define CHESS_CLIENT_UI_MODAL_SETTINGSMODAL_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Modal/ModalInterface.hpp>
#include <Chess/Core/UI/Form/TextField.hpp>
#include <Chess/Core/UI/Form/RGBField.hpp>
#include <Chess/Core/UI/BoardTheme.hpp>

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>


namespace Chess {

class ClientPanel;

class SettingsModal : public ModalInterface {
public:
    explicit SettingsModal(ClientPanel& panel);
    virtual ~SettingsModal();

    void onEnter() override;
    void onLeave() override;

    [[nodiscard]] ftxui::Component getComponent() override;

private:
    [[nodiscard]] bool canApply() const;
    [[nodiscard]] BoardTheme buildBoardTheme() const;
    void apply();
    void saveAndClose();

private:
    TextField m_usernameField;

    RGBField m_lightSquareField;
    RGBField m_darkSquareField;
    RGBField m_whitePieceField;
    RGBField m_blackPieceField;
    RGBField m_cursorSquareField;
    RGBField m_selectedSquareField;

    ftxui::Component m_applyButton;
    ftxui::Component m_saveButton;
    ftxui::Component m_cancelButton;

    ftxui::Component m_form;
    ftxui::Box m_formBox{};
    ftxui::Component m_component;
};

}


#endif