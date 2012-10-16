/*
  ProjectHistory.cpp

  This file is part of arduide, The Qt-based IDE for the open-source Arduino electronics prototyping platform.

  Copyright (C) 2010-2012 
  Authors : Martin Peres
	    Denis Martinez

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
 * \file ProjectHistory.cpp
 * \author Martin PERES
 */

#include "ProjectHistory.h"
#include <QVariant>
#include <QFileInfo>
#include <QDebug>

ProjectHistory::ProjectHistory(QObject *parent)
    : QObject(parent),
      historyLength(20)
{
}

QStringList ProjectHistory::history(int maxSize)
{
    bool readOK=true;
    int i=0;
    QStringList list;

    if(maxSize==0 || maxSize>ProjectHistory::historyLength)
        maxSize=ProjectHistory::historyLength;

    while(i<maxSize && readOK)
    {
        QString key=QString("ProjectHistory/%1").arg(i);

        QVariant val=settings.value(key);
        if(val!=QVariant() && QFileInfo(val.toString()).isReadable())
            list.append(val.toString());
        else
            readOK=false;

        i++;
    }

    // build a new list without duplicates
    QStringList list_d;
    bool duplicate;
    foreach (const QString &fileName, list)
    {
        duplicate = false;
        foreach (const QString &other, list_d)
        {
            // check it's not in the list already
            if (QFileInfo(fileName) == QFileInfo(other))
            {
                // check for identical file, with possibly different paths.
                // we don't chether whether the file is available or not at the time,
                // if not it might become available again later.
                duplicate = true;
                break;
            }
        }
        if (! duplicate)
            list_d << fileName;
    }

    return list_d;
}

void ProjectHistory::updateHistory(const QString &newProject)
{
    // get the current history
    QStringList history=this->history();

    // Only keep trace of actual changes
    if(history.size()>0 && newProject==history.at(0))
         return;

    // remove every occurence of the new project in the list
    history.removeAll(newProject);

    // add the new project
    history.prepend(newProject);

    // remove any entry being too old
    while(history.size()>ProjectHistory::historyLength)
        history.removeLast();

    // save the new list
    for(int i=0; i<history.size(); i++)
    {
        QString key=QString("ProjectHistory/%1").arg(i);
        settings.setValue(key, history.at(i));
    }

    // sync to disc
    settings.sync();

    // broadcast the change
    emit historyUpdated(newProject);
}

/*void ProjectHistory::empty()
{
    //TODO
}*/
