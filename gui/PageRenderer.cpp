/**
 * \file PageRenderer.cpp
 * \author Denis Martinez
 */

#include "PageRenderer.h"

#include <QDir>
#include <QDebug>

#include <grantlee_core.h>

#include "IDEApplication.h"

PageRenderer::PageRenderer()
{
}

void PageRenderer::render(const QString &pageName, const QVariantHash &mapping)
{
    try
    {
        Grantlee::Template t = ideApp->engine()->loadByName(pageName);
        Grantlee::Context c(mapping);

        QString path = QDir(ideApp->dataPath()).filePath(QString("templates/%0").arg(pageName));

        // the url determines the relative path to the data files (css, js)
        mUrl.setScheme("file");
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
        // on windows it's file:/// so an extra slash is required
        mUrl.setPath(QString("/") + path);
#else
        mUrl.setPath(path);
#endif
        mPage = t->render(&c);
    }
    catch(Grantlee::Exception e)
    {
        qDebug("PageRenderer::PageRenderer: %s", qPrintable(e.what()));
    }
}
