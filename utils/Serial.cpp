/**
 * \file Serial.cpp
 * \author Denis Martinez
 */

#include "Serial.h"

#include "Compat.h"

Serial::Serial(const QString &port)
    : mPort(port),
      mSerial(mIo)
{
}

asio::serial_port::native_type Serial::serialDescriptor()
{
    return mSerial.native();
}

bool Serial::isSequential() const
{
    return true;
}

bool Serial::open(OpenMode mode)
{
    // mode is ignored
    (void) mode;

    asio::error_code error;
    mSerial.open(mPort.toStdString(), error);
    if (error)
    {
        setErrorString(QString::fromStdString(error.message()));
        return false;
    }
    else
    {
        setOpenMode(ReadWrite);
        return true;
    }
}

bool Serial::isOpen() const
{
    return mSerial.is_open();
}

void Serial::close()
{
    emit aboutToClose();
    mSerial.close();
    setOpenMode(NotOpen);
    setErrorString(QString());
}

qint64 Serial::readData(char *data, qint64 maxSize)
{
   asio::error_code error;
   std::size_t n = mSerial.read_some(asio::buffer(data, maxSize), error);
   if (error)
   {
       setErrorString(QString::fromStdString(error.message()));
       return -1;
   }
   else
       return n;
}

qint64 Serial::writeData(const char *data, qint64 maxSize)
{
   asio::error_code error;
   std::size_t n = mSerial.write_some(asio::buffer(data, maxSize), error);
   if (error)
   {
       setErrorString(QString::fromStdString(error.message()));
       return -1;
   }
   else
       return n;
}

bool Serial::flushBuffer()
{
    if (! isOpen())
        return false;

    asio::error_code error;
    std::size_t n;
    do
    {
        n = asio::read(mSerial, asio::null_buffers(), asio::transfer_all(), error);
        if (error)
            return false;
        Compat::sleep_ms(100);
    } while (n > 0);

    if (! Compat::setDTR(mSerial.native(), false))
        return false;
    Compat::sleep_ms(100);
    Compat::setDTR(mSerial.native(), true);
}
