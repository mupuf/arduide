/**
 * \file IDEApplication.cpp
 * \author Denis Martinez
 * \date 2010-02-28
 */

#include "IDEApplication.h"

#include <QDir>
#include <QDebug>

IDEApplication::IDEApplication(int argc, char **argv)
    : QApplication(argc, argv)
{
    setOrganizationName(PROJECT_ORGANIZATION);
    setApplicationName(PROJECT_NAME);
    setApplicationVersion(PROJECT_VERSION);

    // fix the data path
    mDataPath = QDir(DATA_PATH).absolutePath();

    // initialize Grantlee
    initializeTemplates();

    // start the GUI
    initializeGui();

    // check the settings are correct, or start the wizard
    initializeSettings();

    // further gui initialization
    mainWindow->initialize();
}

void IDEApplication::initializeTemplates()
{
    mEngine = new Grantlee::Engine(this);
    mEngine->setPluginDirs(QStringList() << GRANTLEE_PLUGIN_DIR "/");
    Grantlee::FileSystemTemplateLoader::Ptr loader = Grantlee::FileSystemTemplateLoader::Ptr(new Grantlee::FileSystemTemplateLoader);
    loader->setTemplateDirs(QStringList() << QDir(mDataPath).filePath("templates"));
    mEngine->addTemplateLoader(loader);
}

void IDEApplication::initializeGui()
{
    mainWindow = new MainWindow;
    mainWindow->show();
}

void IDEApplication::initializeSettings()
{
    if (! Settings::instance().isCorrect())
    {
        FirstTimeWizard w;
        if (w.exec() == QWizard::Rejected)
            exit(1);
    }

    mProjectHistory = new ProjectHistory(this);
}
