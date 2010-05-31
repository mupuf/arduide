/**
 * \file DebuggerPlugin.h
 * \author Martin Peres
 */

#ifndef DEBUGGER_PLUGIN_H
#define DEBUGGER_PLUGIN_H

#include "plugins/IDEPluginInterface.h"

class DebuggerWidget;

class DebuggerPlugin : public QObject, public IDEPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(IDEPluginInterface)

public:
    bool setup(IDEApplication *app);
    const QString &name() { return mName; };

private:
    IDEApplication *mApp;

    QString mName;
    DebuggerWidget *widget;
};

#endif // SERIALPLUGIN_H
