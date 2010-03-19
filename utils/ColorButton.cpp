/**
 * \file ColorButton.cpp
 * \author Denis Martinez
 */

#include "ColorButton.h"

#include <QColorDialog>
#include <QDebug>

ColorButton::ColorButton(QWidget *parent)
    : QPushButton(parent)
{
    connect(this, SIGNAL(clicked()), this, SLOT(chooseColor()));
}

void ColorButton::setColor(const QColor &color)
{
    mColor = color;
    QPalette p = palette();
    p.setColor(QPalette::Button, color);
    setPalette(p);
}

void ColorButton::chooseColor()
{
    QColor newColor = QColorDialog::getColor(mColor, this);
    if (newColor.isValid())
    {
        setColor(newColor);
        emit colorChosen(newColor);
    }
}
