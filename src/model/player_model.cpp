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
#include "player_model.hpp"

#include <QSize>
#include <QStyleOptionViewItem>
#include <QApplication>

QVariant PlayerModel::data(const QModelIndex & index, int role) const
{
    if ( index.row() < 0 || index.row() >= int(players.size()) )
        return {};

    const auto& player = players[index.row()];
    if ( role == Qt::DisplayRole || role == Qt::ToolTipRole )
    {
        switch(index.column())
        {
            case Ip:    return QString::fromStdString(player.ip);
            case Name:  return QString::fromStdString(player.name);
            case Entity:return QString::fromStdString(player.no);
            case Ping:  return QString::fromStdString(player.ping);
            case Pl:    return QString::fromStdString(player.pl);
            case Score: return QString::fromStdString(player.frags);
            case Time:  return QString::fromStdString(player.time);
        }
    }
    else if ( role == Qt::TextAlignmentRole )
    {
        if ( index.column() >= Entity && index.column() <= Time )
            return Qt::AlignCenter;
    }

    return {};
}

QVariant PlayerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if ( orientation != Qt::Horizontal )
        return {};

    if ( role == Qt::DisplayRole )
    {
        switch(section)
        {
            case Ip:     return tr("IP");
            case Name:   return tr("Name");
            case Entity: return tr("#");
            case Ping:   return tr("Ping");
            case Pl:     return tr("PL");
            case Score:  return tr("Score");
            case Time:   return tr("Time");
            case Actions:return tr("Actions");
        }
    }
    else if ( role == Qt::ToolTipRole || role == Qt::WhatsThisRole )
    {
        switch(section)
        {
            case Ip:     return tr("IP Address");
            case Name:   return tr("Name");
            case Entity: return tr("Entity number");
            case Ping:   return tr("Ping");
            case Pl:     return tr("Packet Loss");
            case Score:  return tr("Score");
            case Time:   return tr("Time");
            case Actions:return tr("Actions");
        }
    }
    else if ( role == Qt::SizeHintRole && section >= Entity )
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
