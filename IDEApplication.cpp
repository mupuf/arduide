/*
  IDEApplication.cpp

  This file is part of arduide, The Qt-based IDE for the open-source Arduino electronics prototyping platform.

  Copyright (C) 2010-2012 
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
 * \file IDEApplication.cpp
 * \author Denis Martinez
 * \date 2010-02-28
 */

#include "IDEApplication.h"

#include <QDir>
#include <QPluginLoader>
#include <QDebug>

#include "plugins/IDEPluginInterface.h"

#include <QTextCursor>
#include <QTextCharFormat>
void IDEApplication::registerMetaTypes()
{
    qRegisterMetaType<QTextCursor>("QTextCursor");
    qRegisterMetaType<QTextCharFormat>("QTextCharFormat");
}

IDEApplication::IDEApplication(int& argc, char **argv)
    : QApplication(argc, argv),
      mSettings(NULL)
{
    setOrganizationName(PROJECT_ORGANIZATION);
    setApplicationName(PROJECT_NAME);
    setApplicationVersion(PROJECT_VERSION);
    setWindowIcon(QIcon(":/images/arduide.png"));

    // translation support
    initializeTranslator();

    // fix the data path
    mDataPath = QDir(DATA_PATH).absolutePath();

    registerMetaTypes();

    // initialize Grantlee
    initializeTemplates();

    // start the GUI
    initializeGui();

    // check the settings are correct, or start the wizard
    initializeSettings();

    // further gui initialization
    mMainWindow->initialize();

    // initialize the plugins
    initializePlugins();
}

void IDEApplication::initializeTemplates()
{
    mEngine = new Grantlee::Engine(this);
    mEngine->setPluginPaths(QStringList() << QDir(GRANTLEE_PLUGIN_DIR).absolutePath());

    Grantlee::FileSystemTemplateLoader::Ptr loader = Grantlee::FileSystemTemplateLoader::Ptr(new Grantlee::FileSystemTemplateLoader);
    loader->setTemplateDirs(QStringList() << ":/templates");
    mEngine->addTemplateLoader(loader);
}

void IDEApplication::initializeGui()
{
    mMainWindow = new MainWindow;
    mMainWindow->show();
}

void IDEApplication::initializeSettings()
{
    mSettings = new Settings;

    if (! mSettings->isCorrect())
    {
        FirstTimeWizard w;
        if (w.exec() == QWizard::Rejected)
            ::exit(1);
    }

    mProjectHistory = new ProjectHistory(this);
}

void IDEApplication::initializePlugins()
{
    qDebug() << "initializePlugins: loading the plugins in " PLUGIN_PATH;
    mPluginLoader = new QPluginLoader(this);
    mPluginLoader->setLoadHints(QLibrary::ResolveAllSymbolsHint | QLibrary::ExportExternalSymbolsHint);
    QDir pluginDir(PLUGIN_PATH);
    QString fileName;
    bool ok;
    foreach(const QString &entry, pluginDir.entryList(QDir::Files, QDir::Name))
    {
        fileName = pluginDir.filePath(entry);
        mPluginLoader->setFileName(fileName);
        ok = mPluginLoader->load();
        qDebug() << "Loading" << entry << "result:" << ok;

        if (ok)
        {
            IDEPluginInterface *plugin = dynamic_cast<IDEPluginInterface *>(mPluginLoader->instance());
            ok = plugin != NULL && plugin->setup(this);
            qDebug() << "Initializing" << entry << "result:" << ok;
        }
        else
            qDebug() << mPluginLoader->errorString();
    }
}


void IDEApplication::initializeTranslator()
{
    if(! mTranslator.load("arduide_" + QLocale::system().name(), TRANSLATION_PATH))
     {
      qDebug() << "loading translator failed.";
     }
    installTranslator(&mTranslator);
}
