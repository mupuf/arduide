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
