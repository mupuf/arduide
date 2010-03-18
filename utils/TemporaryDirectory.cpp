/**
 * \file TemporaryDirectory.cpp
 * \author Denis Martinez
 */

#include "TemporaryDirectory.h"

#include <QDir>
#include <QFileInfo>
#include <QDebug>

#include "FileUtils.h"

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
#include <windows.h>

TemporaryDirectory::TemporaryDirectory(const QString &tmpl)
{
    TCHAR szTempFileName[MAX_PATH];
    TCHAR lpTempPathBuffer[MAX_PATH];

    // Gets the temp path env string (no guarantee it's a valid path).
    DWORD dwRetVal = GetTempPath(MAX_PATH,     // length of the buffer
                                 lpTempPathBuffer); // buffer for path
    if (dwRetVal > MAX_PATH || (dwRetVal == 0))
        qDebug() << "GetTempPath failed";
    else
    {
        UINT uRetVal = GetTempFileName(lpTempPathBuffer, TEXT(qPrintable(tmpl)), 1, szTempFileName);

        if (uRetVal == 0)
            qDebug() << "GetTempFileName failed";
        else
        {
            if (CreateDirectory(szTempFileName, NULL))
              mPath = QString::fromLocal8Bit(szTempFileName);
            else
                qDebug() << "CreateDirectory failed";
        }
    }
}
#else
TemporaryDirectory::TemporaryDirectory(const QString &tmpl)
{
    QString fullTmpl = QDir::temp().filePath(tmpl + "XXXXXX");
    QByteArray dir(mkdtemp(fullTmpl.toLocal8Bit().data()));
    if (! dir.isEmpty())
        mPath = QString::fromLocal8Bit(dir);
}
#endif

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
