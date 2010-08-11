/**
 * \file MainWindow.h
 * \author Denis Martinez
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"

#include <QComboBox>
#include <QToolButton>
#include <QNetworkAccessManager>

#include "IDEGlobal.h"

class QUrl;
class Browser;
class Editor;
class QTabWidget;
class QCloseEvent;
class DeviceChooser;
class BoardChooser;
class ConfigDialog;

class QNetworkReply;

class IDE_EXPORT MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    void initialize();

    // widget getters
    QTabWidget *utilityTabWidget() { return ui.utilityTabWidget; }

    Editor *currentEditor();

public slots:
    void setFont(const QFont &font);
    void closeTab(int index = -1);
    void nextTab();
    void previousTab();
    void newProject(const QString &code = QString(), const QString &name = QString(), Editor **pEditor = NULL);
    void open(const QString &fileName = QString());
    void save();
    void uploadToPastebin();
    void chooseDevice();
    void chooseBoard();
    void undo();
    void redo();
    void copy();
    void cut();
    void paste();
    void build();
    void upload();
    void toggleDock();
    void setDevice(const QString &device);
    void setBoard(const QString &board);
    void configure();
    void about();
    void configureEditors();
    void tabContentHasChanged();
    void tabHasChanged();
    void contextualHelp();
    bool docHelpRequested(QString);

private slots:
    void openCommunityArduinoCC();
    void openCommunityArduinoForums();
    void pastebinUploadDone(QNetworkReply* reply);
    void finishedBuilding();

private:
    void setupActions();
    void createBrowserAndTabs();
    void createDeviceChooser();
    void createBoardChooser();

    QStringList names;
    QString createUniqueName(const QString &name);

    QTabWidget* tabWidget;
    Browser *browser;
    QAction *deviceAction;
    DeviceChooser *deviceChooser;
    QAction *boardAction;
    BoardChooser *boardChooser;

    QActionGroup *buildActions;

    ConfigDialog *configDialog;

    QList<Editor *> editors();

    Ui::MainWindow ui;

    QNetworkAccessManager pastebin;

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void tabContentChanged();
    void tabChanged(bool isBrowser);
    void editorDeleted(Editor* editor);
    void buildFinished(bool result);
    void uploadFinished(bool result);
};

#endif // MAINWINDOW_H
