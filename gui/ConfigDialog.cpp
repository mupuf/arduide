/**
 * \file ConfigDialog.cpp
 * \author Denis Martinez
 */

#include "ConfigDialog.h"

#include <QFontDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QDebug>

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
    dialogButtonBox()->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel);

    QWidget *page;

    page = new QWidget;
    uiEditor.setupUi(page);
    addPage(page, QIcon(":/images/32x32/accessories-text-editor.png"), tr("Editor"));

    page = new QWidget;
    uiPaths.setupUi(page);
    addPage(page, QIcon(":/images/32x32/folder.png"), tr("Paths"));

    connect(dialogButtonBox()->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
    connect(uiPaths.arduinoPathEdit, SIGNAL(textChanged(const QString &)), this, SLOT(fieldChange()));
    connect(uiPaths.sketchbookPathEdit, SIGNAL(textChanged(const QString &)), this, SLOT(fieldChange()));

    connect(uiEditor.fontChooseButton, SIGNAL(clicked()), this, SLOT(chooseFont()));
    connect(uiPaths.arduinoPathButton, SIGNAL(clicked()), this, SLOT(chooseArduinoPath()));
    connect(uiPaths.sketchbookPathButton, SIGNAL(clicked()), this, SLOT(chooseSketchbookPath()));
}

bool ConfigDialog::apply()
{
    Settings *settings = ideApp->settings();
    foreach (QWidget *field, mChangedFields)
    {
        if (field == uiEditor.fontLabel)
        {
            QVariant var = uiEditor.fontLabel->property("selectedFont");
            if (var.type() == QVariant::Font)
                ideApp->settings()->setEditorFont(var.value<QFont>());
        }
        else if (field == uiPaths.arduinoPathEdit)
        {
            QString path = uiPaths.arduinoPathEdit->text();
            if (! Toolkit::isValidArduinoPath(path))
            {
                QMessageBox::warning(this, tr("Invalid arduino path"), tr("This path does not contain a valid Arduino installation, please choose another."));
                return false;
            }
            settings->setArduinoPath(path);
        }
        else if (field == uiPaths.sketchbookPathEdit)
        {
            settings->setSketchPath(uiPaths.sketchbookPathEdit->text());
            #pragma message("TODO: update the sketchbook browser")
        }
    }
    mChangedFields.clear();
    return true;
}

void ConfigDialog::accept()
{
    if (apply())
    {
        hide();
        setResult(QDialog::Accepted);
    }
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
        uiEditor.fontLabel->setProperty("selectedFont", f);
        mChangedFields << uiEditor.fontLabel;
        setupFontChooser();
    }
}

void ConfigDialog::chooseArduinoPath()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose Arduino path"), uiPaths.arduinoPathEdit->text());
    if (! path.isEmpty())
            uiPaths.arduinoPathEdit->setText(path);
}

void ConfigDialog::chooseSketchbookPath()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose Sketchbook path"), uiPaths.sketchbookPathEdit->text());
    if (! path.isEmpty())
        uiPaths.sketchbookPathEdit->setText(path);
}

void ConfigDialog::fieldChange()
{
    QWidget *w = qobject_cast<QWidget *>(sender());
    Q_ASSERT(w != NULL);
    mChangedFields << w;
}
