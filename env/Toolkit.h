/**
 * \file Toolkit.h
 * \author Denis Martinez
 */

#ifndef TOOLKIT_H
#define TOOLKIT_H

#include "Settings.h"

class Board;

class Toolkit
{
public:
    static QStringList findSketchesInDirectory(const QString &directory);
    static QStringList findExampleCategories();
    static QStringList findExamples(const QString &category);
    static QString exampleFileName(const QString &category, const QString &example);
    static QString referencePath();
    static QString hardwarePath();
    static QString boardsFileName();
    static QString keywordsFileName();
    static bool isValidArduinoPath(const QString &path);
    static QString avrPath();

    enum AVRTool
    {
        AvrGcc,
        AvrGxx,
        AvrAr,
        AvrObjcopy
    };
    static QString avrTool(AVRTool tool);
    static QStringList avrCFlags(const Board *board);
    static QStringList avrCxxFlags(const Board *board);
    static QStringList avrSFlags(const Board *board);
    static QStringList avrLdFlags(const Board *board);
    static QString corePath(const Board *board);
    static QStringList libraries();
    static QString libraryPath(const QString &libraryName = QString());
    static QString libraryKeywordsFileName(const QString &libraryName);
    static QStringList librariesWithExamples();
    static QStringList findLibraryExamples(const QString &library);
    static QString libraryExampleFileName(const QString &library, const QString &example);
    static QString avrdudePath();
    static QStringList avrdudeFlags(const Board *board);

private:
    static const int ARDUINO_REVISION = 18;
};

#endif // TOOLKIT_H
