
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
