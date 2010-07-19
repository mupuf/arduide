/**
 * \file Browser.cpp
 * \author Denis Martinez
 */

#include "Browser.h"

#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDesktopServices>
#include <QMessageBox>
#include <QWebFrame>
#include <QWebHistory>
#include <QStatusBar>
#include <QDebug>

#include <grantlee_core.h>

#include "IDEApplication.h"
#include "PageRenderer.h"
#include "../env/Toolkit.h"
#include "../env/ProjectHistory.h"

Browser::Browser(QWidget *parent)
    : QWebView(parent), history_curr(0)
{
#ifndef QT_NO_DEBUG
    settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
#endif

    page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect(this, SIGNAL(linkClicked(const QUrl &)), this, SLOT(handleLink(const QUrl &)));
}

void Browser::quickstart()
{
    quickstart_p();
}

void Browser::quickstart_p(bool updateHistory)
{
    // generate the quickstart page
    QVariantHash mapping;
    initializeContext(mapping);

    QVariantList exampleCategories;
    foreach (const QString& cat, Toolkit::findExampleCategories())
    {
        QVariantHash catDescription;
        catDescription.insert("name", cat);
        QVariantList examples;
        foreach (const QString &example, Toolkit::findExamples(cat))
            examples.append(example);
        catDescription.insert("examples", examples);
        exampleCategories << catDescription;
    }

    QVariantList librariesWithExamples;
    foreach (const QString& lib, Toolkit::librariesWithExamples())
    {
        QVariantHash libDescription;
        libDescription.insert("name", lib);
        QVariantList examples;
        foreach (const QString &example, Toolkit::findLibraryExamples(lib))
            examples.append(example);
        libDescription.insert("examples", examples);
        librariesWithExamples << libDescription;
    }

    mapping.insert("exampleCategories", exampleCategories);
    mapping.insert("librariesWithExamples", librariesWithExamples);

    QVariantList recentProjects;
    ProjectHistory *pHistory = ideApp->projectHistory();
    foreach (const QString &project, pHistory->history(10))
    {
        QString name = QFileInfo(project).fileName();
        name = name.left(name.lastIndexOf('.'));

        QVariantHash projectInfo;
        projectInfo["name"] = name;
        projectInfo["filename"] = project;
        recentProjects << projectInfo;
    }
    mapping.insert("recentProjects", recentProjects);

    QVariantList sketches;
    foreach (const QString &sketch, Toolkit::findSketchesInDirectory(ideApp->settings()->sketchPath()))
        sketches.append(sketch);
    mapping.insert("sketches", sketches);

    mapping.insert("documentationHtml", getDocumentationHtml("index.html"));

    PageRenderer *renderer = new PageRenderer;
    renderer->render("quickstart.html", mapping);
    mPage = renderer->page();
    mUrl = renderer->url();
    setHtml(mPage, mUrl);
    delete renderer;

    if (updateHistory)
        addItemToHistory(QUrl("ide://quickstart/"));
}

void Browser::initializeContext(QVariantHash &mapping)
{
    mapping.insert("applicationName", qApp->applicationName());
    mapping.insert("applicationUrl", PROJECT_URL);
    mapping.insert("referencePath", toFileUrl(Toolkit::referencePath()));
}

void Browser::handleLink(const QUrl &url)
{
    if (url.scheme() == "ide")
        handleIdeLink(url);
    else
        QDesktopServices::openUrl(url);
}

void Browser::handleIdeLink(const QUrl &url, bool updateHistory)
{
    // qDebug("Open url '%s'", qPrintable(url.toString()));

    if (url.host() == "quickstart")
    {
        quickstart_p(updateHistory);
    }
    if (url.host() == "new-project")
        // empty project
        emit newProjectRequested();
    else if (url.host() == "example")
    {
        // new project from example
        QFileInfo fi(url.path());
        QString filename = Toolkit::exampleFileName(fi.dir().dirName(), fi.fileName());

        QFile f(filename);
        if (f.open(QFile::ReadOnly))
        {
            QString code = QString::fromLocal8Bit(f.readAll());
            emit newProjectRequested(code, fi.fileName());
        } else
            QMessageBox::warning(this, tr("Load error"), tr("The selected example could not be opened."));
    }
    else if (url.host() == "library-example")
    {
        // new project from example
        QFileInfo fi(url.path());
        QString filename = Toolkit::libraryExampleFileName(fi.dir().dirName(), fi.fileName());

        QFile f(filename);
        if (f.open(QFile::ReadOnly))
        {
            QString code = QString::fromLocal8Bit(f.readAll());
            emit newProjectRequested(code, fi.fileName());
        } else
            QMessageBox::warning(this, tr("Load error"), tr("The selected example could not be opened."));
    }
    else if (url.host() == "open-project")
    {
        QString path = QDir::cleanPath(url.path());
        if (path.isEmpty())
            emit openProjectRequested();
        else
            emit openProjectRequested(path);
    }
    else if (url.host() == "open-sketch")
    {
        QString sketchName = url.path().section("/", 1);
        QString fileName = QDir(ideApp->settings()->sketchPath()).filePath(QString("%0/%0.pde").arg(sketchName));
        emit openProjectRequested(fileName);
    }
    else if (url.host() == "open-documentation")
    {
        QString path = url.path();
        if (path[0] == '/')
            path = path.mid(1);
        openDocumentation(path);

        if (updateHistory)
            addItemToHistory(url);
    }

    emit newPageLoaded(url);
}

void Browser::openDocumentation(const QString &fileName)
{
    QString content = "openDocumentation('{{ html|escapejs }}'); showDocTab();";
    Grantlee::Template t = ideApp->engine()->newTemplate(content, "js");
    QVariantHash mapping;
    QByteArray html = getDocumentationHtml(fileName.isEmpty() ? "index.html" : fileName);
    mapping.insert("html", html);
    Grantlee::Context context(mapping);
    page()->mainFrame()->evaluateJavaScript(t->render(&context));
}

QByteArray Browser::getDocumentationHtml(const QString &fileName)
{
    QByteArray html;
    QString referencePath = Toolkit::referencePath();
    QString fullPath = QDir(referencePath).filePath(fileName);
    QFile f(fullPath);
    if (! f.open(QFile::ReadOnly))
        return html;
    html = f.readAll();
    f.close();
    return html;
}

void Browser::goToHistoryItem(unsigned index)
{
    if (index<(unsigned)history.size())
    {
        history_curr = index;
        refresh();
    }
}

void Browser::addItemToHistory(const QUrl& url)
{
    while ((unsigned)history.size() > history_curr+1)
            history.pop_back();

    history.append(url);
    history_curr = history.size()-1;
}

void Browser::refresh()
{
    if ((unsigned)history.size()>history_curr)
        handleIdeLink(history[history_curr], false);
}

void Browser::back()
{
    goToHistoryItem(history_curr-1);
}

void Browser::forward()
{
    goToHistoryItem(history_curr+1);
}

bool Browser::docHelpRequested(QString name)
{
    QString originalName=name;

    // Shape up
    name[0]=name[0].toUpper();

    int pos=0;
    while ((pos = name.indexOf('.', pos)+1) != 0)
    {
        name[pos]=name[pos].toUpper();
    }
    name = name.replace(QRegExp("Serial\\d"), "Serial");
    name=name.replace('.', '_');

    name+=".html";

    // Does the doc exists ?
    if (getDocumentationHtml(name).size() == 0)
    {
        ideApp->mainWindow()->statusBar()->showMessage(tr("No documentation found for %1.").arg(originalName), 1500);
        return false;
    }

    // Load the doc
    handleIdeLink(QString("ide://open-documentation/%1").arg(name), true);
    return true;
}

QUrl Browser::toFileUrl(const QString &path)
{
    QUrl url;
    url.setScheme("file");
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
    // on windows it's file:/// so an extra slash is required
    url.setPath(QString("/") + path);
#else
    url.setPath(path);
#endif
    return url;
}

QString Browser::toFileName(const QUrl &url)
{
    QString path = url.path();
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
    // remove the initial '/' introduced by file:///
    if (! path.isEmpty() && path[0] == '/')
        return path.mid(1);
#endif
    return url.path();
}

bool Browser::canGoBack()
{
    return history_curr>0;
}

bool Browser::canGoForward()
{
    return history_curr+1<(unsigned)history.size();
}
