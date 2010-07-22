#include "DebuggerPlugin.h"
#include "DebuggerWidget.h"

#include "IDEApplication.h"
#include "gui/Editor.h"

#include <QDomDocument>

bool DebuggerPlugin::setup(IDEApplication *app)
{
    mApp = app;
    mName = tr("Debugger");
    widget.reset(new DebuggerWidget);

    app->mainWindow()->utilityTabWidget()->addTab(widget.data(), name());

    connect(widget.data(), SIGNAL(debuggerStarted()), this, SLOT(startDebugging()));
    connect(widget.data(), SIGNAL(debuggerStopped()), this, SLOT(stopDebugging()));

    connect(widget->treeFrames, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(treeItemClicked(QTreeWidgetItem*,int)));

    return true;
}

int DebuggerPlugin::debugTime()
{
    return startTime.msecsTo(QTime::currentTime());
}

bool DebuggerPlugin::startDebugging()
{
    // Clear the logs
    widget->clearLogs();
    widget->treeFrames->clear();

    // Add some info in the logs
    widget->logImportant(tr("Start debugging"));

    // Recompile and upload the current program
    widget->setStatus(tr("Compile & Upload"));
    if(!mApp->mainWindow()->upload())
    {
        widget->logError(tr("Compilation or Upload failed. Please take a look at the 'Output' tab."));
        widget->stopDebugging();
        return false;
    }

    // Open the serial connection
    if(!openSerial())
        return false;

    // Store the current time
    startTime = QTime::currentTime();

    return true;
}

void DebuggerPlugin::stopDebugging()
{
    // Add some info in the logs
    widget->logImportant(tr("%1ms: Stop debugging").arg(debugTime()));

    // Close the serial connection
    if (serial.data() != NULL)
        serial->close();

    widget->setStatus(tr("Serial port closed."));
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

void DebuggerPlugin::dataArrived(QByteArray data)
{
    QRegExp packet_re("<(trace|frames).*>");

    serialData+=data;

    while(true)
    {
        // Parse traces
        int start = packet_re.indexIn(serialData);
        if (start == -1)
            return;

        QString type = packet_re.capturedTexts().at(1);

        //Look for the ending tag of this one
        int end = serialData.indexOf(QString("</%1>").arg(type));
        if (end == -1)
            return;
        end+=3+type.size();

        QString packet = serialData.mid(start, end-start);
        if (type == "trace")
            parseTrace(packet);
        else if (type == "frames")
            parseState(packet);

        // Remove data we already parsed
        serialData = serialData.mid(end);
    }
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

// Private
QByteArray DebuggerPlugin::readSerial(qint64 readCount)
{
    if (serial.data() != NULL && serial->isOpen())
    {
        QByteArray pData;
        pData.resize(readCount);
        readCount = serial->readData(pData.data(), readCount);
        if (readCount < 0)
            widget->setStatus(tr("Read error: %0").arg(serial->errorString()));
        else
        {
            widget->setStatus(tr("Read %0 bytes of data.").arg(readCount));
            pData.resize(readCount);

            return pData;
        }
    }
    else
        widget->setStatus(tr("Unable to read, the port is not opened."));

    return QByteArray();
}

bool DebuggerPlugin::writeSerial(const QByteArray &data)
{
    if (serial.data() != NULL && serial->isOpen())
        return serial->write(data) == data.size();
    else
        widget->setStatus(tr("Unable to write, the port is not opened."));

    return false;
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
    int arrivalTime = debugTime();

    // Read the xml
    QDomDocument doc;
    QString err;
    int errLine, errCol;
    if(!doc.setContent(state, &err, &errLine, &errCol))
    {
        qDebug("Error while parsing the state '%s' at (%i,%i): %s.",
               qPrintable(state), errLine, errCol, qPrintable(err));
    }

    // Read top element
    bool hasLine;
    QDomElement docElem = doc.documentElement();
    int sourceLine = docElem.attribute("l", "-1").toInt(&hasLine);

    // Create the top element of the treewidget
    QTreeWidgetItem* topNode = new QTreeWidgetItem();
    topNode->setText(0, QString::number(arrivalTime));
    topNode->setText(1, tr("At line %1").arg(sourceLine));
    topNode->setData(0, Qt::UserRole, sourceLine);

    QDomNodeList frames = docElem.elementsByTagName("frame");
    for(int i=0; i<frames.size(); i++)
    {
        QDomElement frame_e = frames.item(i).toElement();

        // Create a new entry from the frame
        QString frame_name = frame_e.attribute("id", tr("<no_name>"));
        int line = frame_e.attribute("l", "-1").toInt(&hasLine);

        QTreeWidgetItem* wnode = new QTreeWidgetItem(topNode);
        //wnode->setText(0, QString::number(arrivalTime));
        wnode->setText(1, frame_name);
        if(hasLine)
            wnode->setData(0, Qt::UserRole, line);

        // Read all the variables of the frame
        QDomNodeList vars = frame_e.elementsByTagName("var");
        for(int e=0; e<vars.size(); e++)
        {
            QDomElement var_e = vars.item(e).toElement();

            int line = var_e.attribute("l", "-1").toInt(&hasLine);
            QString name = var_e.attribute("id");
            QString type = var_e.attribute("t");
            QString value = var_e.attribute("v");

            QTreeWidgetItem* wvar = new QTreeWidgetItem(wnode);
            //wvar->setText(0, QString::number(arrivalTime));
            //wvar->setText(1, frame_name);
            wvar->setText(2, type);
            wvar->setText(3, name);
            wvar->setText(4, value);
            if(hasLine)
                wvar->setData(0, Qt::UserRole, line);
        }
    }

    // Add the new top node
    widget->treeFrames->insertTopLevelItem(0, topNode);

    widget->logResult(tr("%1ms: New state received").arg(arrivalTime));
}

Q_EXPORT_PLUGIN2(debugger, DebuggerPlugin)
