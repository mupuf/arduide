/*
  SerialWidget.h

  This file is part of arduide, The Qt-based IDE for the open-source Arduino electronics prototyping platform.

  Copyright (C) 2010-2016 
  Authors : Denis Martinez
	    Martin Peres

This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * \file SerialWidget.h
 * \author Denis Martinez
 */

#ifndef SERIALWIDGET_H
#define SERIALWIDGET_H

#include "plugins/ui_SerialWidget.h"

#include <QSharedPointer>

#include "SerialWriteDialog.h"

#include "SerialPlugin.h"

class SerialWidget : public QWidget, Ui::SerialWidget
{
    Q_OBJECT

public:
    SerialWidget(QWidget *parent = NULL);
    void setStatus(const QString &text);
    int baudRate();
    int readCount();
    const QSharedPointer<QByteArray> data() const;
    void setData(const QSharedPointer<QByteArray> &data);
    SerialWriteDialog *writeDialog() { return mDialog; }

public slots:
    void setWriteDialogVisible(bool visible);
    void serialOpenEvent(bool opened);

signals:
    void openRequested();
    void closeRequested();
    void readRequested();
    void writeRequested(const QByteArray &data);
    void readModeChangeRequested(bool);

private slots:
    void checkReadMode_clicked(bool value);

private:
    bool eventFilter(QObject *obj, QEvent *event);
    SerialWriteDialog *mDialog;
};

#endif // SERIALWIDGET_H
