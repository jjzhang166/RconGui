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
#ifndef PLAYER_ACTION_DELEGATE_HPP
#define PLAYER_ACTION_DELEGATE_HPP

#include <QApplication>
#include <QStyledItemDelegate>

#include "ui/icon_edit.hpp"
#include "player_action_model.hpp"

/**
 * \brief Shows an icon edit for the icon
 */
class PlayerActionDelegate : public QStyledItemDelegate
{
public:
    PlayerActionDelegate(QObject *parent = 0)
        : QStyledItemDelegate(parent) {}

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override
    {
        if ( index.column() == PlayerActionModel::Icon )
            return new IconEdit(parent);
        return QStyledItemDelegate::createEditor(parent, option, index);
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override
    {
        if ( index.column() != PlayerActionModel::Icon )
            return QStyledItemDelegate::setEditorData(editor, index);

        auto icon_edit = static_cast<IconEdit*>(editor);
        icon_edit->setText(index.data().toString());

    }

    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const override
    {
        if ( index.column() != PlayerActionModel::Icon )
            return QStyledItemDelegate::setModelData(editor, model, index);

        auto icon_edit = static_cast<IconEdit*>(editor);
        model->setData(index, icon_edit->text());
    }


    void updateEditorGeometry(QWidget* editor,
                              const QStyleOptionViewItem& option,
                              const QModelIndex& index) const
    {
        // This is going to hide the decoration icon when the editor is active
        editor->setGeometry(option.rect);
    }
};

#endif // PLAYER_ACTION_DELEGATE_HPP
