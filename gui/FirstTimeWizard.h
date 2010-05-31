/**
 * \file FirstTimeWizard.h
 * \author Denis Martinez
 */

#ifndef FIRSTTIMEWIZARD_H
#define FIRSTTIMEWIZARD_H

#include <QUrl>
#include <QNetworkAccessManager>

#include <ui_FirstTimeWizard.h>

class FirstTimeWizard : public QWizard, Ui::FirstTimeWizard
{
    Q_OBJECT

public:
    FirstTimeWizard(QWidget *parent = NULL);
    void initializePage(int id);
    bool validateCurrentPage();
    int nextId() const;

private slots:
    void chooseArduinoPath();
    void chooseSketchbookPath();
    void onDownloadProgress(qint64 received, qint64 total);

private:
    void setupActions();

    QNetworkAccessManager *mDownloadManager;
    QString mDownloadOs;
    QUrl mDownloadUrl;
};

#endif // FIRSTTIMEWIZARD_H
