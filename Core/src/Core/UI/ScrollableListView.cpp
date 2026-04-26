/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Core/UI/ScrollableListView.hpp>

// FTXUI Includes
#include <ftxui/component/event.hpp>

namespace Chess {


ScrollableListView::ScrollableListView(CountFunction countFunction, RenderFunction renderFunction)
: m_countFunction(std::move(countFunction)), m_renderFunction(std::move(renderFunction)) {

}

ScrollableListView::~ScrollableListView() {

}

int ScrollableListView::visibleRowBudget() const {
    return std::max(1, m_box.y_max - m_box.y_min + 1);
}

int ScrollableListView::totalItems() const {
    if (!m_countFunction) {
        return 0;
    }
    return std::max(0, m_countFunction());
}

bool ScrollableListView::OnEvent(ftxui::Event event) {
    // Manual Take Focus On Clicked
    if (event.is_mouse()) {
        if (event.mouse().button == ftxui::Mouse::Left && event.mouse().motion == ftxui::Mouse::Pressed) {
            if (m_box.Contain(event.mouse().x, event.mouse().y)) {
                TakeFocus();
                return true;
            }
        }
    }

    if (!Focused()) {
        return false;
    }

    if (totalItems() == 0) {
        return false;
    }

    return false;
}


ftxui::Element ScrollableListView::OnRender() {
    int total = totalItems();
    std::vector<ftxui::Element> elements;

    if (total == 0 || !m_renderFunction) {
        elements.push_back(ftxui::emptyElement());
    } else {
        int budget = visibleRowBudget();
        int start = std::max(0,total - budget);

        elements.reserve(std::max(1, total-start) + 1);
        for (int i = start; i < total; ++i) {
            elements.push_back(m_renderFunction(i));
        }
    }

    ftxui::Element entries = ftxui::vbox(std::move(elements));
    entries |= ftxui::focusPositionRelative(0.f, m_focusY);
    entries |= ftxui::yframe;
    entries |= ftxui::vscroll_indicator;
    entries |= ftxui::reflect(m_box);

    if (Focused()) {
        entries |= ftxui::focus;
    }

    return entries;
}
}
