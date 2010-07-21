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

class DebuggerPlugin : public QObject, public IDEPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(IDEPluginInterface)

public:
    bool setup(IDEApplication *app);
    const QString &name() { return mName; };

public slots:
    bool startDebugging();
    void stopDebugging();

private slots:
    bool openSerial();
    void closeSerial();

    void dataArrived(QByteArray data);

private:
    IDEApplication *mApp;
    QString mName;
    QScopedPointer<DebuggerWidget> widget;
    QScopedPointer<Serial> serial;
    QString serialData;

    QByteArray readSerial(qint64 readCount);
    bool writeSerial(const QByteArray &data);

    void parseTrace(QStringRef trace);
    void parseState(QStringRef state);

};

#endif // DEBUGGERPLUGIN_H
