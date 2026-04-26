/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Client/UI/Modal/ModalInterface.hpp>
#include <Chess/Client/UI/ClientPanel.hpp>

// ASIO Includes

// FTXUI Includes

// C++ Includes


namespace Chess {
void ModalInterface::requestDismiss() {
    m_clientPanel.popModal();
}
}
