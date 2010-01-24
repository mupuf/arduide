/**
 * \file Device.cpp
 * \author Denis Martinez
 */

#include "Device.h"

#include <QDBusInterface>
#include <QStringList>
#include <QDebug>

Device::Device(const QString &description, const QString &port)
{
    mDescription = description;
    mPort = port;
}

DeviceList Device::listDevices()
{
    DeviceList l;
    QDBusInterface manager("org.freedesktop.Hal", "/org/freedesktop/Hal/Manager", "org.freedesktop.Hal.Manager", QDBusConnection::systemBus());
    QVariantList devices = manager.call("FindDeviceByCapability", "serial").arguments();
    if (devices.size() == 1)
    {
        foreach (const QString &key, devices[0].toStringList())
        {
            QDBusInterface device("org.freedesktop.Hal", key, "org.freedesktop.Hal.Device", QDBusConnection::systemBus());
            QVariantList description = device.call("GetPropertyString", "info.product").arguments();
            QVariantList port = device.call("GetPropertyString", "serial.device").arguments();
            if (port.size() == 1 && description.size() == 1)
                l.append(Device(description[0].toString(), port[0].toString()));
        }
    }
    return l;
}
