/*
  OutputView.h

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
 * \file OutputView.h
 * \author Denis Martinez
 */

#ifndef OUTPUTVIEW_H
#define OUTPUTVIEW_H

#include <QTextBrowser>

#include "env/ILogger.h"

#include "IDEGlobal.h"

class IDE_EXPORT OutputView : public QTextBrowser, public ILogger
{
    Q_OBJECT
public:
    OutputView(QWidget *parent = NULL);

public slots:
    void log(const QString &text);
    void logImportant(const QString &text);
    void logError(const QString &text);
    void logCommand(const QString &command);
    void logCommand(const QStringList &command);
};

#endif // OUTPUTVIEW_H
