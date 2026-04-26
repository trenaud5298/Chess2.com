#ifndef CHESS_CLIENT_UI_MODAL_CREATEROOMMODAL_HPP
#define CHESS_CLIENT_UI_MODAL_CREATEROOMMODAL_HPP

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
#include <Chess/Core/UI/Form/IntegerField.hpp>
#include <Chess/Core/UI/Form/BoolField.hpp>
#include <Chess/Core/UI/ConditionalButton.hpp>
#include <Chess/Core/Networking/MessagePayloads.hpp>

// FTXUI Includes
#include <ftxui/component/component.hpp>

// C++ Includes
#include <functional>

namespace Chess {

class ClientPanel;

class CreateRoomModal : public ModalInterface {
public:
    CreateRoomModal(ClientPanel& clientPanel);
    ~CreateRoomModal() override;

    [[nodiscard]] ftxui::Component getComponent() override;

private:
    [[nodiscard]] bool canSubmit() const;
    [[nodiscard]] RoomCreateConfig buildConfig() const;
    void submit();

private:
    TextField m_nameField;
    TextField m_passwordField;

    BoolField m_visibleInLobbyField;
    BoolField m_allowSpectatorsField;
    BoolField m_spectatorsCanChatField;
    BoolField m_allowMidgameJoinField;

    IntegerField m_maxSpectatorsField;

    BoolField m_clockEnabledField;
    IntegerField m_clockMinutesField;
    IntegerField m_incrementSecondsField;

    ftxui::Component m_createButton;
    ftxui::Component m_cancelButton;
    ftxui::Component m_form;
    ftxui::Box m_formBox;
    ftxui::Component m_component;
};

}

#endif
