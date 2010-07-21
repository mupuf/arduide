/**
 * \file DebuggerWidget.h
 * \author Martin Peres
 */

#ifndef DEBUGGERWIDGET_H
#define DEBUGGERWIDGET_H

#include "plugins/ui_DebuggerWidget.h"

class DebuggerWidget : public QWidget, Ui::DebuggerWidget
{
    Q_OBJECT

    friend class DebuggerPlugin;

    bool _break;
    bool _started;

public:
    DebuggerWidget(QWidget *parent = NULL);

    bool isStarted();
    bool shouldBreakASAP();

    int baudRate();

public slots:
    void startDebugging();
    void stopDebugging();

    void clearLogs();
    void logImportant(const QString& result);
    void logResult(const QString& result);
    void logError(const QString& error);

    void setStatus(const QString &text);

private slots:
    void onDebugStatusChanged();

    void onStartStopPressed();
    void onBreakToggled(int);
    void onSendCommand();

    void updateBaudList();

signals:
    void debuggerStarted();
    void debuggerStopped();
    void shouldBreakOnTrace(bool);
    void sendCommand(QString);
};

#endif // DEBUGGERWIDGET_H
