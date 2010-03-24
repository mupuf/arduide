/**
 * \file PageRenderer.h
 * \author Denis Martinez
 */

#ifndef PAGERENDERER_H
#define PAGERENDERER_H

#include <QUrl>
#include <QString>
#include <QVariantHash>

class PageRenderer
{
public:
    PageRenderer();
    void render(const QString &pageName, const QVariantHash &mapping);
    const QString &page() { return mPage; }
    const QUrl &url() { return mUrl; }

private:
    QString mPage;
    QUrl mUrl;
};

#endif // PAGERENDERER_H
