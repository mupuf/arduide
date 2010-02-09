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

public slots:
    void save();
    void setFileName(const QString &fileName) { mFileName = fileName; }
    void setLexerFont(const QFont &font);
	void setIndentSize(int padding);

private:
    void setupShortcuts();

    QString mFileName;
    struct
    {
        int line;
        int index;
    } selectionOrigin;

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
