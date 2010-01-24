/**
 * \file EditorFactory.h
 * \author Denis Martinez
 */

#ifndef EDITORFACTORY_H
#define EDITORFACTORY_H

#include <QString>

#include "Editor.h"

class EditorFactory
{
public:
    static Editor *createEditor(const QString &text = initialText, QWidget *parent = NULL);

private:
    static const QString initialText;
};

#endif // EDITORFACTORY_H
