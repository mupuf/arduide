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

    bool _break;
    bool _started;

public:
    DebuggerWidget(QWidget *parent = NULL);
    void setStatus(const QString &text);

private slots:
	void onStartStopPressed();
	void onBreakToggled(int);
	void onSendCommand();

signals:
	void debuggerStarted();
	void debuggerStopped();
	void shouldBreakOnTrace(bool);
	void sendCommand(QString);
};

#endif // DEBUGGERWIDGET_H
