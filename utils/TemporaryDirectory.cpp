/**
 * \file TemporaryDirectory.cpp
 * \author Denis Martinez
 */

#include "TemporaryDirectory.h"

#include <QDir>
#include <QFileInfo>

#include <cstdlib>

#include "FileUtils.h"

TemporaryDirectory::TemporaryDirectory(const QString &tmpl)
{
    QString fullTmpl = QDir::temp().filePath(tmpl + "XXXXXX");
    QByteArray dir(mkdtemp(fullTmpl.toLocal8Bit().data()));
    if (! dir.isEmpty())
        mPath = QString::fromLocal8Bit(dir);
}

TemporaryDirectory::~TemporaryDirectory()
{
    if (isValid())
        FileUtils::recursiveRemove(mPath);
}

bool TemporaryDirectory::isValid()
{
    if (mPath.isEmpty())
        return false;
    QFileInfo fi(mPath);
    return fi.isDir() && fi.isWritable();
}

bool TemporaryDirectory::clear()
{
    // remove everything inside the directory
    if (isValid())
        return FileUtils::recursiveRemove(mPath, true);
    else
        return false;
}
