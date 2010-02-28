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

#include "../env/Settings.h"
#include "../env/Toolkit.h"

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
    QString applicationPath = QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation);
    defaultArduinoPath = QDir(applicationPath).filePath("Arduino");
#else
    defaultArduinoPath = "/usr/share/arduino";
#endif
    arduinoPathEdit->setText(defaultArduinoPath);

    sketchbookPathEdit->setText(QDir(QDir::homePath()).filePath("sketchbook"));
    urlLabel->setText(QString("<a href=\"%0\">%0</a>").arg(PROJECT_URL));

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
            Settings::instance().setArduinoPath(path);

        path = sketchbookPathEdit->text();
        ok = QFileInfo(path).isDir() || QDir().mkdir(path);
        if (! ok)
        {
            QMessageBox::warning(this, tr("Invalid path"), tr("The sketchbook path could not be created. Please choose another."));
            return false;
        }
        else
            Settings::instance().setSketchPath(path);
    }
    return true;
}
