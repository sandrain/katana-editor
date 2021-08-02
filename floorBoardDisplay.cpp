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

#include <QChar>
#include "floorBoardDisplay.h"
#include "Preferences.h"
#include "MidiTable.h"
#include "SysxIO.h"
#include "midiIO.h"
#include "renameWidget.h"
#include "customRenameWidget.h"
#include "customComboBox.h"
#include "globalVariables.h"



// Platform-dependent sleep routines.
#ifdef Q_OS_WIN
#include <windows.h>
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds )
#else // Unix variants
#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif


floorBoardDisplay::floorBoardDisplay(QWidget *parent, QPoint pos)
    : QWidget(parent)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);
    const double fratio = preferences->getPreferences("Window", "Font", "ratio").toDouble(&ok);
    QFont Lfont( "Arial", 11*fratio, QFont::Bold);
    QFont Mfont( "Arial", 10*fratio, QFont::Bold);
    
    this->pos = pos;
    this->timer = new QTimer(this);
    this->patchLoadError = false;
    this->blinkCount = 0;

    this->autosyncTimer = new QTimer(this);
    connect(autosyncTimer, SIGNAL(timeout()), this, SLOT(autosyncGo()));

    int patchDisplayRowOffset = 5*ratio;
    int editButtonRowOffset = 70*ratio;
    int assignButtonRowOffset = 418*ratio;
    int tempRowOffset = 495*ratio;
    //int bottomOffset = 495*ratio;
    int horizontalOffset = 3*ratio;
    this->patchNumDisplay = new customDisplay(QRect(7*ratio, patchDisplayRowOffset, 55*ratio, 34*ratio), this);
    this->patchNumDisplay->setObjectName("display");
    this->patchNumDisplay->setLabelPosition(true);
    this->patchNumDisplay->setMainObjectName("bankMain");
    this->patchNumDisplay->setSubObjectName("bankSub");
    this->patchNumDisplay->setWhatsThis(tr("Patch Number Display.<br>displays the currently selected patch<br>and patch write memory location."));
    this->patchDisplay = new customDisplay(QRect(70*ratio, patchDisplayRowOffset, 150*ratio, 34*ratio), this);
    this->patchDisplay->setObjectName("display");
    this->patchDisplay->setLabelPosition(false);
    this->patchDisplay->setMainObjectName("nameMain");
    this->patchDisplay->setSubObjectName("nameSub");
    this->valueDisplay = new customDisplay(QRect(230*ratio, patchDisplayRowOffset, 150*ratio, 34*ratio), this);
    this->valueDisplay->setMainObjectName("valueMain");
    this->valueDisplay->setSubObjectName("valueSub");

    QString version = preferences->getPreferences("General", "Application", "version");
    this->patchDisplay->setMainText(tr("KatanaFloorBoard"));
    this->patchDisplay->setSubText(tr("version"), version);

    initPatch = new initPatchListMenu(QRect(390*ratio, patchDisplayRowOffset+(19*ratio), 168*ratio, 15*ratio), this);
    initPatch->setWhatsThis(tr("Clicking on this will load a patch from a predefined selection.<br>patches place in the Init Patches folder will appear in this list at the start of the next session."));
    initPatch->setFont(Mfont);
    renameWidget *nameEdit = new renameWidget(this);
    nameEdit->setGeometry(70*ratio, patchDisplayRowOffset, 150*ratio, 34*ratio);
    nameEdit->setWhatsThis(tr("Clicking on this will open<br>a text dialog window<br>allowing user text input."));
    customRenameWidget *userDialog = new customRenameWidget(this, "0B", "00", "60", "Structure", "20");
    userDialog->setGeometry(660*ratio, patchDisplayRowOffset, 280*ratio, 25*ratio);
    userDialog->setFont(Lfont);
    userDialog->setWhatsThis(tr("Clicking on this will open<br>a text dialog window<br>allowing user text input."));
    /*customRenameWidget *patchDialog = new customRenameWidget(this, "0C", "00", "00", "Structure", "80");
    patchDialog->setGeometry(10*ratio, bottomOffset, 1100*ratio, 25*ratio);
    patchDialog->setWhatsThis(tr("Clicking on this will open<br>a text dialog window<br>allowing user text input."));*/

    this->catagory = new customControlListMenu(this, "07", "00", "0F", "right");
    catagory->setGeometry(950*ratio, patchDisplayRowOffset/*+(19*ratio)*/, 180*ratio, 30*ratio);
    catagory->setWhatsThis(tr("This is the tone style Category grouping that the current patch is associated with."));

    // Draw Logo
    logo_state = false;
    this->logo = new customLogo(logo_state, QPoint(0, 0), this);
    this->logo->move(586*ratio, 371*ratio);


    this->autoButton = new customButton(tr("Auto Sync"), false, QPoint(570*ratio, patchDisplayRowOffset), this, ":/images/greenledbutton.png");
    this->autoButton->setWhatsThis(tr("Auto refresh<br>used to automatically update editor settings changes made on the KATANA Fx"));

    this->connectButton = new customButton(tr("Connect"), false, QPoint(387*ratio, patchDisplayRowOffset), this, ":/images/greenledbutton.png");
    this->connectButton->setWhatsThis(tr("Connect Button<br>used to establish a continuous midi connection<br>when lit green, the connection is valid"));

    this->writeButton = new customButton(tr("Write/Sync"), false, QPoint(479*ratio, patchDisplayRowOffset), this, ":/images/ledbutton.png");
    this->writeButton->setWhatsThis(tr("Write/Sync Button<br>if the patch number displays [temp buffer]<br>the current patch is sent to the GT temporary memory only<br>or else the patch will be written to the displayed patch memory location."));


    this->pedal_Button = new customPanelButton(tr("PEDAL Fx"), false, QPoint(horizontalOffset + (20*ratio), editButtonRowOffset), this, ":/images/switch.png");
    this->pedal_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->distortion_Button = new customPanelButton(tr("BOOSTER"), false, QPoint(horizontalOffset + (95*ratio),editButtonRowOffset), this, ":/images/switch.png");
    this->distortion_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->fx1_Button = new customPanelButton(tr("MOD"), false, QPoint(horizontalOffset + (170*ratio), editButtonRowOffset), this, ":/images/switch.png");
    this->fx1_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->preamp1_Button = new customPanelButton(tr("PREAMP"), false, QPoint(horizontalOffset + (245*ratio), editButtonRowOffset), this, ":/images/switch.png");
    this->preamp1_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->ns1_Button = new customPanelButton(tr("NOISE.SUPR"), false, QPoint(horizontalOffset + (320*ratio), editButtonRowOffset), this, ":/images/switch.png");
    this->ns1_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->fv_Button = new customPanelButton(tr("MASTER/FV"), false, QPoint(horizontalOffset + (395*ratio), editButtonRowOffset), this, ":/images/switch.png");
    this->fv_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->eq_Button = new customPanelButton(tr("EQ"), false, QPoint(horizontalOffset + (470*ratio), editButtonRowOffset), this, ":/images/switch.png");
    this->eq_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->sendreturn_Button = new customPanelButton(tr("S/R LOOP"), false, QPoint(horizontalOffset + (545*ratio), editButtonRowOffset), this, ":/images/switch.png");
    this->sendreturn_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    if(preferences->getPreferences("Midi", "Katana", "model").toInt(&ok)==0){this->sendreturn_Button->hide();this->sendreturn_Button->textLabel->setText(""); };

    this->delay_Button = new customPanelButton(tr("DELAY 1"), false, QPoint(horizontalOffset + (620*ratio), editButtonRowOffset), this, ":/images/switch.png");
    this->delay_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->fx2_Button = new customPanelButton(tr("FX"), false, QPoint(horizontalOffset + (695*ratio), editButtonRowOffset), this, ":/images/switch.png");
    this->fx2_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->ce_Button = new customPanelButton(tr("DELAY 2"), false, QPoint(horizontalOffset + (770*ratio), editButtonRowOffset), this, ":/images/switch.png");
    this->ce_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->reverb_Button = new customPanelButton(tr("REVERB"), false, QPoint(horizontalOffset + (845*ratio), editButtonRowOffset), this, ":/images/switch.png");
    this->reverb_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->master_Button = new customPanelButton(tr("ASSIGN"), false, QPoint(horizontalOffset + (920*ratio), editButtonRowOffset), this, ":/images/switch.png");
    this->master_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->ez_edit_Button = new customPanelButton(tr("EZ-TONE"), false, QPoint(horizontalOffset + (1000*ratio), editButtonRowOffset), this, ":/images/switch.png");
    this->ez_edit_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->midi_Button = new customPanelButton(tr("GRY OPTION"), false, QPoint(horizontalOffset + (1100*ratio), editButtonRowOffset), this, ":/images/switch.png");
    this->midi_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->system_Button = new customPanelButton(tr("SYSTEM"), false, QPoint(horizontalOffset + (1180*ratio), editButtonRowOffset), this, ":/images/switch.png");
    this->system_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));


    this->chain_1_Button = new customPanelButton(tr("CHAIN 1"), false, QPoint(horizontalOffset + (290*ratio), editButtonRowOffset+(95*ratio)), this, ":/images/switch.png");
    this->chain_1_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->chain_2_Button = new customPanelButton(tr("CHAIN 2"), false, QPoint(horizontalOffset + (360*ratio), editButtonRowOffset+(95*ratio)), this, ":/images/switch.png");
    this->chain_2_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->chain_3_Button = new customPanelButton(tr("CHAIN 3"), false, QPoint(horizontalOffset + (430*ratio), editButtonRowOffset+(95*ratio)), this, ":/images/switch.png");
    this->chain_3_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->chain_4_Button = new customPanelButton(tr("S/R - post"), false, QPoint(horizontalOffset + (940*ratio), editButtonRowOffset+(95*ratio)), this, ":/images/switch.png");
    this->chain_4_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
    if(preferences->getPreferences("Midi", "Katana", "model").toInt(&ok)==0){this->chain_4_Button->hide(); this->chain_4_Button->textLabel->setText(""); };

    this->chain_5_Button = new customPanelButton(tr("EQ - pre"), false, QPoint(horizontalOffset + (870*ratio), editButtonRowOffset+(95*ratio)), this, ":/images/switch.png");
    this->chain_5_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->chain_6_Button = new customPanelButton(tr("PEDAL - pre"), false, QPoint(horizontalOffset + (800*ratio), editButtonRowOffset+(95*ratio)), this, ":/images/switch.png");
    this->chain_6_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));

    this->chain_txt = new QLineEdit(this);
    this->chain_txt->setObjectName("editdisplay");
    QFont Sfont( "Arial", 9*fratio, QFont::Bold);
    this->chain_txt->setFont(Sfont);
    this->chain_txt->setFixedWidth(280*ratio);
    this->chain_txt->setFixedHeight(20*ratio);
    this->chain_txt->setAlignment(Qt::AlignCenter);
    this->chain_txt->setDisabled(true);
    this->chain_txt->setText("Drag and Drop StompBoxes for custom chain order");
    this->chain_txt->move(490*ratio, 170*ratio);

    this->temp1_copy_Button = new customButton(tr("Patch Copy"), false, QPoint(35*ratio, tempRowOffset), this, ":/images/pushbutton.png");
    this->temp1_copy_Button->setWhatsThis(tr("Copy current patch to clipboard<br>pressing this button will save the current patch to a clipboard<br>the clipboard is saved to file and is re-loaded<br>on the next session startup."));
    this->temp1_paste_Button = new customButton(tr("Patch Paste"), false, QPoint(125*ratio, tempRowOffset), this, ":/images/pushbutton.png");
    this->temp1_paste_Button->setWhatsThis(tr("Paste current patch from clipboard<br>pressing this button will load the current patch to a clipboard<br>the clipboard is re-loaded<br>from the previous session copy."));
    this->temp1Display = new customLabelDisplay(QRect(35*ratio, tempRowOffset+(22*ratio), 170*ratio, 18*ratio), this);
    this->temp1Display->setLabelPosition(true);
    this->temp1Display->setMainObjectName("nameMain");
    this->temp1Display->setMainText(tr("Empty"), Qt::AlignCenter);
    this->temp1Display->setWhatsThis(tr("Name of the currently stored patch in the clipboard."));
    this->temp1Display->setFont(Lfont);

    this->temp2_copy_Button = new customButton(tr("Patch Copy"), false, QPoint(290*ratio, tempRowOffset), this, ":/images/pushbutton.png");
    this->temp2_copy_Button->setWhatsThis(tr("Copy current patch to clipboard<br>pressing this button will save the current patch to a clipboard<br>the clipboard is saved to file and is re-loaded<br>on the next session startup."));
    this->temp2_paste_Button = new customButton(tr("Patch Paste"), false, QPoint(380*ratio, tempRowOffset), this, ":/images/pushbutton.png");
    this->temp2_paste_Button->setWhatsThis(tr("Paste current patch from clipboard<br>pressing this button will load the current patch to a clipboard<br>the clipboard is re-loaded<br>from the previous session copy."));
    this->temp2Display = new customLabelDisplay(QRect(290*ratio, tempRowOffset+(22*ratio), 170*ratio, 18*ratio), this);
    this->temp2Display->setLabelPosition(true);
    this->temp2Display->setMainObjectName("nameMain");
    this->temp2Display->setMainText(tr("Empty"), Qt::AlignCenter);
    this->temp2Display->setWhatsThis(tr("Name of the currently stored patch in the clipboard."));
    this->temp2Display->setFont(Lfont);

    this->temp3_copy_Button = new customButton(tr("Patch Copy"), false, QPoint(550*ratio, tempRowOffset), this, ":/images/pushbutton.png");
    this->temp3_copy_Button->setWhatsThis(tr("Copy current patch to clipboard<br>pressing this button will save the current patch to a clipboard<br>the clipboard is saved to file and is re-loaded<br>on the next session startup."));
    this->temp3_paste_Button = new customButton(tr("Patch Paste"), false, QPoint(640*ratio, tempRowOffset), this, ":/images/pushbutton.png");
    this->temp3_paste_Button->setWhatsThis(tr("Paste current patch from clipboard<br>pressing this button will load the current patch to a clipboard<br>the clipboard is re-loaded<br>from the previous session copy."));
    this->temp3Display = new customLabelDisplay(QRect(550*ratio, tempRowOffset+(22*ratio), 170*ratio, 18*ratio), this);
    this->temp3Display->setLabelPosition(true);
    this->temp3Display->setMainObjectName("nameMain");
    this->temp3Display->setMainText(tr("Empty"), Qt::AlignCenter);
    this->temp3Display->setWhatsThis(tr("Name of the currently stored patch in the clipboard."));
    this->temp3Display->setFont(Lfont);

    this->temp4_copy_Button = new customButton(tr("Patch Copy"), false, QPoint(805*ratio, tempRowOffset), this, ":/images/pushbutton.png");
    this->temp4_copy_Button->setWhatsThis(tr("Copy current patch to clipboard<br>pressing this button will save the current patch to a clipboard<br>the clipboard is saved to file and is re-loaded<br>on the next session startup."));
    this->temp4_paste_Button = new customButton(tr("Patch Paste"), false, QPoint(895*ratio, tempRowOffset), this, ":/images/pushbutton.png");
    this->temp4_paste_Button->setWhatsThis(tr("Paste current patch from clipboard<br>pressing this button will load the current patch to a clipboard<br>the clipboard is re-loaded<br>from the previous session copy."));
    this->temp4Display = new customLabelDisplay(QRect(805*ratio, tempRowOffset+(22*ratio), 170*ratio, 18*ratio), this);
    this->temp4Display->setLabelPosition(true);
    this->temp4Display->setMainObjectName("nameMain");
    this->temp4Display->setMainText(tr("Empty"), Qt::AlignCenter);
    this->temp4Display->setWhatsThis(tr("Name of the currently stored patch in the clipboard."));
    this->temp4Display->setFont(Lfont);

    this->temp5_copy_Button = new customButton(tr("Patch Copy"), false, QPoint(1055*ratio, tempRowOffset), this, ":/images/pushbutton.png");
    this->temp5_copy_Button->setWhatsThis(tr("Copy current patch to clipboard<br>pressing this button will save the current patch to a clipboard<br>the clipboard is saved to file and is re-loaded<br>on the next session startup."));
    this->temp5_paste_Button = new customButton(tr("Patch Paste"), false, QPoint(1145*ratio, tempRowOffset), this, ":/images/pushbutton.png");
    this->temp5_paste_Button->setWhatsThis(tr("Paste current patch from clipboard<br>pressing this button will load the current patch to a clipboard<br>the clipboard is re-loaded<br>from the previous session copy."));
    this->temp5Display = new customLabelDisplay(QRect(1055*ratio, tempRowOffset+(22*ratio), 170*ratio, 18*ratio), this);
    this->temp5Display->setLabelPosition(true);
    this->temp5Display->setMainObjectName("nameMain");
    this->temp5Display->setMainText(tr("Empty"), Qt::AlignCenter);
    this->temp5Display->setWhatsThis(tr("Name of the currently stored patch in the clipboard."));
    this->temp5Display->setFont(Lfont);

    // Draw AssignBar
    this->assignBar = new customAssign(logo_state, QPoint(0, 0), this);
    this->assignBar->move(85*ratio, 400*ratio);

    this->assignBar2 = new customAssign(logo_state, QPoint(0, 0), this);
    this->assignBar2->move(700*ratio, 400*ratio);

    if( preferences->getPreferences("General", "Loader", "active") == "true"){
        assignBar->show();
        assignBar2->show();
    }else{
        assignBar->hide();
        assignBar2->hide(); };

    if( preferences->getPreferences("General", "Loader", "active") == "true")
    {
        this->assign1_Button = new customPanelButton(("1"), false, QPoint(horizontalOffset + (90*ratio), assignButtonRowOffset), this, ":/images/switch.png");
        this->assign1_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
        QObject::connect(this->assign1_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(assign1_buttonSignal(bool)));
        QObject::connect(this->parent(), SIGNAL(assign1_statusSignal(bool)), this->assign1_Button, SLOT(setValue(bool)));

        this->assign2_Button = new customPanelButton(("2"), false, QPoint(horizontalOffset + (150*ratio), assignButtonRowOffset), this, ":/images/switch.png");
        this->assign2_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
        QObject::connect(this->assign2_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(assign2_buttonSignal(bool)));
        QObject::connect(this->parent(), SIGNAL(assign2_statusSignal(bool)), this->assign2_Button, SLOT(setValue(bool)));

        this->assign3_Button = new customPanelButton(("3"), false, QPoint(horizontalOffset + (210*ratio), assignButtonRowOffset), this, ":/images/switch.png");
        this->assign3_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
        QObject::connect(this->assign3_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(assign3_buttonSignal(bool)));
        QObject::connect(this->parent(), SIGNAL(assign3_statusSignal(bool)), this->assign3_Button, SLOT(setValue(bool)));

        this->assign4_Button = new customPanelButton(("4"), false, QPoint(horizontalOffset + (270*ratio), assignButtonRowOffset), this, ":/images/switch.png");
        this->assign4_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
        QObject::connect(this->assign4_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(assign4_buttonSignal(bool)));
        QObject::connect(this->parent(), SIGNAL(assign4_statusSignal(bool)), this->assign4_Button, SLOT(setValue(bool)));

        this->assign5_Button = new customPanelButton(("5"), false, QPoint(horizontalOffset + (330*ratio), assignButtonRowOffset), this, ":/images/switch.png");
        this->assign5_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
        QObject::connect(this->assign5_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(assign5_buttonSignal(bool)));
        QObject::connect(this->parent(), SIGNAL(assign5_statusSignal(bool)), this->assign5_Button, SLOT(setValue(bool)));

        this->assign6_Button = new customPanelButton(("6"), false, QPoint(horizontalOffset + (390*ratio), assignButtonRowOffset), this, ":/images/switch.png");
        this->assign6_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
        QObject::connect(this->assign6_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(assign6_buttonSignal(bool)));
        QObject::connect(this->parent(), SIGNAL(assign6_statusSignal(bool)), this->assign6_Button, SLOT(setValue(bool)));

        this->assign7_Button = new customPanelButton(("7"), false, QPoint(horizontalOffset + (450*ratio), assignButtonRowOffset), this, ":/images/switch.png");
        this->assign7_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
        QObject::connect(this->assign7_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(assign7_buttonSignal(bool)));
        QObject::connect(this->parent(), SIGNAL(assign7_statusSignal(bool)), this->assign7_Button, SLOT(setValue(bool)));

        this->assign8_Button = new customPanelButton(("8"), false, QPoint(horizontalOffset + (510*ratio), assignButtonRowOffset), this, ":/images/switch.png");
        this->assign8_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
        QObject::connect(this->assign8_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(assign8_buttonSignal(bool)));
        QObject::connect(this->parent(), SIGNAL(assign8_statusSignal(bool)), this->assign8_Button, SLOT(setValue(bool)));

        this->assign9_Button = new customPanelButton(tr("9"), false, QPoint(horizontalOffset + (707*ratio), assignButtonRowOffset), this, ":/images/switch.png");
        this->assign9_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
        QObject::connect(this->assign9_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(assign9_buttonSignal(bool)));
        QObject::connect(this->parent(), SIGNAL(assign9_statusSignal(bool)), this->assign9_Button, SLOT(setValue(bool)));

        this->assign10_Button = new customPanelButton(tr("10"), false, QPoint(horizontalOffset + (767*ratio), assignButtonRowOffset), this, ":/images/switch.png");
        this->assign10_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
        QObject::connect(this->assign10_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(assign10_buttonSignal(bool)));
        QObject::connect(this->parent(), SIGNAL(assign10_statusSignal(bool)), this->assign10_Button, SLOT(setValue(bool)));

        this->assign11_Button = new customPanelButton(tr("11"), false, QPoint(horizontalOffset + (827*ratio), assignButtonRowOffset), this, ":/images/switch.png");
        this->assign11_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
        QObject::connect(this->assign11_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(assign11_buttonSignal(bool)));
        QObject::connect(this->parent(), SIGNAL(assign11_statusSignal(bool)), this->assign11_Button, SLOT(setValue(bool)));

        this->assign12_Button = new customPanelButton(tr("12"), false, QPoint(horizontalOffset + (887*ratio), assignButtonRowOffset), this, ":/images/switch.png");
        this->assign12_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
        QObject::connect(this->assign12_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(assign12_buttonSignal(bool)));
        QObject::connect(this->parent(), SIGNAL(assign12_statusSignal(bool)), this->assign12_Button, SLOT(setValue(bool)));

        this->assign13_Button = new customPanelButton(tr("13"), false, QPoint(horizontalOffset + (947*ratio), assignButtonRowOffset), this, ":/images/switch.png");
        this->assign13_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
        QObject::connect(this->assign13_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(assign13_buttonSignal(bool)));
        QObject::connect(this->parent(), SIGNAL(assign13_statusSignal(bool)), this->assign13_Button, SLOT(setValue(bool)));

        this->assign14_Button = new customPanelButton(tr("14"), false, QPoint(horizontalOffset + (1007*ratio), assignButtonRowOffset), this, ":/images/switch.png");
        this->assign14_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
        QObject::connect(this->assign14_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(assign14_buttonSignal(bool)));
        QObject::connect(this->parent(), SIGNAL(assign14_statusSignal(bool)), this->assign14_Button, SLOT(setValue(bool)));

        this->assign15_Button = new customPanelButton(tr("15"), false, QPoint(horizontalOffset + (1067*ratio), assignButtonRowOffset), this, ":/images/switch.png");
        this->assign15_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
        QObject::connect(this->assign15_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(assign15_buttonSignal(bool)));
        QObject::connect(this->parent(), SIGNAL(assign15_statusSignal(bool)), this->assign15_Button, SLOT(setValue(bool)));

        this->assign16_Button = new customPanelButton(tr("16"), false, QPoint(horizontalOffset + (1127*ratio), assignButtonRowOffset), this, ":/images/switch.png");
        this->assign16_Button->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));
        QObject::connect(this->assign16_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(assign16_buttonSignal(bool)));
        QObject::connect(this->parent(), SIGNAL(assign16_statusSignal(bool)), this->assign16_Button, SLOT(setValue(bool)));

        this->assign1_Button->hide();
        this->assign1_Button->textLabel->hide();
        this->assign2_Button->hide();
        this->assign2_Button->textLabel->hide();
        this->assign3_Button->hide();
        this->assign3_Button->textLabel->hide();
        this->assign4_Button->hide();
        this->assign4_Button->textLabel->hide();
        this->assign5_Button->hide();
        this->assign5_Button->textLabel->hide();
        this->assign6_Button->hide();
        this->assign6_Button->textLabel->hide();
        this->assign7_Button->hide();
        this->assign7_Button->textLabel->hide();
        this->assign8_Button->hide();
        this->assign8_Button->textLabel->hide();
        this->assign9_Button->hide();
        this->assign9_Button->textLabel->hide();
        this->assign10_Button->hide();
        this->assign10_Button->textLabel->hide();
        this->assign11_Button->hide();
        this->assign11_Button->textLabel->hide();
        this->assign12_Button->hide();
        this->assign12_Button->textLabel->hide();
        this->assign13_Button->hide();
        this->assign13_Button->textLabel->hide();
        this->assign14_Button->hide();
        this->assign14_Button->textLabel->hide();
        this->assign15_Button->hide();
        this->assign15_Button->textLabel->hide();
        this->assign16_Button->hide();
        this->assign16_Button->textLabel->hide();
    };

    SysxIO *sysxIO = SysxIO::Instance();
    QObject::connect(this, SIGNAL(setStatusSymbol(int)), sysxIO, SIGNAL(setStatusSymbol(int)));
    QObject::connect(this, SIGNAL(setStatusProgress(int)), sysxIO, SIGNAL(setStatusProgress(int)));
    QObject::connect(this, SIGNAL(setStatusMessage(QString)), sysxIO, SIGNAL(setStatusMessage(QString)));

    QObject::connect(sysxIO, SIGNAL(notConnectedSignal()), this, SLOT(notConnected()));
    QObject::connect(this, SIGNAL(notConnectedSignal()), this, SLOT(notConnected()));

    QObject::connect(this->parent(), SIGNAL(updateSignal()), this, SLOT(updateDisplay()));
    QObject::connect(this, SIGNAL(updateSignal()), this->parent(), SIGNAL(updateSignal()));

    QObject::connect(this->connectButton, SIGNAL(valueChanged(bool)), this, SLOT(connectSignal(bool)));

    QObject::connect(this->writeButton, SIGNAL(valueChanged(bool)), this, SLOT(writeSignal(bool)));
    QObject::connect(this->autoButton, SIGNAL(valueChanged(bool)), this, SLOT(autosyncSignal(bool)));

    QObject::connect(this->preamp1_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(preamp1_buttonSignal(bool)));
    QObject::connect(this->distortion_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(distortion_buttonSignal(bool)));
    QObject::connect(this->ns1_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(ns1_buttonSignal(bool)));
    QObject::connect(this->eq_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(eq_buttonSignal(bool)));
    QObject::connect(this->ce_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(ce_buttonSignal(bool)));
    QObject::connect(this->fx1_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(fx1_buttonSignal(bool)));
    QObject::connect(this->fx2_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(fx2_buttonSignal(bool)));
    QObject::connect(this->reverb_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(reverb_buttonSignal(bool)));
    QObject::connect(this->delay_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(delay_buttonSignal(bool)));
    QObject::connect(this->sendreturn_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(sendreturn_buttonSignal(bool)));
    QObject::connect(this->pedal_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(pedal_buttonSignal(bool)));
    QObject::connect(this->fv_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(fv_buttonSignal(bool)));
    QObject::connect(this->master_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(master_buttonSignal(bool)));
    QObject::connect(this->ez_edit_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(ez_edit_buttonSignal(bool)));
    QObject::connect(this->system_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(system_buttonSignal(bool)));
    QObject::connect(this->midi_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(midi_buttonSignal(bool)));
    QObject::connect(this->chain_1_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(chain_1_buttonSignal(bool)));
    QObject::connect(this->chain_2_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(chain_2_buttonSignal(bool)));
    QObject::connect(this->chain_3_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(chain_3_buttonSignal(bool)));
    QObject::connect(this->chain_4_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(chain_4_buttonSignal(bool)));
    QObject::connect(this->chain_5_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(chain_5_buttonSignal(bool)));
    QObject::connect(this->chain_6_Button, SIGNAL(valueChanged(bool)), this->parent(), SIGNAL(chain_6_buttonSignal(bool)));

    QObject::connect(this->parent(), SIGNAL(preamp1_statusSignal(bool)), this->preamp1_Button, SLOT(setValue(bool)));
    QObject::connect(this->parent(), SIGNAL(distortion_statusSignal(bool)), this->distortion_Button, SLOT(setValue(bool)));
    QObject::connect(this->parent(), SIGNAL(ns1_statusSignal(bool)), this->ns1_Button, SLOT(setValue(bool)));
    QObject::connect(this->parent(), SIGNAL(eq_statusSignal(bool)), this->eq_Button, SLOT(setValue(bool)));
    QObject::connect(this->parent(), SIGNAL(ce_statusSignal(bool)), this->ce_Button, SLOT(setValue(bool)));
    QObject::connect(this->parent(), SIGNAL(fx1_statusSignal(bool)), this->fx1_Button, SLOT(setValue(bool)));
    QObject::connect(this->parent(), SIGNAL(fx2_statusSignal(bool)), this->fx2_Button, SLOT(setValue(bool)));
    QObject::connect(this->parent(), SIGNAL(reverb_statusSignal(bool)), this->reverb_Button, SLOT(setValue(bool)));
    QObject::connect(this->parent(), SIGNAL(delay_statusSignal(bool)), this->delay_Button, SLOT(setValue(bool)));
    QObject::connect(this->parent(), SIGNAL(sendreturn_statusSignal(bool)), this->sendreturn_Button, SLOT(setValue(bool)));
    QObject::connect(this->parent(), SIGNAL(pedal_statusSignal(bool)), this->pedal_Button, SLOT(setValue(bool)));
    QObject::connect(this->parent(), SIGNAL(fv_statusSignal(bool)), this->fv_Button, SLOT(setValue(bool)));
    QObject::connect(this->parent(), SIGNAL(master_statusSignal(bool)), this->master_Button, SLOT(setValue(bool)));
    QObject::connect(this->parent(), SIGNAL(system_statusSignal(bool)), this->system_Button, SLOT(setValue(bool)));
    QObject::connect(this->parent(), SIGNAL(midi_statusSignal(bool)), this->midi_Button, SLOT(setValue(bool)));
    QObject::connect(this->parent(), SIGNAL(chain_1_statusSignal(bool)), this->chain_1_Button, SLOT(setValue(bool)));
    QObject::connect(this->parent(), SIGNAL(chain_2_statusSignal(bool)), this->chain_2_Button, SLOT(setValue(bool)));
    QObject::connect(this->parent(), SIGNAL(chain_3_statusSignal(bool)), this->chain_3_Button, SLOT(setValue(bool)));
    QObject::connect(this->parent(), SIGNAL(chain_4_statusSignal(bool)), this->chain_4_Button, SLOT(setValue(bool)));
    QObject::connect(this->parent(), SIGNAL(chain_5_statusSignal(bool)), this->chain_5_Button, SLOT(setValue(bool)));
    QObject::connect(this->parent(), SIGNAL(chain_6_statusSignal(bool)), this->chain_6_Button, SLOT(setValue(bool)));

    QObject::connect(this->temp1_copy_Button, SIGNAL(valueChanged(bool)),  this, SLOT(temp1_copy(bool)));
    QObject::connect(this->temp1_paste_Button, SIGNAL(valueChanged(bool)), this, SLOT(temp1_paste(bool)));
    QObject::connect(this->temp2_copy_Button, SIGNAL(valueChanged(bool)),  this, SLOT(temp2_copy(bool)));
    QObject::connect(this->temp2_paste_Button, SIGNAL(valueChanged(bool)), this, SLOT(temp2_paste(bool)));
    QObject::connect(this->temp3_copy_Button, SIGNAL(valueChanged(bool)),  this, SLOT(temp3_copy(bool)));
    QObject::connect(this->temp3_paste_Button, SIGNAL(valueChanged(bool)), this, SLOT(temp3_paste(bool)));
    QObject::connect(this->temp4_copy_Button, SIGNAL(valueChanged(bool)),  this, SLOT(temp4_copy(bool)));
    QObject::connect(this->temp4_paste_Button, SIGNAL(valueChanged(bool)), this, SLOT(temp4_paste(bool)));
    QObject::connect(this->temp5_copy_Button, SIGNAL(valueChanged(bool)),  this, SLOT(temp5_copy(bool)));
    QObject::connect(this->temp5_paste_Button, SIGNAL(valueChanged(bool)), this, SLOT(temp5_paste(bool)));

    QObject::connect(this->parent(), SIGNAL(valueChanged(QString, QString, QString)), this, SLOT(valueUpdate()));

    QObject::connect(sysxIO, SIGNAL(midiReply(QString)), this, SLOT(midiInput(QString)));

    QObject::connect(sysxIO, SIGNAL(rxRestart(int)), this, SLOT(kickTimer(int)));

    set_temp();

    QString device = preferences->getPreferences("Midi", "Device", "bool");
    midiIO *midi = new midiIO();
    QList<QString> midiInDevices = midi->getMidiInDevices();
    QList<QString> midiOutDevices = midi->getMidiOutDevices();
    if( midiInDevices.contains("KATANA") && midiOutDevices.contains("KATANA") && device=="true")
        {
            QString inputDevice = QString::number(midiInDevices.indexOf("KATANA") + 1, 10);
             preferences->setPreferences("Midi", "MidiIn", "device", inputDevice);
            QString outputDevice = QString::number(midiOutDevices.indexOf("KATANA") + 1, 10);
              preferences->setPreferences("Midi", "MidiOut", "device", outputDevice);
              autoconnect();
        }
    else
    {
        QMessageBox *msgBox = new QMessageBox();
        msgBox->isTopLevel();
        msgBox->setWindowTitle(tr("KATANAFxFloorBoard"));
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setTextFormat(Qt::RichText);
        QString msgText;
        msgText.append("<font size='+1'><b>");
        msgText.append(tr("Midi device not selected in Preferences Menu."));
        msgText.append("<b></font><br>");
        msgText.append(tr("Select the KATANA or a midi adapter from the Preferences USB/Midi menu<br> and press the 'Connect' button"));
        msgBox->setText(msgText);
        msgBox->show();
        QTimer::singleShot(5000, msgBox, SLOT(deleteLater()));
    };

    this->midisyncTimer = new QTimer(this);
    connect(midisyncTimer, SIGNAL(timeout()), sysxIO, SLOT(readMidiInput()));
    midisyncTimer->start(1);

    //QMessageBox *deBugBox = new QMessageBox();
}

QPoint floorBoardDisplay::getPos()
{
    return this->pos;
}

void floorBoardDisplay::setPos(QPoint newPos)
{
    this->move(newPos);
    this->pos = newPos;
}

void floorBoardDisplay::setValueDisplay(QString fxName, QString valueName, QString value)
{
    this->valueDisplay->setMainText(fxName);
    this->valueDisplay->setSubText(valueName, value);
}

void floorBoardDisplay::setPatchDisplay(QString patchName)
{
    SysxIO *sysxIO = SysxIO::Instance();
    if(sysxIO->getFileName() != ":default.syx") // Keep the initial version display if the default.syx is loaded at startup.
    {
        QString fileName = sysxIO->getFileName();
        this->patchDisplay->setMainText(patchName);
        this->patchDisplay->setSubText(fileName.section('/', -1, -1));
        this->patchName = patchName;
    };
    if(sysxIO->getFileName() == tr("init patch") || sysxIO->getFileName() == ":default.syx")
    {
        sysxIO->setFileName("");
        this->patchName = tr("Empty");
    }
    else
    {
        if(sysxIO->getFileName() == deviceType + tr(" patch"))
        {
            sysxIO->setFileName("");
            if(this->patchLoadError)
            {
                QMessageBox *msgBox = new QMessageBox();
                msgBox->setWindowTitle(tr("KATANAFxFloorBoard"));
                msgBox->setIcon(QMessageBox::Warning);
                msgBox->setTextFormat(Qt::RichText);
                QString msgText;
                msgText.append("<font size='+1'><b>");
                msgText.append(tr("Error while changing banks."));
                msgText.append("<b></font><br>");
                msgText.append(tr("An incorrect patch has been loaded. Please try to load the patch again."));
                msgBox->setText(msgText);
                msgBox->show();
                QTimer::singleShot(3000, msgBox, SLOT(deleteLater()));

                sysxIO->setBank(0);
                sysxIO->setPatch(0);
            };
        };
        this->initPatch->setIndex(0);
    };
}

void floorBoardDisplay::setPatchNumDisplay(int bank, int patch)
{
    if(patch > 0)
    {
        if(bank <= 50)
        {
            this->patchNumDisplay->resetAllColor();
            this->patchNumDisplay->setSubText(tr("User"));
            if(patch>9) this->patchNumDisplay->setSubText(tr("File"));
        }
        else if (bank > 50)
        {
            this->patchNumDisplay->setAllColor(QColor(255,0,0));
            this->patchNumDisplay->setSubText(tr("Preset"));
        };

        QString str;
        if(patch>9){ str="U:";
            str.append(QString::number(patch-1, 10));
        }else{
            if(patch==1){ str="Ch A1"; };
            if(patch==2){ str="Ch A2"; };
            if(patch==3){ str="Ch A3"; };
            if(patch==4){ str="Ch A4"; };
            if(patch==5){ str="panel"; };
            if(patch==6){ str="Ch B1"; };
            if(patch==7){ str="Ch B2"; };
            if(patch==8){ str="Ch B3"; };
            if(patch==9){ str="Ch B4"; };
        };
        this->patchNumDisplay->setMainText(str, Qt::AlignCenter);
        if(patch==5){
            this->patchNumDisplay->setSubText(tr("Knob"));
            QString str = tr("Panel");
            this->patchNumDisplay->setMainText(str, Qt::AlignCenter);
        }
    }
    else
    {
        this->patchNumDisplay->resetAllColor();
        this->patchNumDisplay->setSubText(tr("patch"));
        QString str = tr("data");
        this->patchNumDisplay->setMainText(str, Qt::AlignCenter);
    };
    Preferences *preferences = Preferences::Instance();
    if( preferences->getPreferences("General", "Loader", "active") == "true"){
        if(patch>9){
            this->assign1_Button->show();
            this->assign1_Button->textLabel->show();
            this->assign2_Button->show();
            this->assign2_Button->textLabel->show();
            this->assign3_Button->show();
            this->assign3_Button->textLabel->show();
            this->assign4_Button->show();
            this->assign4_Button->textLabel->show();
            this->assign5_Button->show();
            this->assign5_Button->textLabel->show();
            this->assign6_Button->show();
            this->assign6_Button->textLabel->show();
            this->assign7_Button->show();
            this->assign7_Button->textLabel->show();
            this->assign8_Button->show();
            this->assign8_Button->textLabel->show();
            this->assign9_Button->show();
            this->assign9_Button->textLabel->show();
            this->assign10_Button->show();
            this->assign10_Button->textLabel->show();
            this->assign11_Button->show();
            this->assign11_Button->textLabel->show();
            this->assign12_Button->show();
            this->assign12_Button->textLabel->show();
            this->assign13_Button->show();
            this->assign13_Button->textLabel->show();
            this->assign14_Button->show();
            this->assign14_Button->textLabel->show();
            this->assign15_Button->show();
            this->assign15_Button->textLabel->show();
            this->assign16_Button->show();
            this->assign16_Button->textLabel->show();
        }else{
            this->assign1_Button->hide();
            this->assign1_Button->textLabel->hide();
            this->assign2_Button->hide();
            this->assign2_Button->textLabel->hide();
            this->assign3_Button->hide();
            this->assign3_Button->textLabel->hide();
            this->assign4_Button->hide();
            this->assign4_Button->textLabel->hide();
            this->assign5_Button->hide();
            this->assign5_Button->textLabel->hide();
            this->assign6_Button->hide();
            this->assign6_Button->textLabel->hide();
            this->assign7_Button->hide();
            this->assign7_Button->textLabel->hide();
            this->assign8_Button->hide();
            this->assign8_Button->textLabel->hide();
            this->assign9_Button->hide();
            this->assign9_Button->textLabel->hide();
            this->assign10_Button->hide();
            this->assign10_Button->textLabel->hide();
            this->assign11_Button->hide();
            this->assign11_Button->textLabel->hide();
            this->assign12_Button->hide();
            this->assign12_Button->textLabel->hide();
            this->assign13_Button->hide();
            this->assign13_Button->textLabel->hide();
            this->assign14_Button->hide();
            this->assign14_Button->textLabel->hide();
            this->assign15_Button->hide();
            this->assign15_Button->textLabel->hide();
            this->assign16_Button->hide();
            this->assign16_Button->textLabel->hide();
        };
    };
}

void floorBoardDisplay::updateDisplay()
{
    SysxIO *sysxIO = SysxIO::Instance();
    QString area = "Structure";
    QList<QString> nameArray = sysxIO->getFileSource(area, nameAddress, "00");
    QString name;
    for(int i=sysxDataOffset;i<(sysxDataOffset+nameLength);i++ )
    {

        QString hexStr = nameArray.at(i);
        bool ok;
        name.append( (char)(hexStr.toInt(&ok, 16)) );
    };

    QString patchName = name.trimmed();
    sysxIO->setCurrentPatchName(patchName);
    if(sysxIO->getRequestName().trimmed() != patchName.trimmed())
    {
        this->patchLoadError = true;
    }
    else
    {
        this->patchLoadError = false;
    };


    this->setPatchDisplay(patchName);
    if(sysxIO->isDevice())
    {
        int bank = sysxIO->getBank();
        int patch = sysxIO->getPatch();
        this->setPatchNumDisplay(bank, patch);
    }
    else
    {
        this->patchNumDisplay->clearAll();
    };
    this->valueDisplay->clearAll();

    if(sysxIO->isDevice() )  // comment out from here
    {
        if(sysxIO->getBank() > 50)
        {
            this->writeButton->setBlink(false);
            this->writeButton->setValue(false);
        }
        else
        {
            this->writeButton->setBlink(false);
            this->writeButton->setValue(true);
        };
        int bank = sysxIO->getBank();
        int patch = sysxIO->getPatch();
        this->setPatchNumDisplay(bank, patch);
    }
    else if(sysxIO->getBank() != 0)
    {
        if(sysxIO->isConnected())
        {
            this->writeButton->setBlink(false);
            this->writeButton->setValue(false);
        };
        int bank = sysxIO->getBank();
        int patch = sysxIO->getPatch();
        this->setPatchNumDisplay(bank, patch);
    }
    else
    {
        patchNumDisplay->clearAll();
        this->writeButton->setBlink(false);
        this->writeButton->setValue(false);
    };
}

void floorBoardDisplay::autoconnect()
{
    SysxIO *sysxIO = SysxIO::Instance();
    if(!sysxIO->isConnected() && sysxIO->deviceReady())
    {
        emit setStatusSymbol(2);
        emit setStatusMessage(tr("Connecting"));

        this->connectButton->setBlink(true);
        sysxIO->setDeviceReady(false); // Reserve the device for interaction.

        QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)));
        QObject::connect(sysxIO, SIGNAL(sysxReply(QString)),
                         this, SLOT(autoConnectionResult(QString)));

        sysxIO->sendSysx(idRequestString); // KATANA Identity Request.
    }
    else
    {
        emit notConnected();
        sysxIO->setNoError(true);		// Reset the error status (else we could never retry :) ).
    };
}

void floorBoardDisplay::autoConnectionResult(QString sysxMsg)
{
    SysxIO *sysxIO = SysxIO::Instance();
    QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)),
                        this, SLOT(autoConnectionResult(QString)));

    sysxIO->setDeviceReady(true); // Free the device after finishing interaction.
    if(sysxIO->noError())
    {
        if(sysxMsg.contains(idReplyPatern))
        {
            this->connectButton->setBlink(false);
            this->connectButton->setValue(true);
            sysxIO->setConnected(true);
            emit connectedSignal();
            this->logo->show();

            if(sysxIO->getBank() != 0)
            {
                this->writeButton->setBlink(false);
                this->writeButton->setValue(false);
            };
        }else
        {
            this->connectButton->setBlink(false);
            this->connectButton->setValue(false);
            sysxIO->setConnected(false);
            this->logo->hide();
        };
    }
    else
    {
        notConnected();
        sysxIO->setNoError(true);		// Reset the error status (else we could never retry :) ).
        this->logo->hide();
    };
}

void floorBoardDisplay::set_temp()
{

    SysxIO *sysxIO = SysxIO::Instance();
    QByteArray data;

    QFile file1("temp-1.syx");   // Read the sysx file .
    if (file1.open(QIODevice::ReadOnly))
    {	data = file1.readAll(); };

    QString sysxBuffer;
    for(int i=0;i<data.size();i++)
    {
        unsigned char byte = (char)data[i];
        unsigned int n = (int)byte;
        QString hex = QString::number(n, 16).toUpper();     // convert QByteArray to QString
        if (hex.length() < 2) hex.prepend("0");
        sysxBuffer.append(hex);
    };
    if( data.size() == fullPatchSize)
    {
        QString patchText;
        unsigned char r;
        unsigned int a = sysxNameOffset; // locate patch text start position from the start of the file
        for (int b=0;b<nameLength;b++)
        {
            r = (char)data[a+b];
            patchText.append(r);         // extract the text characters from the current patch name.
        };
        this->temp1Display->setMainText(patchText, Qt::AlignCenter);
        sysxIO->temp1_sysxMsg = sysxBuffer;
    };

    data.clear();
    QFile file2("temp-2.syx");   // Read the sysx file .
    if (file2.open(QIODevice::ReadOnly))
    {	data = file2.readAll(); };

    sysxBuffer.clear();
    for(int i=0;i<data.size();i++)
    {
        unsigned char byte = (char)data[i];
        unsigned int n = (int)byte;
        QString hex = QString::number(n, 16).toUpper();     // convert QByteArray to QString
        if (hex.length() < 2) hex.prepend("0");
        sysxBuffer.append(hex);
    };
    if( data.size() == fullPatchSize)
    {
        QString patchText;
        unsigned char r;
        unsigned int a = sysxNameOffset; // locate patch text start position from the start of the file
        for (int b=0;b<nameLength;b++)
        {
            r = (char)data[a+b];
            patchText.append(r);         // extract the text characters from the current patch name.
        };
        this->temp2Display->setMainText(patchText, Qt::AlignCenter);
        sysxIO->temp2_sysxMsg = sysxBuffer;
    };

    data.clear();
    QFile file3("temp-3.syx");   // Read the default GT-3 sysx file so we don't start empty handed.
    if (file3.open(QIODevice::ReadOnly))
    {	data = file3.readAll(); };


    sysxBuffer.clear();
    for(int i=0;i<data.size();i++)
    {
        unsigned char byte = (char)data[i];
        unsigned int n = (int)byte;
        QString hex = QString::number(n, 16).toUpper();     // convert QByteArray to QString
        if (hex.length() < 2) hex.prepend("0");
        sysxBuffer.append(hex);
    };
    if( data.size() == fullPatchSize)
    {
        QString patchText;
        unsigned char r;
        unsigned int a = sysxNameOffset; // locate patch text start position from the start of the file
        for (int b=0;b<nameLength;b++)
        {
            r = (char)data[a+b];
            patchText.append(r);         // extract the text characters from the current patch name.
        };
        this->temp3Display->setMainText(patchText, Qt::AlignCenter);
        sysxIO->temp3_sysxMsg = sysxBuffer;
    };

    data.clear();
    QFile file4("temp-4.syx");   // Read the sysx file .
    if (file4.open(QIODevice::ReadOnly))
    {	data = file4.readAll(); };

    sysxBuffer.clear();
    for(int i=0;i<data.size();i++)
    {
        unsigned char byte = (char)data[i];
        unsigned int n = (int)byte;
        QString hex = QString::number(n, 16).toUpper();     // convert QByteArray to QString
        if (hex.length() < 2) hex.prepend("0");
        sysxBuffer.append(hex);
    };
    if( data.size() == fullPatchSize)

    {
        QString patchText;
        unsigned char r;
        unsigned int a = sysxNameOffset; // locate patch text start position from the start of the file
        for (int b=0;b<nameLength;b++)
        {
            r = (char)data[a+b];
            patchText.append(r);         // extract the text characters from the current patch name.
        };
        this->temp4Display->setMainText(patchText, Qt::AlignCenter);
        sysxIO->temp4_sysxMsg = sysxBuffer;
    };

    data.clear();
    QFile file5("temp-5.syx");   // Read the sysx file .
    if (file5.open(QIODevice::ReadOnly))
    {	data = file5.readAll(); };

    sysxBuffer.clear();
    for(int i=0;i<data.size();i++)
    {
        unsigned char byte = (char)data[i];
        unsigned int n = (int)byte;
        QString hex = QString::number(n, 16).toUpper();     // convert QByteArray to QString
        if (hex.length() < 2) hex.prepend("0");
        sysxBuffer.append(hex);
    };
    if( data.size() == fullPatchSize)

    {
        QString patchText;
        unsigned char r;
        unsigned int a = sysxNameOffset; // locate patch text start position from the start of the file
        for (int b=0;b<nameLength;b++)
        {
            r = (char)data[a+b];
            patchText.append(r);         // extract the text characters from the current patch name.
        };
        this->temp5Display->setMainText(patchText, Qt::AlignCenter);
        sysxIO->temp5_sysxMsg = sysxBuffer;
    };
}

void floorBoardDisplay::temp1_copy(bool value)
{
    Q_UNUSED(value);
    SysxIO *sysxIO = SysxIO::Instance();

    QString sysxMsg;
    QList< QList<QString> > patchData = sysxIO->getFileSource().hex; // Get the loaded patch data.
    //QList<QString> patchAddress = sysxIO->getFileSource().address;
    QString addr1 = tempBulkWrite;  // temp address
    QString addr2 = QString::number(0, 16).toUpper();

    for(int i=0;i<patchData.size();++i)
    {
        QList<QString> data = patchData.at(i);
        for(int x=0;x<data.size();++x)
        {
            QString hex;
            if(x == sysxAddressOffset)
            { hex = addr1; }
            else if(x == sysxAddressOffset + 1)
            {	hex = addr2; }
            else
            {	hex = data.at(x);	};
            if (hex.length() < 2) hex.prepend("0");
            sysxMsg.append(hex);
        };
    };
    if( sysxMsg.size()/2 == fullPatchSize)
    {
        this->patchName = sysxIO->getCurrentPatchName();
        this->temp1Display->setMainText(patchName, Qt::AlignCenter);
        sysxIO->temp1_sysxMsg = sysxMsg;
        save_temp("temp-1.syx", sysxMsg);
    } else {
        QApplication::beep();
        QString size = QString::number(sysxMsg.size()/2, 10);
        sysxIO->emitStatusdBugMessage(tr("in-consistant patch data detected ") + size + tr("bytes: re-save or re-load file to correct"));
    };
}

void floorBoardDisplay::temp1_paste(bool value)
{
    Q_UNUSED(value);
    SysxIO *sysxIO = SysxIO::Instance();
    this->temp1_sysxMsg = sysxIO->temp1_sysxMsg;
    if (!temp1_sysxMsg.isEmpty() && sysxIO->deviceReady() )
    {
        SysxIO *sysxIO = SysxIO::Instance();
        sysxIO->setFileSource("patch", temp1_sysxMsg);
        emit updateSignal();
        if(sysxIO->isConnected()) { sysxIO->writeToBuffer(); };
    }
    else
    {
        QApplication::beep();
        sysxIO->emitStatusdBugMessage(tr("patch must be copied to clipboard first"));
    };
}

void floorBoardDisplay::temp2_copy(bool value)
{
    Q_UNUSED(value);
    SysxIO *sysxIO = SysxIO::Instance();

    QString sysxMsg;
    QList< QList<QString> > patchData = sysxIO->getFileSource().hex; // Get the loaded patch data.
    //QList<QString> patchAddress = sysxIO->getFileSource().address;
    QString addr1 = tempBulkWrite;  // temp address
    QString addr2 = QString::number(0, 16).toUpper();

    for(int i=0;i<patchData.size();++i)
    {
        QList<QString> data = patchData.at(i);
        for(int x=0;x<data.size();++x)
        {
            QString hex;
            if(x == sysxAddressOffset)
            { hex = addr1; }
            else if(x == sysxAddressOffset + 1)
            {	hex = addr2; }
            else
            {	hex = data.at(x);	};
            if (hex.length() < 2) hex.prepend("0");
            sysxMsg.append(hex);
        };
    };
    if( sysxMsg.size()/2 == fullPatchSize)
    {
        this->patchName = sysxIO->getCurrentPatchName();
        this->temp2Display->setMainText(patchName, Qt::AlignCenter);
        sysxIO->temp2_sysxMsg = sysxMsg;
        save_temp("temp-2.syx", sysxMsg);
    } else {
        QApplication::beep();
        QString size = QString::number(sysxMsg.size()/2, 10);
        sysxIO->emitStatusdBugMessage(tr("in-consistant patch data detected ") + size + tr("bytes: re-save or re-load file to correct"));
    };
}

void floorBoardDisplay::temp2_paste(bool value)
{
    Q_UNUSED(value);
    SysxIO *sysxIO = SysxIO::Instance();
    this->temp2_sysxMsg = sysxIO->temp2_sysxMsg;
    if (!temp2_sysxMsg.isEmpty() && sysxIO->deviceReady() )
    {
        SysxIO *sysxIO = SysxIO::Instance();
        sysxIO->setFileSource("patch", temp2_sysxMsg);
        emit updateSignal();
        if(sysxIO->isConnected()) { sysxIO->writeToBuffer(); };
    }else
    {
        QApplication::beep();
        sysxIO->emitStatusdBugMessage(tr("patch must be copied to clipboard first"));
    };
}

void floorBoardDisplay::temp3_copy(bool value)
{
    Q_UNUSED(value);
    SysxIO *sysxIO = SysxIO::Instance();

    QString sysxMsg;
    QList< QList<QString> > patchData = sysxIO->getFileSource().hex; // Get the loaded patch data.
    //QList<QString> patchAddress = sysxIO->getFileSource().address;
    QString addr1 = tempBulkWrite;  // temp address
    QString addr2 = QString::number(0, 16).toUpper();

    for(int i=0;i<patchData.size();++i)
    {
        QList<QString> data = patchData.at(i);
        for(int x=0;x<data.size();++x)
        {
            QString hex;
            if(x == sysxAddressOffset)
            { hex = addr1; }
            else if(x == sysxAddressOffset + 1)
            {	hex = addr2; }
            else
            {	hex = data.at(x);	};
            if (hex.length() < 2) hex.prepend("0");
            sysxMsg.append(hex);
        };
    };
    if( sysxMsg.size()/2 == fullPatchSize)
    {
        this->patchName = sysxIO->getCurrentPatchName();
        this->temp3Display->setMainText(patchName, Qt::AlignCenter);
        sysxIO->temp3_sysxMsg = sysxMsg;
        save_temp("temp-3.syx", sysxMsg);
    } else {
        QApplication::beep();
        QString size = QString::number(sysxMsg.size()/2, 10);
        sysxIO->emitStatusdBugMessage(tr("in-consistant patch data detected ") + size + tr("bytes: re-save or re-load file to correct"));
    };
}

void floorBoardDisplay::temp3_paste(bool value)
{
    Q_UNUSED(value);
    SysxIO *sysxIO = SysxIO::Instance();
    this->temp3_sysxMsg = sysxIO->temp3_sysxMsg;
    if (!temp3_sysxMsg.isEmpty() && sysxIO->deviceReady() )
    {
        SysxIO *sysxIO = SysxIO::Instance();
        sysxIO->setFileSource("patch", temp3_sysxMsg);
        emit updateSignal();
        if(sysxIO->isConnected()) { sysxIO->writeToBuffer(); };
    }else
    {
        QApplication::beep();
        sysxIO->emitStatusdBugMessage(tr("patch must be copied to clipboard first"));
    };
}

void floorBoardDisplay::temp4_copy(bool value)
{
    Q_UNUSED(value);
    SysxIO *sysxIO = SysxIO::Instance();

    QString sysxMsg;
    QList< QList<QString> > patchData = sysxIO->getFileSource().hex; // Get the loaded patch data.
    //QList<QString> patchAddress = sysxIO->getFileSource().address;
    QString addr1 = tempBulkWrite;  // temp address
    QString addr2 = QString::number(0, 16).toUpper();

    for(int i=0;i<patchData.size();++i)
    {
        QList<QString> data = patchData.at(i);
        for(int x=0;x<data.size();++x)
        {
            QString hex;
            if(x == sysxAddressOffset)
            { hex = addr1; }
            else if(x == sysxAddressOffset + 1)
            {	hex = addr2; }
            else
            {	hex = data.at(x);	};
            if (hex.length() < 2) hex.prepend("0");
            sysxMsg.append(hex);
        };
    };
    if( sysxMsg.size()/2 == fullPatchSize)
    {
        this->patchName = sysxIO->getCurrentPatchName();
        this->temp4Display->setMainText(patchName, Qt::AlignCenter);
        sysxIO->temp4_sysxMsg = sysxMsg;
        save_temp("temp-4.syx", sysxMsg);
    } else {
        QApplication::beep();
        QString size = QString::number(sysxMsg.size()/2, 10);
        sysxIO->emitStatusdBugMessage(tr("in-consistant patch data detected ") + size + tr("bytes: re-save or re-load file to correct"));
    };
}

void floorBoardDisplay::temp4_paste(bool value)
{
    Q_UNUSED(value);
    SysxIO *sysxIO = SysxIO::Instance();
    this->temp4_sysxMsg = sysxIO->temp4_sysxMsg;
    if (!temp4_sysxMsg.isEmpty() && sysxIO->deviceReady() )
    {
        SysxIO *sysxIO = SysxIO::Instance();
        sysxIO->setFileSource("patch", temp4_sysxMsg);
        emit updateSignal();
        if(sysxIO->isConnected()) { sysxIO->writeToBuffer(); };
    }else
    {
        QApplication::beep();
        sysxIO->emitStatusdBugMessage(tr("patch must be copied to clipboard first"));
    };
}

void floorBoardDisplay::temp5_copy(bool value)
{
    Q_UNUSED(value);
    SysxIO *sysxIO = SysxIO::Instance();
    QString sysxMsg;
    QList< QList<QString> > patchData = sysxIO->getFileSource().hex; // Get the loaded patch data.
    //QList<QString> patchAddress = sysxIO->getFileSource().address;
    QString addr1 = tempBulkWrite;  // temp address
    QString addr2 = QString::number(0, 16).toUpper();

    for(int i=0;i<patchData.size();++i)
    {
        QList<QString> data = patchData.at(i);
        for(int x=0;x<data.size();++x)
        {
            QString hex;
            if(x == sysxAddressOffset)
            { hex = addr1; }
            else if(x == sysxAddressOffset + 1)
            {	hex = addr2; }
            else
            {	hex = data.at(x);	};
            if (hex.length() < 2) hex.prepend("0");
            sysxMsg.append(hex);
        };
    };
    if( sysxMsg.size()/2 == fullPatchSize)
    {
        this->patchName = sysxIO->getCurrentPatchName();
        this->temp5Display->setMainText(patchName, Qt::AlignCenter);
        sysxIO->temp5_sysxMsg = sysxMsg;
        save_temp("temp-5.syx", sysxMsg);
    } else {
        QApplication::beep();
        QString size = QString::number(sysxMsg.size()/2, 10);
        sysxIO->emitStatusdBugMessage(tr("in-consistant patch data detected ") + size + tr("bytes: re-save or re-load file to correct"));
    };
}

void floorBoardDisplay::temp5_paste(bool value)
{
    Q_UNUSED(value);
    SysxIO *sysxIO = SysxIO::Instance();
    this->temp5_sysxMsg = sysxIO->temp5_sysxMsg;
    if (!temp5_sysxMsg.isEmpty() && sysxIO->deviceReady() )
    {
        SysxIO *sysxIO = SysxIO::Instance();
        sysxIO->setFileSource("patch", temp5_sysxMsg);
        emit updateSignal();
        if(sysxIO->isConnected()) { sysxIO->writeToBuffer(); };
    }else
    {
        QApplication::beep();
        sysxIO->emitStatusdBugMessage(tr("patch must be copied to clipboard first"));
    };
}

void floorBoardDisplay::save_temp(QString fileName, QString sysxMsg)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        QByteArray out;
        unsigned int count=0;
        QString data = sysxMsg;
        int x = data.size()/2;
        for (int a=0;a<x;++a)
        {
            QString str = data.at(a*2);
            str.append(data.at((a*2)+1));
            bool ok;
            unsigned int n = str.toInt(&ok, 16);
            out[count] = (char)n;
            count++;
        };
        file.write(out);
    };
}

void floorBoardDisplay::connectSignal(bool value)
{
    SysxIO *sysxIO = SysxIO::Instance();
    this->connectButtonActive = value;
    if(connectButtonActive == true && sysxIO->deviceReady())
    {
        emit setStatusSymbol(2);
        emit setStatusMessage(tr("Connecting"));

        this->connectButton->setBlink(true);
        sysxIO->setDeviceReady(false); // Reserve the device for interaction.

        QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)));
        QObject::connect(sysxIO, SIGNAL(sysxReply(QString)),
                         this, SLOT(connectionResult(QString)));

        sysxIO->sendSysx(idRequestString); // GT100 Identity Request.
    }
    else
    {
        emit notConnected();
        sysxIO->setNoError(true);		// Reset the error status (else we could never retry :) ).
    };
}

void floorBoardDisplay::connectionResult(QString sysxMsg)
{
    SysxIO *sysxIO = SysxIO::Instance();
    QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)),
                        this, SLOT(connectionResult(QString)));

    sysxIO->setDeviceReady(true); // Free the device after finishing interaction.

    //DeBugGING OUTPUT
    Preferences *preferences = Preferences::Instance(); // Load the preferences.
    if(preferences->getPreferences("Midi", "DBug", "bool")=="true")
    {
        this->connectButton->setBlink(false);
        this->connectButton->setValue(true);
        sysxIO->setConnected(true);
        emit connectedSignal();
        emit setStatusMessage(tr("Ready"));
        this->logo->show();

        if(sysxIO->getBank() != 0)
        {
            this->writeButton->setBlink(false);
            this->writeButton->setValue(false);
        };
    }

    else if(sysxIO->noError())
    {
        if(sysxMsg.contains(idReplyPatern) && connectButtonActive == true)
        {
            this->connectButton->setBlink(false);
            this->connectButton->setValue(true);
            sysxIO->setConnected(true);
            emit connectedSignal();
            this->logo->show();

            if(sysxIO->getBank() != 0)
            {
                this->writeButton->setBlink(false);
                this->writeButton->setValue(false);
            };
        }
        else if(!sysxMsg.isEmpty())
        {
            this->connectButton->setBlink(false);
            this->connectButton->setValue(false);
            sysxIO->setConnected(false);
            this->logo->hide();

            QMessageBox *msgBox = new QMessageBox();
            msgBox->setWindowTitle(tr("KATANAFxFloorBoard connection Error !!"));
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->setTextFormat(Qt::RichText);
            QString msgText;
            msgText.append("<font size='+1'><b>");
            msgText.append(tr("The device connected is not a Boss KATANA."));
            if (sysxMsg.contains(idRequestString))
            {msgText.append(tr("<br>Midi loopback detected, ensure midi device 'thru' is switched off.")); };
            msgText.append(tr("<br>Data=")+sysxMsg);
            msgText.append("<b></font>");
            msgBox->setText(msgText);
            msgBox->show();
            QTimer::singleShot(10000, msgBox, SLOT(deleteLater()));

            notConnected();

            emit setStatusSymbol(0);
            emit setStatusProgress(0);
            emit setStatusMessage(tr("Not connected"));
        }
        else
        {
            this->connectButton->setBlink(false);
            this->connectButton->setValue(false);
            sysxIO->setConnected(false);

            QMessageBox *msgBox = new QMessageBox();
            msgBox->setWindowTitle(tr("KATANAFxFloorBoard connection Error !!"));
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->setTextFormat(Qt::RichText);
            QString msgText;
            msgText.append("<font size='+1'><b>");
            msgText.append(tr("The Boss KATANA was not found."));
            msgText.append(tr("<br><br>Ensure correct midi device is selected in Preferences Menu, "));
            msgText.append(tr("<br>Boss drivers are installed and the KATANA is switched on,"));
            msgText.append("<b></font><br>");
            msgBox->setText(msgText);
            msgBox->show();
            QTimer::singleShot(10000, msgBox, SLOT(deleteLater()));

            notConnected();
            this->logo->hide();

            emit setStatusSymbol(0);
            emit setStatusProgress(0);
            emit setStatusMessage(tr("Not connected"));
        };
    }
    else
    {
        this->logo->hide();
        notConnected();
        sysxIO->setNoError(true);		// Reset the error status (else we could never retry :) ).
    };
    /* QString msgText;
            msgText.append(QObject::tr("<br> data size received = ")+(QString::number(sysxMsg.size()/2, 10)));
            QMessageBox *msgBox = new QMessageBox();
            msgBox->setWindowTitle(msgText);
            msgBox->setIcon(QMessageBox::Information);
            msgBox->setText(sysxMsg);
            msgBox->setStandardButtons(QMessageBox::Ok);
            msgBox->exec();*/

}

void floorBoardDisplay::writeSignal(bool)
{
    autosync_off(true);
    SysxIO *sysxIO = SysxIO::Instance();
    if(sysxIO->isConnected() && sysxIO->deviceReady()) /* Check if we are connected and if the device is free. */
    {
        this->writeButton->setBlink(true);

        if(sysxIO->getBank() == 0) // Check if a bank is sellected.
        {
            sysxIO->setDeviceReady(false);			// Reserve the device for interaction.
            writeToBuffer();
        }
        else // Bank is sellected.
        {
            sysxIO->setDeviceReady(false);			// Reserve the device for interaction.
            {
                if((sysxIO->getBank() > bankTotalUser && sysxIO->getBank() <= bankTotalAll) || (sysxIO->getBank() == 105)) // Preset banks are NOT writable so we check.
                {
                    QMessageBox *msgBox = new QMessageBox();
                    msgBox->setWindowTitle(tr("KATANA_Fx_FloorBoard"));
                    msgBox->setIcon(QMessageBox::Warning);
                    msgBox->setTextFormat(Qt::RichText);
                    QString msgText;
                    msgText.append("<font size='+1'><b>");
                    msgText.append(tr("You can't write to the preset banks."));
                    msgText.append("<b></font><br>");
                    msgText.append(tr("Please select a User bank to write this patch to and try again."));
                    msgBox->setText(msgText);
                    msgBox->show();
                    QTimer::singleShot(3000, msgBox, SLOT(deleteLater()));
                    this->writeButton->setBlink(false); // Allready sync with the buffer so no blinking
                    this->writeButton->setValue(true);	// and so we will also leave the write button active.
                    sysxIO->setDeviceReady(true);
                }
                else /* User bank so we can write to it after confirmation to overwrite stored data. */
                {
                    QString patchNum;
                    int patch = sysxIO->getPatch();
                    if(patch>9){
                        patchNum.prepend("Loader:");
                    }else{
                        patchNum.prepend("Channel:");
                    };

                    if(patch==1){ patchNum="Channel: A1"; }
                    else if(patch==2){ patchNum="Channel: A2"; }
                    else if(patch==3){ patchNum="Channel: A3"; }
                    else if(patch==4){ patchNum="Channel: A4"; }
                    else if(patch==5){ patchNum="Knob Panel";  }
                    else if(patch==6){ patchNum="Channel: B1"; }
                    else if(patch==7){ patchNum="Channel: B2"; }
                    else if(patch==8){ patchNum="Channel: B3"; }
                    else if(patch==9){ patchNum="Channel: B4"; };

                    QMessageBox *msgBox = new QMessageBox();
                    msgBox->setWindowTitle(tr("KATANA Fx FloorBoard"));
                    msgBox->setIcon(QMessageBox::Warning);
                    msgBox->setTextFormat(Qt::RichText);
                    QString msgText;
                    msgText.append("<font size='+1'><b>");
                    if(patch>9){
                        msgText.append(tr("You are about to Write/Insert/Delete the current patch to/from the Loader Patch Set."));
                    }else{
                        msgText.append(tr("You are about to write the current patch to the KATANA Amp."));
                    };
                    msgText.append("<b></font><br>");
                    msgText.append(tr("This will replace the patch currently stored at patch location<br>"));
                    msgText.append("<font size='+2'><b>");
                    msgText.append(patchNum	+ "<b></font> will be saved with <font size='+2'><b>"+ sysxIO->getCurrentPatchName());
                    msgText.append("<b></font><br>");
                    msgBox->setInformativeText(tr("Select Your required Action"));
                    msgBox->setText(msgText);

                    QPushButton *pButtonIns = msgBox->addButton(tr("Insert"), QMessageBox::AcceptRole);
                    QPushButton *pButtonDel = msgBox->addButton(tr("Delete"), QMessageBox::AcceptRole);
                    QPushButton *pButtonWrt = msgBox->addButton(tr("Over-Write"), QMessageBox::AcceptRole);
                    QAbstractButton* pButtonCan = msgBox->addButton(tr("Cancel"), QMessageBox::AcceptRole);
                    if(patch<10 || (patch-9)>sysxIO->patchList.count()){pButtonDel->hide(); pButtonIns->hide();};
                    if((patch-9)>sysxIO->patchList.count()){pButtonWrt->setText("Write");};
                    if(sysxIO->patchList.count()<2){pButtonDel->hide();};
                    msgBox->exec();

                    if(msgBox->clickedButton()==pButtonIns)
                    {
                        writeToLoader(patch, "insert");
                    }else if(msgBox->clickedButton()==pButtonDel)
                    {
                        writeToLoader(patch, "delete");
                    }
                    else if(msgBox->clickedButton()==pButtonWrt)
                    {
                        if(patch>9){
                            writeToLoader(patch, "write");
                        }else{
                            writeToMemory();
                        };
                        sysxIO->setSyncStatus(true);
                    }else if(msgBox->clickedButton()==pButtonCan)
                    {
                        if(sysxIO->isConnected())
                        {
                            sysxIO->setDeviceReady(true);
                            this->writeButton->setBlink(false);
                            this->writeButton->setValue(false);
                        };
                    };
                    msgBox->deleteLater();
                };
            };
        };
    };
    this->writeButton->setValue(false);
}

void floorBoardDisplay::writeToBuffer()
{
    SysxIO *sysxIO = SysxIO::Instance();
    QMessageBox *msgBox = new QMessageBox();
    msgBox->setWindowTitle(tr("KATANA Fx FloorBoard"));
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setTextFormat(Qt::RichText);
    QString msgText;
    msgText.append("<font size='+1'><b>");
    msgText.append(tr("You have not chosen a User Patch address to Write the patch to"));
    msgText.append("<b></font><br>");
    msgText.append(tr("This will only update the KATANA Temporary Buffer memory<br>"));
    msgText.append(tr (" with the editor patch. "));
    msgBox->setInformativeText(tr("Select the required destination patch <br>by a single-click on the left panel Patch-Tree"));
    msgBox->setText(msgText);
    msgBox->show();
    QTimer::singleShot(5000, msgBox, SLOT(deleteLater()));

    sysxIO->writeToBuffer();
    sysxIO->setSyncStatus(true);

    this->writeButton->setBlink(false);	// Sync so we stop blinking the button
    this->writeButton->setValue(false);	// and activate the write button.
}

void floorBoardDisplay::writeToMemory()
{
    SysxIO *sysxIO = SysxIO::Instance();

    QString sysxMsg; bool ok;
    QList< QList<QString> > patchData = sysxIO->getFileSource().hex; // Get the loaded patch data.

    emit setStatusSymbol(2);
    emit setStatusMessage(tr("Writing to Patch"));

    int bank = sysxIO->getBank();
    int patch = sysxIO->getPatch();
    if(patch==1){ patch=2;}
    else if(patch==2){ patch=3;}
    else if(patch==3){ patch=4;}
    else if(patch==4){ patch=5;}
    else if(patch==5){ patch=1;};
    QString addr1;
    QString addr2;
    if (bank < 101)
    {
        int patchOffset = (((bank - 1 ) * patchPerBank) + patch-1);
        int memmorySize = QString("7F").toInt(&ok, 16) + 1;
        int emptyAddresses = (memmorySize) - ((bankTotalUser * patchPerBank) - (memmorySize));
        if(bank > bankTotalUser) patchOffset += emptyAddresses; //System patches start at a new memmory range.
        int addrMaxSize = QString("80").toInt(&ok, 16);
        int n = (int)(patchOffset / addrMaxSize);

        addr1 = QString::number(16 + n, 16).toUpper();
        addr2 = QString::number(patchOffset - (addrMaxSize * n), 16).toUpper();
    }
    else
    {
        addr1 = "30";
        addr2 = QString::number(patch, 16).toUpper();
    };
    if (addr1.length() < 2) addr1.prepend("0");
    if (addr2.length() < 2) addr2.prepend("0");
    for(int i=0;i<patchData.size();++i)
    {
        QList<QString> data = patchData.at(i);
        for(int x=0;x<data.size();++x)
        {
            QString hex;
            if(x == sysxAddressOffset)
            {
                hex = addr1;
            }
            else if(x == sysxAddressOffset + 1)
            {
                hex = addr2;
            }
            else
            {
                hex = data.at(x);
            };
            if (hex.length() < 2) hex.prepend("0");
            sysxMsg.append(hex);
        };
    };
    //sysxMsg.append("7F000104"+addr1+addr2);
    sysxIO->setSyncStatus(true);		// Still in sync
    this->writeButton->setBlink(false); // so no blinking here either...
    this->writeButton->setValue(false);	// ... and still the button will be active also ...

    QObject::connect(sysxIO, SIGNAL(sysxReply(QString)), this, SLOT(resetDevice(QString))); // Connect the result signal to a slot that will reset the device after sending.
    sysxIO->sendSysx(sysxMsg);								// Send the data.
}

void floorBoardDisplay::writeToLoader(int patch, QString function)
{
    SysxIO *sysxIO = SysxIO::Instance();
    bool newf=false;
    if(sysxIO->data.isEmpty())   // check if Loader file has not been loaded.
    {
        QFile file(":default.tsl");           // Read the default Katana sysx file .
        if (file.open(QIODevice::ReadOnly))
        {
            sysxIO->data = file.readAll();   // set the loader data memory as default.tsl
            newf=true;                       // ack a new file has been used.
        };
    };
    patch=patch-9;
    uint_fast32_t start_index=0;
    uint_fast32_t end_index=0;
    QByteArray a("},{");      // separator pattern sequence between tsl/json patches.
    if(patch==1){
        start_index=sysxIO->data.indexOf("[")+1;   // find start of first patch.
        end_index=sysxIO->data.indexOf("},{", start_index)+1;
    }
    else{
        start_index=sysxIO->data.indexOf("[")+1;  // find start of first patch.
        for(int x=1; x<patch; ++x)               // repeat patch pattern search x number of patches.
        {
            start_index=sysxIO->data.indexOf(a, start_index)+1;
            if(patch>=sysxIO->patchList.count()) // if the last patch in the list.
            {
                end_index=sysxIO->data.indexOf("}]", start_index);
            }
            else
            {
                end_index=sysxIO->data.indexOf(a, start_index)+1;
            };
        };
    };
    if(function=="delete")
    {
        if(patch==1)
        {
            sysxIO->data.remove(start_index, (end_index-start_index+1));
        }
        else if(patch>=sysxIO->patchList.count())
        {
            sysxIO->data.remove(start_index-1, (end_index-start_index)+1);
        }
        else
        {
            sysxIO->data.remove(start_index, (end_index-start_index));
        };
    }
    else
    {
        file.hexToTSL();
        QByteArray TSL=file.TSL_default;
        TSL.truncate(TSL.lastIndexOf("]"));
        TSL.remove(0, TSL.indexOf("[")+1);
        if(function=="insert")
        {
            if(patch>1){TSL.prepend(",");}else{TSL.append(",");};
            sysxIO->data.insert(start_index, TSL);
        }
        else if(function=="write")    // Write function for overwrite existing or append to list end.
        {
            if(patch>sysxIO->patchList.count() && !newf)
            {
                TSL.prepend(","); sysxIO->data.insert(sysxIO->data.lastIndexOf("]"), TSL);
            }
            else if(newf) // if a new file, replace default data with the new patch data.
            {
                sysxIO->data.replace(start_index, sysxIO->data.lastIndexOf("]")-start_index, TSL);
            }
            else
            {
                if(patch==1)
                {
                    sysxIO->data.replace(start_index, (end_index-start_index), TSL);
                }
                else
                {
                    sysxIO->data.replace(start_index+1, (end_index-start_index), TSL);
                };
            };
        };
    };

    QString st_ind = QString::number(start_index, 10).toUpper();
    QString end_ind = QString::number(end_index, 10).toUpper();
    deBugBox.setText("start index="+st_ind+
                     "<br>end index="+end_ind);
    // deBugBox.show();

    Preferences *preferences = Preferences::Instance();
    QString fileName = preferences->getPreferences("General", "Loader", "file");
    if(fileName.isEmpty() ){fileName="loaderSave.tsl";};
    file.writeLoaderFile(fileName);
    preferences->setPreferences("General", "Loader", "file", fileName);

    sysxIO->setSyncStatus(true);		// Still in sync
    this->writeButton->setBlink(false); // so no blinking here either...
    this->writeButton->setValue(false);	// ... and still the button will be active also ...
    sysxIO->setDeviceReady(true);
    emit updateSignal();
    sysxIO->fileReady=true;
    emit loadnames();
}

void floorBoardDisplay::autosync_off(bool value)
{
    Q_UNUSED(value);
    autosyncTimer->stop();
    this->autoButton->setBlink(false);
    this->autoButton->setValue(false);
    emit setStatusMessage(tr("Ready"));
}

void floorBoardDisplay::autosyncSignal(bool value)
{
    SysxIO *sysxIO = SysxIO::Instance();
    this->autosyncButtonActive = value;
    if(autosyncButtonActive == true  && sysxIO->isConnected())
    {
        autosyncTimer->start(6000);  //cycle auto sync every 6 seconds (6000 milliseconds)
        autosync(0, 0);
    }
    else
    {
        autosyncTimer->stop();
        this->autoButton->setBlink(false);
        this->autoButton->setValue(false);
        emit setStatusMessage(tr("Ready"));
    };
}

void floorBoardDisplay::autosyncGo()
{
    autosync(0, 0);
}

void floorBoardDisplay::autosync(int bank, int patch)
{
    SysxIO *sysxIO = SysxIO::Instance();
    //if(autosyncButtonActive == true) { emit sysxIO->relayUpdateSignal(); };
    if(/*autosyncButtonActive == true && */sysxIO->deviceReady() && sysxIO->isConnected())
    {

        emit setStatusSymbol(2);
        emit setStatusMessage(tr("Auto Sync"));

        this->autoButton->setBlink(true);
        sysxIO->setDeviceReady(false); // Reserve the device for interaction.

        QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)));
        QObject::connect(sysxIO, SIGNAL(sysxReply(QString)),
                         this, SLOT(autosyncResult(QString)));

        sysxIO->requestPatch(bank, patch); // Katana patch request from temorary buffer memory.

    };
}

void floorBoardDisplay::autosyncResult(QString replyMsg)
{
    SysxIO *sysxIO = SysxIO::Instance();
    QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)),
                        this, SLOT(autosyncResult(QString)));

    sysxIO->setDeviceReady(true); // Free the device after finishing interaction.
    this->autoButton->setBlink(false);

    if (replyMsg.size()/2 == 2928) {
        QByteArray data;
        QFile file(":default.syx");
        if (file.open(QIODevice::ReadOnly)){ data = file.readAll(); };
        QString sysxBuffer;
        for(int i=0;i<data.size();i++)
        {
            unsigned char byte = (char)data[i];
            unsigned int n = (int)byte;
            QString hex = QString::number(n, 16).toUpper();
            if (hex.length() < 2) hex.prepend("0");
            sysxBuffer.append(hex);
        };

        sysxBuffer.replace(22,  256, replyMsg.mid( 24, 256));  //"00 00 00 00"  replace default patch sections with received data
        sysxBuffer.replace(304, 226, replyMsg.mid(280, 226));  //"00 00 01 00"
        sysxBuffer.replace(530,  30, replyMsg.mid(534,  30));  //"00 00 01 00"
        sysxBuffer.replace(586, 256, replyMsg.mid(564, 256));  //"00 00 02 00"
        sysxBuffer.replace(868, 196, replyMsg.mid(820, 196));  //"00 00 03 00"
        sysxBuffer.replace(1064, 60, replyMsg.mid(1044, 60));  //"00 00 03 00"
        sysxBuffer.replace(1150, 256, replyMsg.mid(1104, 256));//"00 00 04 00"
        sysxBuffer.replace(1432, 166, replyMsg.mid(1360, 166));//"00 00 05 00"
        sysxBuffer.replace(1598, 90, replyMsg.mid(1554, 90));  //"00 00 05 00"
        sysxBuffer.replace(1714, 256, replyMsg.mid(1644, 256));//"00 00 06 00"
        sysxBuffer.replace(1996, 136, replyMsg.mid(1900, 136));//"00 00 07 00"
        sysxBuffer.replace(2132, 120, replyMsg.mid(2064, 120));//"00 00 07 00"
        sysxBuffer.replace(2278, 256, replyMsg.mid(2184, 256));//"00 00 08 00"
        sysxBuffer.replace(2560, 106, replyMsg.mid(2440, 106));//"00 00 09 00"
        sysxBuffer.replace(2574, 150, replyMsg.mid(2574, 150));//"00 00 09 00"
        sysxBuffer.replace(2842, 256, replyMsg.mid(2724, 256));//"00 00 0A 00"
        sysxBuffer.replace(3124,  76, replyMsg.mid(2980,  76));//"00 00 0B 00"
        sysxBuffer.replace(3200, 180, replyMsg.mid(3084, 180));//"00 00 0B 00"
        sysxBuffer.replace(3406, 256, replyMsg.mid(3264, 256));//"00 00 0C 00"
        sysxBuffer.replace(3688,  46, replyMsg.mid(3520,  46));//"00 00 0D 00"
        sysxBuffer.replace(3734, 210, replyMsg.mid(3594, 210));//"00 00 0D 00"
        sysxBuffer.replace(3970, 256, replyMsg.mid(3804, 256));//"00 00 0E 00"
        sysxBuffer.replace(4252,  16, replyMsg.mid(4060,  16));//"00 00 0F 00"
        sysxBuffer.replace(4268, 240, replyMsg.mid(4104, 240));//"00 00 0F 00"
        sysxBuffer.replace(4534, 242, replyMsg.mid(4344, 242));//"00 00 10 00"
        sysxBuffer.replace(4776,  14, replyMsg.mid(4614,  14));//"00 00 10 00"
        sysxBuffer.replace(4816, 256, replyMsg.mid(4628, 256));//"00 00 11 00"
        sysxBuffer.replace(5098, 212, replyMsg.mid(4884, 212));//"00 00 12 00"
        sysxBuffer.replace(5380, 256, replyMsg.mid(5124, 256));//"00 00 13 00"
        sysxBuffer.replace(5662, 226, replyMsg.mid(5380, 226));//"00 00 14 00"
        sysxBuffer.replace(5944, 218, replyMsg.mid(5634, 218));//"00 00 15 00"

        // Add correct checksum to patch strings
        replyMsg.clear();
        QString hex;
        QString sysxEOF;
        int msgLength = sysxBuffer.length()/2;
        for(int i=0;i<msgLength*2;++i)
        {
            hex.append(sysxBuffer.mid(i*2, 2));
            sysxEOF = (sysxBuffer.mid((i*2)+4, 2));
            if (sysxEOF == "F7")
            {
                int dataSize = 0; bool ok;
                for(int h=checksumOffset;h<hex.size()-1;++h)
                { dataSize += hex.mid(h*2, 2).toInt(&ok, 16); };
                QString base = "80";                       // checksum calculate.
                unsigned int sum = dataSize % base.toInt(&ok, 16);
                if(sum!=0) { sum = base.toInt(&ok, 16) - sum; };
                QString checksum = QString::number(sum, 16).toUpper();
                if(checksum.length()<2) {checksum.prepend("0");};
                hex.append(checksum);
                hex.append("F7");
                replyMsg.append(hex);
                i=i+2;
                hex.clear();
                sysxEOF.clear();
            };
        };

        /* QList< QList<QString> > patchData = sysxIO->getFileSource().hex; // Get the loaded patch data.
        QString current_data;
        for(int i=0;i<patchData.size();++i)
        {
            QList<QString> data = patchData.at(i);
            for(int x=0;x<data.size();++x)
            {
                QString hex = data.at(x);
                if (hex.length() < 2) hex.prepend("0");
                current_data.append(hex);
            };
        };  */
        sysxIO->setFileSource("Structure", replyMsg);		// Set the source to the data received.
        sysxIO->setDevice(true);				// Patch received from the device so this is set to true.
        sysxIO->setSyncStatus(true);			// We can't be more in sync than right now! :)
        emit updateSignal();
    };
    emit setStatusProgress(0);
}

void floorBoardDisplay::patchChangeFailed()
{
    SysxIO *sysxIO = SysxIO::Instance();
    sysxIO->setBank(sysxIO->getLoadedBank());
    sysxIO->setPatch(sysxIO->getLoadedPatch());
    setPatchNumDisplay(sysxIO->getLoadedBank(), sysxIO->getLoadedPatch());
}

void floorBoardDisplay::resetDevice(QString sysxMsg)
{
    Q_UNUSED(sysxMsg);
    SysxIO *sysxIO = SysxIO::Instance();
    QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)),	this, SLOT(resetDevice(QString)));

    if(sysxIO->getBank() != sysxIO->getLoadedBank() || sysxIO->getPatch() != sysxIO->getLoadedPatch())
    {
        sysxIO->setLoadedBank(sysxIO->getBank());
        sysxIO->setLoadedPatch(sysxIO->getPatch());
    };

    emit setStatusProgress(33); // time wasting sinusidal statusbar progress
    SLEEP(10);
    emit setStatusProgress(66);
    SLEEP(10);
    emit setStatusProgress(100);
    SLEEP(10);
    emit setStatusProgress(75);
    SLEEP(10);
    emit setStatusProgress(42);
    SLEEP(10);
    emit setStatusProgress(25);
    SLEEP(10);
    sysxIO->setDeviceReady(true);	// Free the device after finishing interaction.
    emit connectedSignal();			// Emit this signal to tell we are still connected and to update the patch names in case they have changed.
    sysxIO->setDeviceReady(true);
}

void floorBoardDisplay::patchSelectSignal(int bank, int patch)
{
    SysxIO *sysxIO = SysxIO::Instance();
    if(blinkCount == 0)
    {
        currentSyncStatus = sysxIO->getSyncStatus();
        sysxIO->setSyncStatus(false);
        writeButton->setBlink(false);
    };

    //if( sysxIO->getLoadedBank() != bank ||  sysxIO->getLoadedPatch() != patch)
    //{
    sysxIO->setBank(bank);
    sysxIO->setPatch(patch);

    if(blinkCount == 0)
    {
        QObject::connect(timer, SIGNAL(timeout()), this, SLOT(blinkSellectedPatch()));
        timer->start(sellectionBlinkInterval);
    }
    else
    {
        blinkCount = 0;
    };
}

void floorBoardDisplay::blinkSellectedPatch(bool active)
{
    SysxIO *sysxIO = SysxIO::Instance();
    int bank = sysxIO->getBank();
    int patch = sysxIO->getPatch();

    if(active && blinkCount <= (sellectionBlinks * 2) - 1)
    {
        if(blinkCount % 2 == 0)
        {
            this->patchNumDisplay->clearAll();
        }
        else
        {
            setPatchNumDisplay(bank, patch);
        };
        blinkCount++;
    }
    else
    {
        QObject::disconnect(timer, SIGNAL(timeout()), this, SLOT(blinkSellectedPatch()));
        timer->stop();
        blinkCount = 0;
        sysxIO->setSyncStatus(currentSyncStatus);
        if(currentSyncStatus || sysxIO->getLoadedBank() == 0)
        {
            writeButton->setBlink(false);
        };
        setPatchNumDisplay(bank,patch);
    };
    emit setStatusSymbol(1);
    emit setStatusMessage(tr("Ready"));
}

void floorBoardDisplay::patchLoadSignal(int bank, int patch)
{
    blinkSellectedPatch(false);

    SysxIO *sysxIO = SysxIO::Instance();
    sysxIO->setBank(bank);
    sysxIO->setPatch(patch);
}

void floorBoardDisplay::notConnected()
{
    this->connectButton->setBlink(false);
    this->connectButton->setValue(false);
    this->writeButton->setBlink(false);
    this->writeButton->setValue(false);
    this->autosyncTimer->stop();
    this->autoButton->setBlink(false);
    this->autoButton->setValue(false);
    this->logo->hide();

    SysxIO *sysxIO = SysxIO::Instance();
    sysxIO->setConnected(false);
    sysxIO->setSyncStatus(false);
    sysxIO->setDeviceReady(true);	// Free the device after finishing interaction.

    emit setStatusSymbol(0);
    emit setStatusProgress(0);
    emit setStatusMessage(tr("Not connected"));
}

void floorBoardDisplay::valueUpdate()
{
    SysxIO *sysxIO = SysxIO::Instance();
    int index = sysxIO->getSourceValue("Structure", "07", "00", "0F");
    this->catagory->controlListComboBox->setCurrentIndex(index);
}

void floorBoardDisplay::valueChanged(bool, QString , QString, QString)
{

}

void floorBoardDisplay::midiInput(QString midiMsg)
{
    //MidiTable *midiTable = MidiTable::Instance();
    bool ok;
    Preferences *preferences = Preferences::Instance();
    const int channel = preferences->getPreferences("Midi", "TxCh", "set").toInt(&ok, 10);
    int channels=9;
    if(preferences->getPreferences("Midi", "Katana", "model").toInt(&ok)==0){channels=5;};
    if(preferences->getPreferences("Midi", "Katana", "model").toInt(&ok)>4){channels=3;};
    SysxIO *sysxIO = SysxIO::Instance();
    sysxIO->setDeviceReady(true);
    int index ;
    if((midiMsg.size()==16) && midiMsg.contains("B02000")/*(midiMsg.at(0)=="B")*/) { midiMsg.remove(0, 12); }; // test if Roland bank + control change and remove
    QString ch(QString::number(channel-1, 16).toUpper());
    QString PCnumba("C"+ch);
    QString CCnumba(midiMsg.at(2)); CCnumba.append(midiMsg.at(3));
    QString numba(midiMsg.at(0));
    QString CHnumba(midiMsg.at(1));

    if(midiMsg.size()>1) // if data present and channel number matches
    {
        if( numba=="C" && CHnumba==ch)  // if a patch change
        {
            index = midiMsg.indexOf(PCnumba);
            QString num=midiMsg.at(index+2);
            num.append(midiMsg.at(index+3));
            int patch=num.toInt(&ok, 16);

            if(patch<channels)
            {
                sysxIO->requestPatchChange(1, patch);

                patch++; //if(patch==9){patch=0;};
                autosync(1, patch+1);
                patch--;

            }else{
                if(sysxIO->fileReady==true)
                {
                    file.convertFromTSL(patch-4);
                    emit updateSignal();
                    sysxIO->writeToBuffer();
                    updateDisplay();
                };
            };
            setPatchNumDisplay(0, patch+1);
        }
        else if( numba=="B" && CHnumba==ch ) // if a cc# check if assigns are enabled
        {
            int t=0; // t used to test if an assign cc# was matched, if t=0 then pass the midi data through to midi output.
            // check each assign if enabled and specific cc# is matched
            if(sysxIO->getFileSource("Structure", "08", "00").at(58)=="01"  && sysxIO->getFileSource("Structure", "08", "00").at(66)==CCnumba) {process_assign("08", "30", midiMsg, 1); t=1;};
            if(sysxIO->getFileSource("Structure", "08", "00").at(90)=="01" && sysxIO->getFileSource("Structure", "08", "00").at(98)==CCnumba) {process_assign("08", "50", midiMsg, 2); t=1;};
            if(sysxIO->getFileSource("Structure", "08", "00").at(122)=="01" && sysxIO->getFileSource("Structure", "08", "00").at(130)==CCnumba){process_assign("08", "70", midiMsg, 3); t=1;};
            if(sysxIO->getFileSource("Structure", "09", "00").at(26)=="01"  && sysxIO->getFileSource("Structure", "09", "00").at(34)==CCnumba) {process_assign("09", "10", midiMsg, 4); t=1;};
            if(sysxIO->getFileSource("Structure", "09", "00").at(58)=="01"  && sysxIO->getFileSource("Structure", "09", "00").at(66)==CCnumba) {process_assign("09", "30", midiMsg, 5); t=1;};
            if(sysxIO->getFileSource("Structure", "09", "00").at(90)=="01" && sysxIO->getFileSource("Structure", "09", "00").at(98)==CCnumba) {process_assign("09", "50", midiMsg, 6); t=1;};
            if(sysxIO->getFileSource("Structure", "09", "00").at(122)=="01" && sysxIO->getFileSource("Structure", "09", "00").at(130)==CCnumba){process_assign("09", "70", midiMsg, 7); t=1;};
            if(sysxIO->getFileSource("Structure", "0A", "00").at(26)=="01"  && sysxIO->getFileSource("Structure", "0A", "00").at(34)==CCnumba) {process_assign("0A", "10", midiMsg, 8); t=1;};

            if(sysxIO->getFileSource("System", "03", "00").at(0)=="01" && sysxIO->getFileSource("System", "03", "00").at(7)==CCnumba) {process_assign("03", "00", midiMsg, 9); t=1; };
            if(sysxIO->getFileSource("System", "03", "00").at(15)=="01" && sysxIO->getFileSource("System", "03", "00").at(22)==CCnumba) {process_assign("03", "0F", midiMsg, 10); t=1; };
            if(sysxIO->getFileSource("System", "03", "00").at(30)=="01" && sysxIO->getFileSource("System", "03", "00").at(37)==CCnumba){process_assign("03", "1E", midiMsg, 11); t=1; };
            if(sysxIO->getFileSource("System", "03", "00").at(45)=="01" && sysxIO->getFileSource("System", "03", "00").at(52)==CCnumba) {process_assign("03", "2D", midiMsg, 12); t=1; };
            if(sysxIO->getFileSource("System", "03", "00").at(60)=="01" && sysxIO->getFileSource("System", "03", "00").at(67)==CCnumba) {process_assign("03", "3C", midiMsg, 13); t=1; };
            if(sysxIO->getFileSource("System", "03", "00").at(75)=="01" && sysxIO->getFileSource("System", "03", "00").at(82)==CCnumba) {process_assign("03", "4B", midiMsg, 14); t=1; };
            if(sysxIO->getFileSource("System", "03", "00").at(90)=="01" && sysxIO->getFileSource("System", "03", "00").at(97)==CCnumba){process_assign("03", "5A", midiMsg, 15); t=1; };
            if(sysxIO->getFileSource("System", "03", "00").at(105)=="01" && sysxIO->getFileSource("System", "03", "00").at(112)==CCnumba) {process_assign("03", "69", midiMsg, 16); t=1; };

            if(t==1) { emit updateSignal(); }else { /*sysxIO->sendMidi(midiMsg); */};
        }
        else
        {
            sysxIO->sendSysx(midiMsg);  // if none of the above, pass through data to Katana
        };
    };
}

void floorBoardDisplay::process_assign(QString hex1, QString hex3, QString sysxMsg, int assnumb)
{
    QString area="Structure";
    if(assnumb>8){area="System";};
    bool ok;
    int index = hex3.toInt(&ok, 16)+12;
    MidiTable *midiTable = MidiTable::Instance();
    SysxIO *sysxIO = SysxIO::Instance();
    QString hex4 = sysxIO->getFileSource(area, hex1, "00").at(index);
    QString hex5 = sysxIO->getFileSource(area, hex1, "00").at(index+1);
    Midi items = midiTable->getMidiMap("Structure", "08", "00", "31", hex4, hex5);

    int targetMin = (((sysxIO->getFileSource(area, hex1, "00").at(index+2)).toInt(&ok, 16))*128)+((sysxIO->getFileSource("Structure", hex1, "00").at(index+3)).toInt(&ok, 16));
    int targetMax = (((sysxIO->getFileSource(area, hex1, "00").at(index+4)).toInt(&ok, 16))*128)+((sysxIO->getFileSource("Structure", hex1, "00").at(index+5)).toInt(&ok, 16));
    int min = (sysxIO->getFileSource(area, hex1, "00").at(index+8)).toInt(&ok, 16); // get Act Rng Lo
    int max = (sysxIO->getFileSource(area, hex1, "00").at(index+9)).toInt(&ok, 16); // get Act Rng Hi

    QString sysx = sysxMsg.at(sysxMsg.size()-2);  // get value 2nd to last digit
    sysx.append(sysxMsg.at(sysxMsg.size()-1));    // append with get value last digit
    double value = sysx.toInt(&ok, 16);
    int rangeMin = midiTable->getRangeMinimum("Structure", items.desc, "00", items.customdesc); //assign target min value
    int rangeMax = midiTable->getRange("Structure", items.desc, "00", items.customdesc); //assign target max value

    int toggle = (sysxIO->getFileSource(area, hex1, "00").at(index+7)).toInt(&ok, 16);  // get source mode value
    if(toggle>0) { if(value>((min+max)/2)){ value=127; }else{ value=0; }; }  // if toggle set, convert value to 0 or 127
    else{
        value=value-min; if(value<0){value=0;};             //hysterysis for min limit
        if(value>0){value=value+(min);};                    //level min limit
        value=((127-max)+value); if(value>127){value=127;}; //hysterysis for max limit
        if(min>0 || max<127) {value=(value)/(128-value);};     //scale range down
    };


    double maxdiv;
    double assign;
    if(targetMax<128){
        maxdiv=128/((targetMax-targetMin)+1);  // find division ratio from max controller value (127) to max effect range value.
        assign=(value/maxdiv)+targetMin; }     // divide controller value to a proportion of the effect value.
    else{
        maxdiv=(targetMax-targetMin)/128;  // find division ratio from max controller value (127) to max effect range value.
        assign=(value*(maxdiv+1))+targetMin-(maxdiv*maxdiv/4);                   // unless target is bigger than 127 so proportionally multiply it.
    };
    int ass = assign;
    if(ass<rangeMin){ass=rangeMin;};
    if(ass>rangeMax){ass=rangeMax;};
    int overflow=0;
    if(ass>127){ overflow=ass/127; ass=ass-(overflow*127); };


    QString result = QString::number(ass, 16).toUpper();
    if(result.size()==1) {result.prepend("0"); };
    if(overflow>0){result.prepend(QString::number(overflow, 16).toUpper()); };
    if(result.size()==3) {result.prepend("0"); };
    if(rangeMax>127 && result.size()==2) {result.prepend("00"); };
    if(result.size()>4 || ass<0){result="0001";};  // if something goes wrong with a negetive integer result.

    int val = value;
    QString a_min = QString::number(min, 10).toUpper();
    QString a_max = QString::number(max, 10).toUpper();
    QString a_value = QString::number(val, 10).toUpper();
    QString fxRangeMin = QString::number(rangeMin, 10).toUpper();
    QString fxRangeMax = QString::number(rangeMax, 10).toUpper();
    QString targetValMin = QString::number(targetMin, 10).toUpper();
    QString targetValMax = QString::number(targetMax, 10).toUpper();
    QString assgn=QString::number(ass, 16).toUpper();
    QString overf=QString::number(overflow, 16).toUpper();

    deBugBox.setText("Act Range Lo="+a_min+
                     "<br>Act Range Hi="+a_max+
                     "<br>TargetRangeMin="+fxRangeMin+
                     "<br>TargetRangeMax="+fxRangeMax+
                     "<br>TargetSetMin="+targetValMin+
                     "<br>TargetSetMax="+targetValMax+
                     "<br>cc# value="+a_value+
                     "<br>assign="+assgn+
                     "<br>overflow="+overf+
                     "<br>sysx result="+result+
                     "<br>output=F04100000030126000"+items.desc+items.customdesc+result+"007F");
    deBugBox.show();


    if(assnumb==1 && result!=prev1){ sysxIO->setFileSource("Structure", items.desc, "00", items.customdesc, result); prev1=result; };
    if(assnumb==2 && result!=prev2){ sysxIO->setFileSource("Structure", items.desc, "00", items.customdesc, result); prev2=result; };
    if(assnumb==3 && result!=prev3){ sysxIO->setFileSource("Structure", items.desc, "00", items.customdesc, result); prev3=result; };
    if(assnumb==4 && result!=prev4){ sysxIO->setFileSource("Structure", items.desc, "00", items.customdesc, result); prev4=result; };
    if(assnumb==5 && result!=prev5){ sysxIO->setFileSource("Structure", items.desc, "00", items.customdesc, result); prev5=result; };
    if(assnumb==6 && result!=prev6){ sysxIO->setFileSource("Structure", items.desc, "00", items.customdesc, result); prev6=result; };
    if(assnumb==7 && result!=prev7){ sysxIO->setFileSource("Structure", items.desc, "00", items.customdesc, result); prev7=result; };
    if(assnumb==8 && result!=prev8){ sysxIO->setFileSource("Structure", items.desc, "00", items.customdesc, result); prev8=result; };
    if(assnumb==9 && result!=prev9){ sysxIO->setFileSource("Structure", items.desc, "00", items.customdesc, result); prev9=result; };
    if(assnumb==10 && result!=prev10){ sysxIO->setFileSource("Structure", items.desc, "00", items.customdesc, result); prev10=result; };
    if(assnumb==11 && result!=prev11){ sysxIO->setFileSource("Structure", items.desc, "00", items.customdesc, result); prev11=result; };
    if(assnumb==12 && result!=prev12){ sysxIO->setFileSource("Structure", items.desc, "00", items.customdesc, result); prev12=result; };
    if(assnumb==13 && result!=prev13){ sysxIO->setFileSource("Structure", items.desc, "00", items.customdesc, result); prev13=result; };
    if(assnumb==14 && result!=prev14){ sysxIO->setFileSource("Structure", items.desc, "00", items.customdesc, result); prev14=result; };
    if(assnumb==15 && result!=prev15){ sysxIO->setFileSource("Structure", items.desc, "00", items.customdesc, result); prev15=result; };
    if(assnumb==16 && result!=prev16){ sysxIO->setFileSource("Structure", items.desc, "00", items.customdesc, result); prev16=result; };
}

void floorBoardDisplay::kickTimer(int value)
{
    if(value==1){this->midisyncTimer->start(1); }
    else{ this->midisyncTimer->stop(); };
}
