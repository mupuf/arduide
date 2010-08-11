#include "DebuggerPlugin.h"
#include "DebuggerWidget.h"

#include "IDEApplication.h"
#include "gui/Editor.h"

#include <QXmlStreamReader>

bool DebuggerPlugin::setup(IDEApplication *app)
{
    mApp = app;
    mName = tr("Debugger");
    widget.reset(new DebuggerWidget);
    debuggedEditor = NULL;

    app->mainWindow()->utilityTabWidget()->addTab(widget.data(), name());

    connect(widget.data(), SIGNAL(debuggerStarted()), this, SLOT(startDebugging()));
    connect(widget.data(), SIGNAL(debuggerStopped()), this, SLOT(stopDebugging()));
    connect(widget.data(), SIGNAL(sendCommand(QString)), this, SLOT(sendCommand(QString)));
    connect(widget.data(), SIGNAL(shouldBreakOnTrace(bool)), this, SLOT(shouldBreakOnTrace(bool)));
    connect(widget->treeFrames, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(treeItemClicked(QTreeWidgetItem*,int)));
    connect(app->mainWindow(), SIGNAL(tabChanged(bool)), this, SLOT(mainWindowTabChanged(bool)));
    connect(app->mainWindow(), SIGNAL(editorDeleted(Editor*)), this, SLOT(mainWindowEditorDeleted(Editor*)));

    return true;
}

bool DebuggerPlugin::isDebugging()
{
    return serial->isOpen();
}

int DebuggerPlugin::debugTime()
{
    return startTime.msecsTo(QTime::currentTime());
}

void DebuggerPlugin::startDebugging()
{
    // Clear the logs
    widget->clearLogs();
    widget->treeFrames->clear();

    // Store the debugged editor
    debuggedEditor = mApp->mainWindow()->currentEditor();

    // Add some info in the logs
    widget->logImportant(tr("Start debugging"));

    // Prepare to receive the uploadFinished signal from the main window
    connect(mApp->mainWindow(), SIGNAL(uploadFinished(bool)), this, SLOT(uploadCompleted(bool)));

    // Recompile and upload the current program
    widget->setStatus(tr("Compile & Upload"));
    mApp->mainWindow()->upload();
}

void DebuggerPlugin::stopDebugging()
{
    // Add some info in the logs
    widget->logImportant(tr("%1ms: Stop debugging").arg(debugTime()));

    // Close the serial connection
    if (serial.data() != NULL)
        serial->close();

    widget->setStatus(tr("Serial port closed."));

    // Reset the debugged editor
    debuggedEditor = NULL;

    // Tell the widget we stopped debugging
    widget->debugStarted(false);
}

void DebuggerPlugin::uploadCompleted(bool res)
{
    // We don't want to get any more messages from the mainwindow
    disconnect(mApp->mainWindow(), SIGNAL(uploadFinished(bool)), this, SLOT(uploadCompleted(bool)));

    if(!res)
    {
        widget->logError(tr("Compilation or Upload failed. Please take a look at the 'Output' tab."));
        widget->stopDebugging();
        return;
    }

    // Open the serial connection
    if(!openSerial())
        return;

    // Store the current time
    startTime = QTime::currentTime();

    // Tell the widget we started debugging
    widget->debugStarted(true);
}

// Private slots
bool DebuggerPlugin::openSerial()
{
    // Close the connection
    if (serial.data() != NULL)
        serial->close();

    // Get the port
    QString port = mApp->settings()->devicePort();
    if (port.isEmpty())
    {
        widget->setStatus(tr("Unable to open, you must select a device first."));
        return false;
    }

    // Get the baudRate
    int baudRate = widget->baudRate();

    // Open the connexion
    serial.reset(new Serial(port, baudRate));
    if (! serial->open(QIODevice::ReadWrite))
    {
        widget->setStatus(tr("Open failed: %0").arg(serial->errorString()));
        return false;
    }

    // Set the status
    widget->setStatus(tr("Debugging serial port opened successfully at %1 bauds.").arg(widget->baudRate()));

    // Set in readEvent mode
    serial->setInReadEventMode(true);
    connect(serial.data(), SIGNAL(dataArrived(QByteArray)), this, SLOT(dataArrived(QByteArray)));

    return true;
}

void DebuggerPlugin::closeSerial()
{
    if (serial.data() != NULL)
        serial->close();

    widget->setStatus(tr("Serial port closed."));
}

void DebuggerPlugin::treeItemClicked(QTreeWidgetItem* item, int column)
{
    if(!item)
        return;

    bool ok;
    int line = item->data(0, Qt::UserRole).toInt(&ok);

    if(!ok)
        return;

    Editor* e = ideApp->mainWindow()->currentEditor();
    if(e)
    {
        line--;
        e->ensureLineVisible(line);
        e->setCursorPosition(line, 0);
        e->setSelection(line, 0, line, e->lineLength(line));
    }
}

#include "data/libraries/IDEdbg/IDEdbgConstants.h"
#include <QStringList>
#include <QMessageBox>
void DebuggerPlugin::sendCommand(QString cmd)
{
    QRegExp func_re("\\s*(\\w+)\\((.*)\\).*");

    if(!func_re.exactMatch(cmd))
    {
        widget->logError(tr("Invalid command format."));
        return;
    }

    // Get the infos
    QString func = func_re.capturedTexts()[1];
    QStringList args = func_re.capturedTexts()[2].split(",");
    for(int i=0; i<args.size(); i++)
        args[i] = args[i].simplified();

    // For each command
    QByteArray data;
    if (func == "openShell")
    {
        data[0] = SHELL_REQUESTED;
        widget->checkBreak->setChecked(true);
        widget->commandEdit->setEnabled(true);
    }
    else if (func == "exit")
    {
        data.append(EXIT_SHELL);
        widget->checkBreak->setChecked(false);
        widget->commandEdit->setEnabled(false);
    }
    else if (func == "digitalRead")
    {
        bool ok;
        int pin = args[0].toInt(&ok);

        if (ok && args.size()==1)
        {
            data.append(DIGITAL_READ);
            data.append(pin);
        }
        else if (args.size()!=1)
        {
            widget->logError(tr("Invalid number of arguments"));
            return;
        }
        else if (!ok)
        {
            widget->logError(tr("Invalid argument %1: '%2' is not a number.").arg(1).arg(args[0]));
            return;
        }
    }
    else if (func == "digitalWrite")
    {
        bool ok1;
        int pin = args[0].toInt(&ok1);
        bool ok2 = (args[1]=="HIGH" || args[1]=="LOW");
        int value = (args[1]=="HIGH"?1:0);

        if (ok1 && ok2 && args.size()==2)
        {
            data.append(DIGITAL_WRITE);
            data.append(pin);
            data.append(value);
        }
        else if (args.size()!=2)
        {
            widget->logError(tr("Invalid number of arguments"));
            return;
        }
        else if (!ok1)
        {
            widget->logError(tr("Invalid argument %1: '%2' is not a number.").arg(1).arg(args[0]));
            return;
        }
        else if (!ok2)
        {
            widget->logError(tr("Invalid argument %1: '%2' should either be %3 or %4.").arg(2).arg(args[1], "HIGH", "LOW"));
            return;
        }
    }
    else if (func == "analogRead")
    {
        bool ok;
        int pin = args[0].toInt(&ok);

        if (ok && args.size()==1)
        {
            data.append(ANALOG_READ);
            data.append(pin);
        }
        else if (args.size()!=1)
        {
            widget->logError(tr("Invalid number of arguments"));
            return;
        }
        else if (!ok)
        {
            widget->logError(tr("Invalid argument %1: '%2' is not a number.").arg(1).arg(args[0]));
            return;
        }
    }
    else if (func == "analogWrite")
    {
        bool ok1, ok2;
        int pin = args[0].toInt(&ok1);
        int value = args[1].toInt(&ok2);

        if (ok1 && ok2 && args.size()==2)
        {
            data.append(ANALOG_WRITE);
            data.append(pin);
            data.append(value);
        }
        else if (args.size()!=2)
        {
            widget->logError(tr("Invalid number of arguments"));
            return;
        }
        else if (!ok1)
        {
            widget->logError(tr("Invalid argument %1: '%2' is not a number.").arg(1).arg(args[0]));
            return;
        }
        else if (!ok2)
        {
            widget->logError(tr("Invalid argument %1: '%2' is not a number.").arg(2).arg(args[1]));
            return;
        }
    }
    else if (func == "pinMode")
    {
        bool ok1;
        int pin = args[0].toInt(&ok1);
        bool ok2 = (args[1]=="INPUT" || args[1]=="OUTPUT");
        int value = (args[1]=="OUTPUT"?1:0);

        if (ok1 && ok2 && args.size()==2)
        {
            data.append(PIN_MODE);
            data.append(pin);
            data.append(value);
        }
        else if (args.size()!=2)
        {
            widget->logError(tr("Invalid number of arguments"));
            return;
        }
        else if (!ok1)
        {
            widget->logError(tr("Invalid argument %1: '%2' is not a number.").arg(1).arg(args[0]));
            return;
        }
        else if (!ok2)
        {
            widget->logError(tr("Invalid argument %1: '%2' should either be %3 or %4.").arg(2).arg(args[1], "INPUT", "OUTPUT"));
            return;
        }
    }
    else if (func == "varWrite")
    {
        if (args.size()==3)
        {
            data.append(VAR_WRITE);
            QMessageBox::information(NULL, tr("args[0] (size=%1)").arg(args[0].size()), args[0]);
            data.append(QString::number((char)args[0].size()));
            data.append(args[0]);
            QMessageBox::information(NULL, "args[1]", args[1]);
            data.append(QString::number((char)args[1].size()));
            data.append(args[1]);
            QMessageBox::information(NULL, "args[2]", args[2]);
            data.append(QString::number((char)args[2].size()));
            data.append(args[2]);
        }
    }
    else if (func == "help")
    {
        widget->logResult("Here is a summary of the existing commands:");
        widget->logResult("openShell(): Open a shell on the arduino");
        widget->logResult("exit(): Exits the arduino shell");
        widget->logResult("digitalRead(pin): Read the value of the pin 'pin'. Result will be either HIGH or LOW.");
        widget->logResult("digitalWrite(pin, value): Write 'value' to the pin 'pin'. 'Value' should be either HIGH or LOW.");
        widget->logResult("analogRead(pin): Read the value of the pin 'pin'. Result will be between 0 and 1023");
        widget->logResult("analogWrite(pin, value): Write 'value' to the pin 'pin'. 'Value' should be between 0 and 254");
        widget->logResult("pinMode(pin, mode): Set the pin 'pin' to mode 'mode'. 'Mode' should be either OUTPUT or INPUT.\n");
    }
    else
    {
        widget->logError(tr("Unknown command"));
        return;
    }

    // Check the serial is available
    if (!serial || !serial->isOpen())
    {
        widget->logError(tr("The serial connection is not openned. Please start the debug"));
        return;
    }

    // Send data
    serial->write(data);

}

void DebuggerPlugin::shouldBreakOnTrace(bool value)
{
    if (value)
        sendCommand("openShell()");
    else
        sendCommand("exit()");
}

void DebuggerPlugin::mainWindowTabChanged(bool isBrowser)
{
    widget->pushStartStop->setEnabled(!isBrowser);

    bool widgetEnabled = debuggedEditor==NULL || debuggedEditor==mApp->mainWindow()->currentEditor();
    widget->setEnabled(widgetEnabled);
}

void DebuggerPlugin::mainWindowEditorDeleted(Editor *editor)
{
    if (editor == debuggedEditor)
    {
        widget->onStartStopPressed();
        widget->clearLogs();
        debuggedEditor = NULL;

        mainWindowTabChanged(false);
    }
}

// Inbound data
void DebuggerPlugin::dataArrived(QByteArray data)
{
    QRegExp packet_re("<(trace|frames|error).*>");
    QRegExp packetRet_re("<(ret) .*/>");

    serialData+=data;

    while(true)
    {
        QString type;
        int start, end;

        // Get the right format
        start = packet_re.indexIn(serialData);
        if (start != -1)
        {
            type = packet_re.capturedTexts().at(1);

            //Look for the ending tag of this one
            end = serialData.indexOf(QString("</%1>").arg(type));
            if (end == -1)
                return;
            end+=3+type.size();
        }
        else
        {
            start = packetRet_re.indexIn(serialData);
            if (start == -1)
                return;

            type = "ret";

            end = start+packetRet_re.matchedLength();
        }

        QString packet = serialData.mid(start, end-start);
        if (type == "trace")
            parseTrace(packet);
        else if (type == "frames")
            parseState(packet);
        else if (type == "error")
            parseError(packet);
        else if (type == "ret")
            parseRet(packet);
        else
            widget->logError(tr("Unknown return type '%1'").arg(type));

        // Remove data we already parsed
        serialData = serialData.mid(end);
    }
}
void DebuggerPlugin::parseTrace(QString trace)
{
    trace = trace.replace("<trace>", "");
    trace = trace.replace("</trace>", "");
    trace = trace.replace("&lt;", "<");
    trace = trace.replace("&gt;", ">");

    widget->logResult(tr("%1ms: %2").arg(debugTime()).arg(trace));
}

void DebuggerPlugin::parseState(QString state)
{
    QTreeWidgetItem* topNode, *currentFrame;
    bool hasLine;

    int arrivalTime = debugTime();

    QXmlStreamReader xml(state);
    while (!xml.atEnd())
    {
        if (!xml.readNextStartElement())
            continue;

        if (xml.name()=="frames")
        {
            int sourceLine = xml.attributes().value("l").toString().toInt(&hasLine);
            topNode = new QTreeWidgetItem();
            topNode->setText(0, QString::number(arrivalTime));
            topNode->setText(1, tr("At line %1").arg(sourceLine));
            topNode->setData(0, Qt::UserRole, sourceLine);
        }
        else if (xml.name()=="frame")
        {
            QString frame_name = xml.attributes().value("id").toString();
            int sourceLine = xml.attributes().value("l").toString().toInt(&hasLine);
            currentFrame = new QTreeWidgetItem(topNode);
            currentFrame->setText(1, frame_name);
            if(hasLine)
                currentFrame->setData(0, Qt::UserRole, sourceLine);
        }
        else if (xml.name()=="var")
        {
            QString name = xml.attributes().value("id").toString();
            QString type = xml.attributes().value("t").toString();
            QString value = xml.attributes().value("v").toString();
            int sourceLine = xml.attributes().value("l").toString().toInt(&hasLine);

            QTreeWidgetItem* var = new QTreeWidgetItem(currentFrame);
            var->setText(2, type);
            var->setText(3, name);
            var->setText(4, value);
            if(hasLine)
                currentFrame->setData(0, Qt::UserRole, sourceLine);
        }
    }

    // Add the new top node
    widget->treeFrames->insertTopLevelItem(0, topNode);

    widget->logResult(tr("%1ms: New state received").arg(arrivalTime));
}

void DebuggerPlugin::parseRet(QString ret)
{
    QString code;
    QXmlStreamReader xml(ret);
    while (!xml.atEnd())
    {
        if (!xml.readNextStartElement())
            continue;

        if (xml.name()=="ret")
            code = xml.attributes().value("v").toString();
    }

    bool ok;
    code.toInt(&ok);

    if (code!="OK" && !ok)
        widget->logError("<<< " + code);
    else
        widget->logResult("<<< " + code);
}

void DebuggerPlugin::parseError(QString error)
{
    error = error.replace("<error>", "");
    error = error.replace("</error>", "");
    error = error.replace("&lt;", "<");
    error = error.replace("&gt;", ">");

    widget->logError(tr(">>> %2").arg(error));
}

Q_EXPORT_PLUGIN2(debugger, DebuggerPlugin)
