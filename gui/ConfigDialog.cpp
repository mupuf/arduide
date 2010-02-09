/**
 * \file ConfigDialog.cpp
 * \author Denis Martinez
 */

#include "ConfigDialog.h"

#include <QFontDialog>

#include "env/Settings.h"

ConfigDialog::ConfigDialog(QWidget *parent)
    : QxtConfigDialog(parent)
{
    setWindowTitle(tr("Configuration"));
    setupUi();
}

void ConfigDialog::setupUi()
{
    QWidget *page;

    page = new QWidget;
    uiEditor.setupUi(page);
    addPage(page, QIcon(":/images/32x32/accessories-text-editor.png"), tr("Editor"));

    connect(uiEditor.fontChooseButton, SIGNAL(clicked()), this, SLOT(chooseFont()));
}

void ConfigDialog::initializePage(int index)
{
    switch (index)
    {
    case editorIndex:
        setupFontChooser();
        break;
    }
}

void ConfigDialog::setupFontChooser()
{
    static QString format("%0 %1");

    QFont f = Settings::instance().editorFont();
    uiEditor.fontLabel->setFont(f);
    uiEditor.fontLabel->setText(format.arg(f.family()).arg(f.pointSize()));
}

void ConfigDialog::chooseFont()
{
    bool ok;
    QFont initialFont = Settings::instance().editorFont();
    QFont f = QFontDialog::getFont(&ok, initialFont, this);
    if (ok)
    {
        Settings::instance().setEditorFont(f);
        setupFontChooser();
    }
}
