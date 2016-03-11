/*
  IDEApplication.h

  This file is part of arduide, The Qt-based IDE for the open-source Arduino electronics prototyping platform.

  Copyright (C) 2010-2016 
  Authors : Denis Martinez
	    Martin Peres 
  Copyright (C) 2011 
	    Laurent Navet (translation support)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * \file IDEApplication.h
 * \author Denis Martinez
 * \date 2010-02-28
 */

#ifndef IDEAPPLICATION_H
#define IDEAPPLICATION_H

#include <QApplication>
#include <QTranslator>
#include <grantlee_core.h>

#include "gui/MainWindow.h"
#include "gui/FirstTimeWizard.h"
#include "env/ProjectHistory.h"
#include "env/Settings.h"
#include "IDEGlobal.h"

class QPluginLoader;

class IDE_EXPORT IDEApplication : public QApplication
{
public:
    IDEApplication(int& argc, char **argv);

    const QString &dataPath() { return mDataPath; }
    MainWindow *mainWindow() { return mMainWindow; }
    Grantlee::Engine *engine() { return mEngine; }
    ProjectHistory *projectHistory() { return mProjectHistory; }
    Settings *settings() { return mSettings; }

private:
    void registerMetaTypes();
    void initializeTemplates();
    void initializeGui();
    void initializeSettings();
    void initializePlugins();
    void initializeTranslator();

    QString mDataPath;
    MainWindow *mMainWindow;
    Grantlee::Engine *mEngine;
    QPluginLoader *mPluginLoader;
    ProjectHistory *mProjectHistory;
    Settings *mSettings;
    QTranslator mTranslator;
};

#define ideApp (static_cast<IDEApplication *>(qApp))

#endif // IDEAPPLICATION_H
