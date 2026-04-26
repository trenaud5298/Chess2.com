/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Screen/MultiplayerGameScreen.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>
#include <Chess/Client/UI/Component/ChatPane.hpp>
#include <Chess/Client/Runtime/GameClient.hpp>
#include <Chess/Core/UI/ChessBoardDisplay.hpp>
#include <Chess/Core/Common/TimeFormat.hpp>
#include <Chess/Client/UI/Modal/ConfirmModal.hpp>

// FTXUI Includes

// C++ Includes
#include <algorithm>
#include <cstdint>
#include <optional>
#include <string>
#include <utility>

namespace Chess {


namespace {

std::string roomMemberTypeLabel(RoomMemberType memberType) {
    switch (memberType) {
        case RoomMemberType::None: return "None";
        case RoomMemberType::Player: return "Player";
        case RoomMemberType::Spectator: return "Spectator";
    }
    return "None";
}

std::string colorLabel(COLOR color) {
    switch (color) {
        case COLOR::EMPTY: return "Empty";
        case COLOR::WHITE: return "White";
        case COLOR::BLACK: return "Black";
    }
    return "Empty";
}

std::string gameStateLabel(ChessGameState state) {
    switch (state) {
        case ChessGameState::NotStarted: return "Waiting For Players";
        case ChessGameState::InProgress: return "In Progress";
        case ChessGameState::Finished: return "Finished";
    }
    return "Unknown";
}

std::string endReasonLabel(ChessGameEndReason reason) {
    switch (reason) {
        case ChessGameEndReason::None: return "None";
        case ChessGameEndReason::Checkmate: return "Checkmate";
        case ChessGameEndReason::Timeout: return "Timeout";
        case ChessGameEndReason::Resign: return "Resign";
        case ChessGameEndReason::Draw: return "Draw";
    }
    return "Unknown";
}

std::string winnerLabel(COLOR winner) {
    switch (winner) {
        case COLOR::EMPTY: return "None";
        case COLOR::WHITE: return "White";
        case COLOR::BLACK: return "Black";
    }
    return "None";
}


std::uint8_t squareFromPos(const Pos& pos) {
    return static_cast<std::uint8_t>(pos[0]*8 + pos[1]);
}

}




MultiplayerGameScreen::MultiplayerGameScreen(ClientPanel& clientPanel)
: ScreenInterface(clientPanel), m_boardDisplay(std::make_shared<ChessBoardDisplay>()) {
    m_globalChat = std::make_unique<ChatPane>(ChatPaneConfig{
        .logAccessor = [this]() -> const ThreadSafeClientChatLog& {
            return m_clientPanel.gameClient().multiplayerGlobalChat();
        },
        .submitFunction = [this](std::string text) {
            return m_clientPanel.handleStatus(
                m_clientPanel.gameClient().submitMultiplayerGlobalChat(std::move(text)),
                "Unable to send global chat", ResultPolicy::LogOnly
            );
        },
        .emptyText = "No global chat messages",
        .inputPlaceholder = "Send a global chat..."
    });

    m_gameChat = std::make_unique<ChatPane>(ChatPaneConfig{
        .logAccessor = [this]() -> const ThreadSafeClientChatLog& {
            return m_clientPanel.gameClient().multiplayerGameChat();
        },
        .submitFunction = [this](std::string text) {
            return m_clientPanel.handleStatus(
                m_clientPanel.gameClient().submitMultiplayerGameChat(std::move(text)),
                "Unable to send game chat", ResultPolicy::LogOnly
            );
        },
        .emptyText = "No game chat messages",
        .inputPlaceholder = "Send a game chat..."
    });

    m_boardDisplay->onMove = [this](Pos from, Pos to) {
        if (!canSubmitMoves()) {
            return;
        }

        ClientStatus status = m_clientPanel.gameClient().submitMultiplayerMove(
            squareFromPos(from), squareFromPos(to), PromotionPiece::None
        );

        m_clientPanel.handleStatus(
            status,
            "Failed To Make Move", ResultPolicy::Modal
        );
    };

    m_component = buildComponent();
}

MultiplayerGameScreen::~MultiplayerGameScreen() {}

ftxui::Component MultiplayerGameScreen::getComponent() {
    return m_component;
}

void MultiplayerGameScreen::onEnter() {
    m_clientPanel.setTickRate(std::chrono::milliseconds(50));
    m_appliedGameVersion.reset();

    m_boardDisplay->setTheme(m_clientPanel.gameClient().persistenceManager().settings().getBoardTheme());

    refreshSnapshot();
    syncProjectedGameFromSnapshot();
    syncBoardFromSnapshot();


    if (m_globalChat) {
        m_globalChat->onEnter();
    }

    if (m_gameChat) {
        m_gameChat->onEnter();
    }
}

void MultiplayerGameScreen::onLeave() {
    if (m_globalChat) {
        m_globalChat->onLeave();
    }

    if (m_gameChat) {
        m_gameChat->onLeave();
    }
    m_appliedGameVersion.reset();
    m_clientPanel.setTickRate(std::nullopt);
}

void MultiplayerGameScreen::onResume() {
    m_boardDisplay->setTheme(m_clientPanel.gameClient().persistenceManager().settings().getBoardTheme());
}

void MultiplayerGameScreen::onTick() {
    refreshSnapshot();
    syncProjectedGameFromSnapshot();
    syncBoardFromSnapshot();
}

void MultiplayerGameScreen::requestExit() {
    m_clientPanel.pushModal(std::make_unique<ConfirmModal>(m_clientPanel, "Are you sure you would like to quit?",[this]() {
        m_clientPanel.handleStatus(
            m_clientPanel.gameClient().requestMultiplayerLeaveRoom(),
            "Unable To Leave Room", ResultPolicy::Modal
        );
    }));
}

void MultiplayerGameScreen::refreshSnapshot() {
    m_view = m_clientPanel.gameClient().multiplayerView();
}

void MultiplayerGameScreen::syncBoardFromSnapshot() {
    if (!hasGameUpdate()) {
        return;
    }
    const ChessGameSnapshot& snapshot = latestSnapshot();
    if (m_appliedGameVersion.has_value() && *m_appliedGameVersion == snapshot.version) {
        return;
    }
    m_appliedGameVersion = snapshot.version;
    m_boardDisplay->setFlipped(localColor() == COLOR::BLACK);
    m_boardDisplay->updateBoard(latestSnapshot().board);
}

void MultiplayerGameScreen::syncProjectedGameFromSnapshot() {
    if (!hasGameUpdate()) {
        m_projectedGame.reset();
        return;
    }

    const ChessGameSnapshot& snapshot = latestSnapshot();

    if (!m_projectedGame.has_value()) {
        m_projectedGame.emplace(snapshot, std::chrono::steady_clock::now());
        return;
    }

    if (m_projectedGame->version() != snapshot.version) {
        m_projectedGame->applySnapshot(snapshot, std::chrono::steady_clock::now());
    }
}

bool MultiplayerGameScreen::hasGameUpdate() const noexcept {
    return m_view.game.hasSnapshot && m_view.game.latestUpdate.has_value();
}

const GameUpdate& MultiplayerGameScreen::latestUpdate() const {
    return *m_view.game.latestUpdate;
}

const ChessGameSnapshot& MultiplayerGameScreen::latestSnapshot() const {
    return latestUpdate().snapshot;
}

bool MultiplayerGameScreen::canSubmitMoves() const {
    if (!hasGameUpdate()) {
        return false;
    }

    if (!m_view.room.joined) {
        return false;
    }

    if (localPlayerIsSpectator()) {
        return false;
    }

    return latestSnapshot().state == ChessGameState::InProgress && isLocalPlayersTurn();
}

bool MultiplayerGameScreen::isLocalPlayersTurn() const {
    if (!hasGameUpdate()) {
        return false;
    }

    if (m_view.room.memberType != RoomMemberType::Player) {
        return false;
    }

    return latestSnapshot().currentTurn == localColor();
}

COLOR MultiplayerGameScreen::localColor() const noexcept {
    return m_view.room.color;
}

bool MultiplayerGameScreen::localPlayerIsSpectator() const noexcept {
    return m_view.room.memberType == RoomMemberType::Spectator;
}

ftxui::Element MultiplayerGameScreen::renderClock(std::chrono::milliseconds remaining, bool isActive, const std::string &label) const {
    std::string timeStr = formatHHMMSS(remaining);
    bool urgent = (remaining <= std::chrono::seconds{30}) && isActive;

    auto clockText = ftxui::text(timeStr)
        | ftxui::bold
        | ftxui::center;

    if (urgent) {
        clockText = clockText | ftxui::color(ftxui::Color::Red);
    } else if (isActive) {
        clockText = clockText | ftxui::color(ftxui::Color::Green);
    } else {
        clockText = clockText | ftxui::dim;
    }

    return ftxui::vbox({
        ftxui::text(label) | ftxui::center | ftxui::dim,
        clockText,
    }) | ftxui::border;
}

ftxui::Element MultiplayerGameScreen::renderGameInfo() const {
    if (!hasGameUpdate()) {
        return ftxui::vbox({
            ftxui::text("Game Info") | ftxui::bold | ftxui::center,
            ftxui::separator(),
            ftxui::text("Waiting for game snapshot...") | ftxui::dim | ftxui::center,
            ftxui::separator(),
            ftxui::text("Room ID: " + std::to_string(m_view.room.roomID)) | ftxui::center,
            ftxui::text("Role: " + roomMemberTypeLabel(m_view.room.memberType)) | ftxui::center,
            ftxui::text("Side: " + colorLabel(m_view.room.color)) | ftxui::center
        });
    }

    const GameUpdate& update = latestUpdate();
    const ChessGameSnapshot& snapshot = latestSnapshot();

    bool whiteActive = snapshot.currentTurn == COLOR::WHITE;
    bool blackActive = snapshot.currentTurn == COLOR::BLACK;

    std::string whiteName = update.whitePlayerName.empty() ? "White" : update.whitePlayerName;
    std::string blackName = update.blackPlayerName.empty() ? "Black" : update.blackPlayerName;

    std::string turnText = "No Active Turn";
    if (snapshot.currentTurn == COLOR::WHITE) {
        turnText = "White's Turn";
    } else if (snapshot.currentTurn == COLOR::BLACK) {
        turnText = "Black's Turn";
    }

    std::string resultText = "Result: In Progress";
    if (snapshot.state == ChessGameState::Finished) {
        resultText = "Result: " + winnerLabel(snapshot.winner) + " (" + endReasonLabel(snapshot.endReason) + ")";
    }

    std::string localStatusText = roomMemberTypeLabel(m_view.room.memberType) + " / " + colorLabel(m_view.room.color);

    auto now = std::chrono::steady_clock::now();

    std::chrono::milliseconds whiteRemaining = snapshot.whiteTimeRemaining;
    std::chrono::milliseconds blackRemaining = snapshot.blackTimeRemaining;

    if (m_projectedGame.has_value()) {
        whiteRemaining = m_projectedGame->whiteTimeRemaining(now);
        blackRemaining = m_projectedGame->blackTimeRemaining(now);
    }

    ftxui::Element whiteClock = renderClock(whiteRemaining, whiteActive, whiteName);
    ftxui::Element blackClock = renderClock(blackRemaining, blackActive, blackName);

    bool flipped = localColor() == COLOR::BLACK;

    return ftxui::vbox({
        (flipped ? whiteClock : blackClock),
        ftxui::separator(),
        ftxui::text("Room " + std::to_string(update.roomID)) | ftxui::bold | ftxui::center,
        ftxui::text("You: " + localStatusText) | ftxui::center,
        ftxui::text("State: " + gameStateLabel(snapshot.state)) | ftxui::center,
        ftxui::text(turnText) | ftxui::bold | ftxui::center,
        ftxui::text(resultText) | ftxui::center,
        ftxui::text("Spectators: " + std::to_string(update.spectatorCount)) | ftxui::center,
        ftxui::text("Room Version: " + std::to_string(update.roomVersion)) | ftxui::dim | ftxui::center,
        ftxui::text("Game Version: " + std::to_string(snapshot.version)) | ftxui::dim | ftxui::center,
        ftxui::separator(),
        (flipped ? blackClock : whiteClock),
    }) | ftxui::flex | ftxui::center;
}

ftxui::Element MultiplayerGameScreen::renderChatPane() const {
    return ftxui::vbox({
        ftxui::text("Chat") | ftxui::bold | ftxui::center,
        ftxui::separator(),
        m_chatTabToggle->Render(),
        ftxui::separator(),
        m_chatTabContainer->Render() | ftxui::flex | ftxui::yflex
    }) | ftxui::flex;
}


ftxui::Component MultiplayerGameScreen::buildComponent() {
    m_chatTabToggle = ftxui::Toggle(m_chatLabels, &m_chatTabIndex);

    m_chatTabContainer = ftxui::Container::Tab({
        m_globalChat->component(),
        m_gameChat->component()
    }, &m_chatTabIndex);

    m_chatComponent = ftxui::Container::Vertical({
        m_chatTabToggle,
        m_chatTabContainer
    });

    auto interactiveContainer = ftxui::Container::Horizontal({
        m_chatComponent,
        m_boardDisplay
    });

    return ftxui::Renderer(interactiveContainer, [this]() {
        ftxui::Element chatPanel = renderChatPane() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 30) | ftxui::yflex;

        ftxui::Element boardPanel;
        if (!hasGameUpdate()) {
            boardPanel = ftxui::vbox({
                ftxui::filler(),
                ftxui::text("Waiting for game state...") | ftxui::center | ftxui::dim,
                ftxui::filler()
            }) | ftxui::flex | ftxui::yflex;
        } else {
            boardPanel = m_boardDisplay->Render() | ftxui::flex | ftxui::yflex;
        }

        ftxui::Element infoPanel = renderGameInfo() | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 24) | ftxui::yflex;


        return ftxui::hbox({
            chatPanel,
            ftxui::separator(),
            boardPanel,
            ftxui::separator(),
            infoPanel
        }) | ftxui::flex;
    });
}
} // namespace Chess