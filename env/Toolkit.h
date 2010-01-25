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
    static QString libraryPath(const QString &libraryName);
    static QString avrdudePath();
    static QStringList avrdudeFlags(const Board *board);

private:
    static const int ARDUINO_REVISION = 18;
};

#endif // TOOLKIT_H
