/*
  Device.h

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
 * \file Device.h
 * \author Denis Martinez
 */

#ifndef DEVICE_H
#define DEVICE_H

#include <QString>
#include <QList>

class Device;

/**
 * @brief List of all upload devices possibly connected
 * 
 */
typedef QList<Device> DeviceList;

class Device
{
public:
    
    /**
     * @brief Class to manage devices for upload
     * 
     * @param description A description about the device
     * @param port Port of connected device
     */
    Device(const QString &description, const QString &port);
    
    /**
     * @brief Function that return a description about the device
     * 
     * @return const QString&
     */
    const QString &description() const { return mDescription; }
    
    /**
     * @brief Return the port of connected device
     * 
     * @return const QString&
     */
    const QString &port() const { return mPort; }

    /**
     * @brief Return all possible devices
     * 
     * @param filterDevices If true realize the filter process
     * @return DeviceList
     */
    static DeviceList listDevices(bool filterDevices=true);

private:
    /**
     * @brief A description message
     * 
     */
    QString mDescription;
    
    /**
     * @brief Name of device's port 
     * 
     */
    QString mPort;
};

#endif // DEVICE_H
