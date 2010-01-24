/**
 * \file FileUtils.cpp
 * \author Denis Martinez
 */

#include "FileUtils.h"

#include <QDir>

bool FileUtils::recursiveRemove(const QString &path, bool keepDirectory)
{
    bool ok = true;

    //refuse to remove the empty path (current directory)
    if (path.isEmpty())
        return false;
    QDir dir(path);
    QString entryPath;
    foreach (const QString &entry, dir.entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System))
    {
        entryPath = dir.filePath(entry);
        QFileInfo fi(entryPath);
        if (fi.isDir())
            ok = recursiveRemove(entryPath) && ok;
        else
            ok = dir.remove(entry) && ok;
    }

    if (! keepDirectory)
        ok = dir.rmdir(dir.absolutePath()) && ok;

    return ok;
}
