/**
 * \file DebuggerWidget.cpp
 * \author Martin Peres
 */

#include "DebuggerWidget.h"

#include <QLineEdit>

DebuggerWidget::DebuggerWidget(QWidget *parent)
    : QWidget(parent),
    _break(false),
    _started(false)
{
    setupUi(this);

    connect(pushStartStop, SIGNAL(pressed()), this, SLOT(onStartStopPressed()));
    connect(checkBreak, SIGNAL(stateChanged(int)), this, SLOT(onBreakToggled(int)));

    // If the combo has a line edit
    if(commandBox->lineEdit())
	   connect(commandBox->lineEdit(), SIGNAL(returnPressed()), this, SLOT(onSendCommand()));
}

void DebuggerWidget::setStatus(const QString &text)
{
    statusLabel->setText(text);
}

// Private slots
void DebuggerWidget::onStartStopPressed()
{
    // Flip the state
    _started=!_started;

    // Update the GUI
    pushStartStop->setText(_started?tr("Stop"):tr("Start"));

    // Add some info in the logs
    QString msg = _started?tr("Start debugging"):tr("Stop debugging");
    debugLogs->logImportant(msg);

    // Emit the signal
    if(_started)
	   emit debuggerStarted();
    else
	   emit debuggerStopped();
}

void DebuggerWidget::onBreakToggled(int state)
{
	// Emit the signal
    emit shouldBreakOnTrace(state==Qt::Checked);

    // Add some info in the logs
    QString msg = state==Qt::Checked?tr("Break ASAP"):tr("Just log");
    debugLogs->log(msg);
}

void DebuggerWidget::onSendCommand()
{
    QString cmd=commandBox->currentText();

    // Add a line in the Log
    debugLogs->logCommand(cmd.split(' '));

    // Send the signal there is a new command
    emit sendCommand(cmd);

    // Clear the line
    if(commandBox->lineEdit())
	   commandBox->lineEdit()->clear();
}
