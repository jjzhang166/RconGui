/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \section License
 *
 * Copyright (C)  Mattia Basaglia
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef NETWORK_TIME_HPP
#define NETWORK_TIME_HPP
#include <chrono>
namespace network {
    /**
     * \brief A reliable clock
     */
    using Clock     = std::chrono::steady_clock;
    /**
     * \brief Time point for network::Clock
     */
    using Time      = Clock::time_point;
} // namespace network
#endif // NETWORK_TIME_HPP
