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
#ifndef CREATE_SERVER_DIALOG_HPP
#define CREATE_SERVER_DIALOG_HPP

#include "ui_create_server_dialog.h"
#include "xonotic/xonotic.hpp"

/**
 * \brief Form used to gather Xonotic connection information
 */
class CreateServerDialog : public QDialog, private Ui::CreateServerDialog
{
    Q_OBJECT

public:
    CreateServerDialog(QWidget* parent = nullptr);

    CreateServerDialog(const xonotic::Xonotic& xonotic, QWidget* parent = nullptr);

    /**
     * \brief Builds a Xonotic connection from the form contents
     */
    xonotic::Xonotic connection_info() const;

private slots:
    void on_button_save_clicked();
    void on_button_delete_clicked();
    void on_input_preset_currentIndexChanged(const QString& text);

private:
    /**
     * \brief Loads the saved servers in the preset combo box
     */
    void update_presets();
    /**
     * \brief Populates the form input fields from the server settings
     */
    void populate(const xonotic::Xonotic& xonotic);
};

#endif // CREATE_SERVER_DIALOG_HPP
