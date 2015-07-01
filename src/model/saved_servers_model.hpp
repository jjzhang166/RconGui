/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \section License
 *
 * Copyright (C)  Mattia Basaglia
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
#ifndef SAVED_SERVERS_MODEL_HPP
#define SAVED_SERVERS_MODEL_HPP

#include <iterator>

#include <QAbstractTableModel>
#include <QSize>
#include <QStyleOptionViewItem>
#include <QApplication>

#include "settings.hpp"

/**
 * \brief Model for the servers saved in the configuration
 */
class SavedServersModel : public QAbstractTableModel
{
public:
    enum ColumnNames {
        Name    = 0,
        Host    = 1,
        Port    = 2,
        Password= 3,
        Secure  = 4,
        Delete  = 5,
    };

    int rowCount(const QModelIndex & = {}) const override
    {
        return settings().saved_servers.size();
    }

    int columnCount(const QModelIndex & = {}) const override
    {
        return 6;
    }

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override
    {
        /// \todo Don't edit in-place as it should wait for the settings dialog
        /// to be accepted before committing changes,
        /// perhaps store the servers locally in a list and copy them over
        /// once done.

        if ( index.row() < 0 || index.row() >= settings().saved_servers.size() )
            return {};

        auto it = settings().saved_servers.begin();
        std::advance(it, index.row());
        const auto& server = *it;
        if ( role == Qt::DisplayRole || role == Qt::EditRole )
        {
            switch(index.column())
            {
                case Name       : return QString::fromStdString(server.name);
                case Host       : return QString::fromStdString(server.server.host);
                case Port       : return server.server.port;
                case Password   : return QString::fromStdString(server.rcon_password);
                case Secure     : return int(server.rcon_secure);
            }
        }
        else if ( role == Qt::UserRole )
        {
            return QString::fromStdString(server.name);
        }
        else if ( role == Qt::ToolTipRole || role == Qt::WhatsThisRole )
        {
            switch(index.column())
            {
                case Name       : return tr("Name used to identify the server");
                case Host       : return tr("Connection host name or address");
                case Port       : return tr("Connection UDP port");
                case Password   : return tr("Rcon password");
                case Secure     : return tr("Rcon secure protocol");
            }
        }

        return {};
    }

    Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        auto flags = QAbstractTableModel::flags(index);
        if ( index.column() != Delete )
            flags |= Qt::ItemIsEditable;
        return flags;
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        if ( orientation == Qt::Vertical )
            return {};

        if ( role == Qt::DisplayRole )
        {
            switch(section)
            {
                case Name       : return tr("Name");
                case Host       : return tr("Host");
                case Port       : return tr("Port");
                case Password   : return tr("Password");
                case Secure     : return tr("Secure");
                case Delete     : return tr("Delete");
            }
        }
        else if ( role == Qt::SizeHintRole )
        {
            // This ensures that the columns are just right and not too wide
            QStyleOptionViewItem option;
            option.features |= QStyleOptionViewItem::HasDisplay;
            option.text = headerData(section, orientation, Qt::DisplayRole).toString();
            option.styleObject = 0;
            return QApplication::style()->sizeFromContents(QStyle::CT_ItemViewItem, &option, QSize(), nullptr);
        }
        return {};
    }

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override
    {
        if ( role != Qt::EditRole || index.row() < 0 ||
                index.row() >= settings().saved_servers.size() )
            return false;

        auto it = settings().saved_servers.begin();
        std::advance(it, index.row());
        auto& server = *it;

        if ( index.column() == Name && value.canConvert<QString>() )
        {
            QString name = value.toString();
            if ( name.isEmpty() )
                name = QString::fromStdString(server.server.name());
            if ( name.toStdString() != server.name )
            {
                server.name = name.toStdString();
                return true;
            }
        }
        else if ( index.column() == Host && value.canConvert<QString>() &&
            server.server.host != value.toString().toStdString() )
        {
            bool rename = server.name == server.server.name();
            server.server.host = value.toString().toStdString();
            if ( rename )
                server.name = server.server.name();
            return true;
        }
        else if ( index.column() == Port && value.canConvert<uint16_t>() &&
            server.server.port != value.value<uint16_t>() )
        {
            bool rename = server.name == server.server.name();
            server.server.port = value.value<uint16_t>();
            if ( rename )
                server.name = server.server.name();
            return true;
        }
        else if ( index.column() == Password && value.canConvert<QString>() &&
            server.rcon_password != value.toString().toStdString() )
        {
            server.rcon_password = value.toString().toStdString();
            return true;
        }
        else if ( index.column() == Secure && value.canConvert<int>() &&
            server.rcon_secure != value.toInt() &&
            value.toInt() >= 0 && value.toInt() <= 2 )
        {
            server.rcon_secure = xonotic::ConnectionDetails::Secure(value.toInt());
            return true;
        }
        return false;
    }

};

#endif // SAVED_SERVERS_MODEL_HPP
