#ifndef CHESS_CLIENT_UI_MODAL_JOINROOMMODAL_HPP
#define CHESS_CLIENT_UI_MODAL_JOINROOMMODAL_HPP

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
#include <Chess/Core/UI/ConditionalButton.hpp>
#include <Chess/Core/Networking/MessagePayloads.hpp>

// FTXUI Includes
#include <ftxui/component/component.hpp>


namespace Chess {

class ClientPanel;

class JoinRoomModal : public ModalInterface {
public:
    JoinRoomModal(ClientPanel& clientPanel, RoomSummary room, bool joinAsSpectator);
    ~JoinRoomModal() override;

    [[nodiscard]] ftxui::Component getComponent() override;

private:
    [[nodiscard]] bool canSubmit() const;
    void submit();

private:
    RoomSummary m_room;
    bool m_joinAsSpectator{false};

    TextField m_passwordField;

    ftxui::Component m_joinButton;
    ftxui::Component m_cancelButton;
    ftxui::Component m_form;
    ftxui::Component m_component;
};

}

#endif
