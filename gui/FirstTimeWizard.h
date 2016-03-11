/*
  FirstTimeWizard.h

  This file is part of arduide, The Qt-based IDE for the open-source Arduino electronics prototyping platform.

  Copyright (C) 2010-2016 
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
 * \file FirstTimeWizard.h
 * \author Denis Martinez
 */

#ifndef FIRSTTIMEWIZARD_H
#define FIRSTTIMEWIZARD_H

#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>

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
    void onDownloadError(QNetworkReply::NetworkError);

private:
    void setupActions();

    QNetworkAccessManager *mDownloadManager;
    QString mDownloadOs;
    QUrl mDownloadUrl;
};

#endif // FIRSTTIMEWIZARD_H
