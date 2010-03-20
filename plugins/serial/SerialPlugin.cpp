/**
 * \file SerialPlugin.cpp
 * \author Denis Martinez
 */

#include "SerialPlugin.h"

#include <QDebug>

#include "SerialWidget.h"
#include "IDEApplication.h"

bool SerialPlugin::setup(IDEApplication *app)
{
    mName = tr("Serial");

    widget = new SerialWidget;
    app->mainWindow()->utilityTabWidget()->addTab(widget, name());

    return true;
}

Q_EXPORT_PLUGIN2(serial, SerialPlugin)
