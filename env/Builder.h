/**
 * \file Builder.h
 * \author Denis Martinez
 */

#ifndef BUILDER_H
#define BUILDER_H

#include <QObject>

#include "../env/ILogger.h"
#include "../utils/TemporaryDirectory.h"

class Board;

class Builder : public QObject
{
    Q_OBJECT
public:
    Builder(ILogger &logger = NullLogger::instance(), QObject *parent = NULL);
    const Board *board() const { return mBoard; }
    void setBoard(const Board *board) { mBoard = board; }
    const QString &device() { return mDevice; }
    void setDevice(const QString &device) { mDevice = device; }
    bool build(const QString &code, bool upload = false);

private:
    QStringList compile(const QStringList &sources, const QStringList &includePaths, const QStringList &cflags, const QStringList &cxxflags, const QStringList &sflags, const QString &outputDirectory = QString());
    enum SourceType
    {
        CSource,
        CxxSource,
        SSource,
        UnknownSource
    };
    bool archive(const QString &fileName, const QStringList &objects);
    bool link(const QString &fileName, const QStringList &objects, const QStringList &ldflags);
    bool extractEEPROM(const QString &input, const QString &output);
    bool extractHEX(const QString &input, const QString &output);
    bool uploadViaBootloader(const QString &hexFileName);
    SourceType identifySource(const QString &fileName);
    int runCommand(const QStringList &command);

    ILogger &mLogger;
    TemporaryDirectory mBuildDir;
    const Board *mBoard;
    QString mDevice;
};

#endif // BUILDER_H
