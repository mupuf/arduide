/**
 * \file SerialPlugin.cpp
 * \author Denis Martinez
 */

#include "SerialPlugin.h"

#include <QDebug>

static const QString PLUGIN_NAME("Serial");

bool SerialPlugin::setup(IDEApplication *app)
{
    return true;
}

const QString &SerialPlugin::name()
{
    return PLUGIN_NAME;
}

Q_EXPORT_PLUGIN2(serial, SerialPlugin)
