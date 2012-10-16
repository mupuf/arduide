/*
  DebuggerWidget.h

  This file is part of arduide, The Qt-based IDE for the open-source Arduino electronics prototyping platform.

  Copyright (C) 2010-2012 
  Authors : Martin Peres
	    Denis Martinez

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
 * \file DebuggerWidget.h
 * \author Martin Peres
 */

#ifndef DEBUGGERWIDGET_H
#define DEBUGGERWIDGET_H

#include "plugins/ui_DebuggerWidget.h"

class DebuggerWidget : public QWidget, Ui::DebuggerWidget
{
    Q_OBJECT

    friend class DebuggerPlugin;

    bool _break;
    bool _started;

    void addCmdLineCompleter();

public:
    DebuggerWidget(QWidget *parent = NULL);

    bool isStarted();
    bool shouldBreakASAP();

    int baudRate();

public slots:
    void startDebugging();
    void stopDebugging();

    void clearLogs();
    void logImportant(const QString& result);
    void logResult(const QString& result);
    void logError(const QString& error);

    void setStatus(const QString &text);

private slots:
    void onDebugStatusChanged();

    void onStartStopPressed();
    void onBreakToggled(int);
    void onSendCommand();

    void debugStarted(bool value);

    void updateBaudList();

signals:
    void debuggerStarted();
    void debuggerStopped();
    void shouldBreakOnTrace(bool);
    void sendCommand(QString);
};

#endif // DEBUGGERWIDGET_H
