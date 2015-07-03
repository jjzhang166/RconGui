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
#include <QIcon>

#include "xonotic/player.hpp"

/**
 * \brief An action to perform on a player
 */
class PlayerAction
{
public:
    PlayerAction(QString name, QString command, QString icon = {})
        : name_(std::move(name)),
          command_(std::move(command)),
          icon_(std::move(icon)){}

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
    QString command(const xonotic::Player& player) const
    {
        auto cmd = command_;
        return cmd
            .replace("$entity", player.no)
            .replace("$ip",     player.ip)
            .replace("$name",   player.name)
        ;
    }

    /**
     * \brief Name/path of the icon for the action
     */
    QString icon_name() const { return icon_; }

    /**
     * \brief Icon for the action
     */
    QIcon icon() const
    {
        if ( icon_.isEmpty() )
            return {};
        if ( !icon_.contains("/") )
            return QIcon::fromTheme(icon_);
        return QIcon(icon_);
    }

private:
    QString name_;
    QString command_;
    QString icon_;
};

#endif // XONOTIC_PLAYER_ACTION_HPP
