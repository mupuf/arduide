/*
  Device.cpp

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
 * \file Device.cpp
 * \author Denis Martinez
 */

#include "Device.h"

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#include <windows.h>
#elif defined(Q_OS_DARWIN)
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/IOBSD.h>
#include <sys/param.h>
#elif defined(USE_LIBUDEV)
#include <libudev.h>
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#elif defined(USE_DBUS)
#include <QDBusInterface>
#endif

#include <unistd.h>

#include <QStringList>
#include <QDebug>

Device::Device(const QString &description, const QString &port)
{
    mDescription = description;
    mPort = port;
}

DeviceList Device::listDevices(bool filterDevices)
{
    DeviceList l;
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
    HKEY key;
    LONG res = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                             "Hardware\\DeviceMap\\SerialComm",
                             0, KEY_READ, &key);
    DWORD index = 0;
    CHAR valueName[256];
    CHAR valueData[256];
    DWORD valueNameSize, valueDataSize;
    DWORD valueType;
    if (res == ERROR_SUCCESS)
    {
        while (res == ERROR_SUCCESS)
        {
            valueNameSize = 256;
            valueDataSize = 256;
            res = RegEnumValueA(key, index, valueName,
                                &valueNameSize, NULL, &valueType,
                                (BYTE *) valueData, &valueDataSize);
            if (res == ERROR_SUCCESS && valueType == REG_SZ)
                l << Device(valueName, valueData);
            index++;
        }
        RegCloseKey(key);
    }
#elif defined(Q_OS_DARWIN)
    kern_return_t kernResult;
    io_iterator_t serialPortIterator;
    io_object_t serialPort;
    mach_port_t masterPort;
    CFMutableDictionaryRef classesToMatch;
    char deviceFilePath[MAXPATHLEN];
    CFTypeRef deviceFilePathAsCFString;
    kernResult = IOMasterPort(MACH_PORT_NULL, &masterPort);
    if (kernResult == KERN_SUCCESS)
    {
        classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue);
        if (classesToMatch != NULL)
        {
            CFDictionarySetValue(classesToMatch, CFSTR(kIOSerialBSDTypeKey),
                                 CFSTR(kIOSerialBSDRS232Type));
            kernResult = IOServiceGetMatchingServices(masterPort,
                                                      classesToMatch,
                                                      &serialPortIterator);
            if (kernResult == KERN_SUCCESS)
            {
                while (serialPort = IOIteratorNext(serialPortIterator))
                {
                    deviceFilePathAsCFString = IORegistryEntryCreateCFProperty(serialPort,
                                                                               CFSTR(kIOCalloutDeviceKey),
                                                                               kCFAllocatorDefault, 0);
                    if (deviceFilePathAsCFString)
                    {
                        Boolean result = CFStringGetCString((CFStringRef) deviceFilePathAsCFString,
                                                            deviceFilePath, MAXPATHLEN,
                                                            kCFStringEncodingASCII);
                        CFRelease(deviceFilePathAsCFString);
                        if (result)
                            l.append(Device(QObject::tr("Serial callout"),
                                            deviceFilePath));
                    }

                    deviceFilePathAsCFString = IORegistryEntryCreateCFProperty(serialPort,
                                                                               CFSTR(kIODialinDeviceKey),
                                                                               kCFAllocatorDefault, 0);
                    if (deviceFilePathAsCFString)
                    {
                        Boolean result = CFStringGetCString((CFStringRef) deviceFilePathAsCFString,
                                                            deviceFilePath, MAXPATHLEN,
                                                            kCFStringEncodingASCII);
                        CFRelease(deviceFilePathAsCFString);
                        if (result)
                            l.append(Device(QObject::tr("Serial dialin"),
                                            deviceFilePath));
                    }
                    IOObjectRelease(serialPort);
                }
                IOObjectRelease(serialPortIterator);
            }
        }
    }
#elif defined(USE_LIBUDEV)
    udev *udev = udev_new();
    udev_enumerate *enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "tty");
    udev_enumerate_scan_devices(enumerate);
    udev_list_entry *list_entry = udev_enumerate_get_list_entry(enumerate);
    udev_list_entry *list_entry_current;
    const char *devnode, *devname;
    serial_struct serial;
    bool is_serial;
    bool is_modem;
    int fd;

    udev_list_entry_foreach(list_entry_current, list_entry)
    {
        udev_device *device = udev_device_new_from_syspath(udev,
                                                           udev_list_entry_get_name(list_entry_current));
        devnode = udev_device_get_devnode(device);
        is_serial = false;
        is_modem = false;
        fd = open(devnode, O_RDONLY | O_NONBLOCK);
        if (fd >= 0)
        {
            // dummy read check (detect non-existing RS232 ports)
            if (read(fd, NULL, 0) == 0)
            {
                // get serial properties from descriptor
                if (ioctl(fd, TIOCGSERIAL, &serial) != -1 && serial.type == 0)
                {
                    is_serial = true;
                    is_modem = false;
                }
                else if (strncmp(devnode, "/dev/ttyACM", 11) == 0 || strncmp(devnode, "/dev/ttyUSB", 11) == 0)
                {
                    is_serial = true;
                    is_modem = true;
                }
            }

            close(fd);
        }

        if (is_serial || !filterDevices)
        {
            devname = NULL;
            // get the name of the parent device, use it as the device description
            for (udev_device *parent_device = device;
                 parent_device != NULL &&devname == NULL;
                 parent_device = udev_device_get_parent(parent_device))
            {
                devname = udev_device_get_sysattr_value(parent_device, "name");
                if (! devname)
                    devname = udev_device_get_sysattr_value(parent_device, "interface");
            }
            if (devname == NULL)
            {
                if (is_modem)
                    devname = "Serial Port";
                else
                    devname = "unknown";
            }
            l << Device(devname, devnode);
        }
        udev_device_unref(device);
    }
    udev_enumerate_unref(enumerate);
    udev_unref(udev);
#elif defined(USE_DBUS)
    QDBusInterface manager("org.freedesktop.Hal", "/org/freedesktop/Hal/Manager",
                           "org.freedesktop.Hal.Manager", QDBusConnection::systemBus());
    QVariantList devices = manager.call("FindDeviceByCapability", "serial").arguments();
    if (devices.size() == 1)
    {
        foreach (const QString &key, devices[0].toStringList())
        {
            QDBusInterface device("org.freedesktop.Hal", key,
                                  "org.freedesktop.Hal.Device",
                                  QDBusConnection::systemBus());
            QVariantList description = device.call("GetPropertyString",
                                                   "info.product").arguments();
            QVariantList port = device.call("GetPropertyString",
                                            "serial.device").arguments();
            if (port.size() == 1 && description.size() == 1)
                l.append(Device(description[0].toString(), port[0].toString()));
        }
    }
#else
#error "No method available for enumerating devices."
#endif
    return l;
}
