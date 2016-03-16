/*
  DebuggerPlugin.h

  This file is part of arduide, The Qt-based IDE for the open-source Arduino electronics prototyping platform.

  Copyright (C) 2010-2016 
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
 * \file DebuggerPlugin.h
 * \author Martin Peres
 */

#ifndef DEBUGGERPLUGIN_H
#define DEBUGGERPLUGIN_H

#include "plugins/IDEPluginInterface.h"

#include "DebuggerWidget.h"
#include "utils/Serial.h"
#include "gui/Editor.h"

#include <QScopedPointer>
#include <QTime>

class QTreeWidgetItem;

class DebuggerPlugin : public QObject, public IDEPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(IDEPluginInterface)

public:
    bool setup(IDEApplication *app);
    const QString &name() { return mName; };

    bool isDebugging();

    int debugTime();

public slots:
    void startDebugging();
    void stopDebugging();

private slots:
    void uploadCompleted(bool res);

    bool openSerial();
    void closeSerial();

    void dataArrived(QByteArray data);
    void treeItemClicked(QTreeWidgetItem* item, int column);
    void sendCommand(QString cmd);
    void shouldBreakOnTrace(bool value);
    void mainWindowTabChanged(bool isBrowser);
    void mainWindowEditorDeleted(Editor *editor);

private:
    IDEApplication *mApp;
    QString mName;
    QScopedPointer<DebuggerWidget> widget;
    QScopedPointer<Serial> serial;
    QString serialData;
    QTime startTime;
    Editor* debuggedEditor;

    void parseTrace(QString trace);
    void parseState(QString state);
    void parseRet(QString ret);
    void parseError(QString error);

};

#endif // DEBUGGERPLUGIN_H
