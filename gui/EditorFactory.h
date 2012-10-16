/*
  EditorFactory.h

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
 * \file EditorFactory.h
 * \author Denis Martinez
 */

#ifndef EDITORFACTORY_H
#define EDITORFACTORY_H

#include <QString>

#include "Editor.h"

class LexerArduino;

class EditorFactory
{
public:
    static Editor *createEditor(const QString &text = initialText, QWidget *parent = NULL);
    static LexerArduino *createLexer(QObject *parent = NULL);

private:
    static const QString initialText;
};

#endif // EDITORFACTORY_H
