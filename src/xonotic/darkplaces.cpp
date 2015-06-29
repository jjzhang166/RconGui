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
#include "darkplaces.hpp"
#include <QMessageAuthenticationCode>

static std::string hmac_md4(const std::string& input, const std::string& key)
{
    QMessageAuthenticationCode code(QCryptographicHash::Md4);
    code.setKey(QByteArray(key.data(), key.size()));
    code.addData(input.data(), input.size());
    auto output = code.result();
    return std::string(output.data(), output.size());
}

namespace xonotic {

Darkplaces::Darkplaces(ConnectionDetails connection_details)
    : connection_details(std::move(connection_details))
{
    io.max_datagram_size(1400);
    io.on_error = [this](const std::string& msg)
    {
        on_network_error(msg);
        close_connection();
    };
    io.on_async_receive = [this](const std::string& datagram)
    {
        read(datagram);
    };
    io.on_failure = [this]()
    {
        disconnect();
    };
}

Darkplaces::~Darkplaces()
{
    if ( connected() )
    {
        disconnect();
    }
    else
    {
        // In case the connection failed
        if ( thread_input.joinable() )
            thread_input.join();
    }
}

void Darkplaces::close_connection()
{
    io.disconnect();
    if ( thread_input.joinable() &&
            thread_input.get_id() != std::this_thread::get_id() )
        thread_input.join();
}

void Darkplaces::disconnect()
{
    on_disconnecting();
    close_connection();
    clear();
    on_disconnect();
}

bool Darkplaces::connect()
{
    if ( !io.connected() )
    {
        if ( io.connect(connection_details.server) )
        {
            if ( thread_input.get_id() == std::this_thread::get_id() )
                return false;

            if ( thread_input.joinable() )
                thread_input.join();

            thread_input = std::move(std::thread([this]{
                clear();
                io.run_input();
            }));

            on_connect();
        }
    }

    return io.connected();
}

bool Darkplaces::reconnect()
{
    disconnect();
    return connect();
}

void Darkplaces::clear()
{
    // Lock lock(mutex);
    // clear rcon secure 2 buffer
}

bool Darkplaces::connected() const
{
    return io.connected();
}

void Darkplaces::read(const std::string& datagram)
{
    if ( datagram.size() < 5 || datagram.substr(0,4) != "\xff\xff\xff\xff" )
    {
        on_network_error("Invalid datagram: "+datagram);
        return;
    }

    // non-log/rcon output
    if ( datagram[4] != 'n' )
    {
        auto space = std::find(datagram.begin(), datagram.end(), ' ');
        std::string command(datagram.begin()+4, space);
        std::string message;
        if ( space != datagram.end() )
            message = std::string(space+1, datagram.end());
        on_receive(command, message);
        return;
    }

    Lock lock(mutex);
    std::istringstream socket_stream(line_buffer+datagram.substr(5));
    line_buffer.clear();
    lock.unlock();

    // convert the datagram into lines
    std::string line;
    while (socket_stream)
    {
        std::getline(socket_stream,line);
        if (socket_stream.eof())
        {
            if (!line.empty())
            {
                lock.lock();
                line_buffer = line;
                lock.unlock();
            }
            break;
        }
        on_receive_log(line);
    }
}

void Darkplaces::set_details(const ConnectionDetails& details)
{
    bool should_reconnect = details.server != connection_details.server;
    connection_details = details;
    if ( should_reconnect )
        reconnect();
}

void Darkplaces::rcon_command(std::string command)
{
    command.erase(std::remove_if(command.begin(), command.end(),
        [](char c){return c == '\n' || c == '\0' || c == '\xff';}),
        command.end());

    /// \todo rcon_secure 2
    if ( connection_details.rcon_secure == xonotic::ConnectionDetails::NO )
    {
        write("rcon "+connection_details.rcon_password+' '+command);
    }
    else if ( connection_details.rcon_secure == xonotic::ConnectionDetails::TIME )
    {
        auto message = std::to_string(std::time(nullptr))+".000000 "+command;
        write("srcon HMAC-MD4 TIME "+
            hmac_md4(message, connection_details.rcon_password)
            +' '+message);
    }
}

void Darkplaces::write(std::string line)
{
    io.write(header+line);
}

network::Server Darkplaces::local_endpoint() const
{
    return io.local_endpoint();
}

} // namespace xonotic
