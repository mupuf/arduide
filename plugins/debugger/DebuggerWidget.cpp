/**
 * \file DebuggerWidget.cpp
 * \author Martin Peres
 */

#include "DebuggerWidget.h"
#include "../../utils/Serial.h"

#include <QLineEdit>

DebuggerWidget::DebuggerWidget(QWidget *parent)
    : QWidget(parent),
    _break(false),
    _started(false)
{
    setupUi(this);

    updateBaudList();

    connect(pushStartStop, SIGNAL(pressed()), this, SLOT(onStartStopPressed()));
    connect(checkBreak, SIGNAL(stateChanged(int)), this, SLOT(onBreakToggled(int)));

    connect(pushClearLogs, SIGNAL(pressed()), debugLogs, SLOT(clear()));

    // If the combo has a line edit
    connect(commandEdit, SIGNAL(returnPressed()), this, SLOT(onSendCommand()));
}

bool DebuggerWidget::isStarted()
{
    return _started;
}

bool DebuggerWidget::shouldBreakASAP()
{
    return _break;
}

int DebuggerWidget::baudRate()
{
    return baudRateBox->itemData(baudRateBox->currentIndex()).toInt();
}

// Public slots
void DebuggerWidget::startDebugging()
{
    _started=true;
    onDebugStatusChanged();
}

void DebuggerWidget::stopDebugging()
{
    _started=false;
    onDebugStatusChanged();
}

void DebuggerWidget::clearLogs()
{
    debugLogs->clear();
}

void DebuggerWidget::logImportant(const QString& result)
{
    debugLogs->logImportant(result);
}

void DebuggerWidget::logResult(const QString& result)
{
    debugLogs->log(result);
}

void DebuggerWidget::logError(const QString& error)
{
    debugLogs->logError(error);
}

void DebuggerWidget::setStatus(const QString &text)
{
    statusLabel->setText(text);
}

// Private slots
void DebuggerWidget::onDebugStatusChanged()
{
    // Update the GUI
    pushStartStop->setText(_started?tr("Stop"):tr("Start"));

    // Emit the signal
    if(_started)
        emit debuggerStarted();
    else
        emit debuggerStopped();
}

void DebuggerWidget::onStartStopPressed()
{
    // Flip the state
    _started=!_started;

    onDebugStatusChanged();
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
    QString cmd=commandEdit->text();

    // Add a line in the Log
    debugLogs->logCommand(cmd.split(' '));

    // Send the signal there is a new command
    emit sendCommand(cmd);

    // Clear the line
    commandEdit->clear();
}

void DebuggerWidget::updateBaudList()
{
    int index = 0;
    foreach(int rate, Serial::baudRates())
    {
        baudRateBox->addItem(QString::number(rate), rate);
        if (rate == 19200)
            baudRateBox->setCurrentIndex(index);
        index++;
    }
}
