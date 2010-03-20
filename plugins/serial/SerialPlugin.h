/**
 * \file SerialPlugin.h
 * \author Denis Martinez
 */

#ifndef SERIALPLUGIN_H
#define SERIALPLUGIN_H

#include "plugins/IDEPluginInterface.h"

class SerialWidget;

class SerialPlugin : public QObject, public IDEPluginInterface
{
    Q_INTERFACES(IDEPluginInterface)

public:
    bool setup(IDEApplication *app);

    const QString &name() { return mName; };

private:
    QString mName;
    SerialWidget *widget;
};

#endif // SERIALPLUGIN_H
