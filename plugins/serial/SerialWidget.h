/**
 * \file SerialWidget.h
 * \author Denis Martinez
 */

#ifndef SERIALWIDGET_H
#define SERIALWIDGET_H

#include "plugins/ui_SerialWidget.h"

#include <QSharedPointer>

#include "SerialWriteDialog.h"

class SerialWidget : public QWidget, Ui::SerialWidget
{
    Q_OBJECT

public:
    SerialWidget(QWidget *parent = NULL);
    void setStatus(const QString &text);
    int baudRate();
    int readCount();
    void setData(const QSharedPointer<QByteArray> &data);

public slots:
    void setWriteDialogVisible(bool visible);

signals:
    void openRequested();
    void closeRequested();
    void readRequested();

private:
    bool eventFilter(QObject *obj, QEvent *event);
    SerialWriteDialog *mDialog;
};

#endif // SERIALWIDGET_H
