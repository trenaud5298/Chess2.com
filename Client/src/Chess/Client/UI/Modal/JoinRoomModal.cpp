/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Modal/JoinRoomModal.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>

// ASIO Includes

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <chrono>

namespace Chess {

namespace {

TextField::Config makePasswordConfig() {
    return TextField::Config{
        .label = "Password",
        .initialValue = "",
        .placeholder = "Enter room password",
        .maxLength = 64,
        .allowEmpty = false,
        .secret = true,
    };
}

std::string joinTitle(bool joinAsSpectator) {
    return joinAsSpectator ? "Join As Spectator" : "Join As Player";
}

std::string joinButtonLabel(bool joinAsSpectator) {
    return joinAsSpectator ? "Join Spectator" : "Join Player";
}

}

JoinRoomModal::JoinRoomModal(ClientPanel& clientPanel, RoomSummary room, bool joinAsSpectator)
: ModalInterface(clientPanel), m_room(std::move(room)), m_joinAsSpectator(joinAsSpectator),
m_passwordField(makePasswordConfig()),
m_joinButton(MakeConditionalButton({
    .label = joinButtonLabel(m_joinAsSpectator),
    .onPress = std::bind_front(&JoinRoomModal::submit, this),
    .isEnabled = std::bind_front(&JoinRoomModal::canSubmit, this),
    .width = 18,
    .centerLabel = true
})),
m_cancelButton(MakeConditionalButton({
    .label = "Cancel",
    .onPress = std::bind_front(&JoinRoomModal::requestDismiss, this),
    .width = 18,
    .centerLabel = true
})) {
    auto buttonRow = ftxui::Container::Horizontal({
        m_joinButton,
        m_cancelButton
    });

    m_form = ftxui::Container::Vertical({
        m_passwordField.component(),
        buttonRow,
    });

    m_component = ftxui::Renderer(m_form, [this]() {
        std::string roomName = m_room.config.presentation.name.empty() ? "Untitled Room" : m_room.config.presentation.name;

        return ftxui::vbox({
            ftxui::text(joinTitle(m_joinAsSpectator)) | ftxui::bold | ftxui::center,
            ftxui::separator(),
            ftxui::text("Room ID: " + std::to_string(m_room.roomID)),
            ftxui::text("Name: " + roomName),
            ftxui::text("Access: Private"),
            ftxui::separator(),
            m_passwordField.renderRow(),
            ftxui::separator(),
            ftxui::hbox({
                m_joinButton->Render(),
                ftxui::text("  "),
                m_cancelButton->Render()
            })
        }) | ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 44);
    });
}

JoinRoomModal::~JoinRoomModal() {

}

ftxui::Component JoinRoomModal::getComponent() {
    return m_component;
}

bool JoinRoomModal::canSubmit() const {
    return m_passwordField.isValid();
}

void JoinRoomModal::submit() {
    if (!canSubmit()) {
        return;
    }

    ClientStatus status;
    if (m_joinAsSpectator) {
        status = m_clientPanel.gameClient().requestMultiplayerJoinRoomAsSpectator(m_room.roomID, m_passwordField.text());
    } else {
        status = m_clientPanel.gameClient().requestMultiplayerJoinRoomAsPlayer(m_room.roomID, m_passwordField.text());
    }

    m_clientPanel.handleStatus(std::move(status),
        m_joinAsSpectator ? "Unable To Join Room As Spectator" : "Unable To Join Room As Player",
        ResultPolicy::Modal
    );

    if (status.isSuccess()) {
        requestDismiss();
    }
}
}
