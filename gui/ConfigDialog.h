/*
  ConfigDialog.h

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
 * \file ConfigDialog.h
 * \author Denis Martinez
 */

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <qxtconfigdialog.h>

#include <QSet>

#include "ui_ConfigEditor.h"
#include "ui_ConfigPaths.h"
#include "ui_ConfigBuild.h"

class Editor;

class ConfigWidget : public QxtConfigWidget
{
    Q_OBJECT

public:
    ConfigWidget(QWidget *parent = NULL);
    void reset();
    bool saveConfig();

private slots:
    void chooseFont();
    void chooseArduinoPath();
    void chooseSketchbookPath();
    void setColorAtIndex(int index);
    void setFgColor(const QColor &);
    void setBgColor(const QColor &);
    void setCaretColor(const QColor &);
    void setSelectionColor(const QColor &);
    void fieldChange();

private:
    // page indexes
    enum ConfigPageIndex
    {
        EditorIndex = 0,
        PathsIndex,
        BuildIndex,
    };

    Ui::ConfigEditor uiEditor;
    Ui::ConfigPaths uiPaths;
    Ui::ConfigBuild uiBuild;
    QSet<QWidget *> mChangedFields;

    // contains the editor settings
    Editor *mEditor;

    void setupUi();
    void initializePage(int index);
    void resetPage(int index);
    void updateFontLabel(const QFont &f);
};

class ConfigDialog : public QxtConfigDialog
{
    Q_OBJECT

public:
    ConfigDialog(QWidget *parent = NULL);
    void reset() { mConfigWidget->reset(); }

public slots:
    void accept();

private slots:
    bool apply();

private:
    ConfigWidget *mConfigWidget;

    void setupUi();
};

#endif // CONFIGDIALOG_H
