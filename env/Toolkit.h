/*
  Toolkit.h

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
    /**
     * @brief Find sketches directory
     * 
     * @param directory Folder location
     * @return QStringList
     */
    static QStringList findSketchesInDirectory(const QString &directory);
    
    /**
     * @brief Return examples categories
     * 
     * @return QStringList
     */
    static QStringList findExampleCategories();
    
    /**
     * @brief Find examples of category
     * 
     * @param category Category
     * @return QStringList
     */
    static QStringList findExamples(const QString &category);
    
    /**
     * @brief Return name of example file
     * 
     * @param category Category
     * @param example Example
     * @return QString
     */
    static QString exampleFileName(const QString &category, const QString &example);
    
    /**
     * @brief Return reference path
     * 
     * @return QString
     */
    static QString referencePath();
    
    /**
     * @brief Return hardware path
     * 
     * @return QString
     */
    static QString hardwarePath();
    
    /**
     * @brief Return the path of all boards.txt
     * 
     * @return QStringList
     */
    static QStringList boardsFileNames();
    
    /**
     * @brief Return the local of keywords.txt
     * 
     * @return QString
     */
    static QString keywordsFileName();
    
    /**
     * @brief Return version of toolkit
     * 
     * @param path toolkit path
     * @return QString
     */
    static QString toolkitVersion(const QString &path);
    
    /**
     * @brief Return version of toolkit 
     * 
     * @param path toolkit path
     * @return int
     */
    static int toolkitVersionInt(const QString &path);
    
    /**
     * @brief Check if path is a valid arduino path
     * 
     * @param path Arduino path
     * @return bool
     */
    static bool isValidArduinoPath(const QString &path);
    
    /**
     * @brief Check if path is a valid arduino path
     * 
     * @return QString
     */
    static QString avrPath();

    enum AVRTool
    {
        AvrGcc,
        AvrGxx,
        AvrAr,
        AvrObjcopy,
        AvrSize
    };
    
    /**
     * @brief Return the avrtoll. e.g: avr-g++, avr-size...
     * 
     * @param tool AVRtool (AvrGcc,AvrGcc,AvrAr,AvrObjcopy,Avrsize)
     * @return QString
     */
    static QString avrTool(AVRTool tool);
    
    /**
     * @brief Return C flags of board
     * 
     * @param board Board
     * @return QStringList
     */
    static QStringList avrCFlags(const Board *board);
    
    /**
     * @brief Return c++ flags of board 
     * 
     * @param board Board
     * @return QStringList
     */
    static QStringList avrCxxFlags(const Board *board);
    
    /**
     * @brief Return S flags
     * 
     * @param board Board
     * @return QStringList
     */
    static QStringList avrSFlags(const Board *board);
    
    /**
     * @brief Return LD flags
     * 
     * @param board Board
     * @return QStringList
     */
    static QStringList avrLdFlags(const Board *board);
    
    /**
     * @brief Return avr-size flags
     * 
     * @param board Board
     * @return QStringList
     */
    static QStringList avrSizeFlags(const Board *board);
    
    /**
     * @brief Return core path
     * 
     * @param board Board
     * @return QString
     */
    static QString corePath(const Board *board);
    
    /**
     * @brief Return the IDE LIbraries
     * 
     * @return QStringList
     */
    static QStringList IDELibraries();
    
    /**
     * @brief Return the user libraries
     * 
     * @return QStringList
     */
    static QStringList userLibraries();
    
    /**
     * @brief Return the arduino libraries
     * 
     * @return QStringList
     */
    static QStringList arduinoLibraries();
    
    /**
     * @brief Return all libraries 
     * 
     * @return QStringList
     */
    static QStringList libraries();
    
    /**
     * @brief Return IDE library path
     * 
     * @return QString
     */
    static QString IDELibraryPath();
    
    /**
     * @brief Return user library path
     * 
     * @return QString
     */
    static QString userLibraryPath();
    
    /**
     * @brief Return all libraries path
     * 
     * @param libraryName ...
     * @return QString
     */
    static QString libraryPath(const QString &libraryName = QString());
    
    /**
     * @brief Return keywords.txt of library
     * 
     * @param libraryName Library name
     * @return QString
     */
    static QString libraryKeywordsFileName(const QString &libraryName);
    
    /**
     * @brief Return a list of libraries of examples
     * 
     * @return QStringList
     */
    static QStringList librariesWithExamples();
    
    /**
     * @brief Find Examples of library
     * 
     * @param library ...
     * @return QStringList
     */
    static QStringList findLibraryExamples(const QString &library);
    
    /**
     * @brief Return the name of example
     * 
     * @param library Library
     * @param example Example
     * @return QString
     */
    static QString libraryExampleFileName(const QString &library, const QString &example);
    
    /**
     * @brief Return the avrdude path
     * 
     * @return QString
     */
    static QString avrdudePath();
    
    /**
     * @brief Return avrdude flags
     * 
     * @param board Board
     * @return QStringList
     */
    static QStringList avrdudeFlags(const Board *board);
    
    /**
     * @brief Run avrdude
     * 
     * @return bool, Return True if success
     */
    static bool avrdudeSystem();
};

#endif // TOOLKIT_H
