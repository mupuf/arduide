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

    mDialog = new SerialWriteDialog(this);
    mDialog->installEventFilter(this);

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
    connect(writeButton, SIGNAL(clicked(bool)), this, SLOT(setWriteDialogVisible(bool)));
    connect(mDialog, SIGNAL(writeRequested(const QByteArray &)), this, SIGNAL(writeRequested(const QByteArray &)));
    connect(checkContinuousRead, SIGNAL(toggled(bool)), this, SLOT(checkReadMode_clicked(bool)));
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

const QSharedPointer<QByteArray> SerialWidget::data() const
{
    return hexView->data();
}

void SerialWidget::setData(const QSharedPointer<QByteArray> &data)
{
    hexView->setData(data);
    hexView->scrollToBottom();
}

void SerialWidget::setWriteDialogVisible(bool visible)
{
    mDialog->setVisible(visible);
}

void SerialWidget::checkReadMode_clicked(bool value)
{
    /* update the GUI */
    serialOpenEvent(true);

    /* clear the current buffer */
    if (value)
    {
        QSharedPointer<QByteArray> sp(new QByteArray(""));
        setData(sp);
    }

    emit readModeChangeRequested(value);
}

void SerialWidget::serialOpenEvent(bool opened)
{
    bool open_and_not_continuous;

    if (!opened)
        checkContinuousRead->setChecked(false);

    open_and_not_continuous= opened && checkContinuousRead->checkState() != Qt::Checked;

    openButton->setEnabled(!opened);
    closeButton->setEnabled(opened);
    writeButton->setEnabled(opened);
    checkContinuousRead->setEnabled(opened);
    readButton->setEnabled(open_and_not_continuous);
    readCountBox->setEnabled(open_and_not_continuous);
}

bool SerialWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == mDialog && event->type() == QEvent::Hide)
    {
        writeButton->setChecked(false);
        return true;
    }
    return QObject::eventFilter(obj, event);
}
