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
#include "server_setup_widget.hpp"
#include "settings.hpp"

ServerSetupWidget::ServerSetupWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUi(this);
    button_save->setShortcut(QKeySequence::Save);
    update_presets();
}

ServerSetupWidget::ServerSetupWidget(const xonotic::ConnectionDetails& xonotic,
                                       QWidget* parent)
    : ServerSetupWidget(parent)
{
    populate(xonotic);
}

xonotic::ConnectionDetails ServerSetupWidget::connection_details() const
{
    return xonotic::ConnectionDetails(
        network::Server(input_host->text().toStdString(), input_port->value()),
        input_password->text().toStdString(),
        xonotic::ConnectionDetails::Secure(input_secure->currentIndex()),
        input_name->text().toStdString()
    );
}

void ServerSetupWidget::on_button_save_clicked()
{
    auto xonotic = connection_details();
    auto name = QString::fromStdString(xonotic.name);
    Settings::instance().saved_servers.insert(name, xonotic);
    update_presets();

    input_preset->blockSignals(true);
    input_preset->setCurrentText(name);
    input_preset->blockSignals(false);
}

void ServerSetupWidget::on_button_delete_clicked()
{
    auto xonotic = connection_details();
    Settings::instance().saved_servers
        .remove(QString::fromStdString(xonotic.name));
    update_presets();
}

void ServerSetupWidget::update_presets()
{
    input_preset->blockSignals(true);
    auto current = input_preset->currentText();
    input_preset->clear();
    input_preset->addItem(QIcon::fromTheme("document-new"), tr("Create New"));
    for ( const auto& sv : Settings::instance().saved_servers.keys() )
        input_preset->addItem(QIcon::fromTheme("network-server"), sv);
    input_preset->setCurrentText(current);
    input_preset->blockSignals(false);
}

void ServerSetupWidget::on_input_preset_currentIndexChanged(const QString& text)
{
    if ( input_preset->currentIndex() != 0 )
    {
        auto it = Settings::instance().saved_servers.find(text);
        if ( it != Settings::instance().saved_servers.end() )
            populate(*it);
    }
}

void ServerSetupWidget::populate(const xonotic::ConnectionDetails& xonotic)
{
    input_host->setText(QString::fromStdString(xonotic.server.host));
    input_port->setValue(xonotic.server.port);
    input_password->setText(QString::fromStdString(xonotic.rcon_password));
    input_secure->setCurrentIndex(int(xonotic.rcon_secure));
    input_name->setText(xonotic.name == xonotic.server.name() ? "" :
        QString::fromStdString(xonotic.name));
}

void ServerSetupWidget::update_placeholder()
{
    input_name->setPlaceholderText(QString("%1:%2")
        .arg(input_host->text())
        .arg(input_port->value())
    );
}
