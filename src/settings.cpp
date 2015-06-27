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
#include "settings.hpp"

Settings::Settings()
{
    load();
}

Settings::~Settings()
{
    save();
}

void Settings::load()
{
    QSettings settings;
    settings.beginGroup("servers");
    for ( const QString& key : settings.childGroups() )
    {
        settings.beginGroup(key);
        std::string host = settings.value("host").toString().toStdString();
        uint16_t port = settings.value("port").toUInt();
        std::string password = settings.value("password").toString().toStdString();
        int secure = qBound(0, settings.value("secure").toInt(), 2);
        saved_servers.insert(key,
            network::Xonotic(network::Server(host, port),
                            password,
                            network::Xonotic::Secure(secure),
                            key.toStdString()
        ));
        settings.endGroup();
    }
}

void Settings::save()
{
    QSettings settings;
    settings.beginGroup("servers");
    for ( const auto& server : saved_servers )
    {
        settings.beginGroup(QString::fromStdString(server.name));
        settings.setValue("host",QString::fromStdString(server.server.host));
        settings.setValue("port",uint(server.server.port));
        settings.setValue("password",QString::fromStdString(server.rcon_password));
        settings.setValue("secure",int(server.rcon_secure));
        settings.endGroup();
    }
}
