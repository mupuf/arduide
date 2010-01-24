/**
 * \file Editor.h
 * \author Denis Martinez
 */

#ifndef EDITOR_H
#define EDITOR_H

#include <Qsci/qsciscintilla.h>

class Editor : public QsciScintilla
{
    Q_OBJECT

public:
    Editor(QWidget *parent = NULL);
    const QString &fileName() { return mFileName; }
    void setFileName(const QString &fileName) { mFileName = fileName; }

public slots:
    void save();

private:
    QString mFileName;
};

#endif // EDITOR_H
