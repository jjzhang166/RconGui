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
#ifndef PLAYER_ACTION_MODEL_HPP
#define PLAYER_ACTION_MODEL_HPP

#include "player_action.hpp"
#include <QAbstractTableModel>
#include <QFont>

/**
 * \brief Model for the player actions status
 */
class PlayerActionModel : public QAbstractTableModel
{
public:
    enum ColumnNames {
        Name,
        Command,
        Icon,

        Columns
    };

    int rowCount(const QModelIndex & = {}) const override
    {
        return player_actions.size();
    }

    int columnCount(const QModelIndex & = {}) const override
    {
        return Columns;
    }

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override
    {
        if ( index.row() < 0 || index.row() >= player_actions.size() )
            return {};

        const auto& action = player_actions[index.row()];
        if ( role == Qt::DisplayRole || role == Qt::ToolTipRole || role == Qt::EditRole )
        {
            switch(index.column())
            {
                case Name:      return action.name();
                case Command:   return action.command();
                case Icon:      return action.icon_name();
            }
        }
        else if ( role == Qt::DecorationRole && index.column() == Icon )
        {
            return action.icon();
        }
        else if ( role == Qt::FontRole && index.column() == Command )
        {
            return QFont("monospace");
        }

        return {};
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        if ( orientation != Qt::Horizontal )
            return {};

        if ( role == Qt::DisplayRole )
        {
            switch(section)
            {
                case Name:    return tr("Name");
                case Command: return tr("Command");
                case Icon:    return tr("Icon");
            }
        }

        return {};
    }

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override
    {
        if ( index.row() < 0 || index.row() >= player_actions.size() ||
            role != Qt::EditRole || !value.canConvert<QString>() )
            return false;

        auto& action = player_actions[index.row()];
        QString val = value.toString();

        if ( val.isEmpty() && index.column() < Icon )
        {
            beginRemoveRows(index.parent(), index.row(), index.row());
            player_actions.removeAt(index.row());
            endRemoveRows();
            return true;
        }

        switch(index.column())
        {
            case Name:
                action.set_name(val);
                return true;
            case Command:
                action.set_command(val);
                return true;
            case Icon:
                action.set_icon(val);
                return true;
        }

        return false;
    }

    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override
    {
        beginInsertRows(parent, row, row+count-1);
        for ( int i = 0; i < count; i++ )
            player_actions.insert(row, PlayerAction(tr("New Action"),""));
        endInsertRows();
        return true;
    }

    Qt::ItemFlags flags(const QModelIndex& index) const override
    {
        auto flags = QAbstractTableModel::flags(index);

        if ( index.row() >= 0 && index.row() < player_actions.size() && index.column() < Columns )
            flags |= Qt::ItemIsEditable;

        return flags;
    }

    /**
     * \brief Returns the player actions
     */
    const QList<PlayerAction>& actions() const
    {
        return player_actions;
    }

public slots:
    /**
     * \brief Sets the player actions
     */
    void set_actions(const QList<PlayerAction>& player_actions)
    {
        beginResetModel();
        this->player_actions = player_actions;
        endResetModel();
    }

    /**
     * \brief Removes all stored player actions
     */
    void clear()
    {
        beginResetModel();
        player_actions.clear();
        endResetModel();
    }

private:
    QList<PlayerAction> player_actions; ///< list of players

};

#endif // PLAYER_ACTION_MODEL_HPP
