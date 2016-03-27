/*
  Board.cpp

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
 * \file Board.cpp
 * \author Denis Martinez
 */

#include "Board.h"

#include <QFile>
#include <QDir>
#include <QDebug>

#include "Toolkit.h"

QMap<QString, Board> Board::mBoards;
bool Board::mListed = false;

const QString& Board::name() const
{
    QHash<QString, QString>::const_iterator it = mAttributes.constFind("name");
    if (it != mAttributes.end())
        return *it;
    else
    {
        static QString nullString;
        return nullString;
    }
}

void Board::listBoards()
{
    if (! mListed)
    {
        foreach(QString boardFile, Toolkit::boardsFileNames()) {
            QFile boardsFile(boardFile);
            boardsFile.open(QFile::ReadOnly);

            QTextStream boardsFileUTF8(&boardsFile);
            boardsFileUTF8.setCodec("UTF-8");

            while (! boardsFileUTF8.atEnd())
            {
                QString line = boardsFileUTF8.readLine().trimmed();

                if (line.isEmpty() || line[0] == '#')
                    continue;

                QString attrName = line.section('=', 0, 0);
                QString attrValue = line.section('=', 1);

                // attrName = <product>.<attrName>
                QString productId = attrName.section('.', 0, 0);
                attrName = attrName.section('.', 1);
                Board &board = mBoards[productId];

                //it does seem pretty odd that they have build.mcu as atmegang which isn't a valid mcu, and then the cpu submenu is being used as the mcu.
                if(!attrValue.contains("atmegang"))
                    board.mAttributes[attrName] = attrValue;
                board.mHardwarePath = QFileInfo(boardFile).dir().absolutePath();

                if(attrName.contains("menu.cpu") and attrName.contains("build.mcu"))
                {
                    if(board.mAttributes.contains("build.mcu"))
                    {
                        if(!board.mAttributes["build.mcu"].contains(attrValue))
                            board.mAttributes["build.mcu"]=board.mAttributes["build.mcu"]+","+attrValue;
                    }
                    else
                        board.mAttributes["build.mcu"]=attrValue;
                }

                if(attrName.contains("menu.cpu") and attrName.contains("build.f_cpu"))
                {
                    if(board.mAttributes.contains("build.f_cpu"))
                    {
                        if(!board.mAttributes["build.f_cpu"].contains(attrValue))
                            board.mAttributes["build.f_cpu"]=board.mAttributes["build.f_cpu"]+","+attrValue;
                    }
                    else
                        board.mAttributes["build.f_cpu"]=attrValue;
                }
            }

            boardsFile.close();
            mBoards.erase(mBoards.find("menu"));
            mListed = true;
        }
    }
}

QStringList Board::boardIds()
{
    listBoards();
    return mBoards.keys();
}

const Board *Board::boardInfo(const QString &name)
{
    listBoards();
    const Board *board = NULL;
    QMap<QString, Board>::const_iterator it = mBoards.constFind(name);
    if (it != mBoards.end())
        board = &*it;
    return board;
}

QString Board::attribute(const QString &attr) const
{
    QHash<QString, QString>::const_iterator it = mAttributes.constFind(attr);
    if (it == mAttributes.end())
        return QString();
    else
        return *it;
}

void Board::setSelectedBoard(QString _name, QString _mcu, QString _freq)
{
    name_ = _name;
    mcu_  = _mcu;
    freq_ = _freq;
}

QString Board::selectedName() const
{
    return name_;
}

QString Board::selectedMcu() const
{
    return mcu_;
}

QString Board::selectedFreq() const
{
    return freq_;
}