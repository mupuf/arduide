/**
 * \file PageRenderer.cpp
 * \author Denis Martinez
 */

#include "PageRenderer.h"

#include <QDir>
#include <QDebug>

#include <grantlee.h>

#include "IDEApplication.h"

PageRenderer::PageRenderer(const QString &pageName, const QVariantHash &mapping)
{
    try
    {
        Grantlee::Engine *engine = Grantlee::Engine::instance();
        Grantlee::Template t = engine->loadByName(pageName);
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
