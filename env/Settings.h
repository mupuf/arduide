/**
 * \file Settings.h
 * \author Denis Martinez
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QStringList>
#include <QFont>

class Settings : public QObject
{
    Q_OBJECT

public:
    static Settings &instance();
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
    static Settings *mInstance;
    Settings();
    QSettings mSettings;
};

#endif // SETTINGS_H
