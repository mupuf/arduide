/**
 * \file FileUtils.h
 * \author Denis Martinez
 */

#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QString>

class FileUtils
{
public:
    static bool recursiveRemove(const QString &path, bool keepDirectory = false);
};

#endif // FILEUTILS_H
