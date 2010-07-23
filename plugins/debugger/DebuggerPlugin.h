/**
 * \file DebuggerPlugin.h
 * \author Martin Peres
 */

#ifndef DEBUGGERPLUGIN_H
#define DEBUGGERPLUGIN_H

#include "plugins/IDEPluginInterface.h"

#include "DebuggerWidget.h"
#include "../../utils/Serial.h"

#include <QScopedPointer>
#include <QTime>

class DebuggerPlugin : public QObject, public IDEPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(IDEPluginInterface)

public:
    bool setup(IDEApplication *app);
    const QString &name() { return mName; };

    int debugTime();

public slots:
    bool startDebugging();
    void stopDebugging();

private slots:
    bool openSerial();
    void closeSerial();

    void dataArrived(QByteArray data);
    void treeItemClicked(QTreeWidgetItem* item, int column);
    void newCommand(QString cmd);

private:
    IDEApplication *mApp;
    QString mName;
    QScopedPointer<DebuggerWidget> widget;
    QScopedPointer<Serial> serial;
    QString serialData;
    QTime startTime;

    void parseTrace(QString trace);
    void parseState(QString state);
    void parseRet(QString ret);
    void parseError(QString error);

};

#endif // DEBUGGERPLUGIN_H
