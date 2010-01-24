/**
 * \file OutputView.cpp
 * \author Denis Martinez
 */

#include "OutputView.h"

OutputView::OutputView(QWidget *parent)
    : QTextBrowser(parent)
{
    setTextColor(Qt::gray);
    setTextBackgroundColor(Qt::black);
}

void OutputView::log(const QString &text)
{
    append(text);
}

void OutputView::logError(const QString &text)
{
    QColor oldColor = textColor();
    setTextColor(Qt::red);
    log(text);
    setTextColor(oldColor);
}

void OutputView::logCommand(const QStringList &command)
{
    static const QString format = tr(">>> %0");
    log(format.arg(command.join(" ")));
}
