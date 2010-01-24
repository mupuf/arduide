/**
 * \file Device.h
 * \author Denis Martinez
 */

#ifndef DEVICE_H
#define DEVICE_H

#include <QString>
#include <QList>

class Device;
typedef QList<Device> DeviceList;

class Device
{
public:
    Device(const QString &description, const QString &port);
    const QString &description() const { return mDescription; }
    const QString &port() const { return mPort; }

    static DeviceList listDevices();

private:
    QString mDescription;
    QString mPort;
};

#endif // DEVICE_H
