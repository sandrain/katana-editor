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

#ifndef FLOORBOARDDISPLAY_H
#define FLOORBOARDDISPLAY_H

#include <QtWidgets>
#include <QWidget>
#include <QTimer>
#include "customButton.h"
#include "customPanelButton.h"
#include "customControlListMenu.h"
#include "customDisplay.h"
#include "customLabelDisplay.h"
#include "initPatchListMenu.h"
#include "customLogo.h"
#include "customAssign.h"
#include "sysxWriter.h"


class floorBoardDisplay : public QWidget
{
    Q_OBJECT

public:
    floorBoardDisplay(QWidget *parent = 0,
                      QPoint pos = QPoint(0, 0));
    QPoint getPos();
    customButton *connectButton;
    bool connectButtonActive;

public slots:
    void setValueDisplay(QString fxName, QString valueName, QString value);
    void setPatchDisplay(QString patchName);
    void setPatchNumDisplay(int bank, int patch);
    void setPos(QPoint newPos);
    void updateDisplay();
    void set_temp();
    void temp1_copy(bool value);
    void temp1_paste(bool value);
    void temp2_copy(bool value);
    void temp2_paste(bool value);
    void temp3_copy(bool value);
    void temp3_paste(bool value);
    void temp4_copy(bool value);
    void temp4_paste(bool value);
    void temp5_copy(bool value);
    void temp5_paste(bool value);
    void save_temp(QString fileName, QString sysxMsg);
    void autoconnect();
    void autoConnectionResult(QString);
    void connectSignal(bool value);
    void connectionResult(QString);
    void autosync_off(bool value);
    void autosyncSignal(bool value);
    void autosyncResult(QString);
    void writeSignal(bool value);
    void resetDevice(QString replyMsg);
    void patchSelectSignal(int bank, int patch);
    void blinkSellectedPatch(bool active = true);
    void patchLoadSignal(int bank, int patch);
    void notConnected();
    void patchChangeFailed();
    void writeToBuffer();
    void writeToMemory();
    void writeToLoader(int patch, QString function);
    void valueUpdate();
    void autosync(int bank, int patch);
    void autosyncGo();
    void valueChanged(bool, QString , QString, QString);
    void midiInput(QString sysxMsg);
    void process_assign(QString hex1, QString hex3, QString sysxMsg, int assnumb);
    void kickTimer(int value);

signals:
    void updateSignal();
    void dialogUpdateSignal();
    void connectedSignal();
    void notConnectedSignal();
    void loadnames();

    void setStatusSymbol(int value);
    void setStatusProgress(int value);
    void setStatusMessage(QString message);
    void connected(bool value);

private:
    QPoint pos;
    customDisplay *valueDisplay;
    customDisplay *patchDisplay;
    customDisplay *patchNumDisplay;
    customLabelDisplay *temp1Display;
    customLabelDisplay *temp2Display;
    customLabelDisplay *temp3Display;
    customLabelDisplay *temp4Display;
    customLabelDisplay *temp5Display;
    customControlListMenu *output;
    customControlListMenu *catagory;
    QLineEdit* chain_txt;
    customButton *autoButton;

    customButton *writeButton;
    customPanelButton *midi_Button;
    customPanelButton *system_Button;
    customPanelButton *master_Button;
    customPanelButton *ez_edit_Button;
    customPanelButton *preamp1_Button;
    customPanelButton *distortion_Button;
    customPanelButton *ns1_Button;
    customPanelButton *fx1_Button;
    customPanelButton *fx2_Button;
    customPanelButton *eq_Button;
    customPanelButton *ce_Button;
    customPanelButton *reverb_Button;
    customPanelButton *delay_Button;
    customPanelButton *sendreturn_Button;
    customPanelButton *pedal_Button;
    customPanelButton *fv_Button;
    customPanelButton *chain_1_Button;
    customPanelButton *chain_2_Button;
    customPanelButton *chain_3_Button;
    customPanelButton *chain_4_Button;
    customPanelButton *chain_5_Button;
    customPanelButton *chain_6_Button;
    customButton *temp1_copy_Button;
    customButton *temp1_paste_Button;
    customButton *temp2_copy_Button;
    customButton *temp2_paste_Button;
    customButton *temp3_copy_Button;
    customButton *temp3_paste_Button;
    customButton *temp4_copy_Button;
    customButton *temp4_paste_Button;
    customButton *temp5_copy_Button;
    customButton *temp5_paste_Button;
    customButton *temp6_copy_Button;
    customButton *temp6_paste_Button;
    customPanelButton *assign1_Button;
    customPanelButton *assign2_Button;
    customPanelButton *assign3_Button;
    customPanelButton *assign4_Button;
    customPanelButton *assign5_Button;
    customPanelButton *assign6_Button;
    customPanelButton *assign7_Button;
    customPanelButton *assign8_Button;
    customPanelButton *assign9_Button;
    customPanelButton *assign10_Button;
    customPanelButton *assign11_Button;
    customPanelButton *assign12_Button;
    customPanelButton *assign13_Button;
    customPanelButton *assign14_Button;
    customPanelButton *assign15_Button;
    customPanelButton *assign16_Button;
    sysxWriter file;
    //bool connectButtonActive;
    bool autosyncButtonActive;
    bool patchLoadError;
    QTimer* timer;
    QTimer* autosyncTimer;
    QTimer* midisyncTimer;
    int blinkCount;
    bool currentSyncStatus;
    initPatchListMenu *initPatch;
    QString patchName;
    QString temp1_sysxMsg;
    QString temp2_sysxMsg;
    QString temp3_sysxMsg;
    QString temp4_sysxMsg;
    QString temp5_sysxMsg;
    int set_bank;
    int set_patch;
    customLogo* logo;
    bool logo_state;
    customAssign* assignBar;
    customAssign* assignBar2;
    QString prev1;
    QString prev2;
    QString prev3;
    QString prev4;
    QString prev5;
    QString prev6;
    QString prev7;
    QString prev8;
    QString prev9;
    QString prev10;
    QString prev11;
    QString prev12;
    QString prev13;
    QString prev14;
    QString prev15;
    QString prev16;
    QMessageBox deBugBox;
};

#endif // FLOORBOARDDISPLAY_H
