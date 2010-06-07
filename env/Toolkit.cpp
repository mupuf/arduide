/**
 * \file Toolkit.cpp
 * \author Denis Martinez
 */

#include "Toolkit.h"

#include <QStringList>
#include <QDir>
#include <QFile>
#include <QDebug>

#include "../env/Board.h"
#include "IDEApplication.h"

QStringList Toolkit::findSketchesInDirectory(const QString &directory)
{
    QStringList sketches;
    QDir dir(directory);
    foreach (const QString &sketch, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name))
    {
        static const QString format = "%0/%0.pde";
        if (dir.exists(format.arg(sketch)))
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
    static const QString format = "%0/examples/%1/%2/%2.pde";
    return format.arg(ideApp->settings()->arduinoPath()).arg(category).arg(example);
}

QString Toolkit::referencePath()
{
    return QDir(ideApp->settings()->arduinoPath()).filePath("reference");
}

QString Toolkit::hardwarePath()
{
    return QDir(ideApp->settings()->arduinoPath()).filePath("hardware");
}

QString Toolkit::boardsFileName()
{
    return QDir(hardwarePath()).filePath("arduino/boards.txt");
}

QString Toolkit::keywordsFileName()
{
    return QDir(ideApp->settings()->arduinoPath()).filePath("lib/keywords.txt");
}

bool Toolkit::isValidArduinoPath(const QString &path)
{
    return QFileInfo(QDir(path).filePath("hardware/arduino/boards.txt")).isReadable();
}

QString Toolkit::avrPath()
{
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64) || defined(Q_OS_DARWIN)
    return QDir(hardwarePath()).filePath("tools/avr/bin");
#else
    // the AVR toolchain should be already present in the PATH
    return QString();
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
    default:
        return QString();
    }

    if (path.isNull())
        return toolName;
    else
        return QDir(path).filePath(toolName);
}

QStringList Toolkit::avrCFlags(const Board *board)
{
    QStringList cflags;
    cflags
        << "-g"
        << "-Os"
        << "-w"
        << "-fno-exceptions"
        << "-ffunction-sections"
        << "-fdata-sections"
        << QString("-mmcu=%0").arg(board->attribute("build.mcu"))
        << QString("-DF_CPU=%0").arg(board->attribute("build.f_cpu"))
        << QString("-DARDUINO=%0").arg(ARDUINO_REVISION);
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
        << QString("-DF_CPU=%0").arg(board->attribute("build.f_cpu"))
        << QString("-DARDUINO=%0").arg(ARDUINO_REVISION);
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

QString Toolkit::corePath(const Board *board)
{
    return QDir(hardwarePath()).filePath(QString("arduino/cores/%0").arg(board->attribute("build.core")));
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
    static const QString format = "%0/examples/%1/%1.pde";
    return format.arg(libraryPath(library)).arg(example);
}

QString Toolkit::avrdudePath()
{
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64) || defined(Q_OS_DARWIN)
    return QDir(hardwarePath()).filePath("tools/avr/bin/avrdude");
#else
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
    flags
        << "-C"
#if defined(Q_OS_WIN32) || defined(Q_OS_WIN64) || defined(Q_OS_DARWIN)
        << QDir(hardwarePath()).filePath("tools/avr/etc/avrdude.conf")
#else
        << QDir(hardwarePath()).filePath("tools/avrdude.conf")
#endif
        << QString("-p%0").arg(board->attribute("build.mcu"));
    return flags;
}

