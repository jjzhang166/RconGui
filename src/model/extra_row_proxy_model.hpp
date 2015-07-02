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
#ifndef EXTRA_ROW_PROXY_MODEL_HPP
#define EXTRA_ROW_PROXY_MODEL_HPP

#include <QAbstractTableModel>

/**
 * \brief Adds an extra editable row at the end of the model to easily insert new rows
 * \note inheriting from QAbstractTableModel is much easier than inhiriting from QAbstractProxyModel
 */
class ExtraRowProxyModel : public QAbstractTableModel
{
public:
    using QAbstractTableModel::QAbstractTableModel;


    void setSourceModel(QAbstractTableModel* source)
    {
        if ( this->source )
            disconnect(this->source, 0, this, 0);
        this->source = source;
        if ( source )
        {
            connect_signal(source, &QAbstractTableModel::dataChanged);
            connect_signal(source, &QAbstractTableModel::headerDataChanged);
            connect_signal(source, &QAbstractTableModel::layoutAboutToBeChanged);
            connect_signal(source, &QAbstractTableModel::layoutChanged);
            connect_signal(source, &QAbstractTableModel::rowsAboutToBeInserted);
            connect_signal(source, &QAbstractTableModel::rowsInserted);
            connect_signal(source, &QAbstractTableModel::rowsAboutToBeRemoved);
            connect_signal(source, &QAbstractTableModel::rowsRemoved);
            connect_signal(source, &QAbstractTableModel::columnsAboutToBeInserted);
            connect_signal(source, &QAbstractTableModel::columnsInserted);
            connect_signal(source, &QAbstractTableModel::columnsAboutToBeRemoved);
            connect_signal(source, &QAbstractTableModel::columnsRemoved);
            connect_signal(source, &QAbstractTableModel::modelAboutToBeReset);
            connect_signal(source, &QAbstractTableModel::modelReset);
            connect_signal(source, &QAbstractTableModel::rowsAboutToBeMoved);
            connect_signal(source, &QAbstractTableModel::rowsMoved);
            connect_signal(source, &QAbstractTableModel::columnsAboutToBeMoved);
            connect_signal(source, &QAbstractTableModel::columnsMoved);
        }
    }

    QAbstractTableModel* sourceModel() const
    {
        return source;
    }

    int columnCount(const QModelIndex& parent = {}) const override
    {
        return source->columnCount();
    }

    int rowCount(const QModelIndex& parent = {}) const override
    {
        return source->rowCount()+1;
    }

    QModelIndex mapToSource(const QModelIndex& proxyIndex) const
    {
        if ( proxyIndex.row() == rowCount() )
            return {};
        return source->index(proxyIndex.row(), proxyIndex.column());
    }

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
    {
        if ( is_extra_index(index) )
        {
            if (role == Qt::DisplayRole || role == Qt::EditRole)
                return QString();
            return {};
        }
        return source->data(index, role);
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        return source->headerData(section, orientation, role);
    }

    Qt::ItemFlags flags(const QModelIndex& index) const override
    {
        if ( is_extra_index(index) )
            return Qt::ItemIsEditable|Qt::ItemIsSelectable|Qt::ItemIsEnabled;
        return source->flags(mapToSource(index));
    }

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override
    {
        if ( is_extra_index(index) )
        {
            if ( role == Qt::EditRole &&
                 source->insertRow(index.row(), index.parent()) )
            {
                return source->setData(index, value, role);
            }

            return false;
        }
        return source->setData(mapToSource(index), value, role);
    }

private:
    bool is_extra_index(const QModelIndex& index) const
    {
        return index.row() == source->rowCount();
    }

    template<class Member>
        void connect_signal(QAbstractTableModel* other, const Member& member)
        {
            connect(other, member, this, member);
        }

    QAbstractTableModel* source = nullptr;
};

#endif // EXTRA_ROW_PROXY_MODEL_HPP
