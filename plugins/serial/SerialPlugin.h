/**
 * \file SerialPlugin.h
 * \author Denis Martinez
 */

#ifndef SERIALPLUGIN_H
#define SERIALPLUGIN_H

#include "plugins/IDEPluginInterface.h"

#include <QScopedPointer>

#include "utils/Serial.h"

class SerialWidget;

class SerialPlugin : public QObject, public IDEPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(IDEPluginInterface)

public:
    bool setup(IDEApplication *app);

    const QString &name() { return mName; };

private slots:
    void open();
    void close();
    void read();

private:
    IDEApplication *mApp;

    QString mName;
    SerialWidget *widget;

    QScopedPointer<Serial> mSerial;
};

#endif // SERIALPLUGIN_H
