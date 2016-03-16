/*
  PageRenderer.h

  This file is part of arduide, The Qt-based IDE for the open-source Arduino electronics prototyping platform.

  Copyright (C) 2010-2016 
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
