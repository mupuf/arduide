/**
 * \file ProjectHistory.h
 * \author Martin PERES
 */

#ifndef PROJECTHISTORY_H
#define PROJECTHISTORY_H

#include <QString>
#include <QStringList>
#include <QSettings>

class ProjectHistory : public QObject
{
    Q_OBJECT

public:
    QStringList history(int maxSize=0);

private:
    ProjectHistory(QObject *parent = NULL);
    QSettings settings;
    const int historyLength;

    friend class IDEApplication;

signals:
    void historyUpdated(const QString &newProject);

public slots:
    void updateHistory(const QString &newProject);
    //void empty();
};

#endif // PROJECTHISTORY_H
