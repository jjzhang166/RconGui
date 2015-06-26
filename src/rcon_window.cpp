/*
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

#include "rcon_window.hpp"

#include <QtWidgets/QApplication>

#include "create_server_dialog.hpp"
#include "server_widget.hpp"

RconWindow::RconWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi(this);

    connect(tabWidget,&QTabWidget::tabCloseRequested, [this](int index){
        delete tabWidget->widget(index);
        tabWidget->removeTab(index);
    });
    connect(tabWidget, &QTabWidget::currentChanged, [this](int index){
        setWindowTitle(tabWidget->tabText(index));
    });
}


void RconWindow::new_tab()
{
    CreateServerDialog dlg(this);
    if ( dlg.exec() )
    {
        auto xonotic = dlg.connection_info();
        QString name = QString::fromStdString(xonotic.name);
        auto tab = new ServerWidget(std::move(xonotic));
        tabWidget->addTab(tab, name);
        connect(tab, &ServerWidget::nameChanged,[this,tab](const QString& string){
            tabWidget->setTabText(tabWidget->indexOf(tab), string);
        });
    }
}
