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
#ifndef SERVER_DELEGATE_HPP
#define SERVER_DELEGATE_HPP

#include <QStyledItemDelegate>
#include <QComboBox>

#include "functional.hpp"
#include <QApplication>
#include <QPalette>

/**
 * \brief Shows a combo box for the map
 */
class ServerDelegate : public QStyledItemDelegate
{
public:
    ServerDelegate(QObject *parent = 0)
        : QStyledItemDelegate(parent) {}

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override
    {
        if ( discard_index(index) )
            return QStyledItemDelegate::createEditor(parent, option, index);

        auto combo = new QComboBox(parent);
        if ( g_maplist )
            combo->addItems(g_maplist().split(" ", QString::SkipEmptyParts));
        combo->setEditable(true);
        return combo;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override
    {
        if ( discard_index(index) )
            return QStyledItemDelegate::setEditorData(editor, index);

        QString current = index.data().toString();
        auto combo = static_cast<QComboBox*>(editor);
        if ( combo->findText(current) == -1 )
            combo->addItem(current);
        combo->setCurrentText(current);
    }

    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const override
    {
        if ( discard_index(index) )
            return QStyledItemDelegate::setModelData(editor, model, index);

        auto combo = static_cast<QComboBox*>(editor);
        callback(chmap, combo->currentText());
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

    /// \brief Function to call to get the map list
    std::function<QString()> g_maplist;
    /// \brief Function to call to change the map
    std::function<void (const QString&)> chmap;

private:
    /**
     * \brief Whether the index is to be discarded
     */
    bool discard_index(const QModelIndex &index) const
    {
        return index.data(Qt::UserRole).toString() != "map";
    }
};

#endif // SERVER_DELEGATE_HPP
