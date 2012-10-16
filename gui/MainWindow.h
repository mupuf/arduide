/*
  MainWindow.h

  This file is part of arduide, The Qt-based IDE for the open-source Arduino electronics prototyping platform.

  Copyright (C) 2010-2012 
  Authors : Denis Martinez
	    Martin Peres

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
    void refreshLibrariesMenu();
    void refreshTitle();

private slots:
    void openCommunityArduinoCC();
    void openCommunityArduinoForums();
    void pastebinUploadDone(QNetworkReply* reply);
    void finishedBuilding();
    void showFindBox(bool show);
    bool find();
    bool replace();
    bool replaceAll();
    void importLib();
    void createAndOpenUserLibDir();

private:
    void setupActions();
    void createBrowserAndTabs();
    void createDeviceChooser();
    void createBoardChooser();

    QStringList names;
    QString createUniqueName(const QString &name);

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

    QMenu menu_lib_arduino;
    QMenu menu_lib_ide;
    QMenu menu_lib_user;

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
