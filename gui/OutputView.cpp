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
    if (! text.isEmpty())
    {
        append(text);
        ensureCursorVisible();
    }
}

void OutputView::logImportant(const QString &text)
{
    int oldWeight = fontWeight();
    setFontWeight(QFont::Bold);
    QColor oldColor = textColor();
    setTextColor(Qt::white);
    log(text);
    setFontWeight(oldWeight);
    setTextColor(oldColor);
}

void OutputView::logError(const QString &text)
{
    QColor oldColor = textColor();
    setTextColor(Qt::red);
    log(text);
    setTextColor(oldColor);
}

void OutputView::logCommand(const QString &command)
{
    static const QString format = tr(">>>> %0");
    log(format.arg(command));
}

void OutputView::logCommand(const QStringList &command)
{
    logCommand(command.join(" "));
}
