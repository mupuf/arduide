/*
  Board.h

  This file is part of arduide, The Qt-based IDE for the open-source Arduino electronics prototyping platform.

  Copyright (C) 2010-2016
  Authors : Denis Martinez
	    Martin Peres

This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * \file Board.h
 * \author Denis Martinez
 */

#ifndef BOARD_H
#define BOARD_H

#include <QString>
#include <QStringList>
#include <QHash>
#include <QMap>

/**
 * @brief A class to help deal with boards.txt
 *
 */

class Board
{
public:

    /**
    * @brief Return the name of the board
    * e.g: uno.name="Arduino Uno"
    *
    * @return const QString&
    */
    const QString& name() const;


    /**
     * @brief Return all Ids of the boards in boards.txt
     *
     * @return QStringList
     */
    static QStringList boardIds();


    /**
     * @brief Return a pointer to Board class with the board information.
     *
     * @param name The board name. e.g: "uno" (uno.name=Arduino Uno)
     * @return const Board*
     */
    static const Board *boardInfo(const QString &name);

    /**
     * @brief Return the path of hardware directory
     *
     * @return QString
     */
    QString hardwarePath() const { return mHardwarePath; };

    /**
     * @brief Return the value of an attribute
     *
     * @param attr attribute name. e.g: "name" (uno.name=Arduino Uno)
     * @return QString
     */
    QString attribute(const QString &attr) const;

    /**
     * @brief Stores all board's attribute
     *
     */
    QHash<QString, QString> mAttributes;

    /**
     * @brief save all boards in mBoards
     *
     */
    static QMap<QString, Board> mBoards;

    /**
     * @brief Set info about selected board
     *
     * @param name name of selected board
     * @param mcu mcu of selected board
     * @param freq freq of selected mcu
     * @return void
     */
    void setSelectedBoard(QString _name, QString _mcu, QString _freq);

    /**
     * @brief Return selected board name
     *
     * @return QString
     */
    QString selectedName() const;

    /**
     * @brief Return selected mcu of the board
     *
     * @return QString
     */

    QString selectedMcu() const;

    /**
     * @brief Return selected freq of mcu
     *
     * @return QString
     */
    QString selectedFreq() const;

private:
    /**
     * @brief Useful info to identify the selected board
     *
     */
    QString name_,mcu_,freq_;

    /**
    * @brief Function that read boards.txt to identify all compatible boards
    *
    * @return void
    */
    static void listBoards();


    /**
     * @brief Check if boards.txt has already been read
     *
     */
    static bool mListed;

    QString mHardwarePath;
};

#endif // BOARD_H
