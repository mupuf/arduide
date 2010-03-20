/**
 * \file SerialWidget.h
 * \author Denis Martinez
 */

#ifndef SERIALWIDGET_H
#define SERIALWIDGET_H

#include "plugins/ui_SerialWidget.h"

class SerialWidget : public QWidget, Ui::SerialWidget
{
public:
    SerialWidget(QWidget *parent = NULL);
};

#endif // SERIALWIDGET_H
