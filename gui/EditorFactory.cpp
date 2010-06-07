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
