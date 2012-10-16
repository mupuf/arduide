/*
  SerialPlugin.h

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
 * \file SerialPlugin.h
 * \author Denis Martinez
 */

#ifndef SERIALPLUGIN_H
#define SERIALPLUGIN_H

#include "plugins/IDEPluginInterface.h"

#include <QScopedPointer>

#include "utils/Serial.h"

class SerialWidget;

class SerialPlugin : public QObject, public IDEPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(IDEPluginInterface)

public:
    bool setup(IDEApplication *app);
    const QString &name() { return mName; }

signals:
    void currentStateChanged(bool opened);

private slots:
    void open();
    void close();
    void read();
    void write(const QByteArray &data);
    void changeReadMode(bool mode);
    void continuousRead(const QByteArray &data);

private:
    IDEApplication *mApp;

    QString mName;
    SerialWidget *widget;

    QScopedPointer<Serial> mSerial;
};

#endif // SERIALPLUGIN_H
