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
#include <QCompleter>

#include "ui_server_widget.h"
#include "xonotic/qdarkplaces.hpp"
#include "xonotic/connection_details.hpp"
#include "xonotic/log_parser.hpp"
#include "model/server_model.hpp"
#include "model/server_delegate.hpp"
#include "model/cvar_model.hpp"
#include "model/cvar_delegate.hpp"
#include "model/player_model.hpp"
#include "model/player_action.hpp"

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
    void on_action_save_log_triggered();
    void on_tabWidget_currentChanged(int tab);
    void on_input_cvar_filter_section_currentIndexChanged(int index);
    void on_input_console_lineExecuted(QString cmd);

    /**
     * \brief Attaches log_dest_udp
     */
    void attach_log();
    /**
     * \brief Detaches
     */
    void detach_log();

    /**
     * \brief Clears the console
     */
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
    void xonotic_log_end();
    void xonotic_log(const QString& log);

private:
    /**
     * \brief Initializes the server status UI
     */
    void init_status_table();


    /**
     * \brief Initializes the cvar list UI
     */
    void init_cvar_table();

    /**
     * \brief Initializes the player list UI
     */
    void init_player_table();

    /**
     * \brief Initializes the console UI
     */
    void init_console();

    /**
     * \brief Initializes the Darkplaces connection
     */
    void init_connection();

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
    QAbstractButton* create_button(const PlayerAction& action,
                                   const xonotic::Player& player);

    /**
     * \brief Ensures cvars have been loaded
     */
    void ensure_has_cvars();

    /// Object handling the DP protocol
    xonotic::QDarkplaces        connection;
    /// Parses the log from the connection to populate the model
    xonotic::LogParser          log_parser;
    /// Buffer used to cache log received from darkplaces
    QStringList                 log_buffer;
    /// Server status model
    ServerModel                 model_server;
    /// Server status edit delegate
    ServerDelegate              delegate_server;
    /// Cvar list model
    CvarModel                   model_cvar;
    /// Proxy to filter the cvar list model
    QSortFilterProxyModel       proxy_cvar;
    /// Completer for cvars
    QCompleter                  complete_cvar;
    /// Cvar edit delegate
    CvarDelegate                delegate_cvar;
    /// Connected player model
    PlayerModel                 model_player;
    /// Commands used to request status
    QStringList                 cmd_status = {"status 1", "g_maplist"};
    /// Commands used to request cvars
    QStringList                 cmd_cvars  = {"cvarlist"};
    /// Whether log_dest_udp has been set and needs cleanup
    bool                        log_dest_set = false;

};

#endif // SERVER_WIDGET_HPP
