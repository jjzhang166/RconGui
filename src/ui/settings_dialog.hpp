/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \section License
 *
 * Copyright (C)  Mattia Basaglia
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
#ifndef SETTINGS_DIALOG_HPP
#define SETTINGS_DIALOG_HPP

#include "ui_settings_dialog.h"
#include "model/extra_row_proxy_model.hpp"
#include "model/quick_commands_model.hpp"
#include "model/player_action_delegate.hpp"

class SettingsDialog : public QDialog, public Ui_SettingsDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);

public slots:
    void accept() override;

private:
    void init_tab_network();
    void init_tab_quick_commands();
    void init_tab_player_actions();
    void init_tab_internal_commands();
    void init_tab_console();

    QuickCommandsModel   model_quick_commands;
    ExtraRowProxyModel   proxy_quick_commands;
    PlayerActionModel    model_player_actions;
    ExtraRowProxyModel   proxy_player_actions;
    PlayerActionDelegate delegate_player_actions;
};

#endif // SETTINGS_DIALOG_HPP
