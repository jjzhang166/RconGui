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
                case Name       : return QString::fromStdString(cvar.name);
                case Value      : return QString::fromStdString(cvar.value);
                case Default    : return QString::fromStdString(cvar.default_value);
                case Description: return QString::fromStdString(cvar.description);
            }
        }
        else if ( role == Qt::ToolTipRole || role == Qt::WhatsThisRole )
        {
            return QString::fromStdString(cvar.description);
        }
        else if ( role == Qt::EditRole && index.column() == Value )
        {
            return QString::fromStdString(cvar.value);
        }
        else if ( role == Qt::UserRole && index.column() == Value )
        {
            return QString::fromStdString(cvar.name);
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

    /**
     * \brief Get a cvar
     */
    xonotic::Cvar cvar(const QString& name) const
    {
        return cvars[name];
    }

    /**
     * \brief Returns the value of a cvar as a QString
     */
    QString cvar_value(const QString& name) const
    {
        return QString::fromStdString(cvars[name].value);
    }


public slots:
    /**
     * \brief Sets a server property
     */
    void set_cvar(const xonotic::Cvar& cvar)
    {
        if ( cvar.name.empty() )
            return;

        if ( !no_update ) beginResetModel();
        cvars[QString::fromStdString(cvar.name)] = cvar;
        if ( !no_update ) endResetModel();

        /*QString name = QString::fromStdString(cvar.name);
        auto it = cvars.lowerBound(name);

        if ( it != cvars.end() && it.key() == name )
        {
            *it = cvar;
            if ( !no_update )
            {
                int row = std::distance(cvars.begin(), it);
                if ( row != -1 )
                    emit dataChanged(index(row,0), index(row, columnCount()-1));
            }
        }
        else
        {
            if ( !no_update )
            {
                int row = std::distance(cvars.begin(), it);
                beginInsertRows({},row,row);
            }
            cvars.insert(it, name, cvar);
            if ( !no_update )
                endInsertRows();
        }*/
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
