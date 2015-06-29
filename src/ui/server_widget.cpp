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

ServerWidget::ServerWidget(xonotic::ConnectionDetails details, QWidget* parent)
    : QWidget(parent), connection(std::move(details))
{
    setupUi(this);

    button_refresh_status->setShortcut(QKeySequence::Refresh);
    button_refresh_cvars->setShortcut(QKeySequence::Refresh);

    table_server_status->setModel(&model_server);
    connect(&log_parser, &xonotic::LogParser::server_property_changed,
            &model_server, &xonotic::ServerModel::set_server_property);

    table_cvars->setModel(&proxy_cvar);
    proxy_cvar.setSourceModel(&model_cvar);
    connect(&log_parser, &xonotic::LogParser::cvar,
            &model_cvar, &xonotic::CvarModel::set_cvar);
    auto header_view = table_cvars->horizontalHeader();
    header_view->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header_view->setSectionResizeMode(1, QHeaderView::Stretch);
    table_cvars->hideColumn(2);
    table_cvars->hideColumn(3);
    for ( int i = 0; i < model_cvar.columnCount(); i++ )
        input_cvar_filter_section->addItem(
            model_cvar.headerData(i,Qt::Horizontal).toString());

    table_players->setModel(&model_player);
    connect(&log_parser, &xonotic::LogParser::players_changed,
            &model_player, &xonotic::PlayerModel::set_players,
            Qt::QueuedConnection);
    header_view = table_players->horizontalHeader();
    header_view->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header_view->setSectionResizeMode(1, QHeaderView::Stretch);
    header_view->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    header_view->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    header_view->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    header_view->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    header_view->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    connect(&model_player, &xonotic::PlayerModel::players_changed,
        [this](const std::vector<xonotic::Player>& players) {
            for ( unsigned i = 0; i < players.size(); i++ )
            {
                QDialogButtonBox *buttons = new QDialogButtonBox();
                for ( const auto& action : settings().player_actions )
                    buttons->addButton(create_button(action,players[i]),
                                       QDialogButtonBox::ActionRole);
                auto index = model_player.index(i, xonotic::PlayerModel::Actions);
                table_players->setIndexWidget(index, buttons);
            }
        });

    connect(action_clear_log, &QAction::triggered, this, &ServerWidget::clear_log);

    input_console->setFont(settings().console_font);
    input_console->setHistory(settings().get_history(connection.details().name));

    cmd_status = settings().get("behaviour/cmd_status", cmd_status);
    cmd_cvars = settings().get("behaviour/cmd_cvars", cmd_cvars);

    clear_log();

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

void ServerWidget::xonotic_log(const QString& log)
{
    log_parser.parse(log.toStdString());

    auto scrollbar = output_console->verticalScrollBar();
    bool scroll = scrollbar->value() == scrollbar->maximum();

    if ( !action_parse_colors->isChecked() )
    {
        output_console->append(log);
    }
    else
    {
        QTextCursor cursor(output_console->document());
        cursor.movePosition(QTextCursor::End);
        QTextCharFormat format;
        QString text;
        static QRegExp regex_xoncolor("^([0-9]|x[0-9a-fA-F]{3})");
        format.setForeground(settings().console_foreground);
        for ( int i = 0; i < log.size(); i++ )
        {
            if ( log[i] == '^' && i < log.size()-1 )
            {
                i++;
                if ( log[i] == '^' )
                {
                    text += '^';
                }
                else if ( regex_xoncolor.indexIn(log, i, QRegExp::CaretAtOffset) != -1 )
                {
                    cursor.insertText(text,format);
                    i += regex_xoncolor.matchedLength()-1;
                    auto color = xonotic_color(regex_xoncolor.cap());
                    format.setForeground(QColor::fromHsl(
                        color.hue(),
                        color.saturation(),
                        qBound(settings().console_brightness_min,
                               color.lightness(),
                               settings().console_brightness_max)
                    ));
                    text.clear();
                }
                else
                {
                    text += '^';
                    text += log[1];
                }
            }
            /// \todo qfont
            else
            {
                text += log[i];
            }
        }
        cursor.insertText(text,format);
        format.setForeground(settings().console_foreground);
        cursor.insertText("\n",format);
    }

    if ( scroll )
        scrollbar->setValue(scrollbar->maximum());
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

/// \todo Move out
QColor ServerWidget::xonotic_color(const QString& s)
{
    if ( s.size() == 4 )
    {
        return QColor(
            hex_to_int(s[1].unicode())*255/15,
            hex_to_int(s[2].unicode())*255/15,
            hex_to_int(s[3].unicode())*255/15
        );
    }

    if ( s.size() != 1 )
        return Qt::gray;

    switch ( s[0].unicode() )
    {
        case '0': return Qt::black;
        case '1': return Qt::red;
        case '2': return Qt::green;
        case '3': return Qt::yellow;
        case '4': return Qt::blue;
        case '5': return Qt::cyan;
        case '6': return Qt::magenta;
        case '7': return Qt::white;
        case '8': return Qt::darkGray;
        case '9': return Qt::gray;
    }

    return Qt::gray;
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

void ServerWidget::cvarlist_apply_filter()
{
    proxy_cvar.setFilterKeyColumn(input_cvar_filter_section->currentIndex());
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
    model_cvar.clear();
    for ( const auto& cmd : cmd_cvars )
        rcon_command(cmd);
    label_refresh_cvar->setText(QTime::currentTime().toString("hh:mm:ss"));
}

QAbstractButton* ServerWidget::create_button(const xonotic::PlayerAction& action,
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
