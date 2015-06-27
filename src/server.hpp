/*
 * Copyright (C) 2015 Mattia Basaglia
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
#ifndef SERVER_HPP
#define SERVER_HPP

#include <sstream>

namespace network {

/**
 * \brief Identifies a network server
 */
struct Server
{
    std::string host; ///< Host name or addess
    uint16_t    port; ///< Port number

    /**
     * \brief Creates an object from explicit values
     */
    Server(std::string host, uint16_t port) : host(std::move(host)), port(port) {}

    /**
     * \brief Creates an invalid server
     */
    Server() : host(""), port(0) {}

    /**
     * \brief Name as host:port
     */
    std::string name() const
    {
        std::ostringstream ss;
        if ( host.find(':') != std::string::npos )
            ss << '[' << host << ']'; // might be an IPv6 address
        else
            ss << host;
        ss << ':' << port;
        return ss.str();
    }

    bool operator==(const Server& server) const
    {
        return host == server.host && port == server.port;
    }

    bool operator!=(const Server& server) const
    {
        return host != server.host || port != server.port;
    }
};

} // namespace network

#endif // SERVER_HPP
