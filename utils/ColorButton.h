/*
  ColorButton.h

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
 * \file ColorButton.h
 * \author Denis Martinez
 */

#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QPushButton>

class ColorButton : public QPushButton
{
    Q_OBJECT

public:
    ColorButton(QWidget *parent = NULL);
    const QColor &color() const { return mColor; }
    void setColor(const QColor &color);

signals:
    void colorChosen(const QColor &color);

private:
    QColor mColor;

private slots:
    void chooseColor();
};

#endif // COLORBUTTON_H
