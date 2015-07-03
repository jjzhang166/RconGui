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
#ifndef REGEX_HPP
#define REGEX_HPP

#include <QRegularExpression>

namespace regex {

using Regex = QRegularExpression;
using Match = QRegularExpressionMatch;

inline Regex optimized(const QString& pattern)
{
    return Regex(pattern, Regex::OptimizeOnFirstUsageOption);
}

inline bool match(const QString& str, const Regex& exp, Match& match)
{
    match = exp.match(str);
    return match.hasMatch();
}

inline bool match(const QString& str, const Regex& exp)
{
    Match regex_match;
    return match(str, exp, regex_match);
}

} // namespace regex

#endif // REGEX_HPP