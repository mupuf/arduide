/*
  Editor.h

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
 * \file Editor.h
 * \author Denis Martinez
 */

#ifndef EDITOR_H
#define EDITOR_H

#include <Qsci/qsciscintilla.h>

#include "IDEGlobal.h"

class QShortcut;

class IDE_EXPORT Editor : public QsciScintilla
{
    Q_OBJECT

public:
    Editor(QWidget *parent = NULL);
    const QString &fileName() { return mFileName; }
    void setFileName(const QString &fileName) { mFileName = fileName; }
    void setLexerFont(const QFont &font);

    void setCaretForegroundColor(const QColor &col);
    void setCaretWidth(int width);
    const QColor &caretForegroundColor() { return mCaretForegroundColor; }
    int &caretWidth() { return mCaretWidth; }
    void setSelectionBackgroundColor(const QColor &col);
    const QColor &selectionBackgroundColor() { return mSelectionBackgroundColor; }

    bool addCustomShortcut(const QKeySequence &key, QObject *receiver, const char *slot);
    bool removeCustomShortcut(const QKeySequence &key);

public slots:
    void save();
    void showContextualHelp();

private:
    void setupShortcuts();

    QString mFileName;
    struct
    {
        int line;
        int index;
    } selectionOrigin;

    struct EditorShortcut
    {
        QShortcut *shortcut;
        QObject *receiver;
        const char *slot;
    };
    QList<EditorShortcut> mCustomShortcuts;

    QColor mCaretForegroundColor, mSelectionBackgroundColor;
    int mCaretWidth;

private slots:
    void findPreviousParagraph(int *line, int *index);
    void findNextParagraph(int *line, int *index);
    void selectTillPreviousParagraph();
    void selectTillNextParagraph();
    void updateSelectionOrigin();

    void goToPreviousParagraph();
    void goToNextParagraph();

signals:
    void helpRequested(QString);
};

#endif // EDITOR_H
