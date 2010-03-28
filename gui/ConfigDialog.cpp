/**
 * \file ConfigDialog.cpp
 * \author Denis Martinez
 */

#include "ConfigDialog.h"

#include <QFontDialog>
#include <QFileDialog>
#include <QMessageBox>

#include "env/Settings.h"
#include "env/Toolkit.h"
#include "IDEApplication.h"

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

    page = new QWidget;
    uiPaths.setupUi(page);
    addPage(page, QIcon(":/images/32x32/folder.png"), tr("Paths"));

    connect(uiEditor.fontChooseButton, SIGNAL(clicked()), this, SLOT(chooseFont()));
    connect(uiPaths.arduinoPathButton, SIGNAL(clicked()), this, SLOT(chooseArduinoPath()));
    connect(uiPaths.sketchbookPathButton, SIGNAL(clicked()), this, SLOT(chooseSketchbookPath()));
}

void ConfigDialog::initializePage(int index)
{
    switch (index)
    {
    case editorIndex:
        setupFontChooser();
        break;
    case pathsIndex:
        uiPaths.arduinoPathEdit->setText(ideApp->settings()->arduinoPath());
        uiPaths.sketchbookPathEdit->setText(ideApp->settings()->sketchPath());
        break;
    }
}

void ConfigDialog::setupFontChooser()
{
    static QString format("%0 %1");

    QFont f = ideApp->settings()->editorFont();
    uiEditor.fontLabel->setFont(f);
    uiEditor.fontLabel->setText(format.arg(f.family()).arg(f.pointSize()));
}

void ConfigDialog::chooseFont()
{
    bool ok;
    QFont initialFont = ideApp->settings()->editorFont();
    QFont f = QFontDialog::getFont(&ok, initialFont, this);
    if (ok)
    {
        ideApp->settings()->setEditorFont(f);
        setupFontChooser();
    }
}

void ConfigDialog::chooseArduinoPath()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose Arduino path"));
    bool valid;
    if (! path.isEmpty())
    {
        valid = Toolkit::isValidArduinoPath(path);
        if (valid)
            uiPaths.arduinoPathEdit->setText(path);
        else
            QMessageBox::warning(this, tr("Invalid arduino path"), tr("This path does not contain a valid Arduino installation, please choose another."));
    }
}

void ConfigDialog::chooseSketchbookPath()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose Sketchbook path"));
    if (! path.isEmpty())
    {
        uiPaths.sketchbookPathEdit->setText(path);
#pragma message("TODO: update the sketchbook browser")
    }
}
