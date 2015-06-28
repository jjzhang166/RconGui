/*
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
#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <QSettings>
#include <QHash>
#include <QColor>
#include <QFont>

#include "xonotic/xonotic.hpp"

/**
 * \brief Settings singleton
 */
class Settings
{
public:
    static Settings& instance()
    {
        static Settings singleton;
        return singleton;
    }

    ~Settings();

    void load();

    void save();

    QHash<QString, xonotic::Xonotic> saved_servers; ///< Server presets

    QColor console_foreground{192, 192, 192};   ///< Default text color for the console
    QColor console_background{Qt::black};       ///< Background color for the console
    int    console_brightness_max{255};         ///< Maximum brightness for console colors
    int    console_brightness_min{80};          ///< Minimum brightness for console colors
    QFont  console_font{"monospace", 10};       ///< Console text font

private:
    Settings();

};

/**
 * \brief Shorthand for Settings::instance()
 */
inline Settings& settings() { return Settings::instance(); }

#endif // SETTINGS_HPP
