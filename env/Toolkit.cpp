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

    return userHwList;
}

QString Toolkit::keywordsFileName()
{
    return QDir(ideApp->settings()->arduinoPath()).filePath("lib/keywords.txt");
}

QString Toolkit::toolkitVersion(const QString &path)
{
    if(QFileInfo(QDir(path).filePath("hardware/arduino/boards.txt")).isReadable())
    {
        QFile file(QDir(path).filePath("revisions.txt"));
        if(!file.open(QFile::ReadOnly))
            return QString();

        QByteArray arduinoVersion = file.readLine();
        QList<QByteArray> list = arduinoVersion.split(' ');
        if (list.size() >= 2)
            return  list.at(1).trimmed();
    }

    return QString();
}

bool Toolkit::isValidArduinoPath(const QString &path)
{
    QString version = toolkitVersion(path);

    return version == "1.0.1" || version == "1.0" || version == "0023";
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
    case AvrSize:
        toolName = "avr-size";
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
        << QString("-DARDUINO=%0").arg(ARDUINO_SDK_VERSION);

    if (!board->attribute("build.vid").isEmpty())
        cflags << QString("-DUSB_VID=%0").arg(board->attribute("build.vid"));

    if (!board->attribute("build.pid").isEmpty())
        cflags << QString("-DUSB_PID=%0").arg(board->attribute("build.pid"));

    QString arduinoPinDirName = QString("arduino/variants/%0").arg(board->attribute("build.variant"));
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
        << QString("-DF_CPU=%0").arg(board->attribute("build.f_cpu"))
        << QString("-DARDUINO=%0").arg(ARDUINO_SDK_VERSION);
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

