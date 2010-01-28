/**
 * \file FirstTimeWizard.h
 * \author Denis Martinez
 */

#ifndef FIRSTTIMEWIZARD_H
#define FIRSTTIMEWIZARD_H

#include <ui_FirstTimeWizard.h>

class FirstTimeWizard : public QWizard, Ui::FirstTimeWizard
{
    Q_OBJECT

public:
    FirstTimeWizard(QWidget *parent = NULL);
    bool validateCurrentPage();

private:
    void setupActions();

private slots:
    void chooseArduinoPath();
    void chooseSketchbookPath();
};

#endif // FIRSTTIMEWIZARD_H
