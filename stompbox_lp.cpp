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

#include "stompbox_lp.h"
#include "Preferences.h"

stompbox_lp::stompbox_lp(QWidget *parent)
    : stompBox(parent)
{
    /* LOOP */
    setImage(":/images/lp.png");
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const int model = preferences->getPreferences("Midi", "Katana", "model").toInt(&ok);
    if(model>0){

    setLSB("06", "00");
    setKnob1("06", "00", "57");
    setKnob2("06", "00", "58");
    setComboBox("06", "00", "56");
    setButton("06", "00", "55");
    editDetails()->patchPos(1884, 8, "06", "55");
    };
    setEditPages();
}

void stompbox_lp::updateSignal()
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const int model = preferences->getPreferences("Midi", "Katana", "model").toInt(&ok);
    if(model>0){
    updateKnob1("06", "00", "57");
    updateKnob2("06", "00", "58");
    updateComboBox("06", "00", "56");
    updateButton("06", "00", "55");
    updateSwitch("06", "00", "55");
    };
}

void stompbox_lp::setEditPages()
{
    QString text = tr("Send/Return LOOP");
    editDetails()->page()->addLabel(0, 0, 1, 3, text);
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const int model = preferences->getPreferences("Midi", "Katana", "model").toInt(&ok);
    if(model>0){
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "06", "00", "55"); //on/off
    editDetails()->page()->addComboBox(1, 0, 1, 1, "06", "00", "56"); //mode
    editDetails()->page()->addGroupBox(1, 0, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "06", "00", "57"); //send
    editDetails()->page()->addKnob(0, 1, 1, 1, "06", "00", "58"); //return
    editDetails()->page()->addGroupBox(1, 1, 1, 1);

}else{editDetails()->page()->addLabel(1, 0, 1, 3, "no available for this model"); };

    editDetails()->addPage();
}
