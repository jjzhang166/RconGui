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

#include "settings_dialog.hpp"
#include "settings.hpp"
#include <QFontDialog>

SettingsDialog::SettingsDialog(QWidget* parent):
    QDialog(parent)
{
    setupUi(this);

    init_tab_network();
    init_tab_commands();
    init_tab_console();
}

void SettingsDialog::init_tab_network()
{
    table_sv->load();
    input_sv_new->show_action_button(QIcon::fromTheme("list-add"), tr("Add"));
    connect(input_sv_new,&InlineServerSetupWidget::action_button_clicked,[this]{
        table_sv->append(input_sv_new->connection_details());
        input_sv_new->clear();
    });
}

void SettingsDialog::init_tab_commands()
{
    model_quick_commands.load();
    proxy_quick_commands.setSourceModel(&model_quick_commands);
    proxy_quick_commands.set_placeholder(tr("New Command"));
    table_cmd_cmd->setModel(&proxy_quick_commands);
    if ( bool(settings().quick_commands_expansion) )
    {
        input_cmd_cmd_cvar->setChecked(true);
        input_cmd_cmd_nocvar->setCurrentIndex(
            int(settings().quick_commands_expansion)-1
        );
    }
    else
    {
        input_cmd_cmd_cvar->setChecked(false);
    }

    model_player_actions.set_actions(settings().player_actions);
    proxy_player_actions.setSourceModel(&model_player_actions);
    proxy_player_actions.set_placeholder(tr("New Action"));
    table_cmd_usr->setModel(&proxy_player_actions);
    table_cmd_usr->setItemDelegate(&delegate_player_actions);
    auto header_view = table_cmd_usr->horizontalHeader();
    header_view->setSectionResizeMode(PlayerActionModel::Name,    QHeaderView::ResizeToContents);
    header_view->setSectionResizeMode(PlayerActionModel::Command, QHeaderView::Stretch);
    header_view->setSectionResizeMode(PlayerActionModel::Icon,    QHeaderView::ResizeToContents);
    if ( bool(settings().player_actions_expansion) )
    {
        input_cmd_usr_cvar->setChecked(true);
        input_cmd_usr_nocvar->setCurrentIndex(
            int(settings().player_actions_expansion)-1
        );
    }
    else
    {
        input_cmd_usr_cvar->setChecked(false);
    }
}

void SettingsDialog::init_tab_console()
{
    input_con_fg->setColor(settings().console_foreground);
    input_con_bg->setColor(settings().console_background);
    input_con_bright_min->setValue(settings().console_brightness_min);
    input_con_bright_max->setValue(settings().console_brightness_max);
    input_con_font->setCurrentFont(settings().console_font);
    input_con_histsize->setValue(settings().console_max_history);
    input_con_nocvar->setCurrentIndex(int(settings().console_expansion)-1);

    /// \todo Having actual members in settings will ensure defaults are the same everywhere
    input_con_complete->setChecked(settings().get("console/autocomplete", true));
    input_con_complete_minchars->setValue(settings().get("console/autocomplete/min_chars",1));
    input_con_complete_max->setValue(settings().get("console/autocomplete/max_suggestions",128));

    input_con_attach->setText(settings().console_attach_command);
    input_con_detach->setText(settings().console_detach_command);

    connect(button_con_font,&QPushButton::clicked,[this]{
        QFontDialog dialog(input_con_font->currentFont(), this);
        if ( dialog.exec() )
            input_con_font->setCurrentFont(dialog.currentFont());
    });
}

void SettingsDialog::accept()
{
    table_sv->save();

    model_quick_commands.save();
    if ( input_cmd_cmd_cvar->isChecked() )
        settings().quick_commands_expansion = CvarExpansion(input_cmd_cmd_nocvar->currentIndex()+1);
    else
        settings().quick_commands_expansion = CvarExpansion::NotExpanded;

    settings().player_actions = model_player_actions.actions();
    if ( input_cmd_usr_cvar->isChecked() )
        settings().player_actions_expansion = CvarExpansion(input_cmd_usr_nocvar->currentIndex()+1);
    else
        settings().player_actions_expansion = CvarExpansion::NotExpanded;

    settings().console_foreground =  input_con_fg->color();
    settings().console_background = input_con_bg->color();
    settings().console_brightness_min = input_con_bright_min->value();
    settings().console_brightness_max = input_con_bright_max->value();
    settings().console_font = input_con_font->currentFont();
    settings().console_max_history = input_con_histsize->value();
    settings().console_expansion = CvarExpansion(input_con_nocvar->currentIndex()+1);

    /// \todo Having actual members in settings will ensure defaults are the same everywhere
    settings().put("console/autocomplete", input_con_complete->isChecked());
    settings().put("console/autocomplete/min_chars", input_con_complete_minchars->value());
    settings().put("console/autocomplete/max_suggestions", input_con_complete_max->value());

    settings().console_attach_command = input_con_attach->text();
    settings().console_detach_command = input_con_detach->text();

    settings().save();

    QDialog::accept();
}
