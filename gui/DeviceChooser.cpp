/*
  DeviceChooser.cpp

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
 * \file DeviceChooser.cpp
 * \author Denis Martinez
 */

#include "DeviceChooser.h"

#include "env/Device.h"
#include "env/Settings.h"
#include "IDEApplication.h"

DeviceChooser::DeviceChooser(QWidget *parent)
    : QMenu(parent),
      actionGroup(NULL)
{
    setTitle(tr("Device"));
    refresh();
}

void DeviceChooser::refresh()
{
    QString defaultPort = ideApp->settings()->devicePort();
    delete actionGroup;
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);
    connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(onActionTriggered(QAction *)));

    clear();

    QAction *action;
    foreach (const Device &dev, Device::listDevices(ideApp->settings()->filterSerialDevices()))
    {
        QString name = QString("%0: %1").arg(dev.port()).arg(dev.description());
        const QString &port = dev.port();
        action = new QAction(name, actionGroup);
        action->setData(port);
        action->setCheckable(true);
        if (port == defaultPort)
            action->setChecked(true);
        addAction(action);
    }

    // If there are no devices, add a dummy one
    if (actionGroup->actions().size() == 0)
    {
        action = new QAction("No device found", actionGroup);
        action->setData(-1);
        action->setCheckable(true);
        action->setEnabled(false);
        addAction(action);
    }
}

void DeviceChooser::onActionTriggered(QAction *action)
{
    emit deviceChosen(action->data().toString());
}
