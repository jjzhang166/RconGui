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
#ifndef QUICK_COMMANDS_MODEL_HPP
#define QUICK_COMMANDS_MODEL_HPP

#include <QAbstractTableModel>
#include <QList>

#include "settings.hpp"

/**
 * \brief Model for quick commands
 */
class QuickCommandsModel : public QAbstractTableModel
{
private:
    using CmdList = std::vector<QPair<QString, QString>>;

public:
    enum ColumnNames {
        Name,
        Command,

        Columns
    };

    int rowCount(const QModelIndex & = {}) const override
    {
        return cmdlist.size();
    }

    int columnCount(const QModelIndex & = {}) const override
    {
        return Columns;
    }

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override
    {
        if ( index.row() < 0 || index.row() >= int(cmdlist.size()) )
            return {};

        auto cmd = cmdlist[index.row()];
        if ( role == Qt::DisplayRole || role == Qt::EditRole )
        {
            switch(index.column())
            {
                case Name       : return cmd.first;
                case Command    : return cmd.second;
            }
        }

        return {};
    }

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override
    {
        if ( index.row() < 0 || index.row() >= int(cmdlist.size()) ||
                !value.canConvert<QString>() || role != Qt::EditRole )
            return false;

        QString newvalue = value.toString();

        if ( newvalue.isEmpty() )
        {
            beginRemoveRows(index.parent(), index.row(), index.row());
            cmdlist.erase(cmdlist.begin()+index.row());
            endRemoveRows();
            return true;
        }

        auto& cmd = cmdlist[index.row()];

        if ( index.column() == Name )
        {
            unique_name(newvalue, index.row());
            cmd.first = newvalue;
            return true;
        }
        else if ( index.column() == Command )
        {
            cmd.second = newvalue;
            return true;
        }

        return false;
    }

    Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        auto flags = QAbstractTableModel::flags(index);

        if ( index.row() >= 0 && index.row() < int(cmdlist.size()) )
            flags |= Qt::ItemIsEditable;

        return flags;
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        if ( orientation == Qt::Horizontal && role == Qt::DisplayRole )
        {
            switch(section)
            {
                case Name       : return tr("Name");
                case Command    : return tr("Command");
            }
        }
        return {};
    }

    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override
    {
        beginInsertRows(parent, row, row+count-1);
        cmdlist.insert(cmdlist.begin()+row, count, {});
        endInsertRows();
        return true;
    }

    /**
     * \brief Returns the command with the given name
     */
    QString command(const QString& name) const
    {
        auto it = find(name);
        if ( it == cmdlist.end() )
            return {};
        return it->second;
    }

public slots:
    /**
     * \brief Adds a commands
     * \returns Whether the command was inserted successfully
     */
    bool add_command(const QString& name, const QString& value)
    {
        if ( name.isEmpty() || value.isEmpty() || contains(name) )
            return false;

        beginInsertRows({}, cmdlist.size(), cmdlist.size());
        cmdlist.emplace_back(name, value);
        endInsertRows();

        return true;
    }

    bool contains(const QString& name) const
    {
        return find(name) != cmdlist.end();
    }

    /**
     * \brief Removes all stored commands
     */
    void clear()
    {
        beginResetModel();
        cmdlist.clear();
        endResetModel();
    }

    /**
     * \brief Save to settings
     */
    void save()
    {
        settings().quick_commands = cmdlist;
    }

    /**
     * \brief Load from settings
     */
    void load()
    {
        beginResetModel();
        cmdlist = settings().quick_commands;
        endResetModel();
    }

private:
    CmdList::iterator find(const QString& name)
    {
        return std::find_if(cmdlist.begin(), cmdlist.end(),
            [&name](CmdList::const_reference cmd) { return cmd.first == name; });
    }

    CmdList::const_iterator find(const QString& name) const
    {
        return std::find_if(cmdlist.begin(), cmdlist.end(),
            [&name](CmdList::const_reference cmd) { return cmd.first == name; });
    }

    void unique_name(QString& name, int skip)
    {
        for ( int i = 0; i < int(cmdlist.size()); i++ )
        {
            if ( i != skip && name == cmdlist[i].first )
            {
                name += "_";
                unique_name(name, skip);
                return;
            }
        }
    }

    CmdList cmdlist; ///< List of pairs (name-value)

};

#endif // QUICK_COMMANDS_MODEL_HPP
