/**
 * \file LexerArduino.h
 * \author Denis Martinez
 */

#ifndef LEXERARDUINO_H
#define LEXERARDUINO_H

#include <string>

#include <Qsci/qscilexercpp.h>

class LexerArduino : public QsciLexerCPP
{
public:
    LexerArduino(QObject *parent = NULL, bool caseInsensitiveKeywords = false);
    const char *keywords(int set) const;

private:
    void loadKeywords();
    void loadKeywordsFile(const QString &fileName);

    static std::string mKeywords[3];
};

#endif // LEXERARDUINO_H
