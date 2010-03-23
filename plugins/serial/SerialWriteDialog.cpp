/**
 * \file SerialWriteDialog.cpp
 * \author Denis Martinez
 */

#include "SerialWriteDialog.h"

SerialWriteDialog::SerialWriteDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    intValueEdit->setSampleText(tr("integer values, e.g. 0 1 2 3"));
    hexValueEdit->setSampleText(tr("hexadecimal bytes, e.g. 01AB"));

    intTypeBox->addItem(tr("Byte"), 1);
    intTypeBox->addItem(tr("Int (2 bytes)"), 2);
    intTypeBox->addItem(tr("Long (4 bytes)"), 4);
}
