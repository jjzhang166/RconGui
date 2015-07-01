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
#ifndef INLINE_SERVER_SETUP_WIDGET_HPP
#define INLINE_SERVER_SETUP_WIDGET_HPP

#include "ui_inline_server_setup_widget.h"
#include "xonotic/connection_details.hpp"

/**
 * \brief Form used to gather Xonotic connection information
 */
class InlineServerSetupWidget : public QWidget, private Ui::InlineServerSetupWidget
{
    Q_OBJECT

public:
    InlineServerSetupWidget(QWidget* parent = nullptr);

    InlineServerSetupWidget(const xonotic::ConnectionDetails& xonotic, QWidget* parent = nullptr);

    /**
     * \brief Builds a Xonotic connection from the form contents
     */
    xonotic::ConnectionDetails connection_details() const;

    /**
     * \brief Populates the form input fields from the server settings
     */
    void populate(const xonotic::ConnectionDetails& xonotic);

    /**
     * \brief Shows the action button the way it is
     */
    void show_action_button();

    /**
     * \brief Shows the action button changing its properties
     */
    void show_action_button(const QIcon& icon, const QString& text);

    /**
     * \brief Hides the action button
     */
    void hide_action_button();

    /**
     * \brief Clears the form
     */
    void clear();

signals:
    /**
     * \brief Emitted when the action button has been clicked
     */
    void action_button_clicked();

private slots:
    void update_placeholder();
};

#endif // INLINE_SERVER_SETUP_WIDGET_HPP
