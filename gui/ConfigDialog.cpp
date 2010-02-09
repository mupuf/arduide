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
    connect(uiEditor.spinIndentSize, SIGNAL(valueChanged(int)), this, SLOT(indentChanged(int)));
	connect(uiEditor.checkAutoIndent, SIGNAL(stateChanged(int)), this, SLOT(autoIndentChanged(int)));
}

void ConfigDialog::initializePage(int index)
{
    switch (index)
    {
    case editorIndex:
        setupFontChooser();
        uiEditor.spinIndentSize->setValue(Settings::instance().editorIndentation());
        uiEditor.checkAutoIndent->setCheckState(Settings::instance().editorAutoIndent()?Qt::Checked:Qt::Unchecked);
		autoIndentChanged(uiEditor.checkAutoIndent->checkState());
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

void ConfigDialog::indentChanged(int value)
{
    Settings::instance().setEditorIndentation(value);
}

void ConfigDialog::autoIndentChanged(int checked)
{
    bool value=(checked==Qt::Checked);
    Settings::instance().setEditorAutoIndent(value);
    uiEditor.checkAutoIndent->setText(value?tr("true"):tr("false"));
}