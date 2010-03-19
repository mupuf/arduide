/**
 * \file ColorButton.h
 * \author Denis Martinez
 */

#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <QPushButton>

class ColorButton : public QPushButton
{
    Q_OBJECT

public:
    ColorButton(QWidget *parent = NULL);
    const QColor &color() const { return mColor; }
    void setColor(const QColor &color);

signals:
    void colorChosen(const QColor &color);

private:
    QColor mColor;

private slots:
    void chooseColor();
};

#endif // COLORBUTTON_H
