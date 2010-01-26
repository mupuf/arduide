/**
 * \file Serial.cpp
 * \author Denis Martinez
 */

#include "Serial.h"

Serial::Serial(const QString &port)
    : mPort(port),
      mSerial(mIo)
{
}

boost::asio::serial_port::native_type Serial::serialDescriptor()
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

    boost::system::error_code error;
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
   boost::system::error_code error;
   std::size_t n = mSerial.read_some(boost::asio::buffer(data, maxSize), error);
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
   boost::system::error_code error;
   std::size_t n = mSerial.write_some(boost::asio::buffer(data, maxSize), error);
   if (error)
   {
       setErrorString(QString::fromStdString(error.message()));
       return -1;
   }
   else
       return n;
}

bool Serial::setDTR(bool enabled)
{
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
    // TODO
    return false;
#else
    if (! isOpen())
        return false;
    int fd = serialDescriptor();
    unsigned int result = 0;
    if (ioctl(fd, TIOCMGET, &result) == -1)
        return false;
    if (enabled)
        result |= TIOCM_DTR;
    else
        result &= ~ TIOCM_DTR;
    if (ioctl(fd, TIOCMSET, &result) == -1)
        return false;
    return true;
#endif
}
