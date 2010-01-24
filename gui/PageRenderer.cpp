/**
 * \file PageRenderer.cpp
 * \author Denis Martinez
 */

#include "PageRenderer.h"

#include <QDir>
#include <grantlee/engine.h>

PageRenderer::PageRenderer(const QString &pageName, const QVariantHash &mapping)
{
    Grantlee::Engine *engine = Grantlee::Engine::instance();
    Grantlee::Template t = engine->loadByName(pageName);
    Grantlee::Context c(mapping);

    mUrl.setScheme("file");
    mUrl.setPath(QDir(DATA_PATH).filePath(QString("templates/%0").arg(pageName)));
    mPage = t->render(&c);
}
