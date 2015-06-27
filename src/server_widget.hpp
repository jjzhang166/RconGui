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

#include <thread>
#include <mutex>

#include "ui_server_widget.h"
#include "xonotic.hpp"
#include "udp_io.hpp"

using Lock = std::unique_lock<std::mutex>;

/**
 * \brief Widget interfacing to rcon
 */
class ServerWidget : public QWidget, private Ui_ServerWidget
{
    Q_OBJECT

public:
    ServerWidget(network::Xonotic xonotic, QWidget* parent = nullptr);
    ~ServerWidget();

    /**
     * \brief Close the xonotic connection and clear connection data
     */
    void xonotic_disconnect();

    /**
     * \brief Open the xonotic connection
     * \returns Whether the connection has been successful
     */
    bool xonotic_connect();

    /**
     * \brief Whether the widget is connected to the xonotic server
     */
    bool xonotic_connected();

    /**
     * \brief Returns Xonotic connection details
     */
    const network::Xonotic& xonotic_connection() const { return xonotic; }

    /**
     * \brief Name of the xonotic connection as a QString
     */
    QString name() const;

signals:
    /**
     * \brief Emitted when the name of the xonotic connection changes
     */
    void name_changed(const QString& name);

    /**
     * \brief Emitted on network errors
     */
    void network_error(const QString& message);

public slots:
    /**
     * \brief Runs a rcon command
     */
    void rcon_command(const QString& command);

private slots:
    void on_button_setup_clicked();
    void on_output_console_customContextMenuRequested(const QPoint &pos);
    void on_button_send_clicked();

    /**
     * \brief Requests xonotic status
     */
    void xonotic_request_status();

private:

    /**
     * \brief Low level xonotic disconnection
     */
    void xonotic_close_connection();

    /**
     * \brief Clear connection data
     */
    void xonotic_clear();

    /**
     * \brief Writes \c line to the xonotic server
     */
    void xonotic_write(std::string line);

    std::mutex          mutex;
    std::string         header = "\xff\xff\xff\xff";    ///< Connection message header
    network::Xonotic    xonotic;
    network::UdpIo      io;
    std::thread         thread_input;
};

#endif // SERVER_WIDGET_HPP
