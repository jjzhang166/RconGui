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
#ifndef XONOTIC_PLAYER_MODEL_HPP
#define XONOTIC_PLAYER_MODEL_HPP

#include <vector>

#include <QAbstractTableModel>

#include "player.hpp"

namespace xonotic {

/**
 * \brief Model for the player status
 */
class PlayerModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum ColumnNames {
        Ip      = 0,
        Name    = 1,
        Entity  = 2,
        Ping    = 3,
        Pl      = 4,
        Score   = 5,
        Time    = 6,
        Actions = 7
    };

    int rowCount(const QModelIndex & = {}) const override
    {
        return players.size();
    }

    int columnCount(const QModelIndex & = {}) const override
    {
        return 8;
    }

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

public slots:
    /**
     * \brief Sets the players
     */
    void set_players(const std::vector<Player>& player_list)
    {
        beginResetModel();
        players = player_list;
        endResetModel();
        emit players_changed(players);
    }

    /**
     * \brief Removes all stored cvars
     */
    void clear()
    {
        beginResetModel();
        players.clear();
        endResetModel();
        emit players_changed(players);
    }

signals:
    /**
     * \brief Emitted after the model has changed
     */
    void players_changed(const std::vector<Player>& players);

private:
    std::vector<Player> players; ///< list of players

};
} // namespace xonotic

#endif // XONOTIC_PLAYER_MODEL_HPP
