/*
  Serial.cpp

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
 * \file Serial.cpp
 * \author Denis Martinez
 * \date 2010-03-29
 */

#include "Serial.h"

#include <QDebug>

#include "Compat.h"

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
const Serial::descriptor Serial::INVALID_SERIAL_DESCRIPTOR = INVALID_HANDLE_VALUE;
#endif

Serial::Serial(const QString &port, int baudRate)
    : mPort(port),
      mBaudRate(baudRate),
      mSerial(INVALID_SERIAL_DESCRIPTOR),
      watcher(NULL)
{
}

Serial::~Serial()
{
    setInReadEventMode(false);
}

const QList<int> &Serial::baudRates()
{
    static const QList<int> rates = QList<int>() << 300 << 1200 << 2400 << 4800 << 9600 << 19200 << 38400 << 57600 << 115200;
    return rates;
}

Serial::descriptor Serial::serialDescriptor()
{
    return mSerial;
}

bool Serial::isSequential() const
{
    return true;
}

bool Serial::isOpen() const
{
    return mSerial != INVALID_SERIAL_DESCRIPTOR;
}

QByteArray Serial::readAll()
{
    QByteArray ret;
    char buf[16];
    qint64 size;

    while ((size = readData(buf, 16)) > 0)
        ret.append(buf, size);

    return ret;
}

bool Serial::flushBuffer()
{
    if (! isOpen())
        return false;

    while (! readAll().isEmpty())
        Compat::sleep_ms(100);

    if (! setDTR(false))
        return false;
    Compat::sleep_ms(100);
    if (! setDTR(true))
      return false;
    return true;
}

bool Serial::isInReadEventMode()
{
    return watcher!=NULL;
}

void Serial::setInReadEventMode(bool value)
{
    if (value && watcher==NULL)
    {
        watcher = new SerialWatcher(this, this);
        watcher->start();
    }
    else if (!value)
    {
        if (watcher)
            delete watcher;
        watcher = NULL;
    }
}

void Serial::onNewDataArrived(QByteArray data)
{
    emit dataArrived(data);
}
