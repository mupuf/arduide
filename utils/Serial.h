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
                if (serial->waitForReadyRead(300) && watch)
                    serial->onNewDataArrived(serial->readAll());
            }
        }

        void startWatching(bool watch=true) { this->watch = watch; }
        void stopWatching(){ startWatching(false);}

    } *watcher;
    friend class SerialWatcher;
};

#endif // SERIAL_H
