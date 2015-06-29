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

namespace xonotic {


QVariant PlayerModel::data(const QModelIndex & index, int role) const
{
    if ( index.row() < 0 || index.row() >= int(players.size()) )
        return {};

    const auto& player = players[index.row()];
    if ( role == Qt::DisplayRole || role == Qt::ToolTipRole )
    {
        switch(index.column())
        {
            case 0: return QString::fromStdString(player.ip);
            case 1: return QString::fromStdString(player.name);
            case 2: return QString::fromStdString(player.no);
            case 3: return QString::fromStdString(player.ping);
            case 4: return QString::fromStdString(player.pl);
            case 5: return QString::fromStdString(player.frags);
            case 6: return QString::fromStdString(player.time);
        }
    }
    else if ( role == Qt::TextAlignmentRole )
    {
        if ( index.column() >= 2 && index.column() <= 6 )
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
            case 0: return tr("IP");
            case 1: return tr("Name");
            case 2: return tr("#");
            case 3: return tr("Ping");
            case 4: return tr("PL");
            case 5: return tr("Score");
            case 6: return tr("Time");
            case 7: return tr("Actions");
        }
    }
    else if ( role == Qt::ToolTipRole || role == Qt::WhatsThisRole )
    {
        switch(section)
        {
            case 0: return tr("IP Address");
            case 1: return tr("Name");
            case 2: return tr("Entity number");
            case 3: return tr("Ping");
            case 4: return tr("Packet Loss");
            case 5: return tr("Score");
            case 6: return tr("Time");
            case 7: return tr("Actions");
        }
    }
    else if ( role == Qt::SizeHintRole && section >= 2 )
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

} // namespace xonotic
