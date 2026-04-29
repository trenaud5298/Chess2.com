#ifndef CHESS_CLIENT_UI_MULTIPLAYERGAMESCREEN_HPP
#define CHESS_CLIENT_UI_MULTIPLAYERGAMESCREEN_HPP


/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Screen/ScreenInterface.hpp>
#include <Chess/Client/Runtime/MultiplayerClient.hpp>
#include <Chess/Core/Game/ChessGame.hpp>

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace Chess {

class ClientPanel;
class ChatPane;
class ChessBoardDisplay;

class MultiplayerGameScreen : public ScreenInterface {
public:
    explicit MultiplayerGameScreen(ClientPanel& clientPanel);
    ~MultiplayerGameScreen();

    ftxui::Component getComponent() override;

    void onEnter() override;
    void onLeave() override;
    void onResume() override;
    void onTick() override;

    bool canRequestExit() const override {return true;}
    std::string exitLabel() const override { return "Exit Room";}
    void requestExit() override;


private:
    void refreshSnapshot();
    void syncBoardFromSnapshot();
    void syncProjectedGameFromSnapshot();

    [[nodiscard]] bool hasGameUpdate() const noexcept;
    [[nodiscard]] const GameUpdate& latestUpdate() const;
    [[nodiscard]] const ChessGameSnapshot& latestSnapshot() const;

    [[nodiscard]] bool canSubmitMoves() const;
    [[nodiscard]] bool isLocalPlayersTurn() const;
    [[nodiscard]] COLOR localColor() const noexcept;
    [[nodiscard]] bool localPlayerIsSpectator() const noexcept;
    [[nodiscard]] bool currentSideInCheck() const noexcept;

    [[nodiscard]] ftxui::Element renderClock(std::chrono::milliseconds remaining, bool isActive, const std::string& label) const;
    [[nodiscard]] ftxui::Element renderGameInfo() const;
    [[nodiscard]] ftxui::Element renderChatPane() const;

    [[nodiscard]] ftxui::Component buildComponent();

private:
    MultiplayerView m_view{};
    std::optional<std::uint64_t> m_appliedGameVersion;
    std::optional<ChessGame> m_projectedGame;

    std::shared_ptr<ChessBoardDisplay> m_boardDisplay{};
    std::unique_ptr<ChatPane> m_globalChat;
    std::unique_ptr<ChatPane> m_gameChat;

    int m_chatTabIndex{0};

    const std::vector<std::string> m_chatLabels = {"Global", "Game"};
    ftxui::Component m_chatTabToggle;
    ftxui::Component m_chatTabContainer;
    ftxui::Component m_chatComponent;
    ftxui::Component m_component;

    static constexpr Screen SCREEN_TYPE = Screen::Multiplayer_Game;
};

}
#endif