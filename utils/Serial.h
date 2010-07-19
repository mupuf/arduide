/**
 * \file Serial.h
 * \author Denis Martinez
 */

#ifndef SERIAL_H
#define SERIAL_H

#include <QIODevice>

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#include <windows.h>
#endif

#include "IDEGlobal.h"

class IDE_EXPORT Serial : public QIODevice
{
public:
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
    typedef ::HANDLE descriptor;
    static const descriptor INVALID_SERIAL_DESCRIPTOR;
#else
    typedef int descriptor;
    static const descriptor INVALID_SERIAL_DESCRIPTOR = -1;
#endif

    Serial(const QString &port, int baudRate = 9600);
    static const QList<int> &baudRates();

    descriptor serialDescriptor();
    bool flushBuffer();

    // QIODevice implementation
    bool isSequential() const;
    bool open(OpenMode mode);
    bool isOpen() const;
    void close();
    qint64 readData(char *data, qint64 maxSize);
    qint64 writeData(const char *data, qint64 maxSize);

private:
    bool setDTR(bool enable);

    QString mPort;
    int mBaudRate;
    descriptor mSerial;
};

#endif // SERIAL_H
