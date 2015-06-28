/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \section License
 *
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
#ifndef XONOTIC_HPP
#define XONOTIC_HPP

#include "network/server.hpp"

namespace xonotic {

/**
 * \brief Xonotic connection information
 * \todo rename it to Server or Connection
 */
struct Xonotic
{
    enum Secure {
        NO        = 0,  ///< Plaintext rcon
        TIME      = 1,  ///< Time-based digest
        CHALLENGE = 2   ///< Challeng-based security
    };

    Xonotic(network::Server     server,
            std::string         rcon_password,
            Secure              rcon_secure = NO,
            std::string         name = {})
        : server(std::move(server)),
          rcon_password(std::move(rcon_password)),
          rcon_secure(rcon_secure),
          name(std::move(name))
    {
        if ( this->name.empty() )
            this->name = this->server.name();
    }

    network::Server     server;         ///< Connection server
    std::string         rcon_password;  ///< Rcon Password
    Secure              rcon_secure;    ///< Rcon Secure
    std::string         name;           ///< Server name
};

} // namespace xonotic
#endif // XONOTIC_HPP
