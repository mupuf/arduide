/*
  PageRenderer.cpp

  This file is part of arduide, The Qt-based IDE for the open-source Arduino electronics prototyping platform.

  Copyright (C) 2010-2012 
  Authors : Denis Martinez
	    Martin Peres

This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
