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
#ifndef CVAR_EXPANSION_HPP
#define CVAR_EXPANSION_HPP


/**
 * \brief Cvar expansion rule
 */
enum class CvarExpansion {
    NotExpanded,        ///< Cvars are never expanded
    ExpandOrWarn,       ///< Cvars are expanded if a value is known, otherwise issue a warning
    ExpandIfValue,      ///< Cvars are expanded if a value is known, otherwise the $identifier is left unchanged
    ExpandAlways        ///< Cvars are expanded no matter what
};

inline QString cvar_expansion_to_string(CvarExpansion exp)
{
    switch ( exp )
    {
        case CvarExpansion::NotExpanded:  return "not_expanded";
        case CvarExpansion::ExpandOrWarn: return "expand_or_warn";
        case CvarExpansion::ExpandIfValue:return "expand_if_value";
        case CvarExpansion::ExpandAlways: return "expand_always";
        default: return {};
    }
}

inline CvarExpansion cvar_expansion_from_string(const QString& exp)
{
    if ( exp == "not_expanded"    ) return CvarExpansion::NotExpanded;
    if ( exp == "expand_or_warn"  ) return CvarExpansion::ExpandOrWarn;
    if ( exp == "expand_if_value" ) return CvarExpansion::ExpandIfValue;
    if ( exp == "expand_always"   ) return CvarExpansion::ExpandAlways;
    return CvarExpansion::NotExpanded;
}

#endif // CVAR_EXPANSION_HPP
