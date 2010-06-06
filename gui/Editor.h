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
    void setLexerFont(const QFont &font);

    void setCaretForegroundColor(const QColor &col);
    const QColor &caretForegroundColor() { return mCaretForegroundColor; }
    void setSelectionBackgroundColor(const QColor &col);
    const QColor &selectionBackgroundColor() { return mSelectionBackgroundColor; }

public slots:
    void save();

private:
    void setupShortcuts();

    QString mFileName;
    struct
    {
        int line;
        int index;
    } selectionOrigin;

    QColor mCaretForegroundColor, mSelectionBackgroundColor;

private slots:
    void findPreviousParagraph(int *line, int *index);
    void findNextParagraph(int *line, int *index);
    void selectTillPreviousParagraph();
    void selectTillNextParagraph();
    void updateSelectionOrigin();

    void goToPreviousParagraph();
    void goToNextParagraph();
};

#endif // EDITOR_H
