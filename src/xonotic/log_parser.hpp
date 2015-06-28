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

#include <regex>
#include <map>
#include <QString>
#include <QObject>

namespace xonotic {

/**
 * \brief Xonotic player info
 *
 * Contains raw data as read from the status line
 */
struct Player
{
    std::string ip;
    std::string pl;
    std::string ping;
    std::string time;
    std::string frags;
    std::string no;
    std::string name;
};

/**
 * \brief Xonotic console variable info
 *
 * Contains raw data as read from the log line
 */
struct Cvar
{
    std::string name;
    std::string value;
    std::string default_value;
    std::string description;
};

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
    void parse(const std::string& line);

    /**
     * \brief Returns the vector of parsed players
     */
    const std::vector<Player> players() const { return players_; }

signals:
    void server_property_changed(const QString& name, const QString& value);

    void cvar(Cvar var);

    void players_changed();

private:
    enum {
        DEFAULT           = 0x00,
        STATUS            = 0x01,
        STATUS_PLAYERS    = 0x02|STATUS,
    } listening = DEFAULT;
    std::vector<Player> players_;
    unsigned players_active = 0;

    /**
     * \brief Parses a player line
     */
    void parse_player(const std::string& line);

    /**
     * \brief Parses a server status line
     */
    void parse_status(const std::string& line);
};

} // namespace xonotic
#endif // XONOTIC_LOG_PARSER_HPP
