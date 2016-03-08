/*
  FirstTimeWizard.cpp

  This file is part of arduide, The Qt-based IDE for the open-source Arduino electronics prototyping platform.

  Copyright (C) 2010-2012 
  Authors : Denis Martinez
	    Martin Peres

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
 * \file FirstTimeWizard.cpp
 * \author Denis Martinez
 */

#include "FirstTimeWizard.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QDesktopServices>
#include <QNetworkReply>
#include <QNetworkDiskCache>
#include <QTemporaryFile>
#include <QtConcurrentRun>
#include <QFutureWatcher>
#include <QProcess>
#include <qxtsignalwaiter.h>
#include <qxttemporarydir.h>
#include <QDebug>

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#include <shlobj.h>
#endif

#include "env/Settings.h"
#include "env/Toolkit.h"
#include "IDEApplication.h"

FirstTimeWizard::FirstTimeWizard(QWidget *parent)
    : QWizard(parent)
{
    QString url;

    setupUi(this);

    // paths to search for an existing installation
    QStringList defaultArduinoPaths;

#ifdef Q_OS_DARWIN
    QString applicationPath = QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation);
    defaultArduinoPaths << QDir(applicationPath).filePath("Arduino.app");
#elif defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
    TCHAR applicationPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, 0, applicationPath)))
        defaultArduinoPaths
            << QDir(applicationPath).filePath("arduino-" ARDUINO_SDK_VERSION)
            << QDir(applicationPath).filePath("arduino");
#else
    QString applicationPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);

    defaultArduinoPaths
        << QDir(applicationPath).filePath("arduino-" ARDUINO_SDK_VERSION_NAME)
        << QDir(applicationPath).filePath("arduino")
        << "/usr/local/share/arduino-" ARDUINO_SDK_VERSION_NAME
        << "/usr/local/share/arduino"
        << "/usr/share/arduino-" ARDUINO_SDK_VERSION_NAME
        << "/usr/share/arduino";
#endif

    automaticInstallButton->setChecked(true);
    foreach(const QString &path, defaultArduinoPaths)
    {
        if (Toolkit::isValidArduinoPath(path))
        {
            arduinoPathEdit->setText(path);
            existingInstallButton->setChecked(true);
            break;
        }
    }

    QString sketchBookPath = ideApp->settings()->sketchPath();
    if (sketchBookPath.isEmpty())
        sketchBookPath = QDir(QDir::homePath()).filePath("sketchbook");

    sketchbookPathEdit->setText(sketchBookPath);
    projectLabel->setText(projectLabel->text().arg(PROJECT_NAME).arg(PROJECT_AUTHORS));
    urlLabel->setText(urlLabel->text().arg(PROJECT_URL));

    if(QString(ARDUINO_SDK_VERSION_NAME) >= "1.6.0")
        url = "https://downloads.arduino.cc/arduino-" ARDUINO_SDK_VERSION_NAME;
    else
        url = "http://arduino.googlecode.com/files/arduino-" ARDUINO_SDK_VERSION_NAME;

    // set up the download page
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64) // Windows
    mDownloadOs = "Windows";
    if (QString(ARDUINO_SDK_VERSION_NAME) >= "1.0.1")
        url += "-windows.zip";
    else
        url += ".zip";
#elif defined(Q_OS_DARWIN) // MacOSX
    #warn TODO: platform not supported yet
    mDownloadOs = "MacOSX";
    //mDownloadUrl = "http://arduino.googlecode.com/files/arduino-" ARDUINO_SDK_VERSION_NAME ".dmg" ;
#else // Linux, other Unix
    #if defined(__x86_64__) // 64-bit Unix
        mDownloadOs = "64-bit Linux";
        if (QString(ARDUINO_SDK_VERSION_NAME) >= "1.6.0")
            url += "-linux64.tar.xz";
	else if (QString(ARDUINO_SDK_VERSION_NAME) >= "1.0.1")
	    url += "-linux64.tgz";
        else
            url += "-64.tgz";
    #elif defined(__i386__) // 32-bit Unix
        mDownloadOs = "32-bit Linux";
	if (QString(ARDUINO_SDK_VERSION_NAME) >= "1.6.0")
            url += "-linux64.tar.xz";
        else if (QString(ARDUINO_SDK_VERSION_NAME) >= "1.0.1")
            url += "-linux32.tgz";
        else
            url += ".tgz";
    #else // other
        #error unsupported architecture
    #endif
#endif

    mDownloadUrl = url;

    downloadLabel->setText(downloadLabel->text().arg(ARDUINO_SDK_VERSION_NAME).arg(mDownloadOs));

    mDownloadManager = new QNetworkAccessManager(this);
    QNetworkDiskCache *downloadCache = new QNetworkDiskCache(mDownloadManager);
    downloadCache->setCacheDirectory(QDesktopServices::storageLocation(QDesktopServices::CacheLocation));
    mDownloadManager->setCache(downloadCache);

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

void FirstTimeWizard::initializePage(int id)
{
    if (id == 1)
    {
        downloadStatusLabel->setText(QString());
        installStatusLabel->setText(QString());
    }
}

bool FirstTimeWizard::validateCurrentPage()
{
    if (currentId() == 0)
    {
        QString path;
        bool ok;
        if (automaticInstallButton->isChecked())
            // reset the path
            ideApp->settings()->setArduinoPath(path);
        else
        {
#ifndef Q_OS_DARWIN
            path = arduinoPathEdit->text();
#else
            path = QDir(arduinoPathEdit->text()).filePath("Contents/Resources/Java");
#endif
            ok = Toolkit::isValidArduinoPath(path);
            if (! ok)
            {
                QMessageBox::warning(this, tr("Invalid path"), tr("Please enter a valid path to your Arduino installation."));
                return false;
            }
            else
                ideApp->settings()->setArduinoPath(path);
        }

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
    else if (currentId() == 1)
    {
        // do the download
        downloadProgressBar->setValue(0);
        QNetworkRequest request(mDownloadUrl);
        request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
        QNetworkReply *reply = mDownloadManager->get(request);
        QxtSignalWaiter downloadWaiter(reply, SIGNAL(finished()));
        connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(onDownloadProgress(qint64, qint64)));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onDownloadError(QNetworkReply::NetworkError)));
        QList<QAbstractButton *> buttons = QList<QAbstractButton *>()
            << button(BackButton)
            << button(NextButton)
            << button(CancelButton);
        foreach (QAbstractButton *button, buttons)
            button->setEnabled(false);

        downloadWaiter.wait();
        downloadStatusLabel->setPixmap(QPixmap(":/images/16x16/task-complete.png"));

        // extract the archive
        QTemporaryFile archive("arduino");
        bool extractSuccess = archive.open();
        QString destinationPath = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
        QDir destinationDir(destinationPath);
        if (! destinationDir.exists())
            extractSuccess = extractSuccess && destinationDir.mkpath(".");

        if (extractSuccess)
        {
            // write the reply to the temporary file
            QByteArray buffer;
            static const int bufferSize = 8192;
            buffer.resize(bufferSize);
            qint64 readBytes = reply->read(buffer.data(), bufferSize);
            while (readBytes > 0)
            {
                archive.write(buffer.data(), readBytes);
                readBytes = reply->read(buffer.data(), bufferSize);
            }
            archive.seek(0);

            // call tar to extract
            QString extractCommand;
            QStringList extractArgs = QStringList();

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64) // Windows
            QxtTemporaryDir unzipDir(QDir::tempPath() + "/unzip");
            QString unzipFileName = unzipDir.dir().filePath("unzip.exe");
            QFile::copy(":/windows/unzip.exe", unzipFileName);
            extractCommand = unzipFileName;
            extractArgs << "-d" << destinationPath << archive.fileName();
#elif defined(Q_OS_DARWIN) // MacOSX
    #warn TODO: platform not supported yet
#else // Linux, other Unix
            extractCommand = "tar";
            if (QString(ARDUINO_SDK_VERSION_NAME) >= "1.6.0")
                extractArgs = QStringList()
                    << "-x" << "-J" << "-f" << archive.fileName()
                    << "-C" << destinationPath;
            else
                extractArgs = QStringList()
                    << "-x" << "-z" << "-f" << archive.fileName()
                    << "-C" << destinationPath;
#endif
            QFutureWatcher<int> extractWatcher;
            QxtSignalWaiter extractWaiter(&extractWatcher, SIGNAL(finished()));
            QFuture<int> extractFuture = QtConcurrent::run(&QProcess::execute, extractCommand, extractArgs);
            extractWatcher.setFuture(extractFuture);
            extractWaiter.wait();
            extractSuccess = extractFuture.result() == 0;
        }

        if (! extractSuccess)
        {
            QMessageBox::warning(
                this,
                tr("Installation error"),
                tr("An error occured during the extraction."));
            foreach (QAbstractButton *button, buttons)
                button->setEnabled(true);
            return false;
        }

        installStatusLabel->setPixmap(QPixmap(":/images/16x16/task-complete.png"));

        QString arduinoPath = QDir(destinationPath).filePath("arduino-" ARDUINO_SDK_VERSION_NAME);
        if (! Toolkit::isValidArduinoPath(arduinoPath))
        {
            QMessageBox::warning(
                this,
                tr("Installation error"),
                tr("The extracted Arduino package is not valid."));
            foreach (QAbstractButton *button, buttons)
                button->setEnabled(true);
            return false;
        }
        ideApp->settings()->setArduinoPath(arduinoPath);

        QMessageBox::information(
            this,
            tr("Installation"),
            tr("Arduino was successfully installed to:") + "\n" + arduinoPath);

        foreach (QAbstractButton *button, buttons)
            button->setEnabled(true);
    }
    return true;
}

int FirstTimeWizard::nextId() const
{
    if (currentId() == 0 && existingInstallButton->isChecked())
        // skip the automatic install page
        return 2;
    return QWizard::nextId();
}

void FirstTimeWizard::onDownloadProgress(qint64 received, qint64 total)
{
    int percent = 0;
    if(total)
       percent = 100 * received / total;
    downloadProgressBar->setValue(percent);
}

void FirstTimeWizard::onDownloadError(QNetworkReply::NetworkError)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QList<QAbstractButton *> buttons = QList<QAbstractButton *>()
            << button(BackButton)
            << button(NextButton)
            << button(CancelButton);
    QMessageBox::warning(
                this,
                tr("Download error"),
                tr("An error occured during the download:") + " " + reply->errorString());
    foreach (QAbstractButton *button, buttons)
        button->setEnabled(true);
}
