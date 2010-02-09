/**
 * \file MainWindow.h
 * \author Denis Martinez
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <ui_MainWindow.h>

#include <QComboBox>
#include <QToolButton>
#include "../env/ProjectHistory.h"

class QUrl;
class Browser;
class Editor;
class QTabWidget;
class QCloseEvent;

class MainWindow : public QMainWindow, Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow();
    void initialize();

public slots:
    void setFont(const QFont &font);
    void setIndentSize(int value);

private:
    void setupActions();
    void createBrowserAndTabs();
    void createDeviceChooser();

    ProjectHistory& pHistory;

    QStringList names;
    QString createUniqueName(const QString &name);

    QTabWidget* tabWidget;
    Browser *browser;
    QComboBox *deviceBox;
    QComboBox *boardBox;
    QAction *refreshDevicesAct;

    QActionGroup *buildActions;

    Editor *currentEditor();
    QList<Editor *> editors();

private slots:
    void closeTab(int index = -1);
    void nextTab();
    void previousTab();
    void newProject(const QString &code = QString(), const QString &name = QString(), Editor **pEditor = NULL);
    void open(const QString &fileName = QString());
    void save();
    void fillDeviceBox();
    void fillBoardsBox();
    void copy();
    void cut();
    void paste();
    void build();
    void upload();
    void toggleDock();
    void setDeviceAtIndex(int index);
    void setBoardAtIndex(int index);
    void configure();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
