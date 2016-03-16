/*
  ProjectHistory.h

  This file is part of arduide, The Qt-based IDE for the open-source Arduino electronics prototyping platform.

  Copyright (C) 2010-2016 
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
 * \file ProjectHistory.h
 * \author Martin PERES
 */

#ifndef PROJECTHISTORY_H
#define PROJECTHISTORY_H

#include <QString>
#include <QStringList>
#include <QSettings>

/**
 * @brief Class to mage old projects
 * 
 */
class ProjectHistory : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Return a number of projects
     * 
     * @param maxSize Maximun number of projects 
     * @return QStringList
     */
    QStringList history(int maxSize=0);

private:
    ProjectHistory(QObject *parent = NULL);
    
    /**
     * @brief Project settings
     * 
     */
    QSettings settings;
    
    /**
     * @brief History size
     * 
     */
    const int historyLength;

    friend class IDEApplication;

signals:
    /**
     * @brief Update history with new project
     * 
     * @param newProject New project name
     * @return void
     */
    
    void historyUpdated(const QString &newProject);

public slots:
    
    /**
     * @brief Update project history
     * 
     * @param newProject New project name
     * @return void
     */
    void updateHistory(const QString &newProject);
};

#endif // PROJECTHISTORY_H
