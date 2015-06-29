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

#include <QSortFilterProxyModel>

#include "ui_server_widget.h"
#include "xonotic/qdarkplaces.hpp"
#include "xonotic/connection_details.hpp"
#include "xonotic/log_parser.hpp"
#include "xonotic/server_model.hpp"
#include "xonotic/cvar_model.hpp"
#include "xonotic/player_model.hpp"
#include "xonotic/player_action.hpp"

/**
 * \brief Widget interfacing to rcon
 */
class ServerWidget : public QWidget, private Ui_ServerWidget
{
    Q_OBJECT

    friend class Ui_ServerWidget;

public:
    ServerWidget(xonotic::ConnectionDetails xonotic, QWidget* parent = nullptr);

    ~ServerWidget();

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

    bool xonotic_reconnect();

private slots:
    void on_button_setup_clicked();
    void on_output_console_customContextMenuRequested(const QPoint &pos);
    void on_action_attach_log_triggered();
    void on_action_detach_log_triggered();
    void on_action_save_log_triggered();

    void clear_log();

    /**
     * \brief Requests xonotic status
     */
    void request_status();

    /**
     * \brief Requests xonotic cvar list
     */
    void request_cvars();

    /**
     * \brief Update the model with the error message
     */
    void network_error_status(const QString& msg);

    /**
     * \brief Applys the current filter settings to the cvar list
     */
    void cvarlist_apply_filter();

    void xonotic_disconnected();
    void xonotic_connected();
    void xonotic_log(const QString& line);

private:

    /**
     * \brief Cleans up the connection
     */
    void xonotic_clear();

    /**
     * \brief Sets the network status message
     */
    void set_network_status(const QString& msg);

    /**
     * \brief Creates a button which executes the given action on the given player
     */
    QAbstractButton* create_button(const xonotic::PlayerAction& action,
                                   const xonotic::Player& player);

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

    xonotic::QDarkplaces        connection;
    xonotic::LogParser          log_parser;
    xonotic::ServerModel        model_server;
    xonotic::CvarModel          model_cvar;
    QSortFilterProxyModel       proxy_cvar;
    xonotic::PlayerModel        model_player;
};

#endif // SERVER_WIDGET_HPP
