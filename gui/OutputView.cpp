/*
  OutputView.cpp

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
 * \file OutputView.cpp
 * \author Denis Martinez
 */

#include "OutputView.h"

OutputView::OutputView(QWidget *parent)
    : QTextBrowser(parent)
{
    setTextColor(Qt::gray);
    setTextBackgroundColor(Qt::black);
}

void OutputView::log(const QString &text)
{
    if (! text.isEmpty())
    {
        append(text);
        moveCursor(QTextCursor::End);
    }
}

void OutputView::logImportant(const QString &text)
{
    int oldWeight = fontWeight();
    setFontWeight(QFont::Bold);
    QColor oldColor = textColor();
    setTextColor(Qt::white);
    log(text);
    setFontWeight(oldWeight);
    setTextColor(oldColor);
}

void OutputView::logError(const QString &text)
{
    QColor oldColor = textColor();
    setTextColor(Qt::red);
    log(text);
    setTextColor(oldColor);
}

void OutputView::logCommand(const QString &command)
{
    static const QString format = tr(">>>> %0");
    setTextColor(Qt::gray);
    log(format.arg(command));
}

void OutputView::logCommand(const QStringList &command)
{
    setTextColor(Qt::gray);
    logCommand(command.join(" "));
}
