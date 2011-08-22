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
#include <QWebSecurityOrigin>
#include <QDebug>
#include <QDesktopServices>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "EditorFactory.h"
#include "LexerArduino.h"
#include "Browser.h"
#include "ConfigDialog.h"
#include "DeviceChooser.h"
#include "BoardChooser.h"
#include "../env/Device.h"
#include "../env/Board.h"
#include "../env/Builder.h"
#include "../env/Settings.h"
#include "../env/ProjectHistory.h"
#include "IDEApplication.h"

#include "ui_AboutDialog.h"

MainWindow::MainWindow()
    : QMainWindow(), configDialog(NULL)
{
    ui.setupUi(this);
    ui.actionAbout->setText(ui.actionAbout->text().arg(PROJECT_NAME));

    ui.dockWidget->hide();
    ui.dockSearchReplace->hide();
}

void MainWindow::initialize()
{
    createBrowserAndTabs();

    createDeviceChooser();
    createBoardChooser();

    setupActions();

    tabHasChanged();

    restoreState(ideApp->settings()->mainWindowState());
    showSearchBox(false);
}

void MainWindow::setupActions()
{
    buildActions = new QActionGroup(this);
    buildActions->addAction(ui.action_Build);
    buildActions->addAction(ui.action_Upload);

    connect(ui.tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabHasChanged()));
    connect(ui.action_New, SIGNAL(triggered()), this, SLOT(newProject()));
    connect(ui.action_Open, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui.action_Save, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui.action_Close, SIGNAL(triggered()), this, SLOT(closeTab()));
    connect(ui.actionUpPastebin, SIGNAL(triggered()), this, SLOT(uploadToPastebin()));
    connect(ui.actionUndo, SIGNAL(triggered()), this, SLOT(undo()));
    connect(ui.actionRedo, SIGNAL(triggered()), this, SLOT(redo()));
    connect(ui.action_Copy, SIGNAL(triggered()), this, SLOT(copy()));
    connect(ui.action_Cut, SIGNAL(triggered()), this, SLOT(cut()));
    connect(ui.action_Search, SIGNAL(triggered(bool)), this, SLOT(showSearchBox(bool)));
    connect(ui.action_Paste, SIGNAL(triggered()), this, SLOT(paste()));
    connect(ui.action_Build, SIGNAL(triggered()), this, SLOT(build()));
    connect(ui.action_Upload, SIGNAL(triggered()), this, SLOT(upload()));
    connect(ui.action_Utilities, SIGNAL(triggered()), this, SLOT(toggleDock()));
    connect(ui.actionGo_to_the_next_tab, SIGNAL(triggered()), this, SLOT(nextTab()));
    connect(ui.actionGo_to_the_previous_tab, SIGNAL(triggered()), this, SLOT(previousTab()));
    connect(ui.action_Configure_the_IDE, SIGNAL(triggered()), this, SLOT(configure()));
    connect(ui.action_Contextual_help, SIGNAL(triggered()), this, SLOT(contextualHelp()));
    connect(ui.actionCommunityArduinoCC, SIGNAL(triggered()), this, SLOT(openCommunityArduinoCC()));
    connect(ui.actionCommunityArduinoForums, SIGNAL(triggered()), this, SLOT(openCommunityArduinoForums()));
    connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui.actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    connect(ui.lineSearch, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(ui.lineReplace, SIGNAL(returnPressed()), this, SLOT(search()));
    connect(ui.pushSearch, SIGNAL(clicked()), this, SLOT(search()));
    connect(ui.pushReplace, SIGNAL(clicked()), this, SLOT(replace()));
    connect(ui.pushReplaceAll, SIGNAL(clicked()), this, SLOT(replaceAll()));
    connect(this, SIGNAL(tabChanged(bool)), ui.pushReplace, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(tabChanged(bool)), ui.pushReplaceAll, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(tabChanged(bool)), ui.checkRegExp, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(tabChanged(bool)), ui.checkWordOnly, SLOT(setEnabled(bool)));

    connect(browser, SIGNAL(newProjectRequested()), this, SLOT(newProject()));
    connect(browser, SIGNAL(newProjectRequested(const QString &, const QString &)), this, SLOT(newProject(const QString &, const QString &)));
    connect(browser, SIGNAL(openProjectRequested()), this, SLOT(open()));
    connect(browser, SIGNAL(openProjectRequested(const QString &)), this, SLOT(open(const QString &)));
    connect(browser, SIGNAL(newPageLoaded(QUrl)), this, SLOT(tabContentHasChanged()));
    connect(ui.action_Prev, SIGNAL(triggered()), browser, SLOT(back()));
    connect(ui.action_Next, SIGNAL(triggered()), browser, SLOT(forward()));

    connect(ideApp->projectHistory(), SIGNAL(historyUpdated(QString)), browser, SLOT(refresh()));

    connect(ideApp->settings(), SIGNAL(fontChanged(const QFont &)), this, SLOT(setFont(const QFont &)));

    connect(&pastebin, SIGNAL(finished(QNetworkReply*)), this, SLOT(pastebinUploadDone(QNetworkReply*)));
}

void MainWindow::createBrowserAndTabs()
{
    ui.tabWidget->setTabsClosable(true);
    ui.tabWidget->setMovable(true);
    ui.tabWidget->addAction(ui.actionGo_to_the_next_tab);
    ui.tabWidget->addAction(ui.actionGo_to_the_previous_tab);

    QWebSecurityOrigin::addLocalScheme("ide");
    QWebSecurityOrigin::addLocalScheme("qrc");
    browser = new Browser;
    ui.tabWidget->addTab(browser, tr("Browser"));
    setCentralWidget(ui.tabWidget);
    browser->quickstart();
}

void MainWindow::nextTab()
{
    int index = ui.tabWidget->currentIndex();
    int count = ui.tabWidget->count();
    if (index != -1)
    {
        index = (index + 1) % count;
        ui.tabWidget->setCurrentIndex(index);
    }
}

void MainWindow::previousTab()
{
    int index = ui.tabWidget->currentIndex();
    int count = ui.tabWidget->count();
    if (index != -1)
    {
        index--;
        ui.tabWidget->setCurrentIndex((index < 0) ? (count - 1) : index);
    }
}

void MainWindow::createDeviceChooser()
{
    deviceAction = ui.deviceToolBar->addAction(QIcon(":/images/atmega168_icon.png"), tr("Device"));
    deviceChooser = new DeviceChooser(this);
    connect(deviceAction, SIGNAL(triggered()), this, SLOT(chooseDevice()));
    connect(deviceChooser, SIGNAL(deviceChosen(const QString &)), SLOT(setDevice(const QString &)));
}

void MainWindow::chooseDevice()
{
    QWidget *w = ui.deviceToolBar->widgetForAction(deviceAction);
    int x = ui.deviceToolBar->mapToGlobal(QPoint(ui.deviceToolBar->width(), 0)).x();
    int y = w->mapToGlobal(QPoint(0, 0)).y();
    deviceChooser->refresh();
    deviceChooser->exec(QPoint(x, y));
}

void MainWindow::createBoardChooser()
{
    boardAction = ui.deviceToolBar->addAction(QIcon(":/images/arduino_diecimila.png"), tr("Board"));
    boardChooser = new BoardChooser(this);
    connect(boardAction, SIGNAL(triggered()), this, SLOT(chooseBoard()));
    connect(boardChooser, SIGNAL(boardChosen(const QString &)), SLOT(setBoard(const QString &)));
}

void MainWindow::chooseBoard()
{
    QWidget *w = ui.deviceToolBar->widgetForAction(boardAction);
    int x = ui.deviceToolBar->mapToGlobal(QPoint(ui.deviceToolBar->width(), 0)).x();
    int y = w->mapToGlobal(QPoint(0, 0)).y();
    boardChooser->refresh();
    boardChooser->exec(QPoint(x, y));
}

void MainWindow::newProject(const QString &code, const QString &name, Editor **pEditor)
{
    Editor *editor;
    if (! code.isEmpty())
        editor = EditorFactory::createEditor(code);
    else
        editor = EditorFactory::createEditor();

    int tab = ui.tabWidget->addTab(editor, (name.isNull()) ? createUniqueName("New project") : name);
    ui.tabWidget->setCurrentIndex(tab);

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
        index = ui.tabWidget->currentIndex();

    QWidget *w = ui.tabWidget->widget(index);
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
                    QMessageBox::Yes) == QMessageBox::Yes);
            if (close)
            {
                names.removeOne(ui.tabWidget->tabText(index));
                ui.tabWidget->removeTab(index);

                emit editorDeleted(editor);
            }
        }
    }
}

Editor *MainWindow::currentEditor()
{
    return qobject_cast<Editor *>(ui.tabWidget->currentWidget());
}

QList<Editor *> MainWindow::editors()
{
    QList<Editor *> editors;
    Editor *editor;
    for (int i = 0; i < ui.tabWidget->count(); i++)
    {
        editor = qobject_cast<Editor *>(ui.tabWidget->widget(i));
        if (editor != NULL)
            editors << editor;
    }
    return editors;
}

void MainWindow::configureEditors()
{
    Settings *settings = ideApp->settings();
    foreach (Editor *editor, editors())
    {
        settings->loadEditorSettings(editor);
        LexerArduino *lexer = dynamic_cast<LexerArduino *>(editor->lexer());
        Q_ASSERT(lexer != NULL);
        settings->loadLexerProperties(lexer);
    }
}

void MainWindow::tabContentHasChanged()
{
    bool undoAvail = false;
    bool redoAvail = false;
    bool previousAvail = false;
    bool forwardAvail = false;
    bool contextualHelpAvail = false;

    Editor *e = currentEditor();
    if (e)
    {
        undoAvail = e->isUndoAvailable();
        redoAvail = e->isRedoAvailable();
        contextualHelpAvail = true;
    }
    else
    {
        previousAvail = browser->canGoBack();
        forwardAvail = browser->canGoForward();
    }

    ui.actionUndo->setEnabled(undoAvail);
    ui.actionRedo->setEnabled(redoAvail);
    ui.action_Prev->setEnabled(previousAvail);
    ui.action_Next->setEnabled(forwardAvail);
    ui.action_Contextual_help->setEnabled(contextualHelpAvail);
}

void MainWindow::tabHasChanged()
{
    tabContentChanged();

    Editor *e = currentEditor();
    emit tabChanged(e!=NULL);
}

void MainWindow::contextualHelp()
{
    Editor *e = currentEditor();
    if (e && e->hasFocus())
    {
        e->showContextualHelp();
    }
    else
    {
        // TODO: Show the IDE manual
    }
}

bool MainWindow::docHelpRequested(QString word)
{
    if (browser->docHelpRequested(word))
    {
        ui.tabWidget->setCurrentIndex(0);
        return true;
    }

    return false;
}

void MainWindow::openCommunityArduinoCC()
{
    QDesktopServices::openUrl(QUrl("http://www.arduino.cc"));
}

void MainWindow::openCommunityArduinoForums()
{
    QDesktopServices::openUrl(QUrl("http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl"));
}

void MainWindow::pastebinUploadDone(QNetworkReply* reply)
{
    QString result = reply->readAll();
    QUrl url(result);

    if (url.isValid())
        QDesktopServices::openUrl(url);
    else
        QMessageBox::warning(this, tr("Pastebin error"), tr("The pastebin upload failed with code:\n%1").arg(result));
}

void MainWindow::finishedBuilding()
{

    emit buildFinished(true);
}

void MainWindow::showSearchBox(bool show)
{
     ui.dockSearchReplace->setVisible(show);
     if (show)
     {
         ui.dockSearchReplace->resize(ui.dockSearchReplace->size().width(), 10);
         ui.lineSearch->setFocus();
     }
}

bool MainWindow::search()
{
     QString searchText = ui.lineSearch->text();
     bool re = ui.checkRegExp->isChecked();
     bool wo = ui.checkWordOnly->isChecked();
     bool cs = ui.checkCaseSensitive->isChecked();

     Editor *e = currentEditor();
     if (!e) {
          int flags = QWebPage::FindWrapsAroundDocument;
          if (cs)
               flags |= QWebPage::FindCaseSensitively;
          browser->findText(searchText, (QWebPage::FindFlags)flags);
          return false;
     }

     bool found = e->findFirst(searchText, re, cs, wo, true);
     if (!found)
          QMessageBox::warning(this,
                               tr("Arduide - No occurence found"),
                               tr("No occurence of '%1' found").arg(searchText));
     return found;
}

bool MainWindow::replace()
{
     QString replaceText = ui.lineReplace->text();

     Editor *e = currentEditor();
     if (!e)
          return false;
     bool found = search();
     if (found)
          e->replace(replaceText);

     return found;
}

bool MainWindow::replaceAll()
{
     QString replaceText = ui.lineReplace->text();

     Editor *e = currentEditor();
     if (!e)
          return false;

     bool found = search();
     if (found)
     {
          int count = 0;
          do
          {
               e->replace(replaceText);
               count++;
          } while (e->findNext());

          QMessageBox::information(this, tr("Arduide - Replace All"),
                                   tr("The Replace All feature replaced %1 occurences").arg(count));

          return true;
     }
     else
          return false;
}

void MainWindow::open(const QString &_fileName)
{
    QString fileName(_fileName);

    if (fileName.isEmpty())
    {
        fileName = QFileDialog::getOpenFileName(this, tr("Open project"), ideApp->settings()->sketchPath(), tr("Arduino sketches (*.pde)"));
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
    ideApp->projectHistory()->updateHistory(fileName);
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
            index = ui.tabWidget->currentIndex();
            names.removeOne(ui.tabWidget->tabText(index));
            ui.tabWidget->setTabText(index, createUniqueName(QFileInfo(e->fileName()).fileName()));
        }

        // update the history
        ideApp->projectHistory()->updateHistory(e->fileName());
    }
}

void MainWindow::uploadToPastebin()
{
    Editor *e = currentEditor();
    if (e)
    {
        QNetworkRequest request(QUrl("http://pastebin.com/api_public.php"));
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

        QByteArray data;
        QUrl params;
        params.addQueryItem("paste_code",e->text());
        params.addQueryItem("paste_format","c");
        data.append(params.encodedQuery());

        pastebin.post(request,data);
    }
}

void MainWindow::undo()
{
    Editor *e = currentEditor();
    if (e) e->undo();
}

void MainWindow::redo()
{
    Editor *e = currentEditor();
    if (e) e->redo();
}

void MainWindow::copy()
{
    Editor *e = currentEditor();
    if (e)
        e->copy();
    else
        browser->triggerPageAction(QWebPage::Copy);
}

void MainWindow::cut()
{
    Editor *e = currentEditor();
    if (e)
       e->cut();
    else
       browser->triggerPageAction(QWebPage::Cut);
}

void MainWindow::paste()
{
    Editor *e = currentEditor();
    if (e)
       e->paste();
    else
       browser->triggerPageAction(QWebPage::Paste);
}

void MainWindow::setDevice(const QString &device)
{
    qDebug() << "setDevice:" << device;
    ideApp->settings()->setDevicePort(device);
}

void MainWindow::setBoard(const QString &board)
{
    qDebug() << "setBoard:" << board;
    ideApp->settings()->setBoard(board);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    for (int i = 0; i < ui.tabWidget->count(); i++)
    {
        Editor *editor = dynamic_cast<Editor *>(ui.tabWidget->widget(i));
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

    ideApp->settings()->saveMainWindowState(saveState());

    event->accept();
}

void MainWindow::build()
{
    Editor *editor = currentEditor();
    if (editor)
    {
        ui.dockWidget->show();
        ui.outputView->clear();

        BackgroundBuilder *builder = new BackgroundBuilder(ui.outputView);
        builder->setRelatedActions(buildActions);
        connect(builder, SIGNAL(buildFinished(bool)), builder, SLOT(deleteLater()));
        connect(builder, SIGNAL(buildFinished(bool)), this, SIGNAL(buildFinished(bool)));
        connect(builder, SIGNAL(log(QString)), ui.outputView, SLOT(log(QString)));
        connect(builder, SIGNAL(logError(QString)), ui.outputView, SLOT(logError(QString)));
        connect(builder, SIGNAL(logImportant(QString)), ui.outputView, SLOT(logImportant(QString)));
        connect(builder, SIGNAL(logCommand(QStringList)), ui.outputView, SLOT(logCommand(QStringList)));
        builder->backgroundBuild(editor->text());
    }
}

void MainWindow::upload()
{
    Editor *editor = currentEditor();
    if (editor)
    {
        buildActions->setEnabled(false);

        QString device = ideApp->settings()->devicePort();
        ui.dockWidget->show();
        ui.outputView->clear();

        BackgroundBuilder *builder = new BackgroundBuilder(ui.outputView);
        builder->setRelatedActions(buildActions);
        connect(builder, SIGNAL(buildFinished(bool)), builder, SLOT(deleteLater()));
        connect(builder, SIGNAL(buildFinished(bool)), this, SIGNAL(uploadFinished(bool)));
        connect(builder, SIGNAL(log(QString)), ui.outputView, SLOT(log(QString)));
        connect(builder, SIGNAL(logError(QString)), ui.outputView, SLOT(logError(QString)));
        connect(builder, SIGNAL(logImportant(QString)), ui.outputView, SLOT(logImportant(QString)));
        connect(builder, SIGNAL(logCommand(QStringList)), ui.outputView, SLOT(logCommand(QStringList)));
        builder->backgroundBuild(editor->text(), true);
    }
}

void MainWindow::toggleDock()
{
    ui.dockWidget->setVisible(! ui.dockWidget->isVisible());
}

void MainWindow::configure()
{
    if (configDialog == NULL)
        configDialog = new ConfigDialog(this);
    configDialog->reset();
    configDialog->exec();
}

void MainWindow::setFont(const QFont &font)
{
    foreach (Editor *editor, editors())
        editor->setLexerFont(font);
}

void MainWindow::about()
{
    QDialog *dialog = new QDialog(this);
    Ui::AboutDialog ui;
    ui.setupUi(dialog);
    dialog->setWindowTitle(dialog->windowTitle().arg(PROJECT_NAME));
    ui.nameLabel->setText(ui.nameLabel->text().arg(PROJECT_NAME));
    ui.urlLabel->setText(ui.urlLabel->text().arg(PROJECT_URL));
    ui.authorsLabel->setText(ui.authorsLabel->text().arg(PROJECT_AUTHORS));
    ui.licenseLabel->setText(ui.licenseLabel->text().arg(PROJECT_LICENSE));
    ui.descriptionLabel->setText(ui.descriptionLabel->text().arg(PROJECT_NAME));
    dialog->exec();
    delete dialog;
}
