/*
  BoardChooser.cpp

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
 * \file BoardChooser.cpp
 * \author Denis Martinez
 */
#include "BoardChooser.h"

#include "env/Settings.h"
#include "env/Board.h"
#include "IDEApplication.h"

BoardChooser::BoardChooser(QWidget *parent)
    : QMenu(parent),
      actionGroup(NULL)
{
    setTitle(tr("Board"));
    refresh();
}

void BoardChooser::refresh()
{
    QString defaultBoard = ideApp->settings()->board();
    delete actionGroup;
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);
    connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(onActionTriggered(QAction *)));

    clear();

    QAction *action;
    foreach(const QString &boardId, Board::boardIds())
    {  
        const Board *board = Board::boardInfo(boardId);
        QStringList cpus = board->attribute("menu.cpu").split(",");
        QStringList freqs = board->attribute("build.f_cpu").split(",");
        if(cpus.size() > 1)
        {
            QMenu *menu1 = new QMenu;
            foreach(const QString &cpu, cpus)
            {   
                menu1->setTitle(board->name());
                if(freqs.size() == 1)
                {
                    action = new QAction(cpu, actionGroup);
                    action->setData(boardId+","+cpu);
                    action->setCheckable(true);
                    if(boardId+","+cpu == defaultBoard)
                        action->setChecked(true);
                    menu1->addAction(action);
                }
                else
                {
                    qDebug() << "FREQS > 1";
                    QMenu *menu2 = new QMenu;
                    foreach(const QString &freq, freqs)
                    {    
                        QAction *action2 = new QAction(QString::number(freq.left(freq.lastIndexOf("0")+1).toInt()/1e6)+"MHz", actionGroup);
                        action2->setData(boardId+","+cpu+","+freq);
                        action2->setCheckable(true);
                        if (boardId+","+cpu+","+freq == defaultBoard)
                            action2->setChecked(true);
                        menu2->addAction(action2);
                        menu2->setTitle(cpu);
                        menu1->addMenu(menu2);
                        qDebug() << freqs << freq;
                    }
                }
                
                this->addMenu(menu1);
                qDebug() << board->name() << cpu << cpus << freqs;
            }
        }
        else
        {
            action = new QAction(board->name(), actionGroup);
            action->setData(boardId);
            action->setCheckable(true);
            if (boardId == defaultBoard)
                action->setChecked(true);
            addAction(action);
            qDebug() << board->name() << board->attribute("menu.cpu");
        }

    }
}

void BoardChooser::onActionTriggered(QAction *action)
{
    qDebug() << action->data().toString();
    emit boardChosen(action->data().toString());
}
