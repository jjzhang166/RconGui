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
#include <QDialog>

ServerSetupWidget::ServerSetupWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUi(this);
    setup_secure_combo(input_secure);
    button_save->setShortcut(QKeySequence::Save);
    connect(button_box, &QDialogButtonBox::accepted, this, &ServerSetupWidget::accepted);
    connect(button_box, &QDialogButtonBox::rejected, this, &ServerSetupWidget::rejected);
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

void ServerSetupWidget::setup_secure_combo(QComboBox* box)
{
    box->clear();
    secure_to_item(box, xonotic::ConnectionDetails::NO);
    secure_to_item(box, xonotic::ConnectionDetails::TIME);
    secure_to_item(box, xonotic::ConnectionDetails::CHALLENGE);
    box->setWhatsThis(R"(<html><head/><body>
<p>Rcon secure protocol, must match the value of the cvar <b>rcon_secure</b> in the Xonotic server.</p>
<ul>
    <li>"No" (<tt>rcon_secure 0<tt>) the password is not encrypted and is the most reliable option.</li>
    <li>"Time Based" (<tt>rcon_secure 1<tt>) hashes the command based on the system time, it requires to have the system clock to be synchronized with the server clock.</li>
    <li>"Challenge Based" (<tt>rcon_secure 2<tt>) asks the server for challenges, secure but not always reliable.</li>
</ul></body></html>)");
    box->setToolTip("Rcon Secure Protocol");
    /// \todo set secure 1 as the default?
}

QString ServerSetupWidget::secure_to_text(xonotic::ConnectionDetails::Secure secure)
{
    switch (secure)
    {
        case xonotic::ConnectionDetails::NO:
            return tr("No");
        case xonotic::ConnectionDetails::TIME:
            return tr("Time Based");
        case xonotic::ConnectionDetails::CHALLENGE:
            return tr("Challenge Based");
        default: return {};
    }
}

QIcon ServerSetupWidget::secure_to_icon(xonotic::ConnectionDetails::Secure secure)
{
    switch (secure)
    {
        case xonotic::ConnectionDetails::NO:
            return QIcon::fromTheme("security-low");
        case xonotic::ConnectionDetails::TIME:
            return QIcon::fromTheme("security-medium");
        case xonotic::ConnectionDetails::CHALLENGE:
            return QIcon::fromTheme("security-high");
        default: return {};
    }
}

void ServerSetupWidget::secure_to_item(QComboBox* box,
                                       xonotic::ConnectionDetails::Secure secure)
{
    box->addItem(secure_to_icon(secure), secure_to_text(secure), int(secure));
}
