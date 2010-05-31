/**
 * \file DebuggerWidget.h
 * \author Martin Peres
 */

#ifndef DEBUGGERWIDGET_H
#define DEBUGGERWIDGET_H

#include "plugins/ui_DebuggerWidget.h"

class DebuggerWidget : public QWidget, Ui::DebuggerWidget
{
    Q_OBJECT

public:
    DebuggerWidget(QWidget *parent = NULL);
    void setStatus(const QString &text);
};

#endif // SERIALWRITEDIALOG_H