/**
 * \file main.cpp
 * \author Denis Martinez
 */

#include <QApplication>
#include <QDebug>
#include <engine.h>

#include "gui/MainWindow.h"
#include "gui/FirstTimeWizard.h"
#include "env/Settings.h"

void initializeTemplates()
{
    Grantlee::Engine *engine = Grantlee::Engine::instance();
    engine->setPluginDirs(QStringList() << GRANTLEE_PLUGIN_DIR "/");
    Grantlee::FileSystemTemplateLoader::Ptr loader = Grantlee::FileSystemTemplateLoader::Ptr(new Grantlee::FileSystemTemplateLoader);
    loader->setTemplateDirs(QStringList() << DATA_PATH "/templates");
    engine->addTemplateLoader(loader);
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setOrganizationName("mupuf.org");
    app.setApplicationName("Arduino IDE");

    // initialize Grantlee
    initializeTemplates();

    // start the GUI
    MainWindow mw;
    mw.show();

    // check the Arduino path is defined, or start the wizard
    if (! Settings::instance().isCorrect())
    {
        FirstTimeWizard w;
        if (w.exec() == QWizard::Rejected)
            return 1;
    }

    mw.initialize();

    return app.exec();
}
