/**
 * \file IDEApplication.h
 * \author Denis Martinez
 * \date 2010-02-28
 */

#ifndef IDEAPPLICATION_H
#define IDEAPPLICATION_H

#include <QApplication>
#include <grantlee_core.h>

#include "gui/MainWindow.h"
#include "gui/FirstTimeWizard.h"
#include "env/ProjectHistory.h"
#include "env/Settings.h"

class IDEApplication : public QApplication
{
public:
    IDEApplication(int argc, char **argv);

    const QString &dataPath() { return mDataPath; }
    Grantlee::Engine *engine() { return mEngine; }
    ProjectHistory *projectHistory() { return mProjectHistory; }
    Settings *settings() { return mSettings; }

private:
    void initializeTemplates();
    void initializeGui();
    void initializeSettings();

    QString mDataPath;
    MainWindow *mainWindow;
    Grantlee::Engine *mEngine;
    ProjectHistory *mProjectHistory;
    Settings *mSettings;
};

#define ideApp (static_cast<IDEApplication *>(qApp))

#endif // IDEAPPLICATION_H
