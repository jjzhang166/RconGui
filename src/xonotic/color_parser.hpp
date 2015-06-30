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
#ifndef XONOTIC_COLOR_PARSER_HPP
#define XONOTIC_COLOR_PARSER_HPP

#include <QString>
#include <QRegExp>
#include <QColor>
#include <QTextCursor>

namespace xonotic {
/**
 * \brief Parses a xonotic-colored and formats a QTextDocument accordingly
 */
class ColorParser
{
public:

    ColorParser() = default;

    explicit ColorParser(QColor default_color,     ///< Default text color
                         int    min_brightness=0,  ///< Minimum color brightness
                         int    max_brightness=255)///< Maximum color brightness
        : default_color (std::move(default_color)),
          min_brightness(min_brightness),
          max_brightness(max_brightness)
        {}


    /**
     * \brief Turns a string like ^2 or ^x123 into a QColor
     * \return The true color represented by the string, not bounded
     */
    QColor to_color(const QString& color);

    /**
     * \brief Ensures the color brightness is within the accepted range
     */
    QColor bounded_color(const QColor& color);

    /**
     * \brief Turns some text into html
     */
    void convert(const QString& text, QTextCursor *out);

    /**
     * \brief Turns a small fragment into html,
     *        without adding a new line at the end
     */
    void convert_fragment(const QString& text, QTextCursor *out);

    /**
     * \brief Turns some text into html
     */
    void convert(const QStringList& lines, QTextCursor *out);

private:
    /**
     * \brief Color from regex match
     */
    QColor match_to_color(const QString& s);

    /**
     * \brief Hex character to integer
     */
    static int hex_to_int(char c)
    {
        if ( c <= '9' && c >= '0' )
            return c - '0';
        if ( c <= 'f' && c >= 'a' )
            return c - 'a' + 0xa;
        if ( c <= 'F' && c >= 'A' )
            return c - 'A' + 0xa;
        return 0;
    }

    /**
     * \brief Sets up the object for parse()
     * \returns Whether it can proceed with the parsing
     */
    bool before_parse(QTextCursor *output);

    /**
     * \brief Parses \c string appending the result to \c output
     */
    void parse(const QString& string);

    /**
     * \brief Cleans up before_parse()
     */
    void after_parse();

    /**
     * \brief Pushes the \c work_string substring [\c start_index, \c end_index) to \c output
     */
    void push_string();

    /**
     * \brief pushes special characters to \c output, does nothing on normal characters
     */
    void push_char(const QChar& c);

    /**
     * \brief Pushes a color to \c output (calls push_string())
     */
    void push_color(const QColor& color);

    /**
     * \brief Pushes a new line to \c output (calls push_string())
     */
    void push_line();

    /**
     * \brief Pushes the default color to \c output
     */
    void push_start();


    /**
     * \brief Cleans up \c push_start (calls push_string())
     */
    void push_end();


    QString     work_string;    ///< String currently being parsed
    QTextCursor *output=nullptr;
    int         start_index=0;  ///< Starting index for a new substring in \c work_string
    int         end_index=0;    ///< Ending index for a new substring in \c work_string
    QColor      default_color=Qt::gray; ///< Default text color
    int         min_brightness=0;       ///< Minimum color brightness
    int         max_brightness=255;     ///< Maximum color brightness

    static QRegExp regex_xoncolor;      ///< Regex matching color codes

};

} // namespace xonotic
#endif // XONOTIC_COLOR_PARSER_HPP
