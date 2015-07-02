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
#include "color_parser.hpp"

#include <QTextDocumentFragment>

namespace xonotic {

QRegExp AbstractColorParser::regex_xoncolor{"^\\^([0-9]|x[0-9a-fA-F]{3})"};


QColor AbstractColorParser::to_color(const QString& color)
{
    if ( !color.isEmpty() && regex_xoncolor.exactMatch(color) )
        return match_to_color(regex_xoncolor.cap(1));
    return default_color;

}

QColor AbstractColorParser::bounded_color(const QColor& color)
{
    return QColor::fromHsl(
        color.hslHue(),
        color.hslSaturation(),
        qBound(min_brightness, color.lightness(), max_brightness)
    );
}

QColor AbstractColorParser::match_to_color(const QString& s)
{
    if ( s.size() == 4 )
    {
        return QColor(
            hex_to_int(s[1].unicode())*255/15,
            hex_to_int(s[2].unicode())*255/15,
            hex_to_int(s[3].unicode())*255/15
        );
    }

    if ( s.size() != 1 )
        return default_color;

    switch ( s[0].unicode() )
    {
        case '0': return Qt::black;
        case '1': return Qt::red;
        case '2': return Qt::green;
        case '3': return Qt::yellow;
        case '4': return Qt::blue;
        case '5': return Qt::cyan;
        case '6': return Qt::magenta;
        case '7': return Qt::white;
        case '8': return Qt::darkGray;
        case '9': return Qt::gray;
    }

    return default_color;
}

void AbstractColorParser::parse(const QString& string)
{
    work_string = string;
    start_index = end_index = 0;

    push_start();

    for ( end_index = 0; end_index < work_string.size(); end_index++ )
    {
        if ( work_string[end_index] == '^' && end_index < work_string.size()-1 )
        {
            if ( work_string[end_index+1] == '^' )
            {
                push_string();
                end_index++;
                start_index = end_index;
            }
            else if ( regex_xoncolor.indexIn(work_string, end_index, QRegExp::CaretAtOffset) != -1 )
            {
                push_color(match_to_color(regex_xoncolor.cap(1)));
                end_index += regex_xoncolor.matchedLength();
                start_index = end_index;
            }
            else
            {
                end_index++;
            }
        }
        push_char(work_string[end_index]);
    }

    push_end();

    work_string.clear();
}

void AbstractColorParser::push_start()
{
    on_start(default_color);
}

void AbstractColorParser::push_end()
{
    push_string();
    on_end();
}

void AbstractColorParser::push_string()
{
    if ( start_index >= end_index || start_index >= work_string.size() )
        return;
    on_append_string(work_string.mid(start_index, end_index-start_index));
    start_index = end_index;
}


void AbstractColorParser::push_char(const QChar& c)
{
    if ( c == '\n' )
    {
        push_end();
        on_new_line();
        push_start();
        start_index++;
    }
    /// \todo Optional QFont parsing
}

void AbstractColorParser::push_color(const QColor& color)
{
    push_string();
    on_change_color(color);
}

void ColorParserTextCursor::on_start(const QColor& color)
{
    auto f = output->charFormat();
    f.setForeground(color);
    output->setCharFormat(f);
}

void ColorParserTextCursor::on_end()
{
}

void ColorParserTextCursor::on_append_string(const QString& string)
{
    output->insertText(string);
}

void ColorParserTextCursor::on_new_line()
{
    output->insertBlock();
}

void ColorParserTextCursor::on_change_color(const QColor& color)
{
    auto f = output->charFormat();
    f.setForeground(bounded_color(color));
    output->setCharFormat(f);
}

bool ColorParserTextCursor::before_parse(QTextCursor* out)
{
    if ( !out )
        return false;
    output = out;
    output->beginEditBlock();
    return true;
}

void ColorParserTextCursor::after_parse()
{
    output->endEditBlock();
    output = nullptr;
}

void ColorParserTextCursor::convert_fragment(const QString& text, QTextCursor* out)
{
    if ( !before_parse(out) ) return;
    parse(text);
    after_parse();
}

void ColorParserTextCursor::convert(const QString& text, QTextCursor* out)
{
    if ( !before_parse(out) ) return;
    parse(text);
    on_new_line();
    after_parse();
}

void ColorParserTextCursor::convert(const QStringList& lines, QTextCursor* out)
{
    if ( !before_parse(out) ) return;
    for ( const auto& line : lines )
    {
        parse(line);
        on_new_line();
    }
    after_parse();
}

void ColorParserPlainText::on_start(const QColor& color)
{
}

void ColorParserPlainText::on_end()
{
}

void ColorParserPlainText::on_append_string(const QString& string)
{
    output += string;
}

void ColorParserPlainText::on_new_line()
{
    output += "\n";
}

void ColorParserPlainText::on_change_color(const QColor& color)
{
}

QString ColorParserPlainText::convert_fragment(const QString& text)
{
    output.clear();
    parse(text);
    return output;
}

QString ColorParserPlainText::convert(const QString& text)
{
    output.clear();
    parse(text);
    on_new_line();
    return output;
}

QString ColorParserPlainText::convert(const QStringList& lines)
{
    output.clear();
    for ( const auto& line : lines )
    {
        parse(line);
        on_new_line();
    }
    return output;
}

} // namespace xonotic
