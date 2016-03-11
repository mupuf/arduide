/*
  SerialWriteDialog.cpp

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
 * \file SerialWriteDialog.cpp
 * \author Denis Martinez
 */

#include "SerialWriteDialog.h"

#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <vector>
#include <algorithm>

SerialWriteDialog::SerialWriteDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    setStatus(tr("Send data over the serial connection."));

    intValueEdit->setSampleText(tr("integer values, e.g. 0 1 2 3"));
    hexValueEdit->setSampleText(tr("hexadecimal bytes, e.g. 01AB"));

    intTypeBox->addItem(tr("Byte"), 1);
    intTypeBox->addItem(tr("Int (2 bytes)"), 2);
    intTypeBox->addItem(tr("Long (4 bytes)"), 4);

    connect(writeIntButton, SIGNAL(clicked()), this, SLOT(writeInt()));
    connect(writeBytesButton, SIGNAL(clicked()), this, SLOT(writeBytes()));
    connect(writeStringButton, SIGNAL(clicked()), this, SLOT(writeString()));
    connect(writeFileButton, SIGNAL(clicked()), this, SLOT(writeFile()));
    connect(fileButton, SIGNAL(clicked()), this, SLOT(chooseFile()));
}

void SerialWriteDialog::setStatus(const QString &text)
{
    statusLabel->setText(text);
}

void SerialWriteDialog::writeInt()
{
    bool msbFirst = msbFirstBox->isChecked();
    int intSize = intTypeBox->itemData(intTypeBox->currentIndex()).toInt();

    // get a list of integers from the line edit
    // qlonglong is assumed big enough to hold all values
    QStringList values = intValueEdit->text().split(' ', QString::SkipEmptyParts);
    if (values.size() == 0)
        return;

    QList<qlonglong> integers;
    qlonglong integer;
    bool ok;

    QByteArray data;
    int index = 0;
    data.resize(values.size() * intSize);

    std::vector<unsigned char> intData;
    intData.reserve(intSize);
    qulonglong uinteger;
    foreach(const QString &value, values)
    {
        intData.clear();
        integer = value.toLongLong(&ok);
        if (! ok)
        {
            setStatus(tr("Invalid value: %0").arg(value));
            return;
        }

        // convert the integer to an array of bytes
        uinteger = reinterpret_cast<qulonglong &>(integer);
        for (int i = 0; i < intSize; i++)
        {
            if (uinteger > 0)
            {
                intData.push_back(uinteger % 256);
                uinteger >>= 8;
            }
            else
                intData.push_back(0);
        }

        if (msbFirst)
            std::reverse(intData.begin(), intData.end());

        // append it to data
        for (int i = 0; i < intSize; i++)
        {
            data[index] = intData[i];
            index++;
        }
    }

    // write
    emit writeRequested(data);
}

void SerialWriteDialog::writeBytes()
{
    static const QRegExp re("^([0-9a-fA-F]{2})+$");
    QString hex = hexValueEdit->text().simplified();
    if (! re.exactMatch(hex))
    {
        setStatus(tr("Invalid hex value"));
        return;
    }
    emit writeRequested(QByteArray::fromHex(hex.toAscii()));
}

void SerialWriteDialog::writeString()
{
    QString text = stringValueEdit->toPlainText();
    if (! text.isEmpty())
        emit writeRequested(text.toLocal8Bit());
}

void SerialWriteDialog::writeFile()
{
    QString fileName = fileEdit->text();
    if (fileName.isEmpty())
        return;
    QFile f(fileName);
    if (! f.open(QFile::ReadOnly))
    {
        setStatus(tr("File error: %0").arg(f.errorString()));
        return;
    }
    emit writeRequested(f.readAll());
}

void SerialWriteDialog::chooseFile()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (! fileName.isEmpty())
        fileEdit->setText(fileName);
}
