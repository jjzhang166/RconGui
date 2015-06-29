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
#include "log_parser.hpp"

/**
 * \brief Regex options
 */
static const auto reo = std::regex::ECMAScript|std::regex::optimize;

namespace xonotic {

void LogParser::parse(const std::string& line)
{
    if ( listening == DEFAULT )
    {
        static std::regex regex_status_begin("host:\\s+(.+)",reo);
        static std::regex regex_cvar(
            R"regex((?:cvar \^3|")?([^"^]+)(?:"|\^7)? is "([^"]*)" \["([^"]*)"\]\s*(.*))regex",reo);
        static std::regex regex_cvarlist_end(
            R"regex(\d+ cvar(?:\(s\))|(?: beginning with .*))regex",reo);

        std::smatch match;
        if ( std::regex_match(line, match, regex_cvar) )
        {
            // only cvarlist and apropos show the description
            if ( !cvarlist && match[4].length() )
            {
                cvarlist = true;
                emit cvarlist_begin();
            }
            emit cvar({match[1], match[2], match[3], match[4]});
            return;
        }
        else if ( cvarlist )
        {
            cvarlist = false;
            emit cvarlist_end();
        }

        if ( std::regex_match(line, match, regex_status_begin) )
        {
            listening = STATUS;
            emit server_property_changed("host", QString::fromStdString(match[1]));
        }

    }
    else if ( listening == STATUS )
    {
        parse_status(line);
    }
    else if ( listening == STATUS_PLAYERS )
    {
        parse_player(line);
    }
}

void LogParser::parse_status(const std::string& line)
{
    static std::regex regex_players(
        R"(players:\s+((\d+) active \(\d+ max\)))",
        std::regex::ECMAScript|std::regex::optimize
    );
    static std::regex regex_property("([a-z]+):\\s+(.*)",reo);
    static std::regex regex_player_header(
        "\\^[0-9]IP\\s+%pl\\s+ping\\s+time\\s+frags\\s+no\\s+name",
        reo
    );

    std::smatch match;
    if ( std::regex_match(line, match, regex_players) )
    {
        players_active = std::stoul(match[2]);
        players_.reserve(players_active);
        server_property_changed("players", QString::fromStdString(match[1]));
    }
    else if ( std::regex_match(line, match, regex_property) )
    {
        server_property_changed(QString::fromStdString(match[1]),
                                QString::fromStdString(match[2]));
    }
    else if ( std::regex_match(line, match, regex_player_header) )
    {
        players_.clear();
        if ( players_active == 0 )
        {
            listening = DEFAULT;
            emit players_changed(players_);
        }
        else
        {
            listening = STATUS_PLAYERS;
        }
    }
    else if ( !line.empty() )
    {
        listening = DEFAULT;
    }
}

void LogParser::parse_player(const std::string& line)
{
    static std::regex regex_player(
        // rowcol IP      %pl     ping    time   frags     no           name
        R"(\^[37](\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+#([0-9]+)\s+\^7(.*))",
        reo
    );

    std::smatch match;
    if ( std::regex_match(line, match, regex_player) )
    {
        players_.emplace_back();
        players_.back().ip    = match[1];
        players_.back().pl    = match[2];
        players_.back().ping  = match[3];
        players_.back().time  = match[4];
        players_.back().frags = match[5];
        players_.back().no    = match[6];
        players_.back().name  = match[7];
        if ( players_active == players_.size() )
        {
            listening = DEFAULT;
            emit players_changed(players_);
        }
    }
}

} // namespace xonotic
