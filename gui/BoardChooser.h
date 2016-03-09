/*
  BoardChooser.h

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
 * \file BoardChooser.h
 * \author Denis Martinez
 */

#ifndef BOARDCHOOSER_H
#define BOARDCHOOSER_H

#include <QMenu>
#include <QDebug>
#include <QActionGroup>
#include <QWidgetAction>

class BoardChooser : public QMenu
{
    Q_OBJECT

public:
    BoardChooser(QWidget *parent = NULL);

signals:
    void boardChosen(const QString &board);

public slots:
    void refresh();

private:
    QActionGroup *actionGroup;

private slots:
    void onActionTriggered(QAction *action);
};

#endif // BOARDCHOOSER_H
