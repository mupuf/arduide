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
#include <QDebug>

#include "IDEApplication.h"

#include "../env/Board.h"
#include "../env/Toolkit.h"

#include "../utils/Serial.h"

Builder::Builder(QObject *parent)
    : QObject(parent)
{
}

const Board *Builder::board() const
{
    return Board::boardInfo(ideApp->settings()->board());
}

const QString Builder::device() const
{
    return ideApp->settings()->devicePort();
}

QString Builder::readAllFile(const QString& filepath)
{
    QFile file(filepath);

    if(!file.open(QIODevice::ReadOnly))
        emit logError(tr("Cannot read file '%1'.").arg(file.fileName()));

    return file.readAll();
}

bool Builder::compileDependencies(QStringList &objects, const QString& code, QStringList& includePaths, QString buildPath, const QStringList& cflags, const QStringList& cxxflags, const QStringList& sflags)
{
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
                    emit logError(tr("Failed to create build directory."));
                    return false;
                }
                foreach (const QString &source, QDir(libPath).entryList(QStringList() << "*.S" << "*.c" << "*.cpp", QDir::Files))
                {
                    QString path=QDir(libPath).filePath(source);
                    libSources << path;
                    if (! compileDependencies(objects, readAllFile(path), includePaths, buildPath, cflags, cxxflags, sflags))
                        return false;
                }
                if (! compile(objects, libSources, includePaths, cflags, cxxflags, sflags, outputDirectory))
                    return false;
            }

            if (utilityPathExists && !includePaths.contains(libPath))
            {
                // Add the path of the utility we are compiling to the global includePaths
                includePaths << utilityPath;

                QString outputDirectory = QDir(buildPath).filePath(QString("%0/utility").arg(library));
                if (! QDir().mkdir(outputDirectory))
                {
                    emit logError(tr("Failed to create build directory."));
                    return false;
                }
                foreach (const QString &source, QDir(utilityPath).entryList(QStringList() << "*.S" << "*.c" << "*.cpp", QDir::Files))
                {
                    QString path=QDir(utilityPath).filePath(source);
                    utilitySources << path;
                    if (! compileDependencies(objects, readAllFile(path), includePaths, buildPath, cflags, cxxflags, sflags))
                        return false;
                }
                if (! compile(objects, utilitySources, includePaths, cflags, cxxflags, sflags, outputDirectory))
                    return false;
            }
        }
    }

    return true;
}

bool Builder::build(const QString &code, bool upload)
{
    if (board() == NULL)
    {
        emit logError(tr("No board selected."));
        return false;
    }

    if (upload && device().isEmpty())
    {
        emit logError(tr("No device selected."));
        return false;
    }

    emit logImportant(tr("Compiling for %0...").arg(board()->name()));
    mBuildDir.reset(new QxtTemporaryDir(QDir(QDir::tempPath()).filePath("arduino-build")));
    QString buildPath = mBuildDir->path();

    QStringList cflags = Toolkit::avrCFlags(board());
    QStringList cxxflags = Toolkit::avrCxxFlags(board());
    QStringList sflags = Toolkit::avrSFlags(board());
    QStringList ldflags = Toolkit::avrLdFlags(board());
    QStringList includePaths;

    // compile the core
    QStringList objects;
    QString corePath = Toolkit::corePath(board());
    includePaths << corePath;
    QDir coreDir(corePath);
    QStringList coreSources;
    foreach (const QString &source, coreDir.entryList(QStringList() << "*.S" << "*.c" << "*.cpp", QDir::Files))
        coreSources.append(coreDir.filePath(source));

    bool success;
    success = compile(objects, coreSources, includePaths, cflags, cxxflags, sflags);
    if (! success)
    {
        emit logError(tr("Compilation failed."));
        return false;
    }

    QString coreFileName = QDir(buildPath).filePath("core.a");
    if (! archive(coreFileName, objects))
    {
        emit logError(tr("Archiving failed."));
        return false;
    }
    objects.clear();

    // compile the libraries
    cxxflags << "-include" << "WProgram.h";
    success = compileDependencies(objects, code, includePaths, buildPath, cflags, cxxflags, sflags);
    if (! success)
    {
        emit logError(tr("Compilation failed."));
        return false;
    }

    // compile the sketch
    QString sketchFileName = QDir(buildPath).filePath("sketch.cpp");
    QFile sketchFile(sketchFileName);
    if (! sketchFile.open(QIODevice::WriteOnly))
    {
        emit logError(tr("Can't write the sketch to disk."));
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
            emit logError(tr("Can't open main.cxx."));
            sketchFile.close();
            return false;
        }
        sketchFile.write(sketchMain.readAll());
        sketchMain.close();
    }

    sketchFile.close();

    compile(objects, QStringList() << sketchFileName, includePaths, cflags, cxxflags, sflags);
    if (objects.isEmpty())
    {
        emit logError(tr("Compilation failed."));
        return false;
    }

    // link it all together into the .elf file
    emit logImportant(tr("Linking..."));
    QString elfFileName = QDir(buildPath).filePath("sketch.elf");
    if (! link(elfFileName, QStringList() << objects << coreFileName, ldflags))
    {
        emit logError(tr("Link failed."));
        return false;
    }

    // extract EEPROM
    QString eepFileName = QDir(buildPath).filePath("sketch.eep");
    if (! extractEEPROM(elfFileName, eepFileName))
    {
        emit logError(tr("Failed to extract EEPROM."));
        return false;
    }

    // extract HEX
    QString hexFileName = QDir(buildPath).filePath("sketch.hex");
    if (! extractHEX(elfFileName, hexFileName))
    {
        emit logError(tr("Failed to extract HEX."));
        return false;
    }

    if (! upload)
    {
        emit logImportant(tr("Success."));
        return true;
    }

    // upload
    emit logImportant(tr("Uploading to %0...").arg(device()));
    if (! uploadViaBootloader(hexFileName))
    {
        emit logError(tr("Uploading failed."));
        return false;
    }
    else
        emit logImportant(tr("Success."));

    return true;
}

bool Builder::compile(QStringList &objects, const QStringList &sources, const QStringList &includePaths, const QStringList &cflags, const QStringList &cxxflags, const QStringList &sflags, const QString &outputDirectory)
{
    Q_ASSERT(board() != NULL);

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
            emit logError(tr("Unknown source type: %0").arg(QFileInfo(source).fileName()));
            continue;
        }

        if (runCommand(cmdline, true) != 0)
            return false;
        else
            objects << objectFileName;
    }

    return true;
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
    emit logCommand(command);

    QStringList arguments = command;
    QString program = arguments.takeFirst();

    QProcess proc;
    proc.setProcessChannelMode(QProcess::MergedChannels);
    proc.start(program, arguments);
    proc.waitForStarted();
    QProcess::ProcessError error = proc.error();

    if (error == QProcess::FailedToStart || error == QProcess::Crashed)
    {
       emit logError(QString("Cannot start program %1").arg(program));
       return -1;
    }
    else
    {
        proc.waitForFinished();

        error = proc.error();
        if (error == QProcess::Crashed)
            return -1;

        if (! errorHighlighting)
            emit log(QString::fromLocal8Bit(proc.readAllStandardOutput()));
        else
        {
            foreach (QString line, QString::fromLocal8Bit(proc.readAllStandardOutput()).split('\n'))
            {
                if (line.contains(QRegExp(".*\\.cpp:\\d+: error:")))
                    emit logError(line);
                else if (line.contains(QRegExp(".*\\.cpp:\\d+: (warning|note):")))
                    emit logImportant(line);
                else
                    emit log(line);
            }
        }

        return proc.exitCode();
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
    QString protocol = board()->attribute("upload.protocol");
    if (protocol == "stk500")
        protocol = "stk500v1";

    QStringList command;
    command
        << Toolkit::avrdudePath()
        << Toolkit::avrdudeFlags(board())
        << "-c" << protocol
        << "-P" << device()
        << "-b" << board()->attribute("upload.speed")
        << "-D"
        << QString("-Uflash:w:%0:i").arg(hexFileName);

    QString disableFlushing = board()->attribute("upload.disable_flushing");
    if (disableFlushing.isNull() || disableFlushing.toLower() == "false")
    {
        Serial ser(device());
        ser.open(QIODevice::ReadWrite);
        ser.flushBuffer();
        ser.close();
    }

    return runCommand(command) == 0;
}

BackgroundBuilder::BackgroundBuilder(QObject *parent) : builder(parent),
                                                        actions(NULL)
{
    connect(&builder, SIGNAL(log(QString)), this, SIGNAL(log(QString)));
    connect(&builder, SIGNAL(logCommand(QStringList)), this, SIGNAL(logCommand(QStringList)));
    connect(&builder, SIGNAL(logError(QString)), this, SIGNAL(logError(QString)));
    connect(&builder, SIGNAL(logImportant(QString)), this, SIGNAL(logImportant(QString)));
}

void BackgroundBuilder::setRelatedActions(QActionGroup *actions)
{
    this->actions = actions;
}

void BackgroundBuilder::backgroundBuild(const QString &code, bool upload)
{
    if (isRunning())
    {
        qWarning() << "BackgroundBuilder thread already running.";
        emit buildFinished(false);
        return;
    }

    this->code = code;
    this->upload = upload;

    start();
}

void BackgroundBuilder::run()
{
    actions->setDisabled(true);
    bool res=builder.build(code, upload);
    actions->setDisabled(false);

    emit buildFinished(res);
}
