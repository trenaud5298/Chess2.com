#ifndef CHESS_CORE_UI_SCROLLABLELISTVIEW_HPP
#define CHESS_CORE_UI_SCROLLABLELISTVIEW_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// FTXUI Includes
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>

// C++ Includes
#include <algorithm>
#include <functional>


namespace Chess {


class ScrollableListView : public ftxui::ComponentBase {
public:
    using CountFunction = std::function<int()>;
    using RenderFunction = std::function<ftxui::Element(int index)>;

    ScrollableListView(CountFunction countFunction, RenderFunction renderFunction);
    ~ScrollableListView();

    [[nodiscard]] bool Focusable() const override {return true;}
    bool OnEvent(ftxui::Event event) override;
    ftxui::Element OnRender() override;

private:
    [[nodiscard]] int visibleRowBudget() const;
    [[nodiscard]] int totalItems() const;

private:
    CountFunction m_countFunction;
    RenderFunction m_renderFunction;
    float m_focusY{1.0f};
    ftxui::Box m_box;
};


}

#endif