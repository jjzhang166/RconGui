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
#ifndef SERVER_WIDGET_HPP
#define SERVER_WIDGET_HPP

#include <thread>
#include <mutex>

#include "ui_server_widget.h"
#include "network/udp_io.hpp"
#include "xonotic/xonotic.hpp"
#include "xonotic/server_model.hpp"
#include "xonotic/log_parser.hpp"

using Lock = std::unique_lock<std::mutex>;

/**
 * \brief Widget interfacing to rcon
 */
class ServerWidget : public QWidget, private Ui_ServerWidget
{
    Q_OBJECT

    friend class Ui_ServerWidget;

public:
    ServerWidget(xonotic::Xonotic xonotic, QWidget* parent = nullptr);
    ~ServerWidget();

    /**
     * \brief Whether the widget is connected to the xonotic server
     */
    bool xonotic_connected();

    /**
     * \brief Returns Xonotic connection details
     */
    const xonotic::Xonotic& xonotic_connection() const { return xonotic; }

    /**
     * \brief Name of the xonotic connection as a QString
     */
    QString name() const;

    /**
     * \brief Runs a rcon command
     */
    void rcon_command(const std::string& command);
    void rcon_command(const char* command)
    {
        rcon_command(std::string(command));
    }

signals:
    /**
     * \brief Emitted when the name of the xonotic connection changes
     */
    void name_changed(const QString& name);

    /**
     * \brief Emitted on network errors
     */
    void network_error(const QString& message);

    /**
     * \brief Emitted when a log message is received
     * \note Not emitted from the main thread
     */
    void log_received(const QString& line);

public slots:
    /**
     * \brief Runs a rcon command
     */
    void rcon_command(const QString& command)
    {
        rcon_command(command.toStdString());
    }

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
     * \brief Diconnect and connect from xonotic
     */
    bool xonotic_reconnect();

private slots:
    void on_button_setup_clicked();
    void on_output_console_customContextMenuRequested(const QPoint &pos);
    void on_action_attach_log_triggered();
    void on_action_detach_log_triggered();
    void on_action_save_log_triggered();

    void clear_log();

    /**
     * \brief Appends log to the output console
     */
    void append_log(const QString& log);

    /**
     * \brief Requests xonotic status
     */
    void xonotic_request_status();

    /**
     * \brief Update the model with the error message
     */
    void network_error_status(const QString& msg);

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

    /**
     * \brief Handles a Xonotic datagram
     */
    void xonotic_read(const std::string& datagram);

    /**
     * \brief Parse a line from the xonotic log
     */
    void xontotic_parse(const std::string& log_line);

    /**
     * \brief Color from regex match
     */
    static QColor xonotic_color(const QString& s);

    /**
     * \brief Hex character to integer
     */
    static int hex_to_int(char c)
    {
        if ( c <= '9' && c >= '0' )
            return c - '0';
        if ( c <= 'f' && c >= 'a' )
            return c - 'a' + 0xa;
        if ( c <= 'F' && c >= 'A' )
            return c - 'A' + 0xa;
        return 0;
    }

    std::mutex                  mutex;
    std::string                 header{"\xff\xff\xff\xff"};     ///< Connection message header
    std::string                 line_buffer;                    ///< Buffer for overflowing messages from Xonotic
    xonotic::Xonotic            xonotic;
    network::UdpIo              io;
    std::thread                 thread_input;
    xonotic::ServerModel        model_server;
    xonotic::LogParser          log_parser;
};

#endif // SERVER_WIDGET_HPP
