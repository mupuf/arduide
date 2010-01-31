/**
 * \file ProjectHistory.cpp
 * \author Martin PERES
 */

#include "ProjectHistory.h"
#include <QVariant>
#include <QFileInfo>

ProjectHistory::ProjectHistory() : historyLength(20)
{
    
}

ProjectHistory& ProjectHistory::instance()
{
    static ProjectHistory pHistory;
    
    return pHistory;
}
    
QStringList ProjectHistory::history(unsigned int maxSize)
{
    bool readOK=true;
    int i=0;
    QStringList list;
    
    if(maxSize==0 || maxSize>ProjectHistory::historyLength)
        maxSize=ProjectHistory::historyLength;
    
    while(i<maxSize && readOK)
    {
        QString key=QString("projectHistory/%1").arg(i);
        
        QVariant val=settings.value(key);
        if(val!=QVariant() && QFileInfo(val.toString()).isReadable())
            list.append(val.toString());
        else
            readOK=false;
        
        i++;
    }
    
    // avoid duplicates
    QStringList list_d;
    for(int i=0; i<list.size(); i++)
        if(!list_d.contains(list.at(i)) && QFileInfo(list.at(i)).isReadable())
            list_d.append(list.at(i));
    
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
        QString key=QString("projectHistory/%1").arg(i);
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