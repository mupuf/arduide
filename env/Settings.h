/**
 * \file Settings.h
 * \author Denis Martinez
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QStringList>
#include <QFont>

#include "IDEGlobal.h"

class IDE_EXPORT Settings : public QObject
{
    Q_OBJECT

public:
    QString arduinoPath();
    void setArduinoPath(const QString &path);
    QString sketchPath();
    void setSketchPath(const QString &path);
    QFont editorFont();
    void setEditorFont(const QFont &f);
    QString devicePort();
    void setDevicePort(const QString &port);
    QString board();
    void setBoard(const QString &board);
    bool verboseUpload();
    void setVerboseUpload(bool verbose);

    bool isCorrect();

signals:
    void fontChanged(const QFont &f);

private:
    Settings();
    QSettings mSettings;

    friend class IDEApplication;
};

#endif // SETTINGS_H
