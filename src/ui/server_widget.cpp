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
#include "server_widget.hpp"

#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QScrollBar>
#include <QTextObject>
#include <QTime>
#include <QToolButton>

#include "server_setup_dialog.hpp"
#include "settings.hpp"
#include "xonotic/color_parser.hpp"

ServerWidget::ServerWidget(xonotic::ConnectionDetails details, QWidget* parent)
    : QWidget(parent), connection(std::move(details))
{
    setupUi(this);

    button_refresh_status->setShortcut(QKeySequence::Refresh);
    button_refresh_cvars->setShortcut(QKeySequence::Refresh);

    init_status_table();

    init_cvar_table();

    init_player_table();

    init_console();

    init_connection();
}

void ServerWidget::init_status_table()
{
    auto header_view = table_server_status->verticalHeader();
    QFont header_font = header_view->font();
    header_font.setBold(true);
    header_view->setFont(header_font);
    table_server_status->setModel(&model_server);
    table_server_status->setItemDelegate(&delegate_server);
    delegate_server.g_maplist = [this] {
        return model_cvar.cvar_value("g_maplist");
    };
    delegate_server.chmap = [this](const QString& map) {
        rcon_command("chmap "+map); /// \todo Read command from settings
        model_server.set_server_property("map", map);
    };
    delegate_server.set_hostname = [this](const QString& name) {
        rcon_command("set hostname "+name);
        model_server.set_server_property("host", name);
    };
    connect(&log_parser, &xonotic::LogParser::server_property_changed,
            &model_server, &ServerModel::set_server_property);
    cmd_status = settings().get("behaviour/cmd_status", cmd_status);

}

void ServerWidget::init_cvar_table()
{
    delegate_cvar.set_cvar = [this](const QString& name, const QString& value)
    {
        rcon_command("set "+name+' '+value);
        auto cvar = model_cvar.cvar(name);
        cvar.value = value.toStdString();
        model_cvar.set_cvar(cvar);
    };
    table_cvars->setModel(&proxy_cvar);
    table_cvars->setItemDelegate(&delegate_cvar);
    proxy_cvar.setSourceModel(&model_cvar);
    connect(&log_parser, &xonotic::LogParser::cvar,
            &model_cvar, &CvarModel::set_cvar);
    connect(&log_parser, &xonotic::LogParser::cvarlist_begin,
            &model_cvar, &CvarModel::block_updates);
    connect(&log_parser, &xonotic::LogParser::cvarlist_end,
            &model_cvar, &CvarModel::unblock_updates);
    auto header_view = table_cvars->horizontalHeader();
    header_view->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header_view->setSectionResizeMode(1, QHeaderView::Stretch);
    table_cvars->hideColumn(2);
    table_cvars->hideColumn(3);
    for ( int i = 0; i < model_cvar.columnCount(); i++ )
        input_cvar_filter_section->addItem(
            model_cvar.headerData(i,Qt::Horizontal).toString());
    cmd_cvars = settings().get("behaviour/cmd_cvars", cmd_cvars);
}

void ServerWidget::init_player_table()
{
    table_players->setModel(&model_player);
    connect(&log_parser, &xonotic::LogParser::players_changed,
            &model_player, &PlayerModel::set_players,
            Qt::QueuedConnection);
    auto header_view = table_players->horizontalHeader();
    header_view->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header_view->setSectionResizeMode(1, QHeaderView::Stretch);
    header_view->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    header_view->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    header_view->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    header_view->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    header_view->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    connect(&model_player, &PlayerModel::players_changed,
        [this](const std::vector<xonotic::Player>& players) {
            for ( unsigned i = 0; i < players.size(); i++ )
            {
                QDialogButtonBox *buttons = new QDialogButtonBox();
                for ( const auto& action : settings().player_actions )
                    buttons->addButton(create_button(action,players[i]),
                                       QDialogButtonBox::ActionRole);
                auto index = model_player.index(i, PlayerModel::Actions);
                table_players->setIndexWidget(index, buttons);
            }
        });

}

void ServerWidget::init_console()
{

    connect(action_clear_log, &QAction::triggered, this, &ServerWidget::clear_log);

    complete_cvar.setModel(&model_cvar);
    complete_cvar.setCaseSensitivity(Qt::CaseSensitive);
    complete_cvar.setModelSorting(QCompleter::CaseSensitivelySortedModel);
    complete_cvar.setCompletionColumn(0);
    complete_cvar.setCompletionRole(Qt::DisplayRole);
    complete_cvar.setMaxVisibleItems(6);
    input_console->setWordCompleter(&complete_cvar);
    input_console->setWordCompleterPrefix("$");
    input_console->setFont(settings().console_font);
    input_console->setHistory(settings().get_history(connection.details().name));

    clear_log();
}

void ServerWidget::init_connection()
{
    connect(&connection, &xonotic::QDarkplaces::disconnected,
            this, &ServerWidget::xonotic_disconnected,
            Qt::QueuedConnection);
    connect(&connection, &xonotic::QDarkplaces::connected,
            this, &ServerWidget::xonotic_connected,
            Qt::QueuedConnection);
    connect(&connection, &xonotic::QDarkplaces::connection_error,
            this, &ServerWidget::network_error_status,
            Qt::QueuedConnection);
    connect(&connection, &xonotic::QDarkplaces::received_log,
            this, &ServerWidget::xonotic_log,
            Qt::QueuedConnection);
    connect(&connection, &xonotic::QDarkplaces::log_end,
            this, &ServerWidget::xonotic_log_end,
            Qt::QueuedConnection);
    /// \todo Connect disconnecting -> clear log_dest_udp if needed
    connection.xonotic_connect();
}

ServerWidget::~ServerWidget()
{
    settings().set_history(connection.details().name, input_console->history());
}

void ServerWidget::clear_log()
{
    output_console->clear();
    QTextFrameFormat fmt;
    fmt.setBackground(settings().console_background);
    output_console->document()->rootFrame()->setFrameFormat(fmt);
    output_console->document()->setDefaultFont(settings().console_font);
    output_console->setTextColor(settings().console_foreground);
}

void ServerWidget::xonotic_disconnected()
{
    xonotic_clear();
    set_network_status(tr("Disconnected"));
}

void ServerWidget::xonotic_connected()
{
    xonotic_clear();
    set_network_status(tr("Connected"));
    request_status();
}

void ServerWidget::xonotic_clear()
{
    model_cvar.clear();
    model_player.clear();
    model_server.clear();
    model_server.set_server_property("server",
        QString::fromStdString(connection.details().server.name()));
}

void ServerWidget::xonotic_log_end()
{
    auto scrollbar = output_console->verticalScrollBar();
    bool scroll = scrollbar->value() == scrollbar->maximum();

    if ( !action_parse_colors->isChecked() )
    {
        output_console->append(log_buffer.join("\n"));
    }
    else
    {
        QTextCursor cursor(output_console->document());
        cursor.movePosition(QTextCursor::End);
        xonotic::ColorParser(
            settings().console_foreground,
            settings().console_brightness_min,
            settings().console_brightness_max
        ).convert(log_buffer, &cursor);
        log_buffer.clear();
    }

    if ( scroll )
        scrollbar->setValue(scrollbar->maximum());

    log_buffer.clear();
}

void ServerWidget::xonotic_log(const QString& log)
{
    log_parser.parse(log.toStdString());

    log_buffer.push_back(log);
}

QString ServerWidget::name() const
{
    return QString::fromStdString(connection.details().name);
}

void ServerWidget::rcon_command(const QString& command)
{
    connection.rcon_command(command.toStdString());
}

bool ServerWidget::xonotic_reconnect()
{
    return connection.reconnect();
}

void ServerWidget::on_button_setup_clicked()
{
    ServerSetupDialog dlg(connection.details(), this);
    if ( dlg.exec() )
    {
        auto oldname = connection.details().name;
        connection.set_details(dlg.connection_details());
        auto newname = connection.details().name;

        if ( newname != oldname )
            emit name_changed(QString::fromStdString(newname));
    }
}

void ServerWidget::on_output_console_customContextMenuRequested(const QPoint &pos)
{
    QMenu* menu = output_console->createStandardContextMenu();

    menu->addSeparator();
    menu->addAction(action_save_log);
    menu->addAction(action_clear_log);

    menu->addSeparator();
    menu->addAction(action_attach_log);
    menu->addAction(action_detach_log);
    menu->addAction(action_parse_colors);

    menu->exec(output_console->mapToGlobal(pos));
}

void ServerWidget::on_action_attach_log_triggered()
{
    connection.rcon_command("log_dest_udp "+connection.local_endpoint().name());
}

void ServerWidget::on_action_detach_log_triggered()
{
    connection.rcon_command("log_dest_udp \"\"");
}

void ServerWidget::on_action_save_log_triggered()
{
    QStringList filters = {
        tr("Text files (*.log *.txt)"),
        tr("HTML (*.htm *.html)"),
        tr("All files (*)"),
    };

    // static so that evey instance points to the same
    static QString directory;

    QFileDialog dialog(this, tr("Save Log"), directory);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilters(filters);

    if ( !dialog.exec() )
        return;

    QString filename = dialog.selectedFiles()[0];
    directory = QFileInfo(filename).dir().path();

    QFile file(filename);

    if ( !file.open(QIODevice::WriteOnly|QIODevice::Text) )
    {
        QMessageBox::warning(this,tr("File Error"),
            tr("Could not write to \"%1\".").arg(file.fileName()));
        return;
    }

    if ( filters.indexOf(dialog.selectedNameFilter()) == 1 ) // html
        file.write(output_console->document()->toHtml("utf-8").toUtf8());
    else
        file.write(output_console->toPlainText().toUtf8());

}

void ServerWidget::on_tabWidget_currentChanged(int tab)
{
    if ( tabWidget->widget(tab) == tab_cvars )
        ensure_has_cvars();
}

void ServerWidget::on_input_cvar_filter_section_currentIndexChanged(int index)
{
    proxy_cvar.setFilterKeyColumn(index);
}

void ServerWidget::on_input_console_lineExecuted(QString cmd)
{
    if ( input_expand_cvars->isChecked() )
    {
        /// \todo ensure cvars?
        static QRegExp regex_cvar_expansion(R"regex(\$(?:([^" $]+))|(?:\$\{([^" $]+)\s*\}))regex");
        int i = 0;
        while ( i < cmd.size() )
        {
            i = regex_cvar_expansion.indexIn(cmd, i);
            if ( i == -1 )
                break;
            /// \todo skip alias arguments
            QString cvar = regex_cvar_expansion.cap(1);
            if ( cvar.isEmpty() )
                cvar = regex_cvar_expansion.cap(2);
            QString value = model_cvar.cvar_value(cvar);
            cmd.replace(i, regex_cvar_expansion.matchedLength(), value);
            i += value.size();
        }
    }

    rcon_command(cmd);
}

void ServerWidget::cvarlist_apply_filter()
{
    if ( input_cvar_filter_regex->isChecked() )
        proxy_cvar.setFilterRegExp(input_cvar_filter_pattern->text());
    else
        proxy_cvar.setFilterFixedString(input_cvar_filter_pattern->text());
}

void ServerWidget::network_error_status(const QString& msg)
{
    xonotic_clear(); /// \todo option to disable clearing
    set_network_status(tr("Error: %1").arg(msg));
}

void ServerWidget::set_network_status(const QString& msg)
{
    label_connection->setText(msg);
    model_server.set_server_property("connection",msg);
}

void ServerWidget::request_status()
{
    for ( const auto& cmd : cmd_status )
        rcon_command(cmd);
    label_refresh_status->setText(QTime::currentTime().toString("hh:mm:ss"));
}

void ServerWidget::request_cvars()
{
    for ( const auto& cmd : cmd_cvars )
        rcon_command(cmd);
    label_refresh_cvar->setText(QTime::currentTime().toString("hh:mm:ss"));
}

QAbstractButton* ServerWidget::create_button(const PlayerAction& action,
                                             const xonotic::Player& player)
{
    auto button = new QToolButton();
    button->setIcon(action.icon());
    auto cmd = action.command(player);
    button->setToolTip(action.name().isEmpty() ? cmd : action.name());
    connect(button, &QPushButton::clicked, [this, cmd]{
        rcon_command(cmd);
        request_status();
    });
    return button;
}

void ServerWidget::ensure_has_cvars()
{
    // Arbitrary heuristic
    // should be loaded from settings and set an explicit flag on request_cvars()
    if ( model_cvar.rowCount() < 256 )
        request_cvars();
}
