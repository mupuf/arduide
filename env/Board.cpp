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
        QFile boardsFile(Toolkit::boardsFileName());
        boardsFile.open(QFile::ReadOnly);

        while (! boardsFile.atEnd())
        {
            QString line = QString::fromLocal8Bit(boardsFile.readLine()).trimmed();
            if (line.isEmpty() || line[0] == '#')
                continue;

            QString attrName = line.section('=', 0, 0);
            QString attrValue = line.section('=', 1);

            // attrName = <product>.<attrName>
            QString productId = attrName.section('.', 0, 0);
            attrName = attrName.section('.', 1);

            Board &board = mBoards[productId];
            board.mAttributes[attrName] = attrValue;
        }

        boardsFile.close();

        mListed = true;
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
