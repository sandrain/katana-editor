/****************************************************************************
**
** Copyright (C) 2007~2020 Colin Willcocks.
** Copyright (C) 2005~2007 Uco Mesdag. 
** All rights reserved.
** This file is part of "KATANA Fx FloorBoard".
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**
****************************************************************************/

#include <QtWidgets>
#include "midiIO.h"
#include "preferencesPages.h"
#include "Preferences.h"

GeneralPage::GeneralPage(QWidget *parent)
    : QWidget(parent)
{
    Preferences *preferences = Preferences::Instance();
    QString dir = preferences->getPreferences("General", "Files", "dir");
    QString loader = preferences->getPreferences("General", "Loader", "active");
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    QGroupBox *patchGroup = new QGroupBox(QObject::tr("Patch folder"));

    QLabel *descriptionLabel = new QLabel(QObject::tr("Select the default folder for storing patches."));
    QLabel *dirLabel = new QLabel(QObject::tr("Default patch folder:"));
    QLineEdit *dirEdit = new QLineEdit(dir);
    QPushButton *browseButton = new QPushButton(QObject::tr("Browse"));

    connect(browseButton, SIGNAL(clicked()), this, SLOT(browseDir()));

    this->dirEdit = dirEdit;

    QGroupBox *loaderGroup = new QGroupBox(QObject::tr("Midi Patch Loader Function:"));
    QLabel *loaderLabel = new QLabel(QObject::tr("use external midi pedal to select/load 128 patches"));
    QCheckBox *loaderCheckBox = new QCheckBox(QObject::tr("Midi Patch Loader Function"));
    this->loaderCheckBox = loaderCheckBox;
    if(loader=="true")
    {
        loaderCheckBox->setChecked(true);
    };

    QHBoxLayout *dirEditLayout = new QHBoxLayout;
    dirEditLayout->addWidget(dirEdit);
    dirEditLayout->addWidget(browseButton);

    QVBoxLayout *dirLayout = new QVBoxLayout;
    dirLayout->addWidget(descriptionLabel);
    dirLayout->addSpacing(12*ratio);
    dirLayout->addWidget(dirLabel);
    dirLayout->addLayout(dirEditLayout);

    QVBoxLayout *loaderLayout = new QVBoxLayout;
    loaderLayout->addWidget(loaderLabel);
    loaderLayout->addSpacing(12*ratio);
    loaderLayout->addWidget(loaderCheckBox);
    loaderGroup->setLayout(loaderLayout);

    QVBoxLayout *patchLayout = new QVBoxLayout;
    patchLayout->addLayout(dirLayout);
    patchGroup->setLayout(patchLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(patchGroup);
    mainLayout->addWidget(loaderGroup);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

MidiPage::MidiPage(QWidget *parent)
    : QWidget(parent)
{
    bool ok; int id;
    midiIO *midi = new midiIO();
    Preferences *preferences = Preferences::Instance();
    QString midiInDevice = preferences->getPreferences("Midi", "MidiIn", "device");
    QString midiInDevice2 = preferences->getPreferences("Midi", "MidiIn2", "device");
    QString midiOutDevice = preferences->getPreferences("Midi", "MidiOut", "device");
    QString dBugScreen = preferences->getPreferences("Midi", "DBug", "bool");
    QString device = preferences->getPreferences("Midi", "Device", "bool");
    QString kat = preferences->getPreferences("Midi", "Katana", "model");
    QString midiTxChSet = preferences->getPreferences("Midi", "TxCh", "set");
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    int midiInDeviceID = midiInDevice.toInt(&ok, 10);
    int midiInDevice2ID = midiInDevice2.toInt(&ok, 10);
    int midiOutDeviceID = midiOutDevice.toInt(&ok, 10);
    int katModel = kat.toInt(&ok, 10);
    QList<QString> midiInDevices = midi->getMidiInDevices();
    QList<QString> midiOutDevices = midi->getMidiOutDevices();

    QGroupBox *midiGroup = new QGroupBox(QObject::tr("Midi settings"));

    QLabel *mididescriptionLabel = new QLabel(QObject::tr("Select your midi in and out device."));
    QLabel *midiInLabel = new QLabel(QObject::tr("Katana Midi in:"));
    QLabel *midiOutLabel = new QLabel(QObject::tr("Katana Midi out:"));
    QLabel *midiInLabel2 = new QLabel(QObject::tr("Loader external Midi in:"));
    QLabel *model = new QLabel(QObject::tr("Katana Model"));
    QLabel *spacer = new QLabel("");

    QComboBox *midiInCombo = new QComboBox;
    this->midiInCombo = midiInCombo;
    midiInCombo->addItem(QObject::tr("Select midi-in device"));
    id = 0;
    for (QList<QString>::iterator dev = midiInDevices.begin(); dev != midiInDevices.end(); ++dev)
    {
        QString str(*dev);
        midiInCombo->addItem(str.toLatin1().data());
        id++;
    };
    if(!midiInDevice.isEmpty())
    {
        midiInCombo->setCurrentIndex(midiInDeviceID + 1); // +1 because there is a default entry at 0
    };
    if ( midiInDevices.contains("KATANA")  && device=="true")
    {
        int inputDevice = midiInDevices.indexOf("KATANA") + 1;
        midiInCombo->setCurrentIndex(inputDevice);
    };

    QComboBox *midiOutCombo = new QComboBox;
    this->midiOutCombo = midiOutCombo;
    midiOutCombo->addItem(QObject::tr("Select midi-out device"));
    id = 0;
    for (QList<QString>::iterator dev = midiOutDevices.begin(); dev != midiOutDevices.end(); ++dev)
    {
        QString str(*dev);
        midiOutCombo->addItem(str.toLatin1().data());
        id++;
    };
    if(!midiOutDevice.isEmpty())
    {
        midiOutCombo->setCurrentIndex(midiOutDeviceID + 1); // +1 because there is a default entry at 0
    };
    if ( midiOutDevices.contains("KATANA") && device=="true" )
    {
        int outputDevice = midiOutDevices.indexOf("KATANA") + 1;
        midiOutCombo->setCurrentIndex(outputDevice);
    };

    QComboBox *midiInCombo2 = new QComboBox;
    this->midiInCombo2 = midiInCombo2;   
    midiInCombo2->addItem(QObject::tr("Select midi controller device"));
     midiInCombo2->addItem(QObject::tr("Select no device"));
    id = 0;
    for (QList<QString>::iterator dev = midiInDevices.begin(); dev != midiInDevices.end(); ++dev)
    {
        QString str(*dev);
        midiInCombo2->addItem(str.toLatin1().data());
        id++;
    };
    if(!midiInDevice.isEmpty())
    {
        midiInCombo2->setCurrentIndex(midiInDevice2ID + 1); // +1 because there is a default entry at 0
    };
    /*if ( midiInDevices.contains("KATANA")  && device=="true")
    {
        int inputDevice = midiInDevices.indexOf("KATANA") + 1;
        midiInCombo->setCurrentIndex(inputDevice);
    };*/

    QCheckBox *autoCheckBox = new QCheckBox(QObject::tr("Auto select KATANA USB"));
    this->autoCheckBox = autoCheckBox;
    if(device=="true")
    {
        autoCheckBox->setChecked(true);

    };
    
    QComboBox *modelCombo = new QComboBox;
    this->modelCombo = modelCombo;   
    modelCombo->addItem(QObject::tr("Katana 50"));
    modelCombo->addItem(QObject::tr("Katana 100 12"));
    modelCombo->addItem(QObject::tr("Katana 100 212"));
    modelCombo->addItem(QObject::tr("Katana 100 Head"));
    modelCombo->addItem(QObject::tr("Katana 100 Artist"));
    //modelCombo->addItem(QObject::tr("Katana Air"));
    modelCombo->setCurrentIndex(katModel);

    QVBoxLayout *midiLabelLayout = new QVBoxLayout;
    midiLabelLayout->addWidget(midiInLabel);
    midiLabelLayout->addWidget(midiOutLabel);
    midiLabelLayout->addWidget(spacer);
    midiLabelLayout->addSpacing(12*ratio);
  if( preferences->getPreferences("General", "Loader", "active") == "true"){
      midiLabelLayout->addWidget(midiInLabel2); };
    midiLabelLayout->addWidget(model);

    QVBoxLayout *midiComboLayout = new QVBoxLayout;
    midiComboLayout->addWidget(midiInCombo);
    midiComboLayout->addWidget(midiOutCombo);
    midiComboLayout->addWidget(autoCheckBox);
    midiComboLayout->addSpacing(12*ratio);
   if( preferences->getPreferences("General", "Loader", "active") == "true"){
    midiComboLayout->addWidget(midiInCombo2); };
    midiComboLayout->addWidget(modelCombo);

    QHBoxLayout *midiSelectLayout = new QHBoxLayout;
    midiSelectLayout->addLayout(midiLabelLayout);
    midiSelectLayout->addLayout(midiComboLayout);

    QVBoxLayout *midiDevLayout = new QVBoxLayout;
    midiDevLayout->addWidget(mididescriptionLabel);
    midiDevLayout->addSpacing(12*ratio);
    midiDevLayout->addLayout(midiSelectLayout);

    QVBoxLayout *midiLayout = new QVBoxLayout;
    midiLayout->addLayout(midiDevLayout);
    midiGroup->setLayout(midiLayout);

    QGroupBox *advancedSettingsGroup = new QGroupBox(QObject::tr("Advanced settings"));

    QLabel *midiTxChDescriptionLabel = new QLabel(tr("Midi Tx Channel:"));


    QCheckBox *dBugCheckBox = new QCheckBox(QObject::tr("deBug Mode"));
    QSpinBox *midiTxChSpinBox = new QSpinBox;
    //midiTxChSpinBox->setMaximumWidth(100);

    this->dBugCheckBox = dBugCheckBox;
    if(dBugScreen=="true")
    {
        dBugCheckBox->setChecked(true);
    };

    this->midiTxChSpinBox = midiTxChSpinBox;
    midiTxChSpinBox->setValue(midiTxChSet.toInt(&ok, 10));
    midiTxChSpinBox->setRange(1, 16);
    //midiTxChSpinBox->setPrefix("Channel ");

    QVBoxLayout *advancedBoxLayout = new QVBoxLayout;
    advancedBoxLayout->addWidget(midiTxChDescriptionLabel);
    advancedBoxLayout->addWidget(midiTxChSpinBox);
    advancedBoxLayout->addSpacing(20*ratio);
    advancedBoxLayout->addWidget(dBugCheckBox);

    QHBoxLayout *advancedLayout = new QHBoxLayout;
    advancedLayout->addLayout(advancedBoxLayout);

    advancedSettingsGroup->setLayout(advancedLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(midiGroup);
    mainLayout->addWidget(advancedSettingsGroup);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

WindowPage::WindowPage(QWidget *parent)
    : QWidget(parent)
{
    bool ok;
    Preferences *preferences = Preferences::Instance();
    QString windowRestore = preferences->getPreferences("Window", "Restore", "window");
    QString sidepanelRestore = preferences->getPreferences("Window", "Restore", "sidepanel");
    QString splashScreen = preferences->getPreferences("Window", "Splash", "bool");
    QString SingleWindow = preferences->getPreferences("Window", "Single", "bool");
    QString WidgetsUse = preferences->getPreferences("Window", "AutoScale", "bool");
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    QGroupBox *windowGroup = new QGroupBox(QObject::tr("Window settings"));

    QLabel *restoreDescriptionLabel = new QLabel(QObject::tr("Select if you want the window position to be saved on exit."));
    QCheckBox *windowCheckBox = new QCheckBox(QObject::tr("Restore window"));
    QCheckBox *sidepanelCheckBox = new QCheckBox(QObject::tr("Restore sidepanel"));
    QCheckBox *singleWindowCheckBox = new QCheckBox(QObject::tr("Single Window Layout"));
    QCheckBox *autoRatioCheckBox = new QCheckBox(QObject::tr("Auto Window Scaling"));
    
    this->windowCheckBox = windowCheckBox;
    this->sidepanelCheckBox = sidepanelCheckBox;
    this->singleWindowCheckBox = singleWindowCheckBox;
    this->autoRatioCheckBox = autoRatioCheckBox;

    if(windowRestore=="true") { windowCheckBox->setChecked(true); };
    if(sidepanelRestore=="true") { sidepanelCheckBox->setChecked(true); };
    if(SingleWindow=="true") { singleWindowCheckBox->setChecked(true); };
    if(WidgetsUse=="true") { autoRatioCheckBox->setChecked(true); };

    QDoubleSpinBox *ratioSpinBox = new QDoubleSpinBox;
    this->ratioSpinBox = ratioSpinBox;
    ratioSpinBox->setDecimals(2);
    ratioSpinBox->setValue(ratio);
    ratioSpinBox->setRange(0.5, 10.0);
    ratioSpinBox->setSingleStep(0.01);
    ratioSpinBox->setPrefix("Resizing Scale = ");
    ratioSpinBox->setSuffix(QObject::tr(" :1"));
            
    QDoubleSpinBox *fontRatioSpinBox = new QDoubleSpinBox;
    this->fontRatioSpinBox = fontRatioSpinBox;
    const double fratio = preferences->getPreferences("Window", "Font", "ratio").toDouble(&ok);
    fontRatioSpinBox->setDecimals(2);
    fontRatioSpinBox->setValue(fratio);
    fontRatioSpinBox->setRange(0.5, 10.0);
    fontRatioSpinBox->setSingleStep(0.01);
    fontRatioSpinBox->setPrefix("Text Font Scale = ");
    fontRatioSpinBox->setSuffix(QObject::tr(" :1"));
    
    QVBoxLayout *restoreLayout = new QVBoxLayout;
    restoreLayout->addWidget(restoreDescriptionLabel);
    restoreLayout->addSpacing(12*ratio);
    restoreLayout->addWidget(windowCheckBox);
    restoreLayout->addWidget(sidepanelCheckBox);
    restoreLayout->addWidget(singleWindowCheckBox);
    restoreLayout->addWidget(autoRatioCheckBox);
    restoreLayout->addWidget(ratioSpinBox);
    restoreLayout->addWidget(fontRatioSpinBox);

    QVBoxLayout *windowLayout = new QVBoxLayout;
    windowLayout->addLayout(restoreLayout);
    windowGroup->setLayout(windowLayout);

    QGroupBox *splashScreenGroup = new QGroupBox(QObject::tr("Show splash screen"));

    QLabel *splashDescriptionLabel = new QLabel(QObject::tr("Disable or enable the splash screen."));
    QCheckBox *splashCheckBox = new QCheckBox(QObject::tr("Splash screen"));
    this->splashCheckBox = splashCheckBox;

    if(splashScreen=="true") { splashCheckBox->setChecked(true); };

    QVBoxLayout *splashLayout = new QVBoxLayout;
    splashLayout->addWidget(splashDescriptionLabel);
    splashLayout->addSpacing(12*ratio);
    splashLayout->addWidget(splashCheckBox);

    QVBoxLayout *splashScreenLayout = new QVBoxLayout;
    splashScreenLayout->addLayout(splashLayout);
    splashScreenGroup->setLayout(splashScreenLayout);

    QLabel *note = new QLabel(QObject::tr("<b>***Changes take effect on next startup***</b>" ));
    
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(windowGroup);
    mainLayout->addWidget(splashScreenGroup);
    mainLayout->addWidget(note);

    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

LanguagePage::LanguagePage(QWidget *parent)
    : QWidget(parent)
{
    Preferences *preferences = Preferences::Instance();
    QString lang = preferences->getPreferences("Language", "Locale", "select");
    bool ok;
    int choice = lang.toInt(&ok, 16);
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    QGroupBox *languageGroup = new QGroupBox(QObject::tr("Language Selection"));

    QRadioButton *englishButton = new QRadioButton(QObject::tr("English"));
    //this->englishButton = englishButton;
    this->frenchButton = new QRadioButton(QObject::tr("French"));
    this->germanButton = new QRadioButton(QObject::tr("German"));
    this->portugueseButton = new QRadioButton(QObject::tr("Portuguese"));
    this->spanishButton = new QRadioButton(QObject::tr("Spanish"));
    this->japaneseButton = new QRadioButton(QObject::tr("Japanese"));
    this->chineseButton = new QRadioButton(QObject::tr("Chinese (simplified)"));
    if (choice == 6)      {chineseButton->setChecked(true); }
    else if (choice == 5) {japaneseButton->setChecked(true); }
    else if (choice == 4) {spanishButton->setChecked(true); }
    else if (choice == 3) {portugueseButton->setChecked(true); }
    else if (choice == 2) {germanButton->setChecked(true); }
    else if (choice == 1) {frenchButton->setChecked(true); }
    else {englishButton->setChecked(true); };

    QVBoxLayout *languageLayout = new QVBoxLayout;
    languageLayout->addWidget(englishButton);
    languageLayout->addWidget(frenchButton);
    languageLayout->addWidget(germanButton);
    languageLayout->addWidget(portugueseButton);
    languageLayout->addWidget(spanishButton);
    languageLayout->addWidget(japaneseButton);
    languageLayout->addWidget(chineseButton);

    languageGroup->setLayout(languageLayout);

    QLabel *note = new QLabel(QObject::tr("Changes take effect on next startup" ));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(languageGroup);
    languageLayout->addSpacing(12*ratio);
    mainLayout->addStretch(1);
    mainLayout->addWidget(note);
    setLayout(mainLayout);
}

StylePage::StylePage(QWidget *parent)
    : QWidget(parent)
{
    Preferences *preferences = Preferences::Instance();
    QString setting = preferences->getPreferences("Scheme", "Style", "select");
    bool ok;
    int choice = setting.toInt(&ok, 16);
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    QGroupBox *styleGroup = new QGroupBox(QObject::tr("Window Style Selection"));

    QRadioButton *standardButton = new QRadioButton(QObject::tr("System"));
    this->standardButton = standardButton;
    this->plastiqueButton = new QRadioButton(QObject::tr("Plastique"));
    this->cdeButton = new QRadioButton(QObject::tr("CDE"));
    this->motifButton = new QRadioButton(QObject::tr("Motif"));
    if (choice == 3) {motifButton->setChecked(true); }
    else if (choice == 2) {cdeButton->setChecked(true); }
    else if (choice == 1) {plastiqueButton->setChecked(true); }
    else {standardButton->setChecked(true); };

    QVBoxLayout *styleLayout = new QVBoxLayout;
    styleLayout->addWidget(standardButton);
    styleLayout->addWidget(plastiqueButton);
    styleLayout->addWidget(cdeButton);
    styleLayout->addWidget(motifButton);

    styleGroup->setLayout(styleLayout);


    setting = preferences->getPreferences("Scheme", "Colour", "select");
    choice = setting.toInt(&ok, 16);
    QGroupBox *colourGroup = new QGroupBox(QObject::tr("Colour Scheme Selection"));

    QRadioButton *blackButton = new QRadioButton(QObject::tr("KATANA (default)"));
    this->blackButton = blackButton;
    this->whiteButton = new QRadioButton(QObject::tr("White"));
    this->blueButton = new QRadioButton(QObject::tr("Blue"));
    this->greenButton = new QRadioButton(QObject::tr("Green"));
    this->systemButton = new QRadioButton(QObject::tr("System"));
    if (choice == 4) {systemButton->setChecked(true); }
    else if (choice == 3) {greenButton->setChecked(true); }
    else if (choice == 2) {blueButton->setChecked(true); }
    else if (choice == 1) {whiteButton->setChecked(true); }
    else {blackButton->setChecked(true); };

    QVBoxLayout *colourLayout = new QVBoxLayout;
    colourLayout->addWidget(blackButton);
    colourLayout->addWidget(whiteButton);
    colourLayout->addWidget(blueButton);
    colourLayout->addWidget(greenButton);
    colourLayout->addWidget(systemButton);

    colourGroup->setLayout(colourLayout);

    QLabel *note = new QLabel(QObject::tr("Changes take effect on next startup" ));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(styleGroup);
    mainLayout->addWidget(colourGroup);
    styleLayout->addSpacing(12*ratio);
    mainLayout->addStretch(1);
    mainLayout->addWidget(note);
    setLayout(mainLayout);
}

void GeneralPage::browseDir()
{
    QString dirName = QFileDialog::getExistingDirectory(this, QObject::tr("Select the default folder for storing patches."),
                                                        this->dirEdit->text(),
                                                        QFileDialog::ShowDirsOnly);
    if(!dirName.isEmpty())
    {
        this->dirEdit->setText(dirName);
    };
}
