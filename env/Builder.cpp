/**
 * \file Builder.cpp
 * \author Denis Martinez
 */

#include "Builder.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegExp>
#include <QProcess>
#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <QDebug>

#include <qxtsignalwaiter.h>

#include "../env/Board.h"
#include "../env/Toolkit.h"

#include "../utils/Serial.h"

Builder::Builder(ILogger &logger, QObject *parent)
    : QObject(parent),
      mLogger(logger),
      mBoard(NULL)
{
}

QString Builder::readAllFile(const QString& filepath)
{
    QFile file(filepath);

    if(!file.open(QIODevice::ReadOnly))
        mLogger.logError(tr("Cannot read file '%1'.").arg(file.fileName()));

    return file.readAll();
}

QStringList Builder::compileDependencies(const QString& code, QStringList& includePaths, QString buildPath, const QStringList& cflags, const QStringList& cxxflags, const QStringList& sflags)
{
    QStringList objects;

    QRegExp importRegexp("^\\s*#include\\s+[<\"](\\S+)[\">]");
    foreach (const QString &line, code.split('\n'))
    {
        if (importRegexp.indexIn(line) != -1)
        {
            QString library = importRegexp.cap(1);
            library = QFileInfo(library).baseName();
            QString libPath = Toolkit::libraryPath(library);
            QString utilityPath = QDir(libPath).filePath("utility");
            bool libPathExists = QFileInfo(libPath).exists();
            bool utilityPathExists = QFileInfo(utilityPath).exists();
            QStringList libSources, utilitySources;

            if (libPathExists && !includePaths.contains(libPath))
            {
                // Add the path of the library we are compiling to the global includePaths
                includePaths << libPath;

                QString outputDirectory = QDir(buildPath).filePath(library);
                if (! QDir().mkdir(outputDirectory))
                {
                    mLogger.logError(tr("Failed to create build directory."));
                    return QStringList();
                }
                foreach (const QString &source, QDir(libPath).entryList(QStringList() << "*.S" << "*.c" << "*.cpp", QDir::Files))
                {
                    QString path=QDir(libPath).filePath(source);
                    libSources << path;
                    objects << compileDependencies(readAllFile(path), includePaths, buildPath, cflags, cxxflags, sflags);
                }
                objects << compile(libSources, includePaths, cflags, cxxflags, sflags, outputDirectory);
            }

            if (utilityPathExists && !includePaths.contains(libPath))
            {
                // Add the path of the utility we are compiling to the global includePaths
                includePaths << utilityPath;

                QString outputDirectory = QDir(buildPath).filePath(QString("%0/utility").arg(library));
                if (! QDir().mkdir(outputDirectory))
                {
                    mLogger.logError(tr("Failed to create build directory."));
                    return QStringList();
                }
                foreach (const QString &source, QDir(utilityPath).entryList(QStringList() << "*.S" << "*.c" << "*.cpp", QDir::Files))
                {
                    QString path=QDir(utilityPath).filePath(source);
                    utilitySources << path;
                    objects << compileDependencies(readAllFile(path), includePaths, buildPath, cflags, cxxflags, sflags);
                }
                objects << compile(utilitySources, includePaths, cflags, cxxflags, sflags, outputDirectory);
            }
        }
    }

    return objects;
}

bool Builder::build(const QString &code, bool upload)
{
    if (mBoard == NULL)
    {
        mLogger.logError(tr("No board selected."));
        return false;
    }

    if (upload && mDevice.isEmpty())
    {
        mLogger.logError(tr("No device selected."));
        return false;
    }

    mLogger.logImportant(tr("Compiling for %0...").arg(mBoard->name()));
    mBuildDir.reset(new QxtTemporaryDir(QDir(QDir::tempPath()).filePath("arduino-build")));
    QString buildPath = mBuildDir->path();

    QStringList cflags = Toolkit::avrCFlags(mBoard);
    QStringList cxxflags = Toolkit::avrCxxFlags(mBoard);
    QStringList sflags = Toolkit::avrSFlags(mBoard);
    QStringList ldflags = Toolkit::avrLdFlags(mBoard);
    QStringList includePaths;

    // compile the core
    QStringList objects;
    QString corePath = Toolkit::corePath(mBoard);
    includePaths << corePath;
    QDir coreDir(corePath);
    QStringList coreSources;
    foreach (const QString &source, coreDir.entryList(QStringList() << "*.S" << "*.c" << "*.cpp", QDir::Files))
        coreSources.append(coreDir.filePath(source));

    objects = compile(coreSources, includePaths, cflags, cxxflags, sflags);
    if (objects.isEmpty())
    {
        mLogger.logError(tr("Compilation failed."));
        return false;
    }

    QString coreFileName = QDir(buildPath).filePath("core.a");
    if (! archive(coreFileName, objects))
    {
        mLogger.logError(tr("Archiving failed."));
        return false;
    }
    objects.clear();

    // compile the libraries
    cxxflags << "-include" << "WProgram.h";
    objects << compileDependencies(code, includePaths, buildPath, cflags, cxxflags, sflags);
    if (objects.isEmpty())
    {
        mLogger.logError(tr("Compilation failed."));
        return false;
    }

    // compile the sketch
    QString sketchFileName = QDir(buildPath).filePath("sketch.cpp");
    QFile sketchFile(sketchFileName);
    if (! sketchFile.open(QIODevice::WriteOnly))
    {
        mLogger.logError(tr("Can't write the sketch to disk."));
        return false;
    }
    sketchFile.write(code.toLocal8Bit());

    // if main.cxx exists, append it to the source
    QString sketchMainFileName = QDir(corePath).filePath("main.cxx");
    if (QFileInfo(sketchMainFileName).exists())
    {
        QFile sketchMain(sketchMainFileName);
        if (! sketchMain.open(QIODevice::ReadOnly))
        {
            mLogger.logError(tr("Can't open main.cxx."));
            sketchFile.close();
            return false;
        }
        sketchFile.write(sketchMain.readAll());
        sketchMain.close();
    }

    sketchFile.close();

    objects << compile(QStringList() << sketchFileName, includePaths, cflags, cxxflags, sflags);
    if (objects.isEmpty())
    {
        mLogger.logError(tr("Compilation failed."));
        return false;
    }

    // link it all together into the .elf file
    mLogger.logImportant(tr("Linking..."));
    QString elfFileName = QDir(buildPath).filePath("sketch.elf");
    if (! link(elfFileName, QStringList() << objects << coreFileName, ldflags))
    {
        mLogger.logError(tr("Link failed."));
        return false;
    }

    // extract EEPROM
    QString eepFileName = QDir(buildPath).filePath("sketch.eep");
    if (! extractEEPROM(elfFileName, eepFileName))
    {
        mLogger.logError(tr("Failed to extract EEPROM."));
        return false;
    }

    // extract HEX
    QString hexFileName = QDir(buildPath).filePath("sketch.hex");
    if (! extractHEX(elfFileName, hexFileName))
    {
        mLogger.logError(tr("Failed to extract HEX."));
        return false;
    }

    if (! upload)
    {
        mLogger.logImportant(tr("Success."));
        return true;
    }

    // upload
    mLogger.logImportant(tr("Uploading to %0...").arg(mDevice));
    if (! uploadViaBootloader(hexFileName))
    {
        mLogger.logError(tr("Uploading failed."));
        return false;
    }
    else
        mLogger.logImportant(tr("Success."));

    return true;
}

QStringList Builder::compile(const QStringList &sources, const QStringList &includePaths, const QStringList &cflags, const QStringList &cxxflags, const QStringList &sflags, const QString &outputDirectory)
{
    Q_ASSERT(mBoard != NULL);

    QStringList objects;

    QStringList includeFlags;
    foreach (const QString &path, includePaths)
        includeFlags << QString("-I%0").arg(path);

    foreach (const QString &source, sources)
    {
        QStringList cmdline;
        SourceType sourceType = identifySource(source);
        QString objectFileName = QFileInfo(source).fileName() + ".o";
        objectFileName = QDir(outputDirectory.isNull() ? mBuildDir->path() : outputDirectory).filePath(objectFileName);
        switch (sourceType)
        {
        case CSource:
            cmdline
                << Toolkit::avrTool(Toolkit::AvrGcc) << "-c"
                << cflags << includeFlags
                << "-o" << objectFileName << source;
            break;
        case CxxSource:
            cmdline
                << Toolkit::avrTool(Toolkit::AvrGxx) << "-c"
                << cxxflags << includeFlags
                << "-o" << objectFileName << source;
            break;
        case SSource:
            cmdline
                << Toolkit::avrTool(Toolkit::AvrGcc) << "-c"
                << sflags << includeFlags
                << "-o" << objectFileName << source;
            break;
        default:
            mLogger.logError(tr("Unknown source type: %0").arg(QFileInfo(source).fileName()));
            continue;
        }

        if (runCommand(cmdline, true) != 0)
            return QStringList();
        else
            objects << objectFileName;
    }

    return objects;
}

bool Builder::archive(const QString &fileName, const QStringList &objects)
{
    QStringList command;
    command << Toolkit::avrTool(Toolkit::AvrAr) << "rcs" << fileName << objects;
    return runCommand(command) == 0;
}

Builder::SourceType Builder::identifySource(const QString &fileName)
{
    if (fileName.endsWith(".c"))
        return CSource;
    else if (fileName.endsWith(".cpp") || fileName.endsWith(".cxx") || fileName.endsWith(".cc"))
        return CxxSource;
    else if (fileName.endsWith(".S"))
        return SSource;
    else
        return UnknownSource;
}

bool Builder::link(const QString &fileName, const QStringList &objects, const QStringList &ldflags)
{
    QStringList command;
    command
        << Toolkit::avrTool(Toolkit::AvrGcc)
        << ldflags
        << "-o" << fileName
        << objects
        << QString("-L%0").arg(mBuildDir->path())
        << "-lm";
    return runCommand(command) == 0;
}

int Builder::runCommand(const QStringList &command, bool errorHighlighting)
{
    mLogger.logCommand(command);

    QStringList arguments = command;
    QString program = arguments.takeFirst();

    QProcess proc;
    proc.setProcessChannelMode(QProcess::MergedChannels);
    proc.start(program, arguments);
    proc.waitForStarted();
    proc.waitForFinished();

    if(proc.error()==QProcess::FailedToStart)
    {
       mLogger.logError(QString("Cannot start program %1").arg(program));
       return -1;
    }
    else
    {
        if(!errorHighlighting)
            mLogger.log(QString::fromLocal8Bit(proc.readAllStandardOutput()));
        else
        {
            foreach (QString line, QString::fromLocal8Bit(proc.readAllStandardOutput()).split('\n'))
            {
                if (line.contains(QRegExp(".*\\.cpp:\\d+: error:")))
                    mLogger.logError(line);
                else if (line.contains(QRegExp(".*\\.cpp:\\d+: (warning|note):")))
                    mLogger.logImportant(line);
                else
                    mLogger.log(line);
            }
        }

        QFutureWatcher<int> watcher;
        QxtSignalWaiter waiter(&watcher, SIGNAL(finished()));
        QFuture<int> futureExitCode = QtConcurrent::run(&proc, &QProcess::exitCode);
        watcher.setFuture(futureExitCode);
        waiter.wait();

        return futureExitCode.result();
    }
}

bool Builder::extractEEPROM(const QString &input, const QString &output)
{
    QStringList command;
    command
        << Toolkit::avrTool(Toolkit::AvrObjcopy)
        << "-O" << "ihex"
        << "-j" << ".eeprom"
        << "--set-section-flags=.eeprom=alloc,load"
        <<"--no-change-warnings"
        << "--change-section-lma"
        << ".eeprom=0"
        << input << output;
    return runCommand(command) == 0;
}

bool Builder::extractHEX(const QString &input, const QString &output)
{
    QStringList command;
    command
        << Toolkit::avrTool(Toolkit::AvrObjcopy)
        << "-O" << "ihex"
        << "-R" << ".eeprom"
        << input << output;
    return runCommand(command) == 0;
}

bool Builder::uploadViaBootloader(const QString &hexFileName)
{
    QString protocol = mBoard->attribute("upload.protocol");
    if (protocol == "stk500")
        protocol = "stk500v1";

    QStringList command;
    command
        << Toolkit::avrdudePath()
        << Toolkit::avrdudeFlags(mBoard)
        << "-c" << protocol
        << "-P" << mDevice
        << "-b" << mBoard->attribute("upload.speed")
        << "-D"
        << QString("-Uflash:w:%0:i").arg(hexFileName);

    QString disableFlushing = mBoard->attribute("upload.disable_flushing");
    if (disableFlushing.isNull() || disableFlushing.toLower() == "false")
    {
        Serial ser(mDevice);
        ser.open(QIODevice::ReadWrite);
        ser.flushBuffer();
        ser.close();
    }

    return runCommand(command) == 0;
}
