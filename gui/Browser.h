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
