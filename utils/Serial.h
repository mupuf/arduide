/**
 * \file Serial.h
 * \author Denis Martinez
 */

#ifndef SERIAL_H
#define SERIAL_H

#include <QIODevice>

#include <boost/asio/io_service.hpp>
#include <boost/asio/serial_port.hpp>

class Serial : public QIODevice
{
public:
    Serial(const QString &port);

    boost::asio::serial_port::native_type serialDescriptor();

    bool setDTR(bool enabled);

    // QIODevice implementation
    bool isSequential() const;
    bool open(OpenMode mode);
    bool isOpen() const;
    void close();
    qint64 readData(char *data, qint64 maxSize);
    qint64 writeData(const char *data, qint64 maxSize);

private:

    QString mPort;
    boost::asio::io_service mIo;
    boost::asio::serial_port mSerial;
};

#endif // SERIAL_H
