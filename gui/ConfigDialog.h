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
public:
    ConfigDialog(QWidget *parent = NULL);

private:
    void setupUi();

    Ui::ConfigEditor uiEditor;
};

#endif // CONFIGDIALOG_H
