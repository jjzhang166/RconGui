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
#include "inline_server_setup_widget.hpp"

InlineServerSetupWidget::InlineServerSetupWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUi(this);
    button_action->hide();
    connect(button_action, &QToolButton::clicked,
            this, &InlineServerSetupWidget::action_button_clicked);
    clear();
}

InlineServerSetupWidget::InlineServerSetupWidget(const xonotic::ConnectionDetails& xonotic,
                                       QWidget* parent)
    : InlineServerSetupWidget(parent)
{
    populate(xonotic);
}

xonotic::ConnectionDetails InlineServerSetupWidget::connection_details() const
{
    return xonotic::ConnectionDetails(
        network::Server(input_host->text().toStdString(), input_port->value()),
        input_password->text().toStdString(),
        xonotic::ConnectionDetails::Secure(input_secure->currentIndex()),
        input_name->text().toStdString()
    );
}

void InlineServerSetupWidget::populate(const xonotic::ConnectionDetails& xonotic)
{
    input_host->setText(QString::fromStdString(xonotic.server.host));
    input_port->setValue(xonotic.server.port);
    input_password->setText(QString::fromStdString(xonotic.rcon_password));
    input_secure->setCurrentIndex(int(xonotic.rcon_secure));
    input_name->setText(xonotic.name == xonotic.server.name() ? "" :
        QString::fromStdString(xonotic.name));
    input_name->setPlaceholderText(QString::fromStdString(xonotic.server.name()));
}

void InlineServerSetupWidget::clear()
{
    input_host->setText("localhost");
    input_port->setValue(26000);
    input_password->setText("");
    input_secure->setCurrentIndex(1);
    input_name->setText("");
    input_name->setPlaceholderText("localhost:26000");
}

void InlineServerSetupWidget::update_placeholder()
{
    input_name->setPlaceholderText(QString("%1:%2")
        .arg(input_host->text())
        .arg(input_port->value())
    );
}

void InlineServerSetupWidget::hide_action_button()
{
    button_action->hide();
}

void InlineServerSetupWidget::show_action_button(const QIcon& icon, const QString& text)
{
    button_action->setText(text);
    button_action->setToolTip(text);
    button_action->setIcon(icon);
    button_action->show();
}

void InlineServerSetupWidget::show_action_button()
{
    button_action->show();
}

