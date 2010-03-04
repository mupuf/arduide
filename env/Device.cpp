/**
 * \file Device.cpp
 * \author Denis Martinez
 */

#include "Device.h"

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
    //TODO
#elif defined(Q_OS_DARWIN)
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/IOBSD.h>
#include <sys/param.h>
#else
#include <QDBusInterface>
#endif

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
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
    //TODO
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
	    CFDictionarySetValue(classesToMatch, CFSTR(kIOSerialBSDTypeKey), CFSTR(kIOSerialBSDRS232Type));
	    kernResult = IOServiceGetMatchingServices(masterPort, classesToMatch, &serialPortIterator);
	    if (kernResult == KERN_SUCCESS)
	    {
		while (serialPort = IOIteratorNext(serialPortIterator))
		{
		    deviceFilePathAsCFString = IORegistryEntryCreateCFProperty(serialPort, CFSTR(kIOCalloutDeviceKey), kCFAllocatorDefault, 0);
		    if (deviceFilePathAsCFString)
		    {
			Boolean result = CFStringGetCString((CFStringRef) deviceFilePathAsCFString, deviceFilePath, MAXPATHLEN, kCFStringEncodingASCII);
			CFRelease(deviceFilePathAsCFString);
			if (result)
			    l.append(Device(QObject::tr("Serial callout"), deviceFilePath));
		    }

		    deviceFilePathAsCFString = IORegistryEntryCreateCFProperty(serialPort, CFSTR(kIODialinDeviceKey), kCFAllocatorDefault, 0);
		    if (deviceFilePathAsCFString)
		    {
			Boolean result = CFStringGetCString((CFStringRef) deviceFilePathAsCFString, deviceFilePath, MAXPATHLEN, kCFStringEncodingASCII);
			CFRelease(deviceFilePathAsCFString);
			if (result)
			    l.append(Device(QObject::tr("Serial dialin"), deviceFilePath));
		    }
		    IOObjectRelease(serialPort);
		}
		IOObjectRelease(serialPortIterator);
	    }
	}
    }
#else
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
#endif
    return l;
}
