/*
  Settings.h

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
 * \file Settings.h
 * \author Denis Martinez
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QStringList>
#include <QFont>

#include "IDEGlobal.h"

class LexerArduino;
class Editor;

class IDE_EXPORT Settings : public QObject
{
    Q_OBJECT

public:
    QString arduinoPath() const;
    void setArduinoPath(const QString &path);
    QString sketchPath() const;
    void setSketchPath(const QString &path);
    QString devicePort() const;
    void setDevicePort(const QString &port);
    QString board() const;
    void setBoard(const QString &board);
    bool filterSerialDevices() const;
    void setFilterDevices(bool value);
    bool verboseUpload() const;
    void setVerboseUpload(bool verbose);

    void loadLexerProperties(LexerArduino *lexer);
    void saveLexerProperties(LexerArduino *lexer);
    void loadEditorSettings(Editor *editor);
    void saveEditorSettings(Editor *editor);

    void saveMainWindowState(const QByteArray& state);
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
