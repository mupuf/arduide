/*
  EditorFactory.cpp

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
 * \file EditorFactory.cpp
 * \author Denis Martinez
 */

#include "EditorFactory.h"

#include <QDebug>

#include "LexerArduino.h"
#include "../env/Settings.h"
#include "IDEApplication.h"

Editor *EditorFactory::createEditor(const QString &text, QWidget *parent)
{
    Editor *editor = new Editor(parent);
    LexerArduino *lexer = createLexer(editor);
    Settings *settings = ideApp->settings();
    editor->setLexer(lexer);
    settings->loadEditorSettings(editor);

    editor->setMarginLineNumbers(1, true);
    editor->setMarginWidth(1, 40);
    editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    editor->setEdgeMode(QsciScintilla::EdgeLine);
    editor->setEdgeColumn(90);
    editor->setFolding(QsciScintilla::BoxedTreeFoldStyle);
    editor->setSelectionToEol(true);
    editor->setAutoIndent(true);
    editor->setText(text);
    editor->setModified(false);

    QObject::connect(editor, SIGNAL(textChanged()), ideApp->mainWindow(), SLOT(tabContentHasChanged()));
    QObject::connect(editor, SIGNAL(modificationChanged(bool)), ideApp->mainWindow(), SLOT(editorModificationChanged(bool)));
    
    return editor;
}

LexerArduino *EditorFactory::createLexer(QObject *parent)
{
    LexerArduino *lexer = new LexerArduino(parent);
    Settings *settings = ideApp->settings();
    settings->loadLexerProperties(lexer);
    return lexer;
}

const QString EditorFactory::initialText =
    "void setup()\n"
    "{\n"
    "}\n"
"\n"
"void loop()\n"
"{\n"
"}\n";
