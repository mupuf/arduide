/**
 * \file MainWindow.cpp
 * \author Denis Martinez
 */

#include "MainWindow.h"

#include <QTabWidget>
#include <QLabel>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>

#include "EditorFactory.h"
#include "Browser.h"
#include "../env/Device.h"
#include "../env/Board.h"
#include "../env/Builder.h"
#include "../env/Settings.h"

MainWindow::MainWindow() : pHistory(ProjectHistory::instance())
{
    setupUi(this);
    dockWidget->hide();
}

void MainWindow::initialize()
{
    createBrowserAndTabs();

    createDeviceChooser();
    fillDeviceBox();
    fillBoardsBox();

    setupActions();
}

void MainWindow::setupActions()
{
    buildActions = new QActionGroup(this);
    buildActions->addAction(action_Build);
    buildActions->addAction(action_Upload);

    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(refreshDevicesAct, SIGNAL(triggered()), this, SLOT(fillDeviceBox()));
    connect(action_New, SIGNAL(triggered()), this, SLOT(newProject()));
    connect(action_Open, SIGNAL(triggered()), this, SLOT(open()));
    connect(action_Save, SIGNAL(triggered()), this, SLOT(save()));
    connect(action_Close, SIGNAL(triggered()), this, SLOT(closeTab()));
    connect(action_Copy, SIGNAL(triggered()), this, SLOT(copy()));
    connect(action_Cut, SIGNAL(triggered()), this, SLOT(cut()));
    connect(action_Paste, SIGNAL(triggered()), this, SLOT(paste()));
    connect(action_Build, SIGNAL(triggered()), this, SLOT(build()));
    connect(action_Upload, SIGNAL(triggered()), this, SLOT(upload()));
    connect(actionToggle_dock, SIGNAL(triggered()), this, SLOT(toggleDock()));
    connect(deviceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setDeviceAtIndex(int)));
    connect(boardBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setBoardAtIndex(int)));
    connect(actionGo_to_the_next_tab, SIGNAL(triggered()), this, SLOT(nextTab()));
    connect(actionGo_to_the_previous_tab, SIGNAL(triggered()), this, SLOT(previousTab()));

    connect(browser, SIGNAL(newProjectRequested()), this, SLOT(newProject()));
    connect(browser, SIGNAL(newProjectRequested(const QString &, const QString &)), this, SLOT(newProject(const QString &, const QString &)));
    connect(browser, SIGNAL(openProjectRequested()), this, SLOT(open()));
    connect(browser, SIGNAL(openProjectRequested(const QString &)), this, SLOT(open(const QString &)));

    connect(&pHistory, SIGNAL(historyUpdated(QString)), browser, SLOT(refresh()));
}

void MainWindow::createBrowserAndTabs()
{
    tabWidget = new QTabWidget;
    tabWidget->setTabsClosable(true);
    tabWidget->setMovable(true);
    tabWidget->addAction(actionGo_to_the_next_tab);
    tabWidget->addAction(actionGo_to_the_previous_tab);

    browser = new Browser;
    tabWidget->addTab(browser, tr("Browser"));
    setCentralWidget(tabWidget);
    browser->quickstart();
}

void MainWindow::nextTab()
{
    int index = tabWidget->currentIndex();
    int count = tabWidget->count();
    if (index != -1)
    {
        index = (index + 1) % count;
        tabWidget->setCurrentIndex(index);
    }
}

void MainWindow::previousTab()
{
    int index = tabWidget->currentIndex();
    int count = tabWidget->count();
    if (index != -1)
    {
        index--;
        tabWidget->setCurrentIndex((index < 0) ? (count - 1) : index);
    }
}

void MainWindow::createDeviceChooser()
{
    deviceBox = new QComboBox;
    boardBox = new QComboBox;

    deviceToolBar->addWidget(new QLabel(tr("Device:")));
    deviceToolBar->addWidget(deviceBox);
    refreshDevicesAct = deviceToolBar->addAction(tr("Refresh"));
    refreshDevicesAct->setIcon(QIcon(":/images/16x16/view-refresh.png"));
    deviceToolBar->addSeparator();
    deviceToolBar->addWidget(boardBox);
}

void MainWindow::newProject(const QString &code, const QString &name, Editor **pEditor)
{
    Editor *editor;
    if (! code.isEmpty())
        editor = EditorFactory::createEditor(code);
    else
        editor = EditorFactory::createEditor();

    int tab = tabWidget->addTab(editor, (name.isNull()) ? createUniqueName("New project") : name);
    tabWidget->setCurrentIndex(tab);

    if (pEditor)
        *pEditor = editor;
}

QString MainWindow::createUniqueName(const QString &name)
{
    if (! names.contains(name))
    {
        names.append(name);
        return name;
    }
    else
    {
        QString newName;
        for (int i = 1;; ++i)
        {
            newName = QString("%0 (%1)").arg(name).arg(i);
            if (! names.contains(newName))
            {
                names.append(newName);
                return newName;
            }
        }
    }
}

void MainWindow::closeTab(int index)
{
    if (index == -1)
        index = tabWidget->currentIndex();

    QWidget *w = tabWidget->widget(index);
    if (w != browser)
    {
        Editor *editor = dynamic_cast<Editor *>(w);
        if (editor)
        {
            bool close = (! editor->isModified()) ||
                (QMessageBox::question(
                    this,
                    tr("Close project"),
                    tr("This project has unsaved changes.<br />Are you sure you want to close it?"),
                    QMessageBox::Yes | QMessageBox::No,
                    QMessageBox::No) == QMessageBox::Yes);
            if (close)
            {
                names.removeOne(tabWidget->tabText(index));
                tabWidget->removeTab(index);
            }
        }
    }
}

void MainWindow::fillDeviceBox()
{
    QString defaultPort = Settings::instance().devicePort();

    deviceBox->clear();

    deviceBox->addItem(tr("Select one:"));
    deviceBox->insertSeparator(1);

    DeviceList l = Device::listDevices();
    foreach(const Device &dev, l)
    {
        QString name = QString("%0: %1").arg(dev.port()).arg(dev.description());
        const QString &port = dev.port();
        deviceBox->addItem(name, port);

        if (port == defaultPort)
        {
            int index = deviceBox->count() - 1;
            deviceBox->setCurrentIndex(index);
        }
    }
}

void MainWindow::fillBoardsBox()
{
    QString defaultBoard = Settings::instance().board();

    boardBox->clear();

    boardBox->addItem(tr("Select one:"));
    boardBox->insertSeparator(1);

    foreach(const QString &boardId, Board::boardIds())
    {
        const Board *board = Board::boardInfo(boardId);
        boardBox->addItem(board->name(), boardId);

        if (defaultBoard == boardId)
        {
            int index = boardBox->count() - 1;
            boardBox->setCurrentIndex(index);
        }
    }
}

Editor *MainWindow::currentEditor()
{
    return qobject_cast<Editor *>(tabWidget->currentWidget());
}

void MainWindow::open(const QString &_fileName)
{
    QString fileName(_fileName);

    if (fileName.isEmpty())
    {
        fileName = QFileDialog::getOpenFileName(this, tr("Open project"), Settings::instance().sketchPath(), tr("Arduino sketches (*.pde)"));
        if (fileName.isEmpty())
            return;
    }

    QFile file(fileName);
    if (! file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, tr("Open error"), tr("The file could not be opened for reading."));
        return;
    }

    // create a new project and obtain the associated editor
    Editor *editor;
    newProject(QString::fromLocal8Bit(file.readAll()), createUniqueName(QFileInfo(fileName).fileName()), &editor);
    editor->setFileName(fileName);
    file.close();

    // update the history
    pHistory.updateHistory(fileName);
}

void MainWindow::save()
{
    Editor *e = currentEditor();
    if (e)
    {
        QString fileName = e->fileName();
        int index;
        e->save();
        if (fileName != e->fileName())
        {
            // the file name changed, update the tab text
            index = tabWidget->currentIndex();
            names.removeOne(tabWidget->tabText(index));
            tabWidget->setTabText(index, createUniqueName(QFileInfo(e->fileName()).fileName()));
        }

        // update the history
        pHistory.updateHistory(e->fileName());
    }
}

void MainWindow::copy()
{
    Editor *e = currentEditor();
    if (e) e->copy();
}

void MainWindow::cut()
{
    Editor *e = currentEditor();
    if (e) e->cut();
}

void MainWindow::paste()
{
    Editor *e = currentEditor();
    if (e) e->paste();
}

void MainWindow::setDeviceAtIndex(int index)
{
    QVariant userData;
    if (index >= 0)
        userData = deviceBox->itemData(index);

    Settings::instance().setDevicePort(userData.toString());
}

void MainWindow::setBoardAtIndex(int index)
{
    QVariant userData;
    if (index >= 0)
        userData = boardBox->itemData(index);

    Settings::instance().setBoard(userData.toString());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    for (int i = 0; i < tabWidget->count(); i++)
    {
        Editor *editor = dynamic_cast<Editor *>(tabWidget->widget(i));
        if (editor && editor->isModified())
        {
            bool close = QMessageBox::question(
                this,
                tr("Quit"),
                tr("Some projects in your workspace have unsaved changes.<br />Are you sure you want to quit?"),
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No) == QMessageBox::Yes;
            event->setAccepted(close);
            return;
        }
    }

    event->accept();
}

void MainWindow::build()
{
    Editor *editor = currentEditor();
    if (editor)
    {
        buildActions->setEnabled(false);

        const Board *board = Board::boardInfo(Settings::instance().board());
        dockWidget->show();
        outputView->clear();

        Builder builder(*outputView);
        builder.setBoard(board);
        builder.build(editor->text());

        buildActions->setEnabled(true);
    }
}

void MainWindow::upload()
{
    Editor *editor = currentEditor();
    if (editor)
    {
        buildActions->setEnabled(false);

        const Board *board = Board::boardInfo(Settings::instance().board());
        QString device = Settings::instance().devicePort();
        dockWidget->show();
        outputView->clear();

        Builder builder(*outputView);
        builder.setBoard(board);
        builder.setDevice(device);
        builder.build(editor->text(), true);

        buildActions->setEnabled(true);
    }
}

void MainWindow::toggleDock()
{
    dockWidget->setVisible(! dockWidget->isVisible());
}
