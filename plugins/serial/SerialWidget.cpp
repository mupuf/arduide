/**
 * \file SerialWidget.cpp
 * \author Denis Martinez
 */

#include "SerialWidget.h"

#include <QDebug>

#include "utils/Serial.h"

SerialWidget::SerialWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    int index = 0;
    foreach(int rate, Serial::baudRates())
    {
        baudRateBox->addItem(QString::number(rate), rate);
        if (rate == 9600)
            baudRateBox->setCurrentIndex(index);
        index++;
    }

    QSharedPointer<QByteArray> sp(new QByteArray("No data."));
    setData(sp);

    setStatus(tr("Open a new connection to start."));

    connect(openButton, SIGNAL(clicked()), this, SIGNAL(openRequested()));
    connect(closeButton, SIGNAL(clicked()), this, SIGNAL(closeRequested()));
    connect(readButton, SIGNAL(clicked()), this, SIGNAL(readRequested()));
}

void SerialWidget::setStatus(const QString &text)
{
    statusLabel->setText(text);
}

int SerialWidget::baudRate()
{
    return baudRateBox->itemData(baudRateBox->currentIndex()).toInt();
}

int SerialWidget::readCount()
{
    return readCountBox->value();
}

void SerialWidget::setData(const QSharedPointer<QByteArray> &data)
{
    hexView->setData(data);
}
