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
#ifndef XONOTIC_CVAR_HPP
#define XONOTIC_CVAR_HPP

#include <string>
#include <QMetaType>

namespace xonotic {
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

} // namespace xonotic

Q_DECLARE_METATYPE(xonotic::Cvar)
#endif // XONOTIC_CVAR_HPP
