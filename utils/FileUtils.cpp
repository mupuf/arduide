/*
  FileUtils.cpp

  This file is part of arduide, The Qt-based IDE for the open-source Arduino electronics prototyping platform.

  Copyright (C) 2010-2016 
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
