/**
 * \file SerialPlugin.cpp
 * \author Denis Martinez
 */

#include "SerialPlugin.h"

#include <QDebug>

#include "SerialWidget.h"
#include "IDEApplication.h"

bool SerialPlugin::setup(IDEApplication *app)
{
    mApp = app;
    mName = tr("Serial");

    widget = new SerialWidget;
    app->mainWindow()->utilityTabWidget()->addTab(widget, name());

    connect(widget, SIGNAL(openRequested()), this, SLOT(open()));
    connect(widget, SIGNAL(closeRequested()), this, SLOT(close()));
    connect(widget, SIGNAL(readRequested()), this, SLOT(read()));
    connect(widget, SIGNAL(writeRequested(const QByteArray &)), this, SLOT(write(const QByteArray &)));

    return true;
}

void SerialPlugin::open()
{
    if (mSerial.data() != NULL)
        mSerial->close();

    QString port = mApp->settings()->devicePort();
    if (port.isEmpty())
    {
        widget->setStatus(tr("Unable to open, you must select a device first."));
        return;
    }

    int baudRate = widget->baudRate();

    mSerial.reset(new Serial(port, baudRate));
    if (! mSerial->open(QIODevice::ReadWrite))
    {
        widget->setStatus(tr("Open failed: %0").arg(mSerial->errorString()));
        return;
    }

    widget->setStatus(tr("Serial port opened successfully."));
}

void SerialPlugin::close()
{
    if (mSerial.data() != NULL)
        mSerial->close();

    widget->setStatus(tr("Serial port closed."));
}

void SerialPlugin::read()
{
    if (mSerial.data() != NULL && mSerial->isOpen())
    {
        qint64 readCount = widget->readCount();
        QSharedPointer<QByteArray> pData(new QByteArray);
        pData->resize(readCount);
        readCount = mSerial->readData(pData->data(), readCount);
        if (readCount == 0)
            widget->setStatus(tr("No data available for reading."));
        else if (readCount < 0)
            widget->setStatus(tr("Read error: %0").arg(mSerial->errorString()));
        else
        {
            widget->setStatus(tr("Read %0 bytes of data.").arg(readCount));
            pData->resize(readCount);
            widget->setData(pData);
        }
    }
    else
        widget->setStatus(tr("Unable to read, the port is not opened."));
}

void SerialPlugin::write(const QByteArray &data)
{
    if (mSerial.data() != NULL && mSerial->isOpen())
    {
        qint64 writeCount = mSerial->write(data);
        if (writeCount < 0)
            widget->writeDialog()->setStatus(tr("Write error: %0").arg(mSerial->errorString()));
        else
            widget->writeDialog()->setStatus(tr("Written %0 bytes of data.").arg(writeCount));
    }
    else
        widget->writeDialog()->setStatus(tr("Unable to write, the port is not opened."));
}

Q_EXPORT_PLUGIN2(serial, SerialPlugin)
