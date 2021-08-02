/****************************************************************************
**
** Copyright (C) 2007~2020 Colin Willcocks.
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

#include "menuPage_system.h"
#include "Preferences.h"

menuPage_system::menuPage_system(QWidget *parent)
    : menuPage(parent)
{ 
    setLSB("00", "00");
    setEditPages();
}

void menuPage_system::updateSignal()
{

}

void menuPage_system::setEditPages()
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const int model = preferences->getPreferences("Midi", "Katana", "model").toInt(&ok);

    QString text = tr("***NOTE*** Settings changes are automatically written to KATANA when changed");

    editDetails()->page()->newGroupBox("User Options " + text);
    editDetails()->page()->newGroupBox("Output");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "00", "04", "30", "System"); // line out mode
    editDetails()->page()->addComboBox(1, 0, 1, 1, "00", "04", "31", "System"); // cabinet resonance
    editDetails()->page()->addGroupBox(0, 1, 3, 1);

    editDetails()->page()->newGroupBox("Global Equalizer");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "00", "04", "32", "System", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "00", "04", "3E", "System"); // position
    editDetails()->page()->newStackControl(0);
    editDetails()->page()->addComboBox(2, 0, 1, 1, "00", "04", "3F", "System"); // type
    editDetails()->page()->addStackControl();
    editDetails()->page()->insertStackField(0, 0, 1, 3, 1);
    editDetails()->page()->addGroupBox(0, 0, 3, 1);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newStackField(0);
    editDetails()->page()->addParaEQ(0, 0, 1, 1, "00", "04", "33", "System");
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);
    editDetails()->page()->addGraphicEQ(0, 0, 1, 1, "00", "04", "40", "System_geq_slider");
    editDetails()->page()->addStackField();

    editDetails()->addPage("00", "00", "2F", "00", "Tables");


    editDetails()->page()->addLabel(0, 0, 1, 3, text);

    editDetails()->page()->newGroupBox("USB settings");
    editDetails()->page()->addKnob(0, 0, 1, 1, "00", "00", "51", "System"); //usb to effect
    editDetails()->page()->addKnob(0, 1, 1, 1, "00", "00", "52", "System"); //usb input level
    editDetails()->page()->addKnob(0, 2, 1, 1, "00", "00", "53", "System"); //usb mix level
    editDetails()->page()->addComboBox(0, 3, 1, 1, "00", "00", "56", "System"); //usb loopback
    editDetails()->page()->addKnob(0, 4, 1, 1, "00", "00", "57", "System"); //usb Dry out level
    editDetails()->page()->addGroupBox(1, 0, 1, 3);

    editDetails()->page()->newGroupBox("Input Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "00", "00", "71", "System"); // input level
    editDetails()->page()->addGroupBox(2, 0, 1, 1);
    editDetails()->page()->newGroupBox("Global NS/Reverb");
    editDetails()->page()->addKnob(0, 2, 1, 1, "00", "00", "77", "System"); // NS threshold
    editDetails()->page()->addKnob(0, 3, 1, 1, "00", "00", "78", "System"); // revberb level
    editDetails()->page()->addGroupBox(2, 2, 1, 1);

    editDetails()->addPage("00", "00", "2F", "01", "Tables");


    editDetails()->page()->newGroupBox(text);
    editDetails()->page()->newGroupBox("Midi Channel");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "02", "00", "00", "System"); // rx channel
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("RECEIVED PROGRAM CHANGE");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "02", "00", "20", "System"); // panel ch
    editDetails()->page()->addComboBox(1, 0, 1, 1, "02", "00", "21", "System"); // A1
    editDetails()->page()->addComboBox(2, 0, 1, 1, "02", "00", "22", "System"); // A2
    if(model>0){
    editDetails()->page()->addComboBox(3, 0, 1, 1, "02", "00", "23", "System"); // A3
    editDetails()->page()->addComboBox(4, 0, 1, 1, "02", "00", "24", "System"); // A4
    };
    editDetails()->page()->addComboBox(5, 0, 1, 1, "02", "00", "25", "System"); // B1
    editDetails()->page()->addComboBox(6, 0, 1, 1, "02", "00", "26", "System"); // B2
    if(model>0){
    editDetails()->page()->addComboBox(7, 0, 1, 1, "02", "00", "27", "System"); // B3
    editDetails()->page()->addComboBox(8, 0, 1, 1, "02", "00", "28", "System"); // B4
    };
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("RECEIVED CC# CONTROL");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "02", "00", "10", "System"); // FX1
    editDetails()->page()->addComboBox(1, 0, 1, 1, "02", "00", "11", "System"); // FX2
    editDetails()->page()->addComboBox(2, 0, 1, 1, "02", "00", "12", "System"); // FX3
    if(model>0){
    editDetails()->page()->addComboBox(3, 0, 1, 1, "02", "00", "13", "System"); // LOOP
    };
    editDetails()->page()->addComboBox(4, 0, 1, 1, "02", "00", "14", "System"); // EXP
    if(model>0){
    editDetails()->page()->addComboBox(5, 0, 1, 1, "02", "00", "15", "System"); // GAFC EXP1
    editDetails()->page()->addComboBox(6, 0, 1, 1, "02", "00", "16", "System"); // GAFC EXP2
    };
    editDetails()->page()->addGroupBox(0, 2, 1, 1);

    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->addPage("00", "00", "2F", "02", "Tables");
}
