/**
 * \file DeviceChooser.h
 * \author Denis Martinez
 */

#ifndef DEVICECHOOSER_H
#define DEVICECHOOSER_H

#include <QMenu>
#include <QActionGroup>

class DeviceChooser : public QMenu
{
    Q_OBJECT

public:
    DeviceChooser(QWidget *parent = NULL);

signals:
    void deviceChosen(const QString &board);

public slots:
    void refresh();

private:
    QActionGroup *actionGroup;

private slots:
    void onActionTriggered(QAction *action);
};

#endif // DEVICECHOOSER_H
