/**
 * \file ConfigDialog.cpp
 * \author Denis Martinez
 */

#include "ConfigDialog.h"

ConfigDialog::ConfigDialog(QWidget *parent)
    : QxtConfigDialog(parent)
{
    setWindowTitle(tr("Configuration"));
}
