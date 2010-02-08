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

#include "../env/Settings.h"

Editor::Editor(QWidget *parent)
    : QsciScintilla(parent)
{
    setupShortcuts();
}

void Editor::save()
{
    if (mFileName.isEmpty())
    {
        QString dirName = QFileDialog::getSaveFileName(this, tr("Save project"), Settings::instance().sketchPath(), tr("Directories (*)"), NULL, QFileDialog::DontConfirmOverwrite);
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
}

void Editor::goToPreviousParagraph()
{
    int line, index;
    getCursorPosition(&line, &index);
    while (line > 0)
    {
        line--;
        if (text(line).trimmed().isEmpty())
            break;
    }
    setCursorPosition(line, 0);
}

void Editor::goToNextParagraph()
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
    setCursorPosition(line, 0);
}
