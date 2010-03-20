/**
 * \file BoardChooser.cpp
 * \author Denis Martinez
 */

#include "BoardChooser.h"

#include "env/Settings.h"
#include "env/Board.h"
#include "IDEApplication.h"

BoardChooser::BoardChooser(QWidget *parent)
    : QMenu(parent),
      actionGroup(NULL)
{
    setTitle(tr("Board"));
    refresh();
}

void BoardChooser::refresh()
{
    QString defaultBoard = ideApp->settings()->board();
    delete actionGroup;
    actionGroup = new QActionGroup(this);
    actionGroup->setExclusive(true);
    connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(onActionTriggered(QAction *)));

    clear();

    QAction *action;
    foreach(const QString &boardId, Board::boardIds())
    {
        const Board *board = Board::boardInfo(boardId);
        action = new QAction(board->name(), actionGroup);
        action->setData(boardId);
        action->setCheckable(true);
        if (boardId == defaultBoard)
            action->setChecked(true);
        addAction(action);
    }
}

void BoardChooser::onActionTriggered(QAction *action)
{
    emit boardChosen(action->data().toString());
}
