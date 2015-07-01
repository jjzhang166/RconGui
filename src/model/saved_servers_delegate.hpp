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
#ifndef SAVED_SERVERS_DELEGATE_HPP
#define SAVED_SERVERS_DELEGATE_HPP

#include <QComboBox>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QPaintDevice>
#include <QtMath>
#include "saved_servers_model.hpp"
#include "ui/server_setup_widget.hpp"

/**
 * \brief Delegate for SavedServersModel
 */
class SavedServersDelegate : public QStyledItemDelegate
{
public:
    SavedServersDelegate(QObject *parent = 0)
        : QStyledItemDelegate(parent) {}

    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const override
    {
        if ( index.column() == SavedServersModel::Secure )
        {
            auto combo = new QComboBox(parent);
            ServerSetupWidget::setup_secure_combo(combo);
            return combo;
        }
        return QStyledItemDelegate::createEditor(parent, option, index);
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override
    {
        if ( index.column() == SavedServersModel::Secure )
        {
            int current = index.data().toInt();
            auto combo = static_cast<QComboBox*>(editor);
            combo->setCurrentIndex(current);
            //combo->showPopup();
        }
        else
        {
            QStyledItemDelegate::setEditorData(editor, index);
        }

    }

    void setModelData(QWidget *editor,
                      QAbstractItemModel *model,
                      const QModelIndex &index) const override
    {
        if ( index.column() == SavedServersModel::Secure )
        {
            auto combo = static_cast<QComboBox*>(editor);
            if ( index.data().toInt() != combo->currentIndex() )
                model->setData(index, combo->currentIndex());
        }
        else
        {
            QStyledItemDelegate::setModelData(editor, model, index);
        }

    }

    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        if ( index.column() == SavedServersModel::Secure )
        {
            QStyle *style = option.widget ? option.widget->style() : QApplication::style();
            auto combo = make_combo(style, option, index);
            style->drawComplexControl(QStyle::CC_ComboBox, &combo, painter);
            style->drawControl(QStyle::CE_ComboBoxLabel, &combo, painter, option.widget);
            return;
        }

        QStyledItemDelegate::paint(painter, option, index);
    }

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        if ( index.column() == SavedServersModel::Secure )
        {
            QStyle *style = option.widget ? option.widget->style() : QApplication::style();
            QFontMetrics fm = QApplication::fontMetrics();
            auto combo = make_combo(style, option, index);
            //return style->sizeFromContents(QStyle::CT_ComboBox, &combo, QSize(), nullptr);
            QSize sh;
            sh.setWidth(fm.boundingRect(combo.currentText).width() +
                combo.iconSize.width() + 4 );
            // height
            sh.setHeight(qMax(qCeil(QFontMetricsF(fm).height()), 14) + 2);
            sh.setHeight(qMax(sh.height(), combo.iconSize.height() + 2));

            // add style and strut values
            sh = style->sizeFromContents(QStyle::CT_ComboBox, &combo, sh, nullptr);
            return sh.expandedTo(QApplication::globalStrut());
        }
        return QStyledItemDelegate::sizeHint(option, index);
    }

private:
    QStyleOptionComboBox make_combo(QStyle *style,
                                    const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const
    {
        auto secure = xonotic::ConnectionDetails::Secure(index.data().toInt());
        QStyleOptionComboBox combo;
        combo.rect = option.rect;
        combo.state = QStyle::State_Enabled;
        combo.editable = false;
        combo.subControls = QStyle::SC_All;
        // activeSubControls
        combo.currentText = ServerSetupWidget::secure_to_text(secure);
        combo.currentIcon = ServerSetupWidget::secure_to_icon(secure);
        auto iconwidth = style->pixelMetric(QStyle::PM_SmallIconSize, 0, nullptr);
        combo.iconSize = {iconwidth, iconwidth};
        return combo;
    }

};

#endif // SAVED_SERVERS_DELEGATE_HPP
