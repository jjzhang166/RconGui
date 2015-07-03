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
#ifndef ICON_EDIT_HPP
#define ICON_EDIT_HPP
#include <QLineEdit>
#include <QIcon>
#include <QResizeEvent>
#include <QPainter>
#include <QFileInfo>
#include <QStyle>
/**
 * \brief Widget that allows to enter a theme or file icon name and displays a preview
 */
class IconEdit : public QLineEdit
{
public:
    explicit IconEdit(QWidget* parent = nullptr) : QLineEdit(parent)
    {
        connect(this, &QLineEdit::textChanged, this, &IconEdit::update_icon);
    }

    /**
     * \brief Returns the icon corresponding to the current text
     */
    QIcon icon() const
    {
        return icon_;
    }

protected:
    void resizeEvent(QResizeEvent* ev) override
    {
        auto margins = textMargins();
        margins.setLeft(contents_rect().bottom());
        setTextMargins(margins);
        QLineEdit::resizeEvent(ev);
    }

    void paintEvent(QPaintEvent* ev) override
    {
        QLineEdit::paintEvent(ev);

        if ( icon_.isNull() )
            return;

        QRect r = contents_rect();

        int icon_size;
        if ( r.width() > r.height() )
        {
            icon_size = r.height();
            r.setWidth(icon_size);
        }
        else
        {
            icon_size = r.width();
            r.setHeight(icon_size);
        }

        QPainter painter(this);
        painter.drawPixmap(r.x() + (r.width()-icon_size)/2,
                           r.y() + (r.height()-icon_size)/2,
                           icon_.pixmap(icon_size, icon_size));
    }

private slots:
    /**
     * \brief Updates the icon to reflect the text change
     */
    void update_icon(const QString& name)
    {
        if ( name.contains('/') )
        {
            if ( QFileInfo(name).exists() )
                icon_ = QIcon(name);
        }
        else
        {
            icon_ = QIcon::fromTheme(name);
        }
    }

private:
    /**
     * \brief Returns the rect for the editable area
     */
    QRect contents_rect()
    {
        QStyleOptionFrameV2 panel;
        initStyleOption(&panel);
        return style()->subElementRect(QStyle::SE_LineEditContents, &panel, this);
    }

    QIcon icon_;
};

#endif // ICON_EDIT_HPP
