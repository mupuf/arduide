/**
 * \file Editor.cpp
 * \author Denis Martinez
 */

#include "Editor.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QDebug>

#include "../env/Settings.h"

Editor::Editor(QWidget *parent)
    : QsciScintilla(parent)
{
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
