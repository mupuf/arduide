/**
 * \file SerialWidget.h
 * \author Denis Martinez
 */

#ifndef SERIALWIDGET_H
#define SERIALWIDGET_H

#include "plugins/ui_SerialWidget.h"

#include <QSharedPointer>

class SerialWidget : public QWidget, Ui::SerialWidget
{
    Q_OBJECT

public:
    SerialWidget(QWidget *parent = NULL);
    void setStatus(const QString &text);
    int baudRate();
    int readCount();
    void setData(const QSharedPointer<QByteArray> &data);

signals:
    void openRequested();
    void closeRequested();
    void readRequested();
};

#endif // SERIALWIDGET_H
