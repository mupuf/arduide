/**
 * \file TemporaryDirectory.cpp
 * \author Denis Martinez
 */

#include "TemporaryDirectory.h"

#include <QDir>
#include <QFileInfo>

#include <cstdlib>

#include "FileUtils.h"

#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64)
	#include <windows.h>
	
	TemporaryDirectory::TemporaryDirectory(const QString &tmpl)
	{
		static const DWORD dwBufSize=101;
		TCHAR szTempFileName[dwBufSize];  
		TCHAR lpTempPathBuffer[dwBufSize];

		 //  Gets the temp path env string (no guarantee it's a valid path).
		DWORD dwRetVal = GetTempPath(dwBufSize,     // length of the buffer
							   lpTempPathBuffer); // buffer for path 
		if (dwRetVal > dwBufSize || (dwRetVal == 0))
		{
			fprintf(stderr, "GetTempPath failed");
			return;
		}

		//  Generates a temporary file name. 
		UINT uRetVal = GetTempFileName(lpTempPathBuffer, // directory for tmp files
								  TEXT(qPrintable(tmpl)),// temp file name prefix 
								  0,                // create unique name 
								  szTempFileName);  // buffer for name 
		if (uRetVal == 0)
		{
			fprintf(stderr, "GetTempFileName failed");
			return;
		}
	
		mPath = QString::fromLocal8Bit(szTempFileName);
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
