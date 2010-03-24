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
    void openDocumentation(const QString &fileName);

    static QUrl toFileUrl(const QString &path);
    static QString toFileName(const QUrl &url);

signals:
    void newProjectRequested();
    void newProjectRequested(const QString &initialCode, const QString &name);
    void openProjectRequested();
    void openProjectRequested(const QString &fileName);

private:
    QString mPage;
    QUrl mUrl;

protected:
    void initializeContext(QVariantHash &mapping);

protected slots:
    void handleLink(const QUrl &url);

public slots:
    void refresh();
};

#endif // BROWSER_H
