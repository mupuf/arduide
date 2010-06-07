/**
 * \file PageRenderer.cpp
 * \author Denis Martinez
 */

#include "PageRenderer.h"

#include <QDir>
#include <QDebug>

#include <grantlee_core.h>

#include "Browser.h"
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

        QString path = QString("/templates/%0").arg(pageName);

        // the url determines the relative path to the data files (css, js)
        mUrl.setScheme("qrc");
        mUrl.setPath(path);

        mPage = t->render(&c);
    }
    catch(Grantlee::Exception e)
    {
        qDebug("PageRenderer::PageRenderer: %s", qPrintable(e.what()));
    }
}
