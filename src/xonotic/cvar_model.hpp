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

#include "cvar.hpp"

namespace xonotic {

/**
 * \brief Model for the server status
 */
class CvarModel : public QAbstractTableModel
{
public:

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
                case 0: return QString::fromStdString(cvar.name);
                case 1: return QString::fromStdString(cvar.value);
                case 2: return QString::fromStdString(cvar.default_value);
                case 3: return QString::fromStdString(cvar.description);
            }
        }
        else if ( role == Qt::ToolTipRole || role == Qt::WhatsThisRole )
        {
            return QString::fromStdString(cvar.description);
        }
        else if ( role == Qt::EditRole && index.column() == 1 )
        {
            return QString::fromStdString(cvar.value);
        }

        return {};
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        if ( orientation == Qt::Horizontal && role == Qt::DisplayRole )
        {
            switch(section)
            {
                case 0: return tr("Name");
                case 1: return tr("Value");
                case 2: return tr("Default");
                case 3: return tr("Description");
            }
        }
        return {};
    }

    /**
     * \brief Get a cvar
     */
    const Cvar& server_property(const QString& name)
    {
        return cvars[name];
    }

public slots:
    /**
     * \brief Sets a server property
     */
    void set_cvar(const Cvar& cvar)
    {
        if ( cvar.name.empty() )
            return;

        QString name = QString::fromStdString(cvar.name);
        auto it = cvars.lowerBound(name);

        if ( it != cvars.end() && it.key() == name )
        {
            *it = cvar;
            int row = std::distance(cvars.begin(), it);
            if ( row != -1 )
                emit dataChanged(index(row,0), index(row, columnCount()-1));
        }
        else
        {
            int row = std::distance(cvars.begin(), it);
            beginInsertRows({},row,row);
            cvars.insert(it, name, cvar);
            endInsertRows();
        }
    }

    /**
     * \brief Removes all stored cvars
     */
    void clear()
    {
        beginResetModel();
        cvars.clear();
        endResetModel();
    }

private:
    QMap<QString, Cvar> cvars; ///< Cvar name -> info

};

} // namespace xonotic
#endif // XONOTIC_CVAR_MODEL_HPP
