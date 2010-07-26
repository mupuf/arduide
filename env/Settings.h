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

class LexerArduino;
class Editor;

class IDE_EXPORT Settings : public QObject
{
    Q_OBJECT

public:
    QString arduinoPath() const;
    void setArduinoPath(const QString &path);
    QString sketchPath() const;
    void setSketchPath(const QString &path);
    QString devicePort() const;
    void setDevicePort(const QString &port);
    QString board() const;
    void setBoard(const QString &board);
    bool verboseUpload() const;
    void setVerboseUpload(bool verbose);

    void loadLexerProperties(LexerArduino *lexer);
    void saveLexerProperties(LexerArduino *lexer);
    void loadEditorSettings(Editor *editor);
    void saveEditorSettings(Editor *editor);

    void saveMainWindowState(const QByteArray& state);
    QByteArray mainWindowState();

    bool isCorrect() const;

signals:
    void fontChanged(const QFont &f);

private:
    Settings();
    QSettings mSettings;

    friend class IDEApplication;
};

#endif // SETTINGS_H
