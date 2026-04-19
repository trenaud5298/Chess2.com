#ifndef CHESS_CLIENT_UI_COMPONENT_CHATPANE_HPP
#define CHESS_CLIENT_UI_COMPONENT_CHATPANE_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/Common/ClientChat.hpp>

// FTXUI Includes
#include <ftxui/component/component.hpp>

// C++ Includes
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace Chess {

class ScrollableListView;

struct ChatPaneConfig {
    std::function<const ThreadSafeClientChatLog&()> logAccessor;
    std::function<bool(std::string)> submitFunction;
    std::string emptyText;
    std::string inputPlaceholder;
};

class ChatPane {

public:
    explicit ChatPane(ChatPaneConfig config);
    ~ChatPane();

    [[nodiscard]] ftxui::Component component();

    void onEnter();
    void onLeave();

private:
    [[nodiscard]] const ThreadSafeClientChatLog& log() const;
    void submitDraft();
    void refreshVisibleEntries();
    [[nodiscard]] ftxui::Element renderEntry(const ClientChatEntry& entry) const;
    ftxui::Component buildComponent();

private:
    ChatPaneConfig m_config;
    std::string m_draftText;

    std::vector<ClientChatEntry> m_visibleEntries;
    std::shared_ptr<ScrollableListView> m_listView;
    ftxui::Component m_inputComponent;
    ftxui::Component m_sendButtonComponent;
    ftxui::Component m_component;
};

}


#endif