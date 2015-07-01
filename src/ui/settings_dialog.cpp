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


SettingsDialog::SettingsDialog(QWidget* parent):
    QDialog(parent)
{
    setupUi(this);

    init_tab_network();
    init_tab_console();
}

void SettingsDialog::init_tab_network()
{
    table_saved_servers->setModel(&model_servers);
    table_saved_servers->setItemDelegate(&delegate_servers);
    table_saved_servers->resizeColumnsToContents();
}

void SettingsDialog::init_tab_console()
{
    input_con_fg->setColor(settings().console_foreground);
    input_con_bg->setColor(settings().console_background);
    input_con_bright_min->setValue(settings().console_brightness_min);
    input_con_bright_max->setValue(settings().console_brightness_max);
    input_con_font->setCurrentFont(settings().console_font);
    input_con_histsize->setValue(settings().console_max_history);

    /// \todo Having actual members in settings will ensure defaults are the same everywhere
    input_con_complete->setChecked(settings().get("console/autocomplete", true));
    input_con_complete_minchars->setValue(settings().get("console/autocomplete/min_chars",1));
    input_con_complete_max->setValue(settings().get("console/autocomplete/max_suggestions",128));
}

void SettingsDialog::accept()
{
    settings().console_foreground =  input_con_fg->color();
    settings().console_background = input_con_bg->color();
    settings().console_brightness_min = input_con_bright_min->value();
    settings().console_brightness_max = input_con_bright_max->value();
    /// \todo size et all? (QFontDialog)
    settings().console_font = input_con_font->currentFont();
    settings().console_max_history = input_con_histsize->value();

    /// \todo Having actual members in settings will ensure defaults are the same everywhere
    settings().put("console/autocomplete", input_con_complete->isChecked());
    settings().put("console/autocomplete/min_chars", input_con_complete_minchars->value());
    settings().put("console/autocomplete/max_suggestions", input_con_complete_max->value());

    settings().save();

    QDialog::accept();
}
