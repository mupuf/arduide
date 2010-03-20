/**
 * \file SerialWidget.cpp
 * \author Denis Martinez
 */

#include "SerialWidget.h"

SerialWidget::SerialWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    QSharedPointer<QByteArray> sp(new QByteArray("No data."));
    hexView->setData(sp);

    statusLabel->setText(tr("Open a new connection to start."));
}
