/*
  Serial.h

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
 * \file Serial.h
 * \author Denis Martinez
 */

#ifndef SERIAL_H
#define SERIAL_H

#include <QIODevice>
#include <QThread>
#include <QByteArray>

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#include <windows.h>
#endif

#include "IDEGlobal.h"

class SerialWatcher;

class IDE_EXPORT Serial : public QIODevice
{
    Q_OBJECT

public:
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
    typedef ::HANDLE descriptor;
    static const descriptor INVALID_SERIAL_DESCRIPTOR;
#else
    typedef int descriptor;
    static const descriptor INVALID_SERIAL_DESCRIPTOR = -1;
#endif

    Serial(const QString &port, int baudRate = 9600);
    ~Serial();
    static const QList<int> &baudRates();

    descriptor serialDescriptor();
    bool flushBuffer();

    bool isInReadEventMode();
    void setInReadEventMode(bool value);


    // QIODevice implementation
    bool isSequential() const;
    bool open(OpenMode mode);
    bool isOpen() const;
    void close();
    qint64 readData(char *data, qint64 maxSize);
    qint64 writeData(const char *data, qint64 maxSize);
    bool waitForReadyRead (int msecs);
    QByteArray readAll();

signals:
    void dataArrived(QByteArray);

private:
    bool setDTR(bool enable);

    QString mPort;
    int mBaudRate;
    descriptor mSerial;

    void onNewDataArrived(QByteArray data);

    /**************************
     **************************
     *****  SerialWatcher *****
     **************************
     **************************
     */
    class SerialWatcher : public QThread
    {
    private:
        bool watch;
        Serial* serial;

    public:
        explicit SerialWatcher(Serial* serial, QObject *parent = 0)  :
                QThread(parent), watch(true), serial(serial)
        {
        }
        ~SerialWatcher()
        {
            watch = false;
            wait(1000);
        }

        void run()
        {
            while (serial && watch)
            {
                msleep(10);
                if (serial->waitForReadyRead(300) && watch) {
                    QByteArray data = serial->readAll();
                    if (data.length() > 0)
                        serial->onNewDataArrived(data);
                }
            }
        }

        void startWatching(bool watch=true) { this->watch = watch; }
        void stopWatching(){ startWatching(false);}

    } *watcher;
    friend class SerialWatcher;
};

#endif // SERIAL_H
