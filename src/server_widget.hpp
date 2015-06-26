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

#ifndef SERVER_WIDGET_HPP
#define SERVER_WIDGET_HPP

#include "ui_server_widget.h"
#include "xonotic.hpp"
#include "udp_io.hpp"

/**
 * \brief Widget interfacing to rcon
 */
class ServerWidget : public QWidget, private Ui_ServerWidget
{
    Q_OBJECT

public:
    ServerWidget(network::Xonotic xonotic, QWidget* parent = nullptr);

signals:
    /**
     * \brief Emitted when the name of the xonotic connection changes
     */
    void nameChanged(const QString& name);

private slots:
    void on_button_setup_clicked();

private:
    network::Xonotic xonotic;
};

#endif // SERVER_WIDGET_HPP
