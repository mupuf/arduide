/**
 * \file IDEPluginInterface.h
 * \author Denis Martinez
 */

#ifndef IDEPLUGININTERFACE_H
#define IDEPLUGININTERFACE_H

#include <QtPlugin>

class IDEApplication;

class IDEPluginInterface
{
public:
    virtual ~IDEPluginInterface() = 0;
    virtual bool setup(IDEApplication *app) = 0;
    virtual const QString &name() = 0;
};

Q_DECLARE_INTERFACE(IDEPluginInterface, "org.mupuf.ArduIDE.IDEPluginInterface/1.0")

#endif // IDEPLUGININTERFACE_H
