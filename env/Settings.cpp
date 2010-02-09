/**
 * \file Settings.cpp
 * \author Denis Martinez
 */

#include "Settings.h"

#include <QFileInfo>
#include <QDebug>

Settings *Settings::mInstance = NULL;

Settings::Settings()
{
}

Settings &Settings::instance()
{
    if (mInstance == NULL)
        mInstance = new Settings;
    return *mInstance;
}

QString Settings::arduinoPath()
{
    return mSettings.value("arduinoPath").toString();
}

void Settings::setArduinoPath(const QString &path)
{
    mSettings.setValue("arduinoPath", path);
}

QString Settings::sketchPath()
{
    return mSettings.value("sketchPath").toString();
}

void Settings::setSketchPath(const QString &path)
{
    mSettings.setValue("sketchPath", path);
}

QFont Settings::editorFont()
{
    QVariant fontSetting = mSettings.value("editorFont");
    QFont font("Monospace", 8);
    if (! fontSetting.isNull())
        font.fromString(fontSetting.toString());
    return font;
}

void Settings::setEditorFont(const QFont &f)
{
    if (f == editorFont())
        return; // no change

    mSettings.setValue("editorFont", f.toString());
    emit fontChanged(f);
}

int Settings::editorIndentation()
{
    return mSettings.value("editorIndentation", 4).toInt();
}

void Settings::setEditorIndentation(int padding)
{
    mSettings.setValue("editorIndentation", padding);
	emit indentationChanged(padding);
}

QString Settings::devicePort()
{
    return mSettings.value("devicePort").toString();
}

void Settings::setDevicePort(const QString &port)
{
    mSettings.setValue("devicePort", port);
}

QString Settings::board()
{
    return mSettings.value("board").toString();
}

void Settings::setBoard(const QString &board)
{
    mSettings.setValue("board", board);
}

bool Settings::verboseUpload()
{
    return mSettings.value("verboseUpload", false).toBool();
}

void Settings::setVerboseUpload(bool verbose)
{
    mSettings.setValue("verboseUpload", verbose);
}

bool Settings::isCorrect()
{
    return QFileInfo(arduinoPath()).isDir() && QFileInfo(sketchPath()).isDir();
}
