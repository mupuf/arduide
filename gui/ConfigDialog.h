/**
 * \file ConfigDialog.h
 * \author Denis Martinez
 */

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <qxtconfigdialog.h>

#include "ui_ConfigEditor.h"

class ConfigDialog : public QxtConfigDialog
{
    Q_OBJECT

public:
    ConfigDialog(QWidget *parent = NULL);

private slots:
    void chooseFont();

private:
    // page indexes
    static const int editorIndex = 0;

    void setupUi();
    void initializePage(int index);

    void setupFontChooser();

    Ui::ConfigEditor uiEditor;
};

#endif // CONFIGDIALOG_H
