/**
 * \file EditorFactory.cpp
 * \author Denis Martinez
 */

#include "EditorFactory.h"

#include <QDebug>
#include <qscilexercpp.h>

#include "../env/Settings.h"

Editor *EditorFactory::createEditor(const QString &text, QWidget *parent)
{
    Editor *editor = new Editor(parent);
    QsciLexerCPP *lexerCPP = new QsciLexerCPP;
    lexerCPP->setFont(Settings::instance().editorFont());
    editor->setLexer(lexerCPP);
    editor->setMarginLineNumbers(1, true);
    editor->setMarginWidth(1, 40);
    editor->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    editor->setEdgeMode(QsciScintilla::EdgeLine);
    editor->setEdgeColumn(90);
    editor->setFolding(QsciScintilla::BoxedTreeFoldStyle);
    editor->setSelectionToEol(true);
    editor->setAutoIndent(true);
    editor->setTabWidth(4);
    editor->setText(text);
    editor->setModified(false);
    return editor;
}

const QString EditorFactory::initialText =
    "void setup()\n"
    "{\n"
    "}\n"
"\n"
"void loop()\n"
"{\n"
"}\n";
