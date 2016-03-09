/*
  Board.h

  This file is part of arduide, The Qt-based IDE for the open-source Arduino electronics prototyping platform.

  Copyright (C) 2010-2012 
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

class Board
{
public:
    const QString& name() const;
    const QString& mcu() const;

    static QStringList boardIds();
    static const Board *boardInfo(const QString &name);

    QString hardwarePath() const { return mHardwarePath; };

    QString attribute(const QString &attr) const;
    
    QHash<QString, QString> mAttributes;
private:
    static QMap<QString, Board> mBoards;
    static void listBoards();
    static bool mListed;

    QString mHardwarePath;
};

#endif // BOARD_H
