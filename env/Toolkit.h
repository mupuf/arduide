/*
  Toolkit.h

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
    static QStringList boardsFileNames();
    static QString keywordsFileName();
    static QString toolkitVersion(const QString &path);
    static bool isValidArduinoPath(const QString &path);
    static QString avrPath();

    enum AVRTool
    {
        AvrGcc,
        AvrGxx,
        AvrAr,
        AvrObjcopy,
        AvrSize
    };
    static QString avrTool(AVRTool tool);
    static QStringList avrCFlags(const Board *board);
    static QStringList avrCxxFlags(const Board *board);
    static QStringList avrSFlags(const Board *board);
    static QStringList avrLdFlags(const Board *board);
    static QStringList avrSizeFlags(const Board *board);
    static QString corePath(const Board *board);
    static QStringList IDELibraries();
    static QStringList userLibraries();
    static QStringList arduinoLibraries();
    static QStringList libraries();
    static QString IDELibraryPath();
    static QString userLibraryPath();
    static QString libraryPath(const QString &libraryName = QString());
    static QString libraryKeywordsFileName(const QString &libraryName);
    static QStringList librariesWithExamples();
    static QStringList findLibraryExamples(const QString &library);
    static QString libraryExampleFileName(const QString &library, const QString &example);
    static QString avrdudePath();
    static QStringList avrdudeFlags(const Board *board);
    static bool avrdudeSystem();
};

#endif // TOOLKIT_H
