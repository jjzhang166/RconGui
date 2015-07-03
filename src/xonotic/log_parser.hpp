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
#ifndef XONOTIC_LOG_PARSER_HPP
#define XONOTIC_LOG_PARSER_HPP

#include <QString>
#include <QObject>

#include "cvar.hpp"
#include "player.hpp"

namespace xonotic {

/**
 * \brief Parser for "status 1"
 */
class LogParser : public QObject
{
    Q_OBJECT

public:
    /**
     * \brief Parse a xonotic log line
     */
    void parse(const QString& line);

    /**
     * \brief Returns the vector of parsed players
     */
    const std::vector<Player> players() const { return players_; }

signals:
    /**
     * \brief Emitted when a status server property has been matched
     */
    void server_property_changed(const QString& name, const QString& value);

    /**
     * \brief Emitted when the log showed info about a cvar
     */
    void cvar(Cvar var);

    /**
     * \brief Emitted at the end of status 1
     */
    void players_changed(const std::vector<Player>& players);

    /**
     * \brief Emitted at the beginning of cvarlist
     */
    void cvarlist_begin();
    /**
     * \brief Emitted at the end of cvarlist
     */
    void cvarlist_end();

private:
    enum {
        DEFAULT           = 0x00,
        STATUS            = 0x01,
        STATUS_PLAYERS    = 0x02|STATUS,
    } listening = DEFAULT;
    std::vector<Player> players_;
    unsigned players_active = 0;
    bool cvarlist = false;

    /**
     * \brief Parses a player line
     */
    void parse_player(const QString& line);

    /**
     * \brief Parses a server status line
     */
    void parse_status(const QString& line);
};

} // namespace xonotic

Q_DECLARE_METATYPE(std::vector<xonotic::Player>)

#endif // XONOTIC_LOG_PARSER_HPP
