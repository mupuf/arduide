
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

class ConfigDialog : public QxtConfigDialog
{
    Q_OBJECT

public:
    ConfigDialog(QWidget *parent = NULL);

public slots:
    void accept();

private slots:
    void chooseFont();
    void chooseArduinoPath();
    void chooseSketchbookPath();

    bool apply();
    void fieldChange();

private:
    // page indexes
    static const int editorIndex = 0;
    static const int pathsIndex = editorIndex + 1;

    void setupUi();
    void initializePage(int index);

    void setupFontChooser();

    Ui::ConfigEditor uiEditor;
    Ui::ConfigPaths uiPaths;
    Ui::ConfigBuild uiBuild;

    QSet<QWidget *> mChangedFields;
};

#endif // CONFIGDIALOG_H
