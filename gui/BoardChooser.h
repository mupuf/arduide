/**
 * \file BoardChooser.h
 * \author Denis Martinez
 */

#ifndef BOARDCHOOSER_H
#define BOARDCHOOSER_H

#include <QMenu>
#include <QActionGroup>

class BoardChooser : public QMenu
{
    Q_OBJECT

public:
    BoardChooser(QWidget *parent = NULL);

signals:
    void boardChosen(const QString &board);

public slots:
    void refresh();

private:
    QActionGroup *actionGroup;

private slots:
    void onActionTriggered(QAction *action);
};

#endif // BOARDCHOOSER_H
