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
#include "history_line_edit.hpp"

#include <QKeyEvent>
#include <QWheelEvent>

HistoryLineEdit::HistoryLineEdit(QWidget *parent) :
    QLineEdit(parent), current_line(0)
{
    connect(this,&HistoryLineEdit::returnPressed,this,&HistoryLineEdit::execute);
}

void HistoryLineEdit::keyPressEvent(QKeyEvent * ev)
{
    if ( ev->key() == Qt::Key_Up )
    {
        previous_line();
    }
    else if ( ev->key() == Qt::Key_Down )
    {
        next_line();
    }
    else
        QLineEdit::keyPressEvent(ev);
}

void HistoryLineEdit::wheelEvent(QWheelEvent *ev )
{
    if ( ev->delta() > 0 )
        previous_line();
    else
        next_line();
}

void HistoryLineEdit::previous_line()
{
    if ( lines.empty() )
        return;

    if ( !text().isEmpty() &&
         ( current_line >= lines.size() || text() != lines[current_line] ) )
        unfinished = text();

    if ( current_line > 0 )
        current_line--;

    setText(lines[current_line]);
}


void HistoryLineEdit::next_line()
{
    if ( lines.empty() )
        return;

    current_line++;

    if ( current_line >= lines.size() )
    {
        setText(unfinished);
        unfinished = "";
        current_line = lines.size();
    }
    else
        setText(lines[current_line]);
}

void HistoryLineEdit::execute()
{
    lines << text();
    current_line = lines.size();
    clear();
    emit lineExecuted(lines.back());
}

void HistoryLineEdit::setHistory(const QStringList& history)
{
    lines = history;
    current_line = lines.size();
}
