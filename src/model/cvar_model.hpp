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
#ifndef XONOTIC_CVAR_MODEL_HPP
#define XONOTIC_CVAR_MODEL_HPP

#include <iterator>

#include <QAbstractTableModel>
#include <QMap>

#include "xonotic/cvar.hpp"

/**
 * \brief Model for the server status
 */
class CvarModel : public QAbstractTableModel
{
public:
    enum ColumnNames {
        Name        = 0,
        Value       = 1,
        Default     = 2,
        Description = 3,
    };

    int rowCount(const QModelIndex & = {}) const override
    {
        return cvars.size();
    }

    int columnCount(const QModelIndex & = {}) const override
    {
        return 4;
    }

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override
    {
        if ( index.row() < 0 || index.row() >= cvars.size() )
            return {};

        auto it = cvars.begin();
        std::advance(it, index.row());
        const auto& cvar = *it;
        if ( role == Qt::DisplayRole )
        {
            switch(index.column())
            {
                case Name       : return cvar.name;
                case Value      : return cvar.value;
                case Default    : return cvar.default_value;
                case Description: return cvar.description;
            }
        }
        else if ( role == Qt::ToolTipRole || role == Qt::WhatsThisRole )
        {
            return cvar.description;
        }
        else if ( role == Qt::EditRole && index.column() == Value )
        {
            return cvar.value;
        }
        else if ( role == Qt::UserRole && index.column() == Value )
        {
            return cvar.name;
        }

        return {};
    }

    Qt::ItemFlags flags(const QModelIndex &index) const override
    {
        auto flags = QAbstractTableModel::flags(index);

        if ( index.column() == Value && index.row() >= 0 && index.row() < cvars.size() )
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
                case Value      : return tr("Value");
                case Default    : return tr("Default");
                case Description: return tr("Description");
            }
        }
        return {};
    }

    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex())
    {
        if ( row < 0 || row >= cvars.size() || count <= 0 || count >= cvars.size()-row )
            return false;

        auto begin = cvars.begin();
        std::advance(begin, row);

        beginRemoveRows(parent, row, row+count-1);
        // no range erase...
        for ( int i = 0; i < count; i++ )
            begin = cvars.erase(begin);
        endRemoveRows();

        return true;
    }

    /**
     * \brief Get a cvar
     */
    xonotic::Cvar cvar(const QString& name) const
    {
        return cvars[name];
    }

    /**
     * \brief Get a cvar
     */
    xonotic::Cvar cvar_at(int row) const
    {
        if ( row < 0 || row >= cvars.size() )
            return {};
        auto it = cvars.begin();
        std::advance(it, row);
        return *it;
    }

    /**
     * \brief Returns the value of a cvar as a QString
     */
    QString cvar_value(const QString& name) const
    {
        return cvars[name].value;
    }


public slots:
    /**
     * \brief Sets a cvars
     */
    void set_cvar(const xonotic::Cvar& cvar)
    {
        if ( cvar.name.isEmpty() )
            return;

        if ( !no_update ) beginResetModel();
        cvars[cvar.name] = cvar;
        if ( !no_update ) endResetModel();
    }

    /**
     * \brief Removes all stored cvars
     */
    void clear()
    {
        unblock_updates();
        beginResetModel();
        cvars.clear();
        endResetModel();
    }

    /**
     * \brief Temporarily block model change signals
     */
    void block_updates()
    {
        no_update = true;
    }

    /**
     * \brief Unblocks model change signals
     */
    void unblock_updates()
    {
        if ( no_update )
        {
            beginResetModel();
            endResetModel();
        }
        no_update = false;
    }

private:
    QMap<QString, xonotic::Cvar> cvars; ///< Cvar name -> info
    bool no_update = false;

};

#endif // XONOTIC_CVAR_MODEL_HPP
