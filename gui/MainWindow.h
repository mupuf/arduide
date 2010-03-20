/**
 * \file MainWindow.h
 * \author Denis Martinez
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <ui_MainWindow.h>

#include <QComboBox>
#include <QToolButton>

class QUrl;
class Browser;
class Editor;
class QTabWidget;
class QCloseEvent;
class DeviceChooser;
class BoardChooser;

class MainWindow : public QMainWindow, Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow();
    void initialize();

public slots:
    void setFont(const QFont &font);

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

    Editor *currentEditor();
    QList<Editor *> editors();

private slots:
    void closeTab(int index = -1);
    void nextTab();
    void previousTab();
    void newProject(const QString &code = QString(), const QString &name = QString(), Editor **pEditor = NULL);
    void open(const QString &fileName = QString());
    void save();
    void chooseDevice();
    void chooseBoard();
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

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
