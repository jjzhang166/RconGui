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
#ifndef XONOTIC_PLAYER_HPP
#define XONOTIC_PLAYER_HPP

#include <QString>

namespace xonotic {

/**
 * \brief Xonotic player info
 *
 * Contains raw data as read from the status line
 */
struct Player
{
    QString ip;
    QString pl;
    QString ping;
    QString time;
    QString frags;
    QString no;
    QString name;
};

} // namespace xonotic
#endif // XONOTIC_PLAYER_HPP
