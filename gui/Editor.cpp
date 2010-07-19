/**
 * \file Editor.cpp
 * \author Denis Martinez
 */

#include "Editor.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QShortcut>
#include <QDebug>
#include <Qsci/qscilexer.h>

#include "../env/Settings.h"
#include "IDEApplication.h"

Editor::Editor(QWidget *parent)
    : QsciScintilla(parent)
{
    setupShortcuts();

    connect(this, SIGNAL(selectionChanged()), this, SLOT(updateSelectionOrigin()));
}

void Editor::save()
{
    if (mFileName.isEmpty())
    {
        QString dirName = QFileDialog::getSaveFileName(this, tr("Save project"), ideApp->settings()->sketchPath(), tr("Directories (*)"), NULL, QFileDialog::DontConfirmOverwrite);
        if (dirName.isEmpty())
            return;
        bool ok = QDir().mkdir(dirName);
        if (! ok)
        {
            QMessageBox::warning(this, tr("Save error"), tr("The specified location could not be opened for writing."));
            return;
        }
        QDir dir(dirName);
        mFileName = dir.filePath(dir.dirName() + ".pde");
    }

    QFile f(mFileName);
    if (! f.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, tr("Save error"), tr("The file could not be opened for writing."));
        return;
    }
    f.write(text().toLocal8Bit());
    f.close();

    setModified(false);
}

void Editor::setupShortcuts()
{
    QShortcut *shortcut;
    shortcut = new QShortcut(QKeySequence("Ctrl+Up"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(goToPreviousParagraph()));
    shortcut = new QShortcut(QKeySequence("Ctrl+Down"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(goToNextParagraph()));
    shortcut = new QShortcut(QKeySequence("Ctrl+Shift+Up"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(selectTillPreviousParagraph()));
    shortcut = new QShortcut(QKeySequence("Ctrl+Shift+Down"), this);
    connect(shortcut, SIGNAL(activated()), this, SLOT(selectTillNextParagraph()));
}

void Editor::findPreviousParagraph(int *pLine, int *pIndex)
{
    int line, index;
    getCursorPosition(&line, &index);
    while (line > 0)
    {
        line--;
        if (text(line).trimmed().isEmpty())
            break;
    }
    *pLine = line;
    *pIndex = 0;
}

void Editor::findNextParagraph(int *pLine, int *pIndex)
{
    int line, index;
    int lastLine = lines() - 1;
    getCursorPosition(&line, &index);
    while (line < lastLine)
    {
        line++;
        if (text(line).trimmed().isEmpty())
            break;
    }
    *pLine = line;
    *pIndex = 0;
}

void Editor::goToPreviousParagraph()
{
    int line, index;
    findPreviousParagraph(&line, &index);
    setCursorPosition(line, index);
}

void Editor::goToNextParagraph()
{
    int line, index;
    findNextParagraph(&line, &index);
    setCursorPosition(line, index);
}

void Editor::selectTillPreviousParagraph()
{
    int lineTo, indexTo;
    if (selectionOrigin.line == -1)
    {
        // no previous selection, start from cursor
        getCursorPosition(&selectionOrigin.line, &selectionOrigin.index);
    }
    findPreviousParagraph(&lineTo, &indexTo);
    setSelection(selectionOrigin.line, selectionOrigin.index, lineTo, indexTo);
}

void Editor::selectTillNextParagraph()
{
    int lineTo, indexTo;
    if (selectionOrigin.line == -1)
    {
        // no previous selection, start from cursor
        getCursorPosition(&selectionOrigin.line, &selectionOrigin.index);
    }
    findNextParagraph(&lineTo, &indexTo);
    setSelection(selectionOrigin.line, selectionOrigin.index, lineTo, indexTo);
}

void Editor::updateSelectionOrigin()
{
    int lineFrom, indexFrom, lineTo, indexTo;
    getSelection(&lineFrom, &indexFrom, &lineTo, &indexTo);
    if (lineFrom == -1)
    {
        selectionOrigin.line = -1;
        selectionOrigin.index = -1;
    } else if (selectionOrigin.line == -1)
    {
        selectionOrigin.line = lineFrom;
        selectionOrigin.index = indexFrom;
    }
}

void Editor::setLexerFont(const QFont &font)
{
    lexer()->setFont(font);
}

// workaround: impossible to get these attributes in QsciScintilla directly
// so override them instead
void Editor::setCaretForegroundColor(const QColor &col)
{
    mCaretForegroundColor = col;
    QsciScintilla::setCaretForegroundColor(col);
}

void Editor::setSelectionBackgroundColor(const QColor &col)
{
    mSelectionBackgroundColor = col;
    QsciScintilla::setSelectionBackgroundColor(col);
}

void Editor::showContextualHelp()
{
    int line, index;
    getCursorPosition(&line, &index);

    QString line_s = text(line);

    if (QChar(line_s[index]).isSpace())
        return;

    int start = line_s.lastIndexOf(QRegExp("\\W"), index)+1;
    if (start > 0 && line_s[start-1] == '.')
        start = line_s.lastIndexOf(QRegExp("\\W"), start-2)+1;

    int end = line_s.indexOf(QRegExp("\\W"), index);

    if (start<end)
    {
        QString word = line_s.mid(start, end-start);
        ideApp->mainWindow()->docHelpRequested(word);
    }
}
