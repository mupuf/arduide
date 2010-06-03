/**
 * \file ILogger.h
 * \author Denis Martinez
 */

#ifndef ILOGGER_H
#define ILOGGER_H

#include <QString>

class ILogger
{
public:
    virtual ~ILogger() {}
    virtual void log(const QString &text) = 0;
    virtual void logImportant(const QString &text) = 0;
    virtual void logError(const QString &text) = 0;
    virtual void logCommand(const QStringList &command) = 0;
};

class NullLogger : public ILogger
{
private:
    NullLogger() {}

public:
    void log(const QString &text);
    void logImportant(const QString &text);
    void logError(const QString &text);
    void logCommand(const QStringList &command);
    static NullLogger &instance();
};

#endif // ILOGGER_H
