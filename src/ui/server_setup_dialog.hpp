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
#ifndef SERVER_SETUP_DIALOG_HPP
#define SERVER_SETUP_DIALOG_HPP

#include "ui_server_setup_dialog.h"


/**
 * \brief Form used to gather Xonotic connection information
 */
class ServerSetupDialog : public QDialog, private Ui::ServerSetupDialog
{
    Q_OBJECT

public:
    ServerSetupDialog(QWidget* parent = nullptr)
        : QDialog(parent)
    {
        setupUi(this);
    }

    ServerSetupDialog(const xonotic::Xonotic& xonotic, QWidget* parent = nullptr)
        : ServerSetupDialog(parent)
    {
        widget->populate(xonotic);
    }

    /**
     * \brief Builds a Xonotic connection from the form contents
     */
    xonotic::Xonotic connection_info() const
    {
        return widget->connection_info();
    }
};


#endif // SERVER_SETUP_DIALOG_HPP
