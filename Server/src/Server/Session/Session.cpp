/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes
#include <Chess/Server/Session/Session.h>

// ASIO Includes

// C++ Includes
#include <iostream>


namespace Chess {

Session::Session(int placeholder) : m_placeholder(placeholder) {
    std::cout<<"Creating Session"<<std::endl;
}

void Session::message() {
    std::cout<<"Session message"<<std::endl;
}

void Session::kick() {
    std::cout<<"Session kick"<<std::endl;
}


}
