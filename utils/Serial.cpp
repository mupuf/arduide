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
      mSerial(INVALID_SERIAL_DESCRIPTOR)
{
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
