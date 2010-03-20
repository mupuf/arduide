/**
 * \file SerialWidget.h
 * \author Denis Martinez
 */

#ifndef SERIALWIDGET_H
#define SERIALWIDGET_H

#include "plugins/ui_SerialWidget.h"

class SerialWidget : public QWidget, Ui::SerialWidget
{
    Q_OBJECT

public:
    SerialWidget(QWidget *parent = NULL);
    void setStatus(const QString &text);
    int baudRate();

signals:
    void openRequested();
    void closeRequested();
};

#endif // SERIALWIDGET_H
