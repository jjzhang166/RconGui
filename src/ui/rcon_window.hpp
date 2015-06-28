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
#ifndef RCON_WINDOW_HPP
#define RCON_WINDOW_HPP

#include <QMainWindow>
#include "ui_rcon_window.h"

/**
 * \brief Main window
 */
class RconWindow : public QMainWindow, private Ui::RconWindow
{
    Q_OBJECT

public:
    RconWindow(QWidget* parent = nullptr);

public slots:
    /**
     * \brief Shows the server creation dialog and creates a new tab
     */
    void new_tab();
};

#endif // RCON_WINDOW_HPP
