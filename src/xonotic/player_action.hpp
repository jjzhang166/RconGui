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
#ifndef XONOTIC_PLAYER_ACTION_HPP
#define XONOTIC_PLAYER_ACTION_HPP

#include <QString>

#include "player.hpp"

namespace xonotic {

/**
 * \brief An action to perform on a player
 */
class PlayerAction
{
public:
    PlayerAction(const QString& name, const QString& command)
        : name_(name), command_(command) {}

    /**
     * \brief Name of the action
     */
    QString name() const { return name_; }

    /**
     * \brief Command template
     */
    QString command() const { return command_; }

    /**
     * \brief Expanded command
     * \todo Use a trie
     */
    QString command(const Player& player) const
    {
        auto cmd = command_;
        return cmd
            .replace("$entity", QString::fromStdString(player.no))
            .replace("$ip",     QString::fromStdString(player.ip))
            .replace("$name",   QString::fromStdString(player.name))
        ;
    }

private:
    QString name_;
    QString command_;
};

} // namespace xonotic
#endif // XONOTIC_PLAYER_ACTION_HPP
