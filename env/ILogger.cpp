/**
 * \file ILogger.cpp
 * \author Denis Martinez
 */

#include "ILogger.h"

void NullLogger::log(const QString &)
{
}

void NullLogger::logError(const QString &)
{
}

void NullLogger::logCommand(const QStringList &)
{
}

NullLogger &NullLogger::instance()
{
    static NullLogger logger;
    return logger;
}
