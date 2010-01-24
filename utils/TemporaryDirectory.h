/**
 * \file TemporaryDirectory.h
 * \author Denis Martinez
 */

#ifndef TEMPORARYDIRECTORY_H
#define TEMPORARYDIRECTORY_H

#include <QString>

class TemporaryDirectory
{
public:
    TemporaryDirectory(const QString &tmpl);
    ~TemporaryDirectory();

    bool isValid();
    const QString &path() const { return mPath; }
    bool clear();

private:
    QString mPath;
};

#endif // TEMPORARYDIRECTORY_H
