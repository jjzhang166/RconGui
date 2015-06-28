/*
 * Copyright (C) 2012-2015 Mattia Basaglia
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
#ifndef HISTORY_LINE_EDIT_HPP
#define HISTORY_LINE_EDIT_HPP

#include <QLineEdit>

class HistoryLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit HistoryLineEdit(QWidget *parent = 0);

    /**
     * \brief Number of available lines
     */
    int lineCount() const { return lines.size(); }

    /**
     * \brief Stored history
     */
    QStringList history() const { return lines; }

    /**
     * \brief Overwrite the line history
     */
    void setHistory(const QStringList& history);

public slots:
    /**
     * \brief Executes the current line
     */
    void execute();

signals:
    /**
     * \brief Emitted when some text is executed
     */
    void lineExecuted(QString);

protected:
    void keyPressEvent(QKeyEvent *);
    void wheelEvent(QWheelEvent *);

    void previous_line();
    void next_line();

private:
    int         current_line;
    QStringList lines;
    QString     unfinished;

};

#endif // HISTORY_LINE_EDIT_HPP
