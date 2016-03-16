/*
  Builder.h

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
 * \file Builder.h
 * \author Denis Martinez
 */

#ifndef BUILDER_H
#define BUILDER_H

#include <QScopedPointer>
#include <qxttemporarydir.h>

#include "Board.h"
#include "ILogger.h"

/**
 * @brief Class to manage the compile process
 * 
 */
class Builder : public QObject
{
    Q_OBJECT
public:
    Builder(QObject *parent = NULL);
    
    /**
     * @brief Return board information to compile
     * 
     * @return const Board*
     */
    const Board *board() const;
    
    /**
     * @brief Return device for upload to the board
     * 
     * @return const QString
     */
    const QString device() const;
    
    /**
     * @brief Function that manage the build process
     * 
     * @param code Source that will be compiled
     * @param upload True to realize the upload process or False to compile only
     * @return bool, True if success or False if not
     */
    bool build(const QString &code, bool upload = false);

private:
    /**
     * @brief Read all file of a path
     * 
     * @param filepath Path to files
     * @return QString, Return file
     */
    QString readAllFile(const QString& filepath);
    
    /**
     * @brief Function to compile all dependencies
     * 
     * @param objects List of objects
     * @param code Source code
     * @param includePaths Path of include files
     * @param buildPath Build path
     * @param cflags C compiler flags 
     * @param cxxflags C++ compiler flags
     * @param sflags S flags
     * @return bool, True if success or False if not
     */
    bool compileDependencies(QStringList &objects, const QString& code, QStringList& includePaths, QString buildPath, const QStringList& cflags, const QStringList& cxxflags, const QStringList& sflags);
    
    /**
     * @brief Manage the compile process of main code
     * 
     * @param objects List of objects
     * @param sources Source code
     * @param includePaths Path of include files
     * @param cflags C compiler flags
     * @param cxxflags C++ compiler flags
     * @param sflags S compiler flags
     * @param outputDirectory Output directory of compile process
     * @return bool, True if success or False if note
     */
    bool compile(QStringList &objects, const QStringList &sources, const QStringList &includePaths, const QStringList &cflags, const QStringList &cxxflags, const QStringList &sflags, const QString &outputDirectory = QString());
    
    /**
     * @brief Enumeration source type
     * 
     */
    enum SourceType
    {
        CSource,
        CxxSource,
        SSource,
        UnknownSource
    };
    
    /**
     * @brief Todo
     * 
     * @param fileName Todo
     * @param objects Todo
     * @return bool
     */
    bool archive(const QString &fileName, const QStringList &objects);

    /**
     * @brief Function to link objects
     * 
     * @param fileName Filename
     * @param objects List of objects
     * @param ldflags Flags to the link process
     * @return bool, Return True if sucess if not False
     */
    bool link(const QString &fileName, const QStringList &objects, const QStringList &ldflags);
    
    /**
     * @brief Take size information of the compiled file
     * 
     * @param fileName Name of the file
     * @param sizeflags flags for the command
     * @return bool, Return True if sucess or if not False
     */
    bool size(const QString &fileName, const QStringList &sizeflags);
    
    /**
     * @brief Extract EEPROM code 
     * 
     * @param input Principal code
     * @param output EEPROM part
     * @return bool, Return True if sucess or if not False
     */
    bool extractEEPROM(const QString &input, const QString &output);
    
    /**
     * @brief Extract HEX 
     * 
     * @param input Principal code
     * @param output Code in hexadecimal
     * @return bool, Return True if sucess or if not False
     */
    bool extractHEX(const QString &input, const QString &output);
    
    /**
     * @brief Upload the hexa to the board
     * 
     * @param hexFileName Name of the file with the compiled code
     * @return bool, Return True if sucess or if not False
     */
    bool uploadViaBootloader(const QString &hexFileName);
    
    /**
     * @brief Identify type of code, C,C++ and S
     * 
     * @param fileName Name of the file
     * @return Builder::SourceType, Type of the file
     */
    SourceType identifySource(const QString &fileName);
    
    /**
     * @brief Run a command
     * 
     * @param command Command
     * @param errorHighlighting Show error
     * @return int, Return the return of the command
     */
    int runCommand(const QStringList &command, bool errorHighlighting=false);

    /**
     * @brief Build directory
     * 
     */
    QScopedPointer<QxtTemporaryDir> mBuildDir;

signals:
    void logCommand(QStringList);
    void logImportant(QString);
    void logError(QString);
    void log(QString);
};

#include <QThread>
#include <QActionGroup>

/**
 * @brief Class the realize the background compilation
 * 
 */
class BackgroundBuilder : public QThread
{
    Q_OBJECT

public:
    BackgroundBuilder(QObject *parent = NULL);
    
    /**
     * @brief Set actions
     * 
     * @param actions Actions
     * @return void
     */
    void setRelatedActions(QActionGroup *actions);
    
    /**
     * @brief Realize the build process in background
     * 
     * @param code Code
     * @param upload If True the upload process will be performed 
     * @return void
     */    
    void backgroundBuild(const QString &code, bool upload = false);
    
    /**
     * @brief Start the build process
     * 
     * @return void
     */
    void run();

signals:
    void buildFinished(bool ok);
    void logCommand(QStringList);
    void logImportant(QString);
    void logError(QString);
    void log(QString);

private:
    /**
     * @brief Perform the build process
     * 
     */
    Builder builder;
    
    
    /**
     * @brief Actions
     * 
     */
    QActionGroup *actions;

    /**
     * @brief Main code
     * 
     */
    QString code;
    
    /**
     * @brief True to start upload process
     * 
     */
    bool upload;

};

#endif // BUILDER_H
