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
