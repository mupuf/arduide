/*
  SerialWriteDialog.h

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
 * \file SerialWriteDialog.h
 * \author Denis Martinez
 */

#ifndef SERIALWRITEDIALOG_H
#define SERIALWRITEDIALOG_H

#include "plugins/ui_SerialWriteDialog.h"

class SerialWriteDialog : public QDialog, Ui::SerialWriteDialog
{
    Q_OBJECT

public:
    SerialWriteDialog(QWidget *parent = NULL);
    void setStatus(const QString &text);

private slots:
    void writeInt();
    void writeBytes();
    void writeString();
    void writeFile();
    void chooseFile();

signals:
    void writeRequested(const QByteArray &data);
};

#endif // SERIALWRITEDIALOG_H
