/*
  Toolkit.cpp

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
 * \file Toolkit.cpp
 * \author Denis Martinez
 */

#include "Toolkit.h"

#include <QStringList>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QDebug>

#include "Board.h"
#include "IDEApplication.h"

QStringList Toolkit::findSketchesInDirectory(const QString &directory)
{
    QStringList sketches;
    QDir dir(directory);
    foreach (const QString &sketch, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name))
    {
        static const QString format1 = "%0/%0.pde";
        static const QString format2 = "%0/%0.ino";
        if (dir.exists(format1.arg(sketch)))
            sketches.append(sketch);
        else if (dir.exists(format2.arg(sketch)))
            sketches.append(sketch);
    }
    return sketches;
}

QStringList Toolkit::findExampleCategories()
{
    QDir exampleDir(ideApp->settings()->arduinoPath() + "/examples");
    return exampleDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
}

QStringList Toolkit::findExamples(const QString &category)
{
    QString categoryDir(ideApp->settings()->arduinoPath() + "/examples/" + category);
    return findSketchesInDirectory(categoryDir);
}

QString Toolkit::exampleFileName(const QString &category, const QString &example)
{
    QList<QString> extensions;
    static const QString format = "%0/examples/%1/%2/%2.%3";

    extensions << "ino" << "pde";

    foreach(const QString ext, extensions)
    {
        QString path = format.arg(ideApp->settings()->arduinoPath()).arg(category).arg(example).arg(ext);
        if (QFileInfo(path).exists())
            return path;
    }
    return QString();
}

QString Toolkit::referencePath()
{
    return QDir(ideApp->settings()->arduinoPath()).filePath("reference");
}

QString Toolkit::hardwarePath()
{
    return QDir(ideApp->settings()->arduinoPath()).filePath("hardware");
}

QStringList Toolkit::boardsFileNames()
{
    QStringList userHwList;
    if(toolkitVersionInt(ideApp->settings()->arduinoPath()) >= 160)
        userHwList << QDir(hardwarePath()).filePath("arduino/avr/boards.txt");
    else
        userHwList << QDir(hardwarePath()).filePath("arduino/boards.txt");

    QDir sketchDir = QDir(ideApp->settings()->sketchPath());
    if (sketchDir.cd("hardware"))
    {
        sketchDir.setFilter(QDir::AllDirs);
        QStringList hwList = sketchDir.entryList();
        foreach(QString dir, hwList)
        {
            if (QDir(sketchDir.filePath(dir)).exists("boards.txt"))
                userHwList.push_back(sketchDir.filePath(dir + "/boards.txt"));
        }
    }
    qDebug() << userHwList;
    return userHwList;
}

QString Toolkit::keywordsFileName()
{
    return QDir(ideApp->settings()->arduinoPath()).filePath("lib/keywords.txt");
}

QString Toolkit::toolkitVersion(const QString &path)
{
    if(QFileInfo(QDir(path).filePath("hardware/arduino/boards.txt")).isReadable() || (QFileInfo(QDir(path).filePath("hardware/arduino/avr/boards.txt")).isReadable() && QFileInfo(QDir(path).filePath("hardware/arduino/sam/boards.txt")).isReadable()))
    {
        QFile file(QDir(path).filePath("revisions.txt"));
        if(!file.open(QFile::ReadOnly))
            return QString();

        QByteArray arduinoVersion = file.readLine();
        while (arduinoVersion == "\n" && ! file.atEnd()) { arduinoVersion = file.readLine();}
        QList<QByteArray> list = arduinoVersion.split(' ');
        if (list.size() >= 2)
            return  list.at(1).trimmed();
    }

    return QString();
}

int Toolkit::toolkitVersionInt(const QString &path)
{
    return Toolkit::toolkitVersion(path).remove('.').toInt();
}

bool Toolkit::isValidArduinoPath(const QString &path)
{
    QString version = toolkitVersion(path);

    return version == "1.6.0" || version == "1.0.5" || version == "1.0.4" || version == "1.0.3" || version == "1.0.2" || version == "1.0.1" || version == "1.0" || version == "0023";
}

QString Toolkit::avrPath()
{
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64) || defined(Q_OS_DARWIN)
    if(toolkitVersionInt(ideApp->settings()->arduinoPath()) >= 160)
        return QDir(hardwarePath()).filePath("/hardware/tools/avr/bin");
    else if (toolkitVersionInt(ideApp->settings()->arduinoPath()) >= 100)
        return QDir(hardwarePath()).filePath("tools/avr/bin");
#else
    if(toolkitVersionInt(ideApp->settings()->arduinoPath()) >= 160)
        return QDir(hardwarePath()).filePath("/hardware/tools/avr/bin");
    else if (toolkitVersionInt(ideApp->settings()->arduinoPath()) >= 100)
        return QDir(hardwarePath()).filePath("tools/avr/bin");
    else {
        // the AVR toolchain should be already present in the PATH
        return QString();
    }
#endif
}
QString Toolkit::avrTool(Toolkit::AVRTool tool)
{
    QString path = avrPath();
    QString toolName;
    switch (tool)
    {
    case AvrGcc:
        toolName = "avr-gcc";
        break;
    case AvrGxx:
        toolName = "avr-g++";
        break;
    case AvrAr:
        toolName = "avr-ar";
        break;
    case AvrObjcopy:
        toolName = "avr-objcopy";
        break;
    case AvrSize:
        toolName = "avr-size";
        break;
    default:
        return QString();
    }

    QString toolPath = QDir(path).filePath(toolName);
    if (QFile::exists(toolPath))
	return toolPath;
    else
        return toolName;
}

QStringList Toolkit::avrCFlags(const Board *board)
{
    qDebug() << board->attribute("build.mcu");
    QString buildMCU="build.mcu";
    QString typeMCU="";
    if(board->mAttributes.contains("menu.cpu.atmega2560"))
        typeMCU="menu.cpu.atmega2560";
    
    qDebug() << buildMCU << typeMCU << board->attribute(typeMCU+"."+buildMCU);
    QStringList cflags;
    cflags
        << "-g"
        << "-Os"
        << "-Wall"
        << "-fno-exceptions"
        << "-ffunction-sections"
        << "-fdata-sections"
        << QString("-mmcu=%0").arg(board->attribute(typeMCU+"."+buildMCU))
        << QString("-DF_CPU=%0").arg(board->attribute("build.f_cpu"))
        << QString("-MMD")
        << QString("-DARDUINO=%0").arg(toolkitVersionInt(ideApp->settings()->arduinoPath()));
	
    qDebug() << cflags;

    if (!board->attribute("build.vid").isEmpty())
        cflags << QString("-DUSB_VID=%0").arg(board->attribute("build.vid"));
    else
        cflags << QString("-DUSB_VID=null");

    if (!board->attribute("build.pid").isEmpty())
        cflags << QString("-DUSB_PID=%0").arg(board->attribute("build.pid"));
    else
        cflags << QString("-DUSB_PID=null");
    
    QString arduinoPinDirName;
    if(toolkitVersionInt(ideApp->settings()->arduinoPath()) >= 160)
        arduinoPinDirName = QString("arduino/avr/variants/%0").arg(board->attribute("build.variant"));
    else
        arduinoPinDirName = QString("arduino/variants/%0").arg(board->attribute("build.variant"));
    
    QString arduinoPinDirPath = QDir(hardwarePath()).filePath(arduinoPinDirName);
    if (QDir(arduinoPinDirPath).exists())
        cflags << QString("-I%0").arg(arduinoPinDirPath);

    return cflags;
}

QStringList Toolkit::avrCxxFlags(const Board *board)
{
    return avrCFlags(board);
}

QStringList Toolkit::avrSFlags(const Board *board)
{
    QStringList sflags;
    sflags
        << "-g"
        << "-assembler-with-cpp"
        << QString("-mmcu=%0").arg(board->attribute("build.mcu"))
        << QString("-MMD")
        << QString("-DF_CPU=%0").arg(board->attribute("build.f_cpu"))
        << QString("-DARDUINO=%0").arg(toolkitVersionInt(ideApp->settings()->arduinoPath()));
    return sflags;
}

QStringList Toolkit::avrLdFlags(const Board *board)
{
    QStringList ldflags;
    ldflags
        << "-Os"
        << "-Wl,--gc-sections"
        << QString("-mmcu=%0").arg(board->attribute("build.mcu"));
    return ldflags;
}

QStringList Toolkit::avrSizeFlags(const Board *board)
{
    QStringList sizeflags;
    sizeflags
        << "-t";
    return sizeflags;
}

QString Toolkit::corePath(const Board *board)
{
    return QDir(board->hardwarePath()).filePath(QString("cores/%0").arg(board->attribute("build.core")));
}

QStringList Toolkit::IDELibraries()
{
    return QDir(IDELibraryPath()).entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
}

QStringList Toolkit::arduinoLibraries()
{
    return QDir(libraryPath()).entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
}

QStringList Toolkit::userLibraries()
{
    return QDir(userLibraryPath()).entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
}

QStringList Toolkit::libraries()
{
    return userLibraries() + IDELibraries() + arduinoLibraries();
}

QString Toolkit::IDELibraryPath()
{
    return QDir(ideApp->dataPath()).filePath("libraries");
}

QString Toolkit::userLibraryPath()
{
    return QDir(ideApp->settings()->sketchPath()).filePath("libraries");
}

QString Toolkit::libraryPath(const QString &libraryName)
{
    if (libraryName.isNull())
    {
        return QDir(ideApp->settings()->arduinoPath()).filePath(QString("libraries"));
    }
    else
    {
        QStringList searchPaths = QStringList() << userLibraryPath() << IDELibraryPath() << libraryPath();
        static QString format("%0/%1");
        QString path;
        foreach (const QString &searchPath, searchPaths)
        {
            path = format.arg(searchPath, libraryName);
            if (QDir(path).exists())
                return path;
        }
        return QString();
    }
}

QString Toolkit::libraryKeywordsFileName(const QString &libraryName)
{
    return QDir(libraryPath(libraryName)).filePath("keywords.txt");
}

QStringList Toolkit::librariesWithExamples()
{
    QStringList libs;
    foreach (const QString &library, libraries())
    {
        QString examplesPath = QDir(libraryPath(library)).filePath("examples");
        if (QFileInfo(examplesPath).isDir())
            libs << library;
    }
    return libs;
}

QStringList Toolkit::findLibraryExamples(const QString &library)
{
    return findSketchesInDirectory(QDir(libraryPath(library)).filePath("examples"));
}

QString Toolkit::libraryExampleFileName(const QString &library, const QString &example)
{
    QList<QString> extensions;
    static const QString format = "%0/examples/%1/%1.%2";

    extensions << "ino" << "pde";

    foreach(const QString ext, extensions)
    {
        QString path = format.arg(libraryPath(library)).arg(example).arg(ext);
        if (QFileInfo(path).exists())
            return path;
    }
    return QString();
}

QString Toolkit::avrdudePath()
{
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64) || defined(Q_OS_DARWIN)
    return QDir(hardwarePath()).filePath("tools/avr/bin/avrdude");
#else
    if(avrdudeSystem())
      return(QString("avrdude"));
    else
      return QDir(hardwarePath()).filePath("tools/avrdude");
#endif
}

QStringList Toolkit::avrdudeFlags(const Board *board)
{
    QStringList flags;
    if (ideApp->settings()->verboseUpload())
        flags << "-v" << "-v" << "-v" << "-v";
    else
        flags << "-q" << "-q";

    flags << "-C";
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64) || defined(Q_OS_DARWIN)
    flags << QDir(hardwarePath()).filePath("tools/avr/etc/avrdude.conf");
#else
    if(avrdudeSystem())
        flags << "/etc/avrdude.conf";
    else
        flags << QDir(hardwarePath()).filePath("tools/avrdude.conf");
#endif
    flags << QString("-p%0").arg(board->attribute("build.mcu"));

    return flags;
}

bool Toolkit::avrdudeSystem()
{
  QProcess proc;

  proc.start(QString("avrdude"));
  if(proc.waitForStarted())
    return true;

  return false;
}
