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

#include "regex.hpp"

namespace xonotic {

void LogParser::parse(const QString& line)
{
    if ( listening == DEFAULT )
    {
        static regex::Regex regex_status_begin = regex::optimized("^host:\\s+(.+)$");
        static regex::Regex regex_cvar = regex::optimized(
            R"regex(^(?:cvar \^3|")?([^"^ ]+)(?:"|\^7)? is "([^"]*)" \["([^"]*)"\]\s*(.*)$)regex");
        static regex::Regex regex_cvarlist_end = regex::optimized(
            R"regex(^\d+ cvar(?:\(s\))|(?: beginning with .*)$)regex");

        regex::Match match;
        if ( regex::match(line, regex_cvar, match) )
        {
            // only cvarlist and apropos show the description
            if ( !cvarlist && match.capturedLength(4) )
            {
                cvarlist = true;
                emit cvarlist_begin();
            }
            emit cvar({match.captured(1), match.captured(2), match.captured(3), match.captured(4)});
            return;
        }
        else if ( cvarlist )
        {
            cvarlist = false;
            emit cvarlist_end();
        }

        if ( regex::match(line, regex_status_begin, match) )
        {
            listening = STATUS;
            emit server_property_changed("host", match.captured(1));
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

void LogParser::parse_status(const QString& line)
{
    static regex::Regex regex_players = regex::optimized(
        R"(^players:\s+((\d+) active \(\d+ max\))$)");
    static regex::Regex regex_property = regex::optimized("^([a-z]+):\\s+(.*)$");
    static regex::Regex regex_player_header = regex::optimized(
        "^\\^[0-9]IP\\s+%pl\\s+ping\\s+time\\s+frags\\s+no\\s+name$");

    regex::Match match;
    if ( regex::match(line, regex_players, match) )
    {
        players_active = match.capturedRef(2).toUInt();
        players_.reserve(players_active);
        server_property_changed("players", match.captured(1));
    }
    else if ( regex::match(line, regex_property, match) )
    {
        server_property_changed(match.captured(1), match.captured(2));
    }
    else if ( regex::match(line, regex_player_header) )
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
    else if ( !line.isEmpty() )
    {
        listening = DEFAULT;
    }
}

void LogParser::parse_player(const QString& line)
{
    static regex::Regex regex_player = regex::optimized(
        // rowcol IP      %pl     ping    time   frags     no           name
        R"(^\^[37](\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+#([0-9]+)\s+\^7(.*)$)"
    );

    regex::Match match;
    if ( regex::match(line, regex_player, match) )
    {
        players_.emplace_back();
        players_.back().ip    = match.captured(1);
        players_.back().pl    = match.captured(2);
        players_.back().ping  = match.captured(3);
        players_.back().time  = match.captured(4);
        players_.back().frags = match.captured(5);
        players_.back().no    = match.captured(6);
        players_.back().name  = match.captured(7);
        if ( players_active == players_.size() )
        {
            listening = DEFAULT;
            emit players_changed(players_);
        }
    }
}

} // namespace xonotic
