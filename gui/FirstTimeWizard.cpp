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
        << QDir(applicationPath).filePath("arduino-" ARDUINO_SDK_VERSION)
        << QDir(applicationPath).filePath("arduino")
        << "/usr/local/share/arduino-" ARDUINO_SDK_VERSION
        << "/usr/local/share/arduino"
        << "/usr/share/arduino-" ARDUINO_SDK_VERSION
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

    sketchbookPathEdit->setText(QDir(QDir::homePath()).filePath("sketchbook"));
    projectLabel->setText(projectLabel->text().arg(PROJECT_NAME).arg(PROJECT_AUTHORS));
    urlLabel->setText(urlLabel->text().arg(PROJECT_URL));

    // set up the download page
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64) // Windows
#pragma message("TODO: platform not supported yet")
    mDownloadOs = "Windows";
#elif defined(Q_OS_DARWIN) // MacOSX
#warn TODO: platform not supported yet
    mDownloadOs = "MacOSX";
#else // Linux, other Unix
#if defined(__x86_64__) // 64-bit Unix
    mDownloadOs = "64-bit Linux";
    mDownloadUrl = "http://arduino.googlecode.com/files/arduino-" ARDUINO_SDK_VERSION "-64-2.tgz" ;
#elif defined(__i386__) // 32-bit Unix
    mDownloadOs = "32-bit Linux";
    mDownloadUrl = "http://arduino.googlecode.com/files/arduino-" ARDUINO_SDK_VERSION ".tgz";
#else // other
#error unsupported architecture
#endif
#endif

    downloadLabel->setText(downloadLabel->text().arg(ARDUINO_SDK_VERSION).arg(mDownloadOs));

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
        QList<QAbstractButton *> buttons = QList<QAbstractButton *>()
            << button(BackButton)
            << button(NextButton)
            << button(CancelButton);
        foreach (QAbstractButton *button, buttons)
            button->setEnabled(false);
        downloadWaiter.wait();

        if (reply->error() != QNetworkReply::NoError)
        {
            QMessageBox::warning(
                this,
                tr("Download error"),
                tr("An error occured during the download:") + " " + reply->errorString());
            foreach (QAbstractButton *button, buttons)
                button->setEnabled(true);
            return false;
        }

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
            QString tarCommand = "tar";
            QStringList tarArgs = QStringList()
                << "-x" << "-z" << "-f" << archive.fileName()
                << "-C" << destinationPath;

            QFutureWatcher<int> tarWatcher;
            QxtSignalWaiter tarWaiter(&tarWatcher, SIGNAL(finished()));
            QFuture<int> tarFuture = QtConcurrent::run(&QProcess::execute, tarCommand, tarArgs);
            tarWatcher.setFuture(tarFuture);
            tarWaiter.wait();
            extractSuccess = tarFuture.result() == 0;
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

        QString arduinoPath = QDir(destinationPath).filePath("arduino-" ARDUINO_SDK_VERSION);
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
    int percent = 100 * received / total;
    downloadProgressBar->setValue(percent);
}
