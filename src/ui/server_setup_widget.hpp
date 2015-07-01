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
#ifndef SERVER_SETUP_WIDGET_HPP
#define SERVER_SETUP_WIDGET_HPP

#include "ui_server_setup_widget.h"
#include "xonotic/connection_details.hpp"

/**
 * \brief Form used to gather Xonotic connection information
 */
class ServerSetupWidget : public QWidget, private Ui::ServerSetupWidget
{
    Q_OBJECT

public:
    ServerSetupWidget(QWidget* parent = nullptr);

    ServerSetupWidget(const xonotic::ConnectionDetails& xonotic, QWidget* parent = nullptr);

    /**
     * \brief Builds a Xonotic connection from the form contents
     */
    xonotic::ConnectionDetails connection_details() const;

    /**
     * \brief Populates the form input fields from the server settings
     */
    void populate(const xonotic::ConnectionDetails& xonotic);

    /**
     * \brief Sets up the items in a combo box to be used to get rcon_secure values
     */
    static void setup_secure_combo(QComboBox* box);

    static QString secure_to_text(xonotic::ConnectionDetails::Secure secure);

    static QIcon secure_to_icon(xonotic::ConnectionDetails::Secure secure);

signals:
    void accepted();
    void rejected();

private slots:
    void on_button_save_clicked();
    void on_button_delete_clicked();
    void on_input_preset_currentIndexChanged(const QString& text);

private:
    /**
     * \brief Loads the saved servers in the preset combo box
     */
    void update_presets();


    static void secure_to_item(QComboBox* box, xonotic::ConnectionDetails::Secure secure);

};

#endif // SERVER_SETUP_WIDGET_HPP
