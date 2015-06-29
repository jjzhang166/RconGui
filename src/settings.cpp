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
    settings.beginGroup("servers");
    for ( const QString& key : settings.childGroups() )
    {
        settings.beginGroup(key);
        std::string host = settings.value("host").toString().toStdString();
        uint16_t port = settings.value("port").toUInt();
        std::string password = settings.value("password").toString().toStdString();
        int secure = qBound(0, settings.value("secure").toInt(), 2);
        saved_servers.insert(key,
            xonotic::ConnectionDetails(network::Server(host, port),
                            password,
                            xonotic::ConnectionDetails::Secure(secure),
                            key.toStdString()
        ));
        auto it = console_history.find(key);
        if ( it == console_history.end() )
            console_history.insert(key, settings.value("console_history").toStringList());
        else
            *it = settings.value("console_history", *it).toStringList();
        settings.endGroup();
    }
    settings.endGroup();

    settings.beginGroup("console");
    console_foreground = settings.value("foreground",console_foreground.name()).toString();
    console_background = settings.value("background",console_background.name()).toString();
    console_brightness_max = qBound(0,settings.value("brightness_max",console_brightness_max).toInt(),255);
    console_brightness_min = qBound(0,settings.value("brightness_min",console_brightness_min).toInt(),255);
    console_font.fromString(settings.value("font",console_font.toString()).toString());
    settings.endGroup();

    settings.beginGroup("behaviour");
    if ( settings.childGroups().contains("player") )
    {
        int sz = settings.beginReadArray("player");
        player_actions.clear();
        player_actions.reserve(sz);
        for ( int i = 0; i < sz; i++ )
        {
            settings.setArrayIndex(i);
            QString label = settings.value("label").toString();
            QString command = settings.value("command").toString();
            QString icon = settings.value("icon").toString();
            if ( !command.isEmpty() )
                player_actions.push_back({label, command, icon});
        }
        settings.endArray();
    }
    settings.endGroup();
}

void Settings::save()
{
    settings.beginGroup("servers");
    settings.remove("");
    for ( const auto& server : saved_servers )
    {
        settings.beginGroup(QString::fromStdString(server.name));
        settings.setValue("host",QString::fromStdString(server.server.host));
        settings.setValue("port",uint(server.server.port));
        settings.setValue("password",QString::fromStdString(server.rcon_password));
        settings.setValue("secure",int(server.rcon_secure));
        settings.setValue("console_history",get_history(server.name));
        settings.endGroup();
    }
    settings.endGroup();

    settings.beginGroup("console");
    settings.setValue("foreground",console_foreground.name());
    settings.setValue("background",console_background.name());
    settings.setValue("brightness_max",console_brightness_max);
    settings.setValue("brightness_min",console_brightness_min);
    settings.setValue("font",console_font.toString());
    settings.endGroup();

    settings.beginGroup("behaviour");
    settings.beginWriteArray("player");
    for ( int i = 0; i < player_actions.size(); i++ )
    {
        settings.setArrayIndex(i);
        settings.setValue("label",player_actions[i].name());
        settings.setValue("command",player_actions[i].command());
        settings.setValue("icon",player_actions[i].icon_name());
    }
    settings.endArray();
    settings.endGroup();
}

QStringList Settings::get_history(const std::string& server) const
{
    auto it = console_history.find(QString::fromStdString(server));
    if ( it == console_history.end() )
        return {};
    return *it;
}

void Settings::set_history(const std::string& server, const QStringList& history)
{
    console_history[QString::fromStdString(server)] = history;
}
