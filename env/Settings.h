/*
  Settings.h

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
 * \file Settings.h
 * \author Denis Martinez
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QStringList>
#include <QFont>

#include "gui/LexerArduino.h"
#include "gui/Editor.h"
#include "IDEGlobal.h"

class IDE_EXPORT Settings : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Return the location of arduino folder
     * 
     * @return QString
     */
    QString arduinoPath() const;
    
    
    /**
     * @brief Set the location of arduino folder
     * 
     * @param path The arduino folder location
     * @return void
     */
    void setArduinoPath(const QString &path);
    
    /**
     * @brief Return the location of sketch folder
     * 
     * @return QString
     */
    QString sketchPath() const;
    
    /**
     * @brief Set the location of sketch folder
     * 
     * @param path the sketch folder location
     * @return void
     */
    void setSketchPath(const QString &path);
    
    /**
     * @brief Return the device port
     * 
     * @return QString
     */
    QString devicePort() const;
    
    /**
     * @brief Set device port
     * 
     * @param port Port of device
     * @return void
     */
    void setDevicePort(const QString &port);
    
    /**
     * @brief Return the board value
     * 
     * @return QString
     */
    QString board() const;
    
    /**
     * @brief Set the board used
     * 
     * @param board Board
     * @return void
     */
    void setBoard(const QString &board);
    
    /**
     * @brief Read the value of filterSerialDevices
     * 
     * @return bool
     */
    bool filterSerialDevices() const;
    
    /**
     * @brief Set FilterDevices value
     * 
     * @param value FilterDevices value
     * @return void
     */
    void setFilterDevices(bool value);
    
    /**
     * @brief Return verboseUpload value
     * 
     * @return bool
     */
    bool verboseUpload() const;
    
    /**
     * @brief Set verboseUpload value
     * 
     * @param verbose verboseUpload value
     * @return void
     */
    void setVerboseUpload(bool verbose);

    /**
     * @brief TODO
     * 
     * @param lexer TODO
     * @return void
     */
    void loadLexerProperties(LexerArduino *lexer);
    
    /**
     * @brief TODO
     * 
     * @param lexer TODO
     * @return void
     */
    void saveLexerProperties(LexerArduino *lexer);
    
    /**
     * @brief Load editor settings
     * 
     * @param editor Editor
     * @return void
     */
    void loadEditorSettings(Editor *editor);
    
    /**
     * @brief Save editor settings
     * 
     * @param editor Editor
     * @return void
     */
    void saveEditorSettings(Editor *editor);

    /**
     * @brief Save state of main window
     * 
     * @param state state
     * @return void
     */
    void saveMainWindowState(const QByteArray& state);
    
    /**
     * @brief ...
     * 
     * @return Return the state of main window
     */
    QByteArray mainWindowState();

    bool isCorrect() const;

signals:
    void fontChanged(const QFont &f);

private:
    Settings();
    QSettings mSettings;

    friend class IDEApplication;
};

#endif // SETTINGS_H
