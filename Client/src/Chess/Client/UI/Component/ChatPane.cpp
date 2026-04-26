/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */


// Chess Includes
#include <Chess/Client/UI/Component/ChatPane.hpp>
#include <Chess/Core/UI/ScrollableListView.hpp>

// FTXUI Includes
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <algorithm>
#include <stdexcept>
#include <utility>



namespace Chess {


ChatPane::ChatPane(ChatPaneConfig config) : m_config(std::move(config)) {
    if (!m_config.logAccessor) {
        throw std::invalid_argument("ChatPane requires a valid log accessor");
    }

    if (!m_config.submitFunction) {
        throw std::invalid_argument("ChatPane requires a valid submitFunction");
    }


    m_listView = std::make_shared<ScrollableListView>(
        [this]() {
            return static_cast<int>(m_visibleEntries.size());
        },
        [this](int index) {
            if (index < 0 || index >= m_visibleEntries.size()) {
                return ftxui::text("");
            }
            return renderEntry(m_visibleEntries[index]);
        }
    );

    ftxui::InputOption inputOption;
    inputOption.content = &m_draftText;
    inputOption.placeholder = m_config.inputPlaceholder;
    inputOption.on_enter = [this]() {submitDraft();};
    m_inputComponent = ftxui::Input(std::move(inputOption));

    ftxui::ButtonOption sendButtonOption = ftxui::ButtonOption::Ascii();
    sendButtonOption.transform = [this](const ftxui::EntryState& state) {
        ftxui::Element label = ftxui::text(" Send ");

        if (m_draftText.empty()) {
            label |= ftxui::dim;
        } else if (state.focused) {
            label |= ftxui::bold;
            label |= ftxui::inverted;
        }

        return label;
    };

    m_sendButtonComponent = ftxui::Button("", [this]() {
        submitDraft();
    }, sendButtonOption);

    m_component = buildComponent();
}

ChatPane::~ChatPane() {

}

[[nodiscard]] ftxui::Component ChatPane::component() {
    return m_component;
}

void ChatPane::onEnter() {
    refreshVisibleEntries();
}

void ChatPane::onLeave() {

}

const ThreadSafeClientChatLog& ChatPane::log() const {
    return m_config.logAccessor();
}

void ChatPane::submitDraft() {
    if (m_draftText.empty()) {
        return;
    }

    if (!m_config.submitFunction(m_draftText)) {
        return;
    }

    m_draftText.clear();
}

void ChatPane::refreshVisibleEntries() {
    const ThreadSafeClientChatLog& logRef = log();
    if (m_visibleEntries.empty()) {
        m_visibleEntries = logRef.tail(96);
        return;
    }

    if (m_visibleEntries.back().sequence == logRef.lastSequence()) {
        return;
    }

    m_visibleEntries = logRef.tail(96);
}


ftxui::Element ChatPane::renderEntry(const ClientChatEntry& entry) const {
    return ftxui::paragraph(entry.text);
}

ftxui::Component ChatPane::buildComponent() {
    auto inputRow = ftxui::Container::Horizontal({
        m_inputComponent,
        m_sendButtonComponent,
    });

    auto container = ftxui::Container::Vertical({
        m_listView,
        inputRow
    });

    return ftxui::Renderer(container, [this]() {
        refreshVisibleEntries();
        ftxui::Element chatBody;
        if (log().empty()) {
            chatBody = ftxui::text(m_config.emptyText) | ftxui::dim | ftxui::center | ftxui::flex;
        } else {
            chatBody = m_listView->Render() | ftxui::flex | ftxui::yflex;
        }

        return ftxui::vbox({
            chatBody,
            ftxui::separator(),
            ftxui::hbox({
                m_inputComponent->Render(),
                ftxui::text(" "),
                m_sendButtonComponent->Render(),
            })
        }) | ftxui::yflex;

    });
}

}