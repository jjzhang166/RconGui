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
#ifndef QDARKPLACES_HPP
#define QDARKPLACES_HPP

#include <QObject>
#include "darkplaces.hpp"

namespace xonotic {

class QDarkplaces : public QObject, public Darkplaces
{
    Q_OBJECT
public:
    explicit QDarkplaces(xonotic::ConnectionDetails details, QObject* parent = nullptr)
        : QObject(parent), Darkplaces(std::move(details)) {}

public slots:
    bool xonotic_connect() { return Darkplaces::connect(); }
    void xonotic_disconnect() { return Darkplaces::disconnect(); }
    bool xonotic_reconnect() { return Darkplaces::reconnect(); }

signals:
    /**
     * \brief Emitted on a successful connection (from the main thread)
     */
    void connected();

    /**
     * \brief Emitted on a disconnection (from the main thread or the network thread)
     */
    void disconnected();

    /**
     * \brief Emitted before a disconnection happens (from the main thread or the network thread)
     * \note If the connection is closed due to a network error, this might not be called
     */
    void disconnecting();


    /**
     * \brief Emitted when received a datagram containing some log
     *
     * Will follow several calls to on_receive_log() (one per log line)
     * and on_log_end()
     */
    void log_begin();

    /**
     * \brief Emitted when received a datagram containing some log
     *
     * After on_log_begin() and on_receive_log()
     */
    void log_end();

    /**
     * \brief Emitted on a log line (from the network thread)
     */
    void received_log(const QString& line);

    /**
     * \brief Emitted on a connection error (from the network thread)
     */
    void connection_error(const QString& message);

protected:
    void on_connect() override { emit connected(); }

    void on_disconnect() override { emit disconnecting(); }
    void on_disconnecting() override { emit disconnected(); }

    void on_log_begin() override { emit log_begin(); }
    void on_log_end() override { emit log_end(); }

    void on_receive_log(const std::string& line) override
    {
        emit received_log(QString::fromStdString(line));
    }

    // on_receive

    void on_network_error(const std::string& msg) override
    {
        emit connection_error(QString::fromStdString(msg));
    }

private:
    using Darkplaces::connect;
    using Darkplaces::disconnect;
};

} // namespace xonotic
#endif // QDARKPLACES_HPP
