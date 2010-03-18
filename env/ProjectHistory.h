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
    ProjectHistory();

    static ProjectHistory& instance();

    QStringList history(int maxSize=0);

private:
    QSettings settings;
    const int historyLength;

signals:
    void historyUpdated(const QString &newProject);

public slots:
    void updateHistory(const QString &newProject);
    //void empty();
};

#endif // PROJECTHISTORY_H
