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
    addCustomShortcut(QKeySequence("Ctrl+Up"), this, SLOT(goToPreviousParagraph()));
    addCustomShortcut(QKeySequence("Ctrl+Down"), this, SLOT(goToNextParagraph()));
    addCustomShortcut(QKeySequence("Ctrl+Shift+Up"), this, SLOT(selectTillPreviousParagraph()));
    addCustomShortcut(QKeySequence("Ctrl+Shift+Down"), this, SLOT(selectTillNextParagraph()));
}

bool Editor::addCustomShortcut(const QKeySequence &key, QObject *receiver, const char *slot)
{
    foreach (const EditorShortcut &sc, mCustomShortcuts)
    {
        if (sc.shortcut->key() == key)
        {
            // already exists
            return false;
        }
    }
    QShortcut *sc = new QShortcut(key, this);
    connect(sc, SIGNAL(activated()), receiver, slot);
    EditorShortcut es = { sc, receiver, slot };
    mCustomShortcuts << es;
    return true;
}

bool Editor::removeCustomShortcut(const QKeySequence &key)
{
    QList<EditorShortcut>::iterator it;
    for (it = mCustomShortcuts.begin(); it != mCustomShortcuts.end(); it++)
    {
        if (it->shortcut->key() == key)
        {
            delete it->shortcut;
            mCustomShortcuts.erase(it);
            return true;
        }
    }
    return false;
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
