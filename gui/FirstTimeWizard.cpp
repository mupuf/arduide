/**
 * \file FirstTimeWizard.cpp
 * \author Denis Martinez
 */

#include "FirstTimeWizard.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDebug>

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#include <shlobj.h>
#endif

#include "../env/Settings.h"
#include "../env/Toolkit.h"
#include "IDEApplication.h"

FirstTimeWizard::FirstTimeWizard(QWidget *parent)
    : QWizard(parent)
{
    setupUi(this);

    // default path to display in the box
    QString defaultArduinoPath;
#ifdef Q_OS_DARWIN
    QString applicationPath = QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation);
    defaultArduinoPath = QDir(applicationPath).filePath("Arduino.app");
#elif defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
    // QString applicationPath = QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation);
    TCHAR applicationPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, 0, applicationPath)))
        defaultArduinoPath = QDir(applicationPath).filePath("arduino-" ARDUINO_SDK_VERSION);
#else
    defaultArduinoPath = "/usr/share/arduino";
#endif
    arduinoPathEdit->setText(defaultArduinoPath);

    sketchbookPathEdit->setText(QDir(QDir::homePath()).filePath("sketchbook"));
    projectLabel->setText(projectLabel->text().arg(PROJECT_NAME).arg(PROJECT_AUTHORS));
    urlLabel->setText(urlLabel->text().arg(PROJECT_URL));

    setupActions();
}

void FirstTimeWizard::setupActions()
{
    connect(arduinoPathButton, SIGNAL(clicked()), this, SLOT(chooseArduinoPath()));
    connect(sketchbookPathButton, SIGNAL(clicked()), this, SLOT(chooseSketchbookPath()));
}

void FirstTimeWizard::chooseArduinoPath()
{
    QString path;
#ifndef Q_OS_DARWIN
    path = QFileDialog::getExistingDirectory(this);
#else
    path = QFileDialog::getOpenFileName(this,
					QString(),
					QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation),
					tr("Applications (*.app)"));
#endif

    if (! path.isEmpty())
        arduinoPathEdit->setText(path);
}

void FirstTimeWizard::chooseSketchbookPath()
{
    QString path;
    path = QFileDialog::getExistingDirectory(this);

    if (! path.isEmpty())
        sketchbookPathEdit->setText(path);
}

bool FirstTimeWizard::validateCurrentPage()
{
    if (currentId() == 0)
    {
        QString path;
#ifndef Q_OS_DARWIN
	path = arduinoPathEdit->text();
#else
	path = QDir(arduinoPathEdit->text()).filePath("Contents/Resources/Java");
#endif
        bool ok = Toolkit::isValidArduinoPath(path);
        if (! ok)
        {
            QMessageBox::warning(this, tr("Invalid path"), tr("Please enter a valid path to your Arduino installation."));
            return false;
        }
        else
            ideApp->settings()->setArduinoPath(path);

        path = sketchbookPathEdit->text();
        ok = QFileInfo(path).isDir() || QDir().mkdir(path);
        if (! ok)
        {
            QMessageBox::warning(this, tr("Invalid path"), tr("The sketchbook path could not be created. Please choose another."));
            return false;
        }
        else
            ideApp->settings()->setSketchPath(path);
    }
    return true;
}
