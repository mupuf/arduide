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
#include <QDebug>

#include <grantlee_core.h>

#include "IDEApplication.h"
#include "PageRenderer.h"
#include "../env/Toolkit.h"
#include "../env/ProjectHistory.h"

Browser::Browser(QWidget *parent)
    : QWebView(parent)
{
    page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect(this, SIGNAL(linkClicked(const QUrl &)), this, SLOT(handleLink(const QUrl &)));
}

void Browser::quickstart()
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

    PageRenderer r("quickstart.html", mapping);
    setHtml(r.page(), r.url());
}

void Browser::initializeContext(QVariantHash &mapping)
{
    mapping.insert("applicationName", qApp->applicationName());
    mapping.insert("applicationUrl", PROJECT_URL);
    mapping.insert("referencePath", Toolkit::referencePath());
}

void Browser::handleLink(const QUrl &url)
{
    if (url.scheme() == "ide")
    {
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
    }
    else if (url.scheme() == "file")
    {
        QDir docDir(Toolkit::referencePath());
        QFileInfo fi(url.path());
        if (fi.dir() == docDir)
        {
            // documentation
            openDocumentation(fi.fileName());
        }
        else
            QDesktopServices::openUrl(url);
    }
    else
        QDesktopServices::openUrl(url);
}

void Browser::openDocumentation(const QString &fileName)
{
    const QString content = "openDocumentation('{{ fileName|escapejs }}');";
    Grantlee::Template t = ideApp->engine()->newTemplate(content, "js");
    QVariantHash mapping;
    mapping.insert("fileName", fileName);
    Grantlee::Context context(mapping);

    page()->mainFrame()->evaluateJavaScript(t->render(&context));
}

void Browser::refresh()
{
    // we should handle this in better way (reload and show the right page)
    quickstart();
}
