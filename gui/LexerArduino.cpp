/*
  LexerArduino.cpp

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
 * \file LexerArduino.cpp
 * \author Denis Martinez
 */

#include "LexerArduino.h"

#include <QFile>
#include <QRegExp>
#include <QDebug>

#include "../env/Toolkit.h"

std::string LexerArduino::mKeywords[3];

LexerArduino::LexerArduino(QObject *parent, bool caseInsensitiveKeywords)
    : QsciLexerCPP(parent, caseInsensitiveKeywords)
{
    loadKeywords();
}

const char *LexerArduino::keywords(int set) const
{
    if (set < 0 || set >= 3)
        return NULL;

    return mKeywords[set].c_str();
}

void LexerArduino::loadKeywords()
{
    static bool loaded = false;
    if (loaded)
        return;

    for (int i = 0; i < 3; i++)
        mKeywords[i].clear();

    loadKeywordsFile(Toolkit::keywordsFileName());
    QStringList libraries = Toolkit::libraries();
    foreach(const QString &library, libraries)
        loadKeywordsFile(Toolkit::libraryKeywordsFileName(library));

    loaded = true;
}

void LexerArduino::loadKeywordsFile(const QString &fileName)
{
    QFile keywordsFile(fileName);
    keywordsFile.open(QFile::ReadOnly);
    QString line;
    QStringList pieces;
    QStringList k[3];
    while (! keywordsFile.atEnd())
    {
        line = QString::fromLocal8Bit(keywordsFile.readLine()).trimmed();
        if (line.isEmpty() || line[0] == '#')
            continue;
        pieces = line.split(QRegExp("\\s"), QString::SkipEmptyParts);
        if (pieces.size() < 2)
            continue;
        if (pieces[1] == "LITERAL1" || pieces[1] == "KEYWORD1")
            k[0] << pieces[0];
        else if (pieces[1] == "KEYWORD2")
            k[1] << pieces[0];
        else if (pieces[1] == "KEYWORD3")
            k[2] << pieces[0];
    }

    for (int i = 0; i < 3; i++)
    {
        if (! mKeywords[i].empty())
            mKeywords[i] += ' ';
        mKeywords[i] += k[i].join(" ").toStdString();
    }
}
