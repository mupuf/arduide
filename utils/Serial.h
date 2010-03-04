/**
 * \file Serial.h
 * \author Denis Martinez
 */

#ifndef SERIAL_H
#define SERIAL_H

#include <QIODevice>

#include <asio/io_service.hpp>
#include <asio/serial_port.hpp>
#include <asio/completion_condition.hpp>
#include <asio/read.hpp>

class Serial : public QIODevice
{
public:
    Serial(const QString &port);

    asio::serial_port::native_type serialDescriptor();

    bool setDTR(bool enabled);
    bool flushBuffer();

    // QIODevice implementation
    bool isSequential() const;
    bool open(OpenMode mode);
    bool isOpen() const;
    void close();
    qint64 readData(char *data, qint64 maxSize);
    qint64 writeData(const char *data, qint64 maxSize);

private:

    QString mPort;
    asio::io_service mIo;
    asio::serial_port mSerial;
};

#endif // SERIAL_H
