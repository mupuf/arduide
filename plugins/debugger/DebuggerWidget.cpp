/*
  DebuggerWidget.cpp

  This file is part of arduide, The Qt-based IDE for the open-source Arduino electronics prototyping platform.

  Copyright (C) 2010-2012 
  Authors : Martin Peres
	    Denis Martinez

This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * \file DebuggerWidget.cpp
 * \author Martin Peres
 */

#include "DebuggerWidget.h"
#include "../../utils/Serial.h"

#include <QLineEdit>
#include <QCompleter>

void DebuggerWidget::addCmdLineCompleter()
{
    QStringList wordList;
    wordList << "openShell()" << "exit()" << "digitalRead(pin)" << "digitalWrite(pin, value)";
    wordList << "analogRead(pin)" << "analogWrite(pin, value)" << "pinMode(pin, mode)";
    wordList << "varWrite(frameName, varName, varValue)" << "help()";

    QCompleter *completer = new QCompleter(wordList, this);
    completer->setCaseSensitivity(Qt::CaseSensitive);
    commandEdit->setCompleter(completer);
}

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
    connect(commandEdit, SIGNAL(returnPressed()), this, SLOT(onSendCommand()));

    // Set the completer list
    addCmdLineCompleter();
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
    treeFrames->clear();
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
    // Add some info in the logs
    QString msg = state==Qt::Checked?tr("openShell() // Break ASAP"):tr("exit(): // Just log");
    debugLogs->logCommand(msg);

    // Emit the signal
    emit shouldBreakOnTrace(state==Qt::Checked);
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

void DebuggerWidget::debugStarted(bool value)
{
    //commandEdit->setEnabled(value);
    checkBreak->setEnabled(value);
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
