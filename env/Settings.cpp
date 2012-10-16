/*
  Settings.cpp

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
 * \file Settings.cpp
 * \author Denis Martinez
 */

#include "Settings.h"

#include <QFileInfo>
#include <QDebug>

#include "gui/LexerArduino.h"
#include "gui/Editor.h"
#include "Toolkit.h"

Settings::Settings()
{
}

QString Settings::arduinoPath() const
{
    return mSettings.value("arduinoPath").toString();
}

void Settings::setArduinoPath(const QString &path)
{
    mSettings.setValue("arduinoPath", path);
}

QString Settings::sketchPath() const
{
    return mSettings.value("sketchPath").toString();
}

void Settings::setSketchPath(const QString &path)
{
    mSettings.setValue("sketchPath", path);
}

QString Settings::devicePort() const
{
    return mSettings.value("devicePort").toString();
}

void Settings::setDevicePort(const QString &port)
{
    mSettings.setValue("devicePort", port);
}

QString Settings::board() const
{
    return mSettings.value("board").toString();
}

void Settings::setBoard(const QString &board)
{
    mSettings.setValue("board", board);
}

bool Settings::filterSerialDevices() const
{
    return mSettings.value("filterSerialDevices", true).toBool();
}

void Settings::setFilterDevices(bool value)
{
    mSettings.setValue("filterSerialDevices", value);
}

bool Settings::verboseUpload() const
{
    return mSettings.value("verboseUpload", false).toBool();
}

void Settings::setVerboseUpload(bool verbose)
{
    mSettings.setValue("verboseUpload", verbose);
}

void Settings::loadLexerProperties(LexerArduino *lexer)
{
    if (! lexer->readSettings(mSettings))
    {
        // set some default settings and save
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
        QFont editorFont("Lucida Console", 9);
#elif defined(Q_OS_DARWIN)
        QFont editorFont("Monaco", 9);
#else
        QFont editorFont("Monospace", 9);
#endif
        editorFont.setStyleHint(QFont::TypeWriter);
        lexer->setDefaultFont(editorFont);
        lexer->setFont(editorFont);
        lexer->setDefaultColor(Qt::white);
        lexer->setColor(Qt::white, LexerArduino::Default);
        lexer->setColor(Qt::white, LexerArduino::Number);
        lexer->setColor(Qt::white, LexerArduino::Operator);
        lexer->setColor(0xff7f24, LexerArduino::Comment);
        lexer->setColor(0xff7f24, LexerArduino::CommentLine);
        lexer->setColor(0xff7f24, LexerArduino::CommentDoc);
        lexer->setColor(0xff7f24, LexerArduino::CommentLineDoc);
        lexer->setColor(0xff7f24, LexerArduino::CommentDocKeyword);
        lexer->setColor(0xff7f24, LexerArduino::CommentDocKeywordError);
        lexer->setColor(0x80ffff, LexerArduino::Keyword);
        lexer->setColor(0xffa07a, LexerArduino::DoubleQuotedString);
        lexer->setColor(0xffa07a, LexerArduino::SingleQuotedString);
        lexer->setColor(0xffa07a, LexerArduino::UnclosedString);
        lexer->setColor(0xffa07a, LexerArduino::VerbatimString);
        lexer->setColor(0xffa07a, LexerArduino::Regex);
        lexer->setColor(0x98fb98, LexerArduino::UUID);
        lexer->setColor(0x98fb98, LexerArduino::Identifier);
        lexer->setColor(0xb0c5de, LexerArduino::PreProcessor);
        lexer->setColor(0xeedd82, LexerArduino::KeywordSet2);
        lexer->setColor(Qt::white, LexerArduino::GlobalClass);
        lexer->setDefaultPaper(0x3f3f3f);
        lexer->setPaper(0x3f3f3f, LexerArduino::Default);
        lexer->setPaper(0x3f3f3f, LexerArduino::Number);
        lexer->setPaper(0x3f3f3f, LexerArduino::Operator);
        lexer->setPaper(0x3f3f3f, LexerArduino::Comment);
        lexer->setPaper(0x3f3f3f, LexerArduino::CommentLine);
        lexer->setPaper(0x3f3f3f, LexerArduino::CommentDoc);
        lexer->setPaper(0x3f3f3f, LexerArduino::CommentLineDoc);
        lexer->setPaper(0x3f3f3f, LexerArduino::CommentDocKeyword);
        lexer->setPaper(0x3f3f3f, LexerArduino::CommentDocKeywordError);
        lexer->setPaper(0x3f3f3f, LexerArduino::Keyword);
        lexer->setPaper(0x3f3f3f, LexerArduino::DoubleQuotedString);
        lexer->setPaper(0x3f3f3f, LexerArduino::SingleQuotedString);
        lexer->setPaper(0x2f2f2f, LexerArduino::UnclosedString);
        lexer->setPaper(0x3f3f3f, LexerArduino::VerbatimString);
        lexer->setPaper(0x3f3f3f, LexerArduino::Regex);
        lexer->setPaper(0x3f3f3f, LexerArduino::UUID);
        lexer->setPaper(0x3f3f3f, LexerArduino::Identifier);
        lexer->setPaper(0x3f3f3f, LexerArduino::PreProcessor);
        lexer->setPaper(0x3f3f3f, LexerArduino::KeywordSet2);
        lexer->setColor(0x3f3f3f, LexerArduino::GlobalClass);
        lexer->writeSettings(mSettings);
    }
}

void Settings::saveLexerProperties(LexerArduino *lexer)
{
    lexer->writeSettings(mSettings);
}

void Settings::loadEditorSettings(Editor *editor)
{
    static const QColor defaultCaretForegroundColor(Qt::white);
    static const int defaultTabWidth = 4;
    static const QColor defaultEditorSelectionBackgroundColor(0x000090);

    QVariant v;
    v = mSettings.value("editorCaretForegroundColor");
    editor->setCaretForegroundColor(v.isNull() ? defaultCaretForegroundColor : v.value<QColor>());
    v = mSettings.value("editorTabWidth");
    editor->setTabWidth(v.isNull() ? defaultTabWidth : v.toInt());
    v = mSettings.value("editorSelectionBackgroundColor");
    editor->setSelectionBackgroundColor(v.isNull() ? defaultEditorSelectionBackgroundColor : v.value<QColor>());
}

void Settings::saveEditorSettings(Editor *editor)
{
    mSettings.setValue("editorCaretForegroundColor", editor->caretForegroundColor());
    mSettings.setValue("editorTabWidth", editor->tabWidth());
    mSettings.setValue("editorSelectionBackgroundColor", editor->selectionBackgroundColor());
}

void Settings::saveMainWindowState(const QByteArray& state)
{
    mSettings.setValue("mainWindowState", state);
}

QByteArray Settings::mainWindowState()
{
    return mSettings.value("mainWindowState").toByteArray();
}

bool Settings::isCorrect() const
{
    return Toolkit::isValidArduinoPath(arduinoPath()) && QFileInfo(sketchPath()).isDir();
}
