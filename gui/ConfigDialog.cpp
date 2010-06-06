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
#include "gui/EditorFactory.h"
#include "gui/LexerArduino.h"
#include "IDEApplication.h"

ConfigWidget::ConfigWidget(QWidget *parent)
    : QxtConfigWidget(parent)
{
    setupUi();
    reset();
}

void ConfigWidget::initializePage(int index)
{
    switch (index)
    {
    case EditorIndex:
        static const QString sampleText =
            "/* Example code */\n"
            "#include <EEPROM/EEPROM.h>\n\n"
            "int a, b = 3;\n"
            "void loop()\n"
            "{\n"
            "    Serial.println(\"Hello, World!\");\n"
            "}\n";
        mEditor->setText(sampleText);
        break;
    case PathsIndex:
        break;
    case BuildIndex:
        break;
    }
}

void ConfigWidget::resetPage(int index)
{
    Settings *settings = ideApp->settings();
    LexerArduino *lexer;

    switch (index)
    {
    case EditorIndex:
        lexer = dynamic_cast<LexerArduino *>(mEditor->lexer());
        Q_ASSERT(lexer != NULL);
        settings->loadLexerProperties(lexer);
        settings->loadEditorSettings(mEditor);
        updateFontLabel(lexer->font(LexerArduino::Default));
        uiEditor.colorBox->setCurrentIndex(0);
        setColorAtIndex(0);
        uiEditor.caretColorButton->setColor(mEditor->caretForegroundColor());
        static const QString sampleText =
            "/* Example code */\n"
            "#include <EEPROM/EEPROM.h>\n\n"
            "int a, b = 3;\n"
            "void loop()\n"
            "{\n"
            "    Serial.println(\"Hello, World!\");\n"
            "}\n";
        mEditor->setText(sampleText);
        break;
    case PathsIndex:
        uiPaths.arduinoPathEdit->setText(settings->arduinoPath());
        uiPaths.sketchbookPathEdit->setText(settings->sketchPath());
        break;
    case BuildIndex:
        uiBuild.verboseBox->setChecked(settings->verboseUpload());
        break;
    }
}

void ConfigWidget::reset()
{
    // resets all the fields and goes to the first page
    resetPage(EditorIndex);
    resetPage(PathsIndex);
    resetPage(BuildIndex);
    setCurrentIndex(EditorIndex);
}

void ConfigWidget::setupUi()
{
    QWidget *page = new QWidget;
    uiEditor.setupUi(page);
    mEditor = EditorFactory::createEditor(QString());
    uiEditor.editorFrame->layout()->addWidget(mEditor);
    uiEditor.colorBox->addItem(tr("Default"), LexerArduino::Default);
    uiEditor.colorBox->addItem(tr("Comment"), LexerArduino::Comment);
    uiEditor.colorBox->addItem(tr("CommentLine"), LexerArduino::CommentLine);
    uiEditor.colorBox->addItem(tr("CommentDoc"), LexerArduino::CommentDoc);
    uiEditor.colorBox->addItem(tr("Number"), LexerArduino::Number);
    uiEditor.colorBox->addItem(tr("Keyword"), LexerArduino::Keyword);
    uiEditor.colorBox->addItem(tr("DoubleQuotedString"), LexerArduino::DoubleQuotedString);
    uiEditor.colorBox->addItem(tr("SingleQuotedString"), LexerArduino::SingleQuotedString);
    uiEditor.colorBox->addItem(tr("UUID"), LexerArduino::UUID);
    uiEditor.colorBox->addItem(tr("PreProcessor"), LexerArduino::PreProcessor);
    uiEditor.colorBox->addItem(tr("Operator"), LexerArduino::Operator);
    uiEditor.colorBox->addItem(tr("Identifier"), LexerArduino::Identifier);
    uiEditor.colorBox->addItem(tr("UnclosedString"), LexerArduino::UnclosedString);
    uiEditor.colorBox->addItem(tr("VerbatimString"), LexerArduino::VerbatimString);
    uiEditor.colorBox->addItem(tr("Regex"), LexerArduino::Regex);
    uiEditor.colorBox->addItem(tr("CommentLineDoc"), LexerArduino::CommentLineDoc);
    uiEditor.colorBox->addItem(tr("KeywordSet2"), LexerArduino::KeywordSet2);
    uiEditor.colorBox->addItem(tr("CommentDocKeyword"), LexerArduino::CommentDocKeyword);
    uiEditor.colorBox->addItem(tr("CommentDocKeywordError"), LexerArduino::CommentDocKeywordError);
    uiEditor.colorBox->addItem(tr("GlobalClass"), LexerArduino::GlobalClass);
    addPage(page, QIcon(":/images/32x32/accessories-text-editor.png"), tr("Editor"));

    page = new QWidget;
    uiPaths.setupUi(page);
    addPage(page, QIcon(":/images/32x32/folder.png"), tr("Paths"));

    page = new QWidget;
    uiBuild.setupUi(page);
    addPage(page, QIcon(":/images/32x32/applications-development.png"), tr("Build"));

    connect(uiPaths.arduinoPathEdit, SIGNAL(textChanged(const QString &)), this, SLOT(fieldChange()));
    connect(uiPaths.sketchbookPathEdit, SIGNAL(textChanged(const QString &)), this, SLOT(fieldChange()));
    connect(uiBuild.verboseBox, SIGNAL(stateChanged(int)), this, SLOT(fieldChange()));

    connect(uiEditor.fontChooseButton, SIGNAL(clicked()), this, SLOT(chooseFont()));
    connect(uiEditor.colorBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setColorAtIndex(int)));
    connect(uiEditor.fgColorButton, SIGNAL(colorChosen(const QColor &)), this, SLOT(setFgColor(const QColor &)));
    connect(uiEditor.bgColorButton, SIGNAL(colorChosen(const QColor &)), this, SLOT(setBgColor(const QColor &)));
    connect(uiEditor.caretColorButton, SIGNAL(colorChosen(const QColor &)), this, SLOT(setCaretColor(const QColor &)));
    connect(uiPaths.arduinoPathButton, SIGNAL(clicked()), this, SLOT(chooseArduinoPath()));
    connect(uiPaths.sketchbookPathButton, SIGNAL(clicked()), this, SLOT(chooseSketchbookPath()));
}

void ConfigWidget::setColorAtIndex(int index)
{
    if (index < 0)
        return;
    int style = uiEditor.colorBox->itemData(index).toInt();
    LexerArduino *lexer = dynamic_cast<LexerArduino *>(mEditor->lexer());
    Q_ASSERT(lexer != NULL);
    uiEditor.fgColorButton->setColor(lexer->color(style));
    uiEditor.bgColorButton->setColor(lexer->paper(style));
}

void ConfigWidget::setFgColor(const QColor &color)
{
    QComboBox *colorBox = uiEditor.colorBox;
    QsciLexer *lexer = mEditor->lexer();
    int style = colorBox->itemData(colorBox->currentIndex()).toInt();
    if (style == LexerArduino::Default)
        lexer->setDefaultColor(color);
    lexer->setColor(color, style);
}

void ConfigWidget::setBgColor(const QColor &color)
{
    QComboBox *colorBox = uiEditor.colorBox;
    QsciLexer *lexer = mEditor->lexer();
    int style = colorBox->itemData(colorBox->currentIndex()).toInt();
    if (style == LexerArduino::Default)
        lexer->setDefaultPaper(color);
    lexer->setPaper(color, style);
}

void ConfigWidget::setCaretColor(const QColor &color)
{
    mEditor->setCaretForegroundColor(color);
}

void ConfigWidget::updateFontLabel(const QFont &f)
{
    static QString format("%0 %1");
    QString text = format.arg(f.family()).arg(f.pointSize());
    uiEditor.fontLabel->setProperty("selectedFont", f);
    uiEditor.fontLabel->setFont(f);
    uiEditor.fontLabel->setText(text);
}

void ConfigWidget::chooseFont()
{
    bool ok;
    QFont initialFont = mEditor->lexer()->font(LexerArduino::Default);
    QFont f = QFontDialog::getFont(&ok, initialFont, this);
    if (ok)
    {
        QsciLexer *lexer = mEditor->lexer();
        lexer->setDefaultFont(f);
        lexer->setFont(f);
        updateFontLabel(f);
        mChangedFields << uiEditor.fontLabel;
    }
}

void ConfigWidget::chooseArduinoPath()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose Arduino path"), uiPaths.arduinoPathEdit->text());
    if (! path.isEmpty())
            uiPaths.arduinoPathEdit->setText(path);
}

void ConfigWidget::chooseSketchbookPath()
{
    QString path = QFileDialog::getExistingDirectory(this, tr("Choose Sketchbook path"), uiPaths.sketchbookPathEdit->text());
    if (! path.isEmpty())
        uiPaths.sketchbookPathEdit->setText(path);
}

void ConfigWidget::fieldChange()
{
    QWidget *w = qobject_cast<QWidget *>(sender());
    Q_ASSERT(w != NULL);
    mChangedFields << w;
}

bool ConfigWidget::saveConfig()
{
    Settings *settings = ideApp->settings();
    foreach (QWidget *field, mChangedFields)
    {
        if (field == uiPaths.arduinoPathEdit)
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
        else if (field == uiBuild.verboseBox)
            settings->setVerboseUpload(uiBuild.verboseBox->isChecked());
    }
    mChangedFields.clear();

    // save all the editor/lexer settings
    settings->saveEditorSettings(mEditor);
    LexerArduino *lexer = dynamic_cast<LexerArduino *>(mEditor->lexer());
    Q_ASSERT(lexer != NULL);
    settings->saveLexerProperties(lexer);
    // update any existing editor
    ideApp->mainWindow()->configureEditors();
    return true;
}

ConfigDialog::ConfigDialog(QWidget *parent)
    : QxtConfigDialog(parent)
{
    setWindowTitle(tr("Configuration"));
    setupUi();
}

void ConfigDialog::setupUi()
{
    setMinimumWidth(500);
    setMinimumHeight(400);

    mConfigWidget = new ConfigWidget;
    QxtConfigWidget *oldConfigWidget = configWidget();
    setConfigWidget(mConfigWidget);
    delete oldConfigWidget;

    dialogButtonBox()->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel);
    connect(dialogButtonBox()->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));
}

bool ConfigDialog::apply()
{
    return mConfigWidget->saveConfig();
}

void ConfigDialog::accept()
{
    if (apply())
    {
        hide();
        setResult(QDialog::Accepted);
    }
}
