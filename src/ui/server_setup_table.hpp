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
#ifndef SERVER_SETUP_TABLE_HPP
#define SERVER_SETUP_TABLE_HPP

#include <QScrollArea>
#include <QVBoxLayout>

#include "xonotic/connection_details.hpp"
#include "inline_server_setup_widget.hpp"

/**
 * \brief A table to edit Xonotic server connections
 */
class ServerSetupTable : public QScrollArea
{
public:
    explicit ServerSetupTable(QWidget* parent=nullptr);

    /**
     * \brief Appends the given connection to the widget
     */
    void append(const xonotic::ConnectionDetails& server);

    /**
     * \brief Clears all connections
     */
    void clear();

public slots:
    /**
     * \brief Save to settings
     */
    void save() const;

    /**
     * \brief Load from settings
     */
    void load();

private:
    QList<InlineServerSetupWidget*> widgets;
    QVBoxLayout* vbox_layout = nullptr;
};

#endif // SERVER_SETUP_TABLE_HPP
