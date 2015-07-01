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
#include "server_setup_table.hpp"
#include "settings.hpp"

ServerSetupTable::ServerSetupTable(QWidget* parent)
    : QScrollArea(parent)
{
    vbox_layout = new QVBoxLayout(this);
    vbox_layout->addStretch();
}

void ServerSetupTable::append(const xonotic::ConnectionDetails& server)
{
    auto wid = new InlineServerSetupWidget(server, this);
    wid->show_action_button(QIcon::fromTheme("list-remove"), tr("Remove"));
    connect(wid, &InlineServerSetupWidget::action_button_clicked, [wid,this]{
        widgets.removeOne(wid);
        wid->deleteLater();
    });
    vbox_layout->insertWidget(vbox_layout->count()-1, wid);
    widgets.push_back(wid);
}

void ServerSetupTable::clear()
{
    for ( const auto& wid : widgets )
        delete wid;
    widgets.clear();
}

void ServerSetupTable::load()
{
    clear();
    for ( const auto& sv : settings().saved_servers )
        append(sv);
}

void ServerSetupTable::save() const
{
    settings().saved_servers.clear();
    for (const auto& wid : widgets)
    {
        auto conn = wid->connection_details();
        settings().saved_servers.insert(QString::fromStdString(conn.name), conn);
    }
}

