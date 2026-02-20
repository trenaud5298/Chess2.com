#include <Chess/Core/Game/test.h>
#include <iostream>

// Asio (standalone)
#include <asio/io_context.hpp>

// FTXUI
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

void coreTest() {
    // --- Asio sanity test ---
    asio::io_context io;
    bool asio_ok = true;

    // --- FTXUI sanity test ---
    using namespace ftxui;

    auto document = text("FTXUI OK") | bold;
    auto screen = Screen::Create(Dimension::Fit(document));
    Render(screen, document);

    std::cout << "Asio OK: " << asio_ok << "\n";
    std::cout << "FTXUI OK\n";
}