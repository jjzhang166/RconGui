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
#ifndef CVAR_DELEGATE_HPP
#define CVAR_DELEGATE_HPP

#include <QApplication>
#include <QLineEdit>
#include <QPalette>
#include <QStyledItemDelegate>

#include "functional.hpp"

/**
 * \brief Used to edit cvars
 */
class CvarDelegate : public QStyledItemDelegate
{
public:
    CvarDelegate(QObject *parent = 0)
        : QStyledItemDelegate(parent) {}


    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const override
    {
        auto edit = static_cast<QLineEdit*>(editor);
        if ( index.data().toString() != edit->text() )
        {
            QString cvar = index.data(Qt::UserRole).toString();
            callback(set_cvar, cvar, edit->text());
        }
    }


    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {

        QStyleOptionViewItem opt = option;

        opt.features |= QStyleOptionViewItem::HasDisplay;
        opt.text = displayText(index.data(Qt::DisplayRole).toString(), opt.locale);

        if ( ! (index.flags() & Qt::ItemIsEditable) )
            opt.backgroundBrush = QApplication::palette()
                .brush(QPalette::Disabled, QPalette::Base);

        QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
    }

    /// \brief Function to call to set a cvar
    std::function<void (const QString&, const QString&)> set_cvar;
};

#endif // CVAR_DELEGATE_HPP
