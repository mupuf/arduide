/**
 * \file Builder.h
 * \author Denis Martinez
 */

#ifndef BUILDER_H
#define BUILDER_H

#include <QScopedPointer>
#include <qxttemporarydir.h>

#include "../env/ILogger.h"

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
    QString readAllFile(const QString& filepath);
    bool compileDependencies(QStringList &objects, const QString& code, QStringList& includePaths, QString buildPath, const QStringList& cflags, const QStringList& cxxflags, const QStringList& sflags);
    bool compile(QStringList &objects, const QStringList &sources, const QStringList &includePaths, const QStringList &cflags, const QStringList &cxxflags, const QStringList &sflags, const QString &outputDirectory = QString());
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
    int runCommand(const QStringList &command, bool errorHighlighting=false);

    ILogger &mLogger;
    QScopedPointer<QxtTemporaryDir> mBuildDir;
    const Board *mBoard;
    QString mDevice;
};

#include <QThread>
#include <QActionGroup>

class BackgroundBuilder : public QThread
{
    Q_OBJECT

public:
    BackgroundBuilder(ILogger &logger = NullLogger::instance(), QObject *parent = NULL);
    void setRelatedActions(QActionGroup *actions);
    void backgroundBuild(const QString &code, bool upload = false);

    void setBoard(const Board *board) { builder.setBoard(board); }
    void setDevice(const QString &device) { builder.setDevice(device); }

    void run();

signals:
    void buildFinished();

private:
    Builder builder;
    QActionGroup *actions;

    QString code;
    bool upload;

};

#endif // BUILDER_H
