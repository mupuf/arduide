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

    static QStringList boardIds();
    static const Board *boardInfo(const QString &name);

    QString attribute(const QString &attr) const;

private:
    static QMap<QString, Board> mBoards;
    static void listBoards();
    static bool mListed;

    QHash<QString, QString> mAttributes;
};

#endif // BOARD_H
