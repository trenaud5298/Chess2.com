#ifndef CHESS_SERVER_COMMON_TIMEFORMAT_HPP
#define CHESS_SERVER_COMMON_TIMEFORMAT_HPP

/*
 * Chess
 * Copyright (c) 2026 Tyler Renaud, Thomas O'Neil
 *
 * This source code is licensed under the BSD 3-Clause License.
 * See the LICENSE file in the root of the source tree for details.
 */

// Chess Includes

// ASIO Includes


// C++ Includes
#include <chrono>
#include <format>
#include <string>

namespace Chess {

// HH:MM:SS
template<typename Rep, typename Period>
std::string formatHHMMSS(std::chrono::duration<Rep, Period> duration) {
    std::chrono::seconds seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    std::chrono::hh_mm_ss hms(seconds);
    return std::format("{:02}:{:02}:{:02}",
        hms.hours().count(),
        hms.minutes().count(),
        hms.seconds().count()
    );
}


// seconds.milliseconds (12.345)
template<typename Rep, typename Period>
std::string formatSecondsMilliseconds(std::chrono::duration<Rep, Period> duration) {
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    double seconds = ms.count() / 1000.0;
    return std::format("{:.3f}", seconds);
}

// fixed width seconds.milliseconds (00012.345)
template<typename Rep, typename Period>
std::string formatSecondsMillisecondsFixed(std::chrono::duration<Rep, Period> duration) {
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    double seconds = ms.count() / 1000.0;
    return std::format("{:08.3f}", seconds);
}

}

#endif