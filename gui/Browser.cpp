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

#include <grantlee/engine.h>

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
    
    QVariantList projects;
    ProjectHistory& pHistory=ProjectHistory::instance();
    QStringList projects_s=pHistory.history(10);
    for(int i=0; i<projects_s.size(); i++)
    {  
        QString name=QFileInfo(projects_s.at(i)).fileName();
        name=name.left(name.lastIndexOf('.'));
        
        QVariantHash projDescription;
        projDescription.insert("name", name);
        projDescription.insert("path", projects_s.at(i));
        
        projects << projDescription;
    }
    mapping.insert("projects", projects);

    QVariantList sketches;
    foreach (const QString &sketch, Toolkit::findSketchesInDirectory(Settings::instance().sketchPath()))
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
            if (url.path().isEmpty())
                emit openProjectRequested();
            else
                emit openProjectRequested(url.path());
        }
        else if (url.host() == "open-sketch")
        {
            QString sketchName = url.path().section("/", 1);
            QString fileName = QDir(Settings::instance().sketchPath()).filePath(QString("%0/%0.pde").arg(sketchName));
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
    Grantlee::Template t = Grantlee::Engine::instance()->newTemplate(content, "js");
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
