/**
 * \file PageRenderer.cpp
 * \author Denis Martinez
 */

#include "PageRenderer.h"

#include <QDir>
#include <QDebug>

#include <grantlee_core.h>

#include "IDEApplication.h"

PageRenderer::PageRenderer(const QString &pageName, const QVariantHash &mapping)
{
    try
    {
        Grantlee::Template t = ideApp->engine()->loadByName(pageName);
        Grantlee::Context c(mapping);

        mUrl.setScheme("file");
        mUrl.setPath(QDir(ideApp->dataPath()).filePath(QString("templates/%0").arg(pageName)));
        mPage = t->render(&c);
    }
    catch(Grantlee::Exception e)
    {
        qDebug("PageRenderer::PageRenderer: %s", qPrintable(e.what()));
    }
}
