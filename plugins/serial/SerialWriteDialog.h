/**
 * \file SerialWriteDialog.h
 * \author Denis Martinez
 */

#ifndef SERIALWRITEDIALOG_H
#define SERIALWRITEDIALOG_H

#include "plugins/ui_SerialWriteDialog.h"

class SerialWriteDialog : public QDialog, Ui::SerialWriteDialog
{
public:
    SerialWriteDialog(QWidget *parent = NULL);
};

#endif // SERIALWRITEDIALOG_H
