/*
  Editor.cpp

  This file is part of arduide, The Qt-based IDE for the open-source Arduino electronics prototyping platform.

  Copyright (C) 2010-2012 
  Authors : Denis Martinez
	    Martin Peres

This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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

void Editor::save(bool saveas)
{
    if (mFileName.isEmpty() or saveas)
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
        mFileName = dir.filePath(dir.dirName() + ".ino");
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

void Editor::setCaretWidth(int width)
{
    mCaretWidth = width;
    QsciScintilla::setCaretWidth(width);
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

    int start = line_s.lastIndexOf(QRegExp("\\W"), index)+1;
    if (start > index)
            start = line_s.lastIndexOf(QRegExp("\\W"), index-1)+1;
    if (start > 0 && line_s[start-1] == '.')
        start = line_s.lastIndexOf(QRegExp("\\W"), start-2)+1;

    int end = line_s.indexOf(QRegExp("\\W"), index);

    if (start<end)
    {
        QString word = line_s.mid(start, end-start);
        ideApp->mainWindow()->docHelpRequested(word);
    }
}
