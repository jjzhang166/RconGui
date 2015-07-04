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
#include "rcon_window.hpp"

#include <QtWidgets/QApplication>
#include <QSettings>
#include <QToolButton>

#include "server_setup_dialog.hpp"
#include "server_widget.hpp"
#include "settings_dialog.hpp"
#include "ui/server_setup_widget.hpp"

RconWindow::RconWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi(this);

    auto button_new_tab = new QToolButton();
    button_new_tab->setText(tr("New Tab"));
    button_new_tab->setToolTip(tr("Create a new tab"));
    button_new_tab->setIcon(QIcon::fromTheme("tab-new"));
    button_new_tab->setShortcut(QKeySequence::New);
    connect(button_new_tab, &QToolButton::clicked, this, &RconWindow::new_tab);
    tabWidget->setCornerWidget(button_new_tab, Qt::TopLeftCorner);

    auto preferences = new QToolButton();
    preferences->setText(tr("Preferences"));
    preferences->setToolTip(tr("Preferences"));
    preferences->setIcon(QIcon::fromTheme("configure"));
    preferences->setShortcut(QKeySequence::Preferences);
    connect(preferences, &QToolButton::clicked, [this]{
        if ( SettingsDialog().exec() )
            emit settings_changed();
    });
    tabWidget->setCornerWidget(preferences, Qt::TopRightCorner);

    connect(tabWidget,&QTabWidget::tabCloseRequested, [this](int index){
        delete tabWidget->widget(index);
        if ( tabWidget->count() == 0 )
            new_tab();
    });
    connect(tabWidget, &QTabWidget::currentChanged, [this](int index){
        setWindowTitle(tabWidget->tabText(index));
    });
    new_tab();
}

void RconWindow::new_tab()
{
    auto tab = new QWidget();
    auto layout = new QVBoxLayout(tab);
    auto createwidget = new ServerSetupWidget(this);
    layout->addWidget(createwidget);
    auto buttonbox = new QDialogButtonBox(this);
    auto button_connect = new QPushButton(QIcon::fromTheme("network-connect"), tr("Connect"));
    buttonbox->addButton(button_connect,QDialogButtonBox::AcceptRole);
    layout->addWidget(buttonbox);
    connect(button_connect, &QPushButton::clicked, [this, tab, createwidget]{
        create_tab(createwidget->connection_details());
        tab->deleteLater();
    });

    int tabindex = tabWidget->addTab(tab, tr("Connect to Server"));
    tabWidget->setCurrentIndex(tabindex);
}

void RconWindow::create_tab(const xonotic::ConnectionDetails& xonotic)
{
    QString name = QString::fromStdString(xonotic.name);
    auto tab = new ServerWidget(std::move(xonotic));
    int tabindex = tabWidget->addTab(tab, name);
    connect(tab, &ServerWidget::name_changed,[this,tab](const QString& string){
        tabWidget->setTabText(tabWidget->indexOf(tab), string);
    });
    connect(this, &RconWindow::settings_changed, tab, &ServerWidget::reload_settings);
    tabWidget->setCurrentIndex(tabindex);
}
