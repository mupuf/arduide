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
