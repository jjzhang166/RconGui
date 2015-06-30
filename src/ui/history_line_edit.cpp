/**
 * \file
 *
 * \author Mattia Basaglia
 *
 * \section License
 *
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

#include <QCompleter>
#include <QAbstractItemView>
#include <QScrollBar>

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
        return;
    }
    else if ( ev->key() == Qt::Key_Down )
    {
        next_line();
        return;
    }
    else if (completer && completer->popup() && completer->popup()->isVisible())
    {
        switch (ev->key())
        {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_F4:
            case Qt::Key_Select:
                completer->popup()->hide();
                return;
        }
    }

    QLineEdit::keyPressEvent(ev);

    if (completer)
    {
        QString current = current_word();
        completer->setCompletionPrefix(current);
        if ( current.isEmpty() )
        {
            completer->popup()->hide();
        }
        else
        {
            int c = cursorPosition();
            setCursorPosition(word_start());
            QRect rect = cursorRect();
            setCursorPosition(c);
            rect.setWidth(
                completer->popup()->sizeHintForColumn(0)
                + completer->popup()->verticalScrollBar()->sizeHint().width()
            );
            completer->complete(rect);
        }
    }
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
    if ( lines.empty() || lines.back() != text() )
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

int HistoryLineEdit::word_start() const
{
    // lastIndexOf returns the index of the last space or -1 if there are no spaces
    // so that + 1 returns the index of the character starting the word or 0
    auto after_space = text().leftRef(cursorPosition()).lastIndexOf(' ') + 1;
    if ( text().rightRef(text().size()-after_space).startsWith(completion_prefix) )
        after_space += completion_prefix.size();
    return after_space;
}

QString HistoryLineEdit::current_word() const
{
    int completion_index = word_start();
    return text().mid(completion_index, cursorPosition() - completion_index);
}

void HistoryLineEdit::autocomplete(const QString& completion)
{
    int completion_index = word_start();
    setText(text().replace(
        completion_index, cursorPosition() - completion_index,
        completion
    ));
    setCursorPosition(completion_index+completion.size());
}

void HistoryLineEdit::setWordCompleter(QCompleter* comp)
{
    if ( completer )
    {
        disconnect(completer, 0, this, 0);
        completer->setWidget(nullptr);
    }

    completer = comp;

    if ( comp )
    {
        /// \todo should set these only when on focus
        connect(completer, SIGNAL(activated(QString)),
                this,      SLOT(autocomplete(QString)));
        connect(completer, SIGNAL(highlighted(QString)),
                this,      SLOT(autocomplete(QString)));
        completer->setWidget(this);
    }
}

void HistoryLineEdit::setWordCompleterPrefix(const QString& prefix)
{
    completion_prefix = prefix;
}
