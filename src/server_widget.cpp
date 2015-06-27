/*
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

#include "server_widget.hpp"

#include <QMenu>
#include <QScrollBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextObject>

#include "create_server_dialog.hpp"
#include "settings.hpp"

ServerWidget::ServerWidget(network::Xonotic xonotic, QWidget* parent)
    : QWidget(parent), xonotic(std::move(xonotic))
{
    setupUi(this);

    button_refresh->setShortcut(QKeySequence::Refresh);

    clear_log();

    io.max_datagram_size(1400);
    io.on_error = [this](const std::string& msg)
    {
        emit network_error(QString::fromStdString(msg));
        xonotic_close_connection();
    };
    io.on_async_receive = [this](const std::string& datagram)
    {
        xonotic_read(datagram);
    };
    io.on_failure = [this]()
    {
        xonotic_disconnect();
    };

    xonotic_connect();

    connect(this, &ServerWidget::log_received,
            this, &ServerWidget::append_log, Qt::QueuedConnection);

    connect(action_clear_log, &QAction::triggered, this, &ServerWidget::clear_log);

}

ServerWidget::~ServerWidget()
{
    xonotic_disconnect();
}

void ServerWidget::clear_log()
{
    output_console->clear();
    QTextFrameFormat fmt;
    fmt.setBackground(settings().console_background);
    output_console->document()->rootFrame()->setFrameFormat(fmt);
    output_console->document()->setDefaultFont(settings().console_font);
    output_console->setTextColor(settings().console_foreground);
}

void ServerWidget::xonotic_close_connection()
{

    if ( io.connected() )
        io.disconnect();
    if ( thread_input.joinable() &&
            thread_input.get_id() != std::this_thread::get_id() )
        thread_input.join();
}

void ServerWidget::xonotic_disconnect()
{
    // if ( io.connected() )
        /// \todo clear log_dest_udp if needed
    xonotic_close_connection();
    xonotic_clear();
}

bool ServerWidget::xonotic_connect()
{
    if ( !io.connected() )
    {
        if ( io.connect(xonotic.server) )
        {
            if ( thread_input.get_id() == std::this_thread::get_id() )
                return false;

            if ( thread_input.joinable() )
                thread_input.join();

            thread_input = std::move(std::thread([this]{
                xonotic_clear();
                xonotic_request_status();
                io.run_input();
            }));
        }
    }

    return io.connected();
}

bool ServerWidget::xonotic_reconnect()
{
    xonotic_disconnect();
    return xonotic_connect();
}

void ServerWidget::xonotic_clear()
{
    /// \todo
    // Lock lock(mutex);
    // clear rcon secure 2 buffer
    // clear status and cvars
    // clear cvars
}

void ServerWidget::xonotic_request_status()
{
    rcon_command("status 1");
}

bool ServerWidget::xonotic_connected()
{
    return io.connected();
}

void ServerWidget::xonotic_read(const std::string& datagram)
{

    if ( datagram.size() < 5 || datagram.substr(0,4) != "\xff\xff\xff\xff" )
    {
        network_error(tr("Invalid datagram: %1")
            .arg(QString::fromStdString(datagram)));
        return;
    }

    // discard non-log/rcon output
    if ( datagram[4] != 'n' )
        return;

    Lock lock(mutex);
    std::istringstream socket_stream(line_buffer+datagram.substr(5));
    line_buffer.clear();
    lock.unlock();

    // convert the datagram into lines
    std::string line;
    while (socket_stream)
    {
        std::getline(socket_stream,line);
        if (socket_stream.eof())
        {
            if (!line.empty())
            {
                lock.lock();
                line_buffer = line;
                lock.unlock();
            }
            break;
        }
        emit log_received(QString::fromUtf8(line.data(), line.size()));
    }
}


QString ServerWidget::name() const
{
    return QString::fromStdString(xonotic.name);
}

void ServerWidget::on_button_setup_clicked()
{
    CreateServerDialog dlg(xonotic, this);
    if ( dlg.exec() )
    {
        auto oldxon = xonotic;
        xonotic = dlg.connection_info();

        if ( oldxon.server != xonotic.server )
            xonotic_reconnect();

        if ( xonotic.name != oldxon.name )
            emit name_changed(QString::fromStdString(xonotic.name));
    }
}

void ServerWidget::on_output_console_customContextMenuRequested(const QPoint &pos)
{
    QMenu* menu = output_console->createStandardContextMenu();

    menu->addSeparator();
    menu->addAction(action_save_log);
    menu->addAction(action_clear_log);

    menu->addSeparator();
    menu->addAction(action_attach_log);
    menu->addAction(action_detach_log);
    menu->addAction(action_parse_colors);

    menu->exec(output_console->mapToGlobal(pos));
}

void ServerWidget::rcon_command(const std::string& command)
{
    xonotic_write("rcon "+xonotic.rcon_password+' '+command);
}

void ServerWidget::xonotic_write(std::string line)
{
    line.erase(std::remove_if(line.begin(), line.end(),
        [](char c){return c == '\n' || c == '\0' || c == '\xff';}),
        line.end());

    io.write(header+line);
}

QColor ServerWidget::xonotic_color ( const QString& s )
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
        return Qt::gray;

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

    return Qt::gray;
}

void ServerWidget::append_log(const QString& log)
{
    auto scrollbar = output_console->verticalScrollBar();
    bool scroll = scrollbar->value() == scrollbar->maximum();

    if ( !action_parse_colors->isChecked() )
    {
        output_console->append(log);
    }
    else
    {
        QTextCursor cursor(output_console->document());
        cursor.movePosition(QTextCursor::End);
        QTextCharFormat format;
        QString text;
        static QRegExp regex_xoncolor("^([0-9]|x[0-9a-fA-F]{3})");
        format.setForeground(settings().console_foreground);
        for ( int i = 0; i < log.size(); i++ )
        {
            if ( log[i] == '^' && i < log.size()-1 )
            {
                i++;
                if ( log[i] == '^' )
                {
                    text += '^';
                }
                else if ( regex_xoncolor.indexIn(log, i, QRegExp::CaretAtOffset) != -1 )
                {
                    cursor.insertText(text,format);
                    i += regex_xoncolor.matchedLength()-1;
                    auto color = xonotic_color(regex_xoncolor.cap());
                    format.setForeground(QColor::fromHsl(
                        color.hue(),
                        color.saturation(),
                        qBound(settings().console_brightness_min,
                               color.lightness(),
                               settings().console_brightness_max)
                    ));
                    text.clear();
                }
                else
                {
                    text += '^';
                    text += log[1];
                }
            }
            /// \todo qfont
            else
            {
                text += log[i];
            }
        }
        cursor.insertText(text,format);
        format.setForeground(settings().console_foreground);
        cursor.insertText("\n",format);
    }

    if ( scroll )
        scrollbar->setValue(scrollbar->maximum());
}

void ServerWidget::on_action_attach_log_triggered()
{
    rcon_command("log_dest_udp "+io.local_endpoint().name());
}

void ServerWidget::on_action_detach_log_triggered()
{
    rcon_command("log_dest_udp \"\"");
}

void ServerWidget::on_action_save_log_triggered()
{
    QStringList filters = {
        tr("Text files (*.log *.txt)"),
        tr("HTML (*.htm *.html)"),
        tr("All files (*)"),
    };

    // static so that evey instance points to the same
    static QString directory;

    QFileDialog dialog(this, tr("Save Log"), directory);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilters(filters);

    if ( !dialog.exec() )
        return;

    QString filename = dialog.selectedFiles()[0];
    directory = QFileInfo(filename).dir().path();

    QFile file(filename);

    if ( !file.open(QIODevice::WriteOnly|QIODevice::Text) )
    {
        QMessageBox::warning(this,tr("File Error"),
            tr("Could not write to \"%1\".").arg(file.fileName()));
        return;
    }

    if ( filters.indexOf(dialog.selectedNameFilter()) == 1 ) // html
        file.write(output_console->document()->toHtml("utf-8").toUtf8());
    else
        file.write(output_console->toPlainText().toUtf8());

}
