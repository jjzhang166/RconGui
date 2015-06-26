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

#ifndef CREATE_SERVER_DIALOG_HPP
#define CREATE_SERVER_DIALOG_HPP

#include "ui_create_server_dialog.h"

class CreateServerDialog : public QDialog, private Ui::CreateServerDialog
{
    Q_OBJECT

public:
    CreateServerDialog(QWidget* parent = nullptr);
};

#endif // CREATE_SERVER_DIALOG_HPP
