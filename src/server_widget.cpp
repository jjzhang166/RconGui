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

#include "server_widget.hpp"
#include "create_server_dialog.hpp"
#include <QMenu>

ServerWidget::ServerWidget(network::Xonotic xonotic, QWidget* parent)
    : QWidget(parent), xonotic(std::move(xonotic))
{
    setupUi(this);

    io.max_datagram_size(1400);
    io.on_error = [this](const std::string& msg)
    {
        emit network_error(QString::fromStdString(msg));
        xonotic_close_connection();
    };
    io.on_async_receive = [this](const std::string& datagram)
    {
        output_console->append(QString::fromStdString(datagram));
    };
    io.on_failure = [this]()
    {
        xonotic_disconnect();
    };

    xonotic_connect();
}

ServerWidget::~ServerWidget()
{
    xonotic_disconnect();
}

void ServerWidget::xonotic_close_connection()
{

    if ( io.connected() )
        io.disconnect();
    if ( thread_input.joinable() &&
            thread_input.get_id() != std::this_thread::get_id() )
        thread_input.join();
}

void ServerWidget::xonotic_disconnect()
{
    // if ( io.connected() )
        // clear log_dest_udp if needed
    xonotic_close_connection();
    xonotic_clear();
}

bool ServerWidget::xonotic_connect()
{
    if ( !io.connected() )
    {
        if ( io.connect(xonotic.server) )
        {
            if ( thread_input.get_id() == std::this_thread::get_id() )
                return false;

            if ( thread_input.joinable() )
                thread_input.join();

            thread_input = std::move(std::thread([this]{
                xonotic_clear();
                xonotic_request_status();
                io.run_input();
            }));
        }
    }

    return io.connected();
}

void ServerWidget::xonotic_clear()
{
    // Lock lock(mutex);
    // clear rcon secure 2 buffer
    // clear status and cvars
    // clear cvars
}

void ServerWidget::xonotic_request_status()
{
    rcon_command("status 1");
}

bool ServerWidget::xonotic_connected()
{
    return io.connected();
}

QString ServerWidget::name() const
{
    return QString::fromStdString(xonotic.name);
}

void ServerWidget::on_button_setup_clicked()
{
    CreateServerDialog dlg(xonotic, this);
    if ( dlg.exec() )
    {
        auto oldxon = xonotic;
        xonotic = dlg.connection_info();

        if ( oldxon.server != xonotic.server )
        {
            xonotic_disconnect();
            xonotic_connect();
        }

        if ( xonotic.name != oldxon.name )
            emit name_changed(QString::fromStdString(xonotic.name));
    }
}

void ServerWidget::on_output_console_customContextMenuRequested(const QPoint &pos)
{
    QMenu* menu = output_console->createStandardContextMenu();
    menu->addSeparator();
    menu->addAction(action_Clear);
    menu->addSeparator();
    menu->addAction(action_Attach_Log);
    menu->addAction(action_Detach_Log);
    menu->exec(output_console->mapToGlobal(pos));
}

void ServerWidget::on_button_send_clicked()
{
    rcon_command(input_console->text());
}

void ServerWidget::rcon_command(const QString& command)
{
    xonotic_write("rcon "+xonotic.rcon_password+' '+command.toStdString());
}

void ServerWidget::xonotic_write(std::string line)
{
    line.erase(std::remove_if(line.begin(), line.end(),
        [](char c){return c == '\n' || c == '\0' || c == '\xff';}),
        line.end());

    io.write(header+line);
}

