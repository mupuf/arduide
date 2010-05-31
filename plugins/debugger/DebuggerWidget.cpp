/**
 * \file DebuggerWidget.cpp
 * \author Martin Peres
 */

#include "DebuggerWidget.h"

DebuggerWidget::DebuggerWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);
}

void DebuggerWidget::setStatus(const QString &text)
{
    statusLabel->setText(text);
}
