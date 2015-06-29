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
#ifndef XONOTIC_SERVER_MODEL_HPP
#define XONOTIC_SERVER_MODEL_HPP

#include <QAbstractListModel>
#include <QHash>

/**
 * \brief Model for the server status
 */
class ServerModel : public QAbstractListModel
{
public:

    int rowCount(const QModelIndex & = {}) const override
    {
        return display_property.size();
    }

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override
    {
        if ( accept(index.row()) && role == Qt::DisplayRole )
            return properties[display_property[index.row()]].second;
        return {};
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        if ( orientation == Qt::Vertical && role == Qt::DisplayRole && accept(section) )
        {
            const auto &pair = properties[display_property[section]];
            if ( pair.first.isEmpty() )
            {
                QString name = display_property[section];
                name[0] = name[0].toUpper();
                return name;
            }
            return pair.first;
        }
        return {};
    }

    /**
     * \brief Get a server property
     */
    QString server_property(const QString& property)
    {
        return properties[property].second;
    }

    /**
     * \brief Removes all stored properties
     */
    void clear()
    {
        beginResetModel();
        properties.clear();
        endResetModel();
    }

public slots:
    /**
     * \brief Sets a server property
     */
    void set_server_property(const QString& property, const QString& value)
    {
        if ( !property.isEmpty() )
        {
            properties[property].second = value;
            if ( !signalsBlocked() )
            {
                int row = display_property.indexOf(property);
                if ( row != -1 )
                    emit dataChanged(index(row), index(row));
            }
        }
    }

    /**
     * \brief Sets the display header for the given property
     */
    void set_header(const QString& property, const QString& text)
    {
        if ( !property.isEmpty() )
        {
            properties[property].first = text;
            if ( !signalsBlocked() )
            {
                int row = display_property.indexOf(property);
                if ( row != -1 )
                    emit headerDataChanged(Qt::Vertical, row, row);
            }
        }
    }

    /**
     * \brief Sets the properties to be displayed
     */
    void set_display(const QStringList& properties)
    {
        emit layoutAboutToBeChanged();
        display_property = properties;
        display_property.removeAll({});
        emit layoutChanged();
    }

private:
    /**
     * \brief Whether a row number is acceptable as an array index
     */
    bool accept(int row) const
    {
        return row >= 0 && row < display_property.size();
    }

    /// List of names of properties to be displayed
    QStringList display_property = {
        "server", "connection", "host", "version", "map", "timing", "players"
    };
    QHash<QString, QPair<QString,QString>> properties; ///< Property name -> (header, value)

};

#endif // XONOTIC_SERVER_MODEL_HPP
