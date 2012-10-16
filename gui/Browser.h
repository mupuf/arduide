/*
  Browser.h

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
 * \file Browser.h
 * \author Denis Martinez
 */

#ifndef BROWSER_H
#define BROWSER_H

#include <QWebView>
#include <QVariant>

class Browser : public QWebView
{
    Q_OBJECT
public:
    Browser(QWidget *parent = NULL);

    void quickstart();
    void openDocumentation(const QString &fileName = QString());

    static QUrl toFileUrl(const QString &path);
    static QString toFileName(const QUrl &url);

    bool canGoBack();
    bool canGoForward();

signals:
    void newProjectRequested();
    void newProjectRequested(const QString &initialCode, const QString &name);
    void openProjectRequested();
    void openProjectRequested(const QString &fileName);
    void newPageLoaded(const QUrl& url);

private:
    QString mPage;
    QUrl mUrl;
    QList<QUrl> history;
    unsigned history_curr;

    void quickstart_p(bool updateHistory=true);
    void handleIdeLink(const QUrl &url, bool updateHistory=true);
    QByteArray getDocumentationHtml(const QString &fileName);
    void goToHistoryItem(unsigned index);
    void addItemToHistory(const QUrl& url);

protected:
    void initializeContext(QVariantHash &mapping);

protected slots:
    void handleLink(const QUrl &url);

public slots:
    void refresh();
    void back();
    void forward();
    bool docHelpRequested(QString);
};

#endif // BROWSER_H
