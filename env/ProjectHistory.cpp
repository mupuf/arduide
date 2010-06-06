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
