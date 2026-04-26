/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Modal/CreateRoomModal.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>

// ASIO Includes

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <chrono>
#include <utility>


namespace Chess {

namespace {

TextField::Config makeRoomNameConfig() {
    return TextField::Config{
        .label = "Room Name",.initialValue = "",
        .placeholder = "Untitled Room",
        .maxLength = 64,
        .allowEmpty = true,
        .secret = false,
    };
}

TextField::Config makePasswordConfig() {
    return TextField::Config{
        .label = "Password",
        .initialValue = "",
        .placeholder = "Leave empty for public room",
        .maxLength = 64,
        .allowEmpty = true,
        .secret = true,
    };
}

BoolField::Config makeVisibleInLobbyConfig() {
    return BoolField::Config{
        .label = "Visible In Lobby",
        .initialValue = true,
        .trueLabel = "Yes",
        .falseLabel = "No",
    };
}

BoolField::Config makeAllowSpectatorsConfig() {
    return BoolField::Config{
        .label = "Allow Spectators",
        .initialValue = true,
        .trueLabel = "Yes",
        .falseLabel = "No",
    };
}

BoolField::Config makeSpectatorsCanChatConfig() {
    return BoolField::Config{
        .label = "Spectators Can Chat",
        .initialValue = true,
        .trueLabel = "Yes",
        .falseLabel = "No",
    };
}

BoolField::Config makeAllowMidgameJoinConfig() {
    return BoolField::Config{
        .label = "Allow Midgame Join",
        .initialValue = true,
        .trueLabel = "Yes",
        .falseLabel = "No",
    };
}

IntegerField::Config makeMaxSpectatorsConfig() {
    return IntegerField::Config{
        .label = "Max Spectators (0 = unlimited)",
        .initialValue = 0,
        .minValue = 0,
        .maxValue = 999,
        .allowEmpty = false,
    };
}

BoolField::Config makeClockEnabledConfig() {
    return BoolField::Config{
        .label = "Clock Enabled",
        .initialValue = true,
        .trueLabel = "Yes",
        .falseLabel = "No",
    };
}

IntegerField::Config makeClockMinutesConfig() {
    return IntegerField::Config{
        .label = "Initial Minutes",
        .initialValue = 10,
        .minValue = 1,
        .maxValue = 180,
        .allowEmpty = false,
    };
}

IntegerField::Config makeIncrementSecondsConfig() {
    return IntegerField::Config{
        .label = "Increment Seconds",
        .initialValue = 3,
        .minValue = 0,
        .maxValue = 300,
        .allowEmpty = false,
    };
}

}

CreateRoomModal::CreateRoomModal(ClientPanel& clientPanel)
: ModalInterface(clientPanel), m_nameField(makeRoomNameConfig()),
m_passwordField(makePasswordConfig()), m_visibleInLobbyField(makeVisibleInLobbyConfig()),
m_allowSpectatorsField(makeAllowSpectatorsConfig()), m_spectatorsCanChatField(makeSpectatorsCanChatConfig()),
m_allowMidgameJoinField(makeAllowMidgameJoinConfig()), m_maxSpectatorsField(makeMaxSpectatorsConfig()),
m_clockEnabledField(makeClockEnabledConfig()), m_clockMinutesField(makeClockMinutesConfig()),
m_incrementSecondsField(makeIncrementSecondsConfig()),
m_createButton(MakeConditionalButton({
    .label = "Create Room",
    .onPress = std::bind_front(&CreateRoomModal::submit, this),
    .isEnabled = std::bind_front(&CreateRoomModal::canSubmit, this),
    .width = 18,
    .centerLabel = true
})),
m_cancelButton(MakeConditionalButton({
    .label = "Cancel",
    .onPress = std::bind_front(&CreateRoomModal::requestDismiss, this),
    .width = 18,
    .centerLabel = true
})) {

    auto buttonRow = ftxui::Container::Horizontal({
        m_createButton,
        m_cancelButton,
    });

    m_form = ftxui::Container::Vertical({
        m_nameField.component(),
        m_passwordField.component(),
        m_visibleInLobbyField.component(),
        m_allowSpectatorsField.component(),
        m_spectatorsCanChatField.component(),
        m_allowMidgameJoinField.component(),
        m_maxSpectatorsField.component(),
        m_clockEnabledField.component(),
        m_clockMinutesField.component(),
        m_incrementSecondsField.component(),
    });

    auto scrollableForm = ftxui::CatchEvent(m_form, [this](ftxui::Event event) {
        if (!event.is_mouse()) {
            return false;
        }

        int x = event.mouse().x;
        int y = event.mouse().y;
        if (x < m_formBox.x_min || x > m_formBox.x_max || y < m_formBox.y_min || y > m_formBox.y_max) {
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
            ftxui::text("Create Room") | ftxui::bold | ftxui::center,
            ftxui::separator(),
            ftxui::vbox({
                m_nameField.renderRow(),
                m_passwordField.renderRow(),
                m_visibleInLobbyField.renderRow(),
                m_allowSpectatorsField.renderRow(),
                m_spectatorsCanChatField.renderRow(),
                m_allowMidgameJoinField.renderRow(),
                m_maxSpectatorsField.renderRow(),
                m_clockEnabledField.renderRow(),
                m_clockMinutesField.renderRow(),
                m_incrementSecondsField.renderRow(),
            }) | ftxui::flex | ftxui::yflex | ftxui::frame | ftxui::reflect(m_formBox),
            ftxui::hbox({
                m_createButton->Render(),
                ftxui::text("  "),
                m_cancelButton->Render()
            })
        }) | ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 56);
    });
}

CreateRoomModal::~CreateRoomModal() {

}

ftxui::Component CreateRoomModal::getComponent() {
    return m_component;
}


bool CreateRoomModal::canSubmit() const {
    if (!m_nameField.isValid() || !m_passwordField.isValid() ||
        !m_maxSpectatorsField.isValid() || !m_clockMinutesField.isValid() ||
        !m_incrementSecondsField.isValid()) {
        return false;
    }

    if (m_clockEnabledField.value() && m_clockMinutesField.value() <= 0) {
        return false;
    }

    return true;
}

RoomCreateConfig CreateRoomModal::buildConfig() const {
    return RoomCreateConfig{
        .spectator = RoomSpectatorConfig{
            .allowSpectators = m_allowSpectatorsField.value(),
            .spectatorsCanChat = m_spectatorsCanChatField.value(),
            .maxSpectators = static_cast<std::uint16_t>(m_maxSpectatorsField.value()),
            .allowMidgameJoin = m_allowMidgameJoinField.value(),
        },
        .game = RoomGameConfig{
            .clock = ChessClockConfig{
                .enabled = m_clockEnabledField.value(),
                .initialTime = std::chrono::minutes(m_clockMinutesField.value()),
                .increment = std::chrono::seconds(m_incrementSecondsField.value()),
            }
        },
        .presentation = RoomPresentationConfig{
            .name = m_nameField.text(),
        },
        .access = RoomAccessConfig{
            .password = m_passwordField.text(),
            .visibleInLobby = m_visibleInLobbyField.value(),
        }
    };
}

void CreateRoomModal::submit() {
    if (!canSubmit()) {
        return;
    }

    ClientStatus status = m_clientPanel.gameClient().requestMultiplayerCreateRoom(buildConfig());
    m_clientPanel.handleStatus(status,
        "Unable To Create Room", ResultPolicy::Modal
    );

    if (status.isSuccess()) {
        requestDismiss();
    }
}
}
