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

#include "stompbox_ch_a.h"

stompbox_ch_a::stompbox_ch_a(QWidget *parent)
    : stompBox(parent)
{
    /* CHANNEL_A PREAMP*/
    setImage(":/images/ch_a.png");
    setLSB("00", "50");
    //setButton("00", "00", "50");
    setComboBox("00", "00", "51");
    setKnob1("00", "00", "52");
    setKnob2("00", "00", "58");
    editDetails()->patchPos(182, 64, "00", "50");
    setEditPages();
}

void stompbox_ch_a::updateSignal()
{
    //updateButton("00", "00", "50");
    updateComboBox("00", "00", "51");
    updateKnob1("00", "00", "52");
    updateKnob2("00", "00", "58");
    updateSwitch("00", "00", "50");
    updateSwitch("00", "00", "5B");
}

void stompbox_ch_a::setEditPages()
{
    // CHANNEL A
    editDetails()->page()->newGroupBox(tr("Channel A"));

    editDetails()->page()->newGroupBox(tr("Pre Amp"));
    editDetails()->page()->newStackControl(0);
    editDetails()->page()->addComboBox(0, 0, 1, 1, "00", "00", "51", "large");        //pre type
    editDetails()->page()->addStackControl();
    //editDetails()->page()->addComboBox(1, 0, 1, 1, "00", "00", "5A", "bottom", Qt::AlignLeft); //gain sw
    editDetails()->page()->addKnob(0, 1, 2, 1, "00", "00", "52", "turbo");   // gain
    //editDetails()->page()->addKnob(0, 2, 2, 1, "00", "00", "53");            //T-Comp
    editDetails()->page()->addKnob(0, 3, 2, 1, "00", "00", "54");            //bass
    editDetails()->page()->addKnob(0, 4, 2, 1, "00", "00", "55");            // mid
    editDetails()->page()->addKnob(0, 5, 2, 1, "00", "00", "56");            // treble
    editDetails()->page()->addKnob(0, 6, 2, 1, "00", "00", "57");            // presence
    editDetails()->page()->insertStackField(0, 0, 7, 2, 1);                // bright button
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox(tr("Pre Amp"));
    editDetails()->page()->addKnob(1, 0, 3, 1, "00", "00", "58");            // effect level
    editDetails()->page()->addGroupBox(0, 3, 4, 1);

    //editDetails()->page()->addSystemOverride(0, 0, 4, 7, "00", "00", "41", "00");
    editDetails()->page()->addGroupBox(0, 0, 4, 1);


    // Bright Button stack fields
    editDetails()->page()->newStackField(0);  //natural clean
    editDetails()->page()->addSwitch(0, 0, 1, 1, "00", "00", "59", "middle", Qt::AlignCenter);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); // full range
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);  //combo crunch
    editDetails()->page()->addSwitch(0, 0, 1, 1, "00", "00", "59", "middle", Qt::AlignCenter);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); // stack crunch
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); //hi gain stack
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); // power drive
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); // extreme lead
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); //core metal
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); //jc-120
    editDetails()->page()->addSwitch(0, 0, 1, 1, "00", "00", "59", "middle", Qt::AlignCenter);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);  //clean twin
    editDetails()->page()->addSwitch(0, 0, 1, 1, "00", "00", "59", "middle", Qt::AlignCenter);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); //pro crunch
    editDetails()->page()->addSwitch(0, 0, 1, 1, "00", "00", "59", "middle", Qt::AlignCenter);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); //tweed
    editDetails()->page()->addSwitch(0, 0, 1, 1, "00", "00", "59", "middle", Qt::AlignCenter);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); //deluxe crunch
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); //VO drive
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); // VO lead
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);  // match drive
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);  //bg lead
    editDetails()->page()->addSwitch(0, 0, 1, 1, "00", "00", "59", "middle", Qt::AlignCenter);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);  //bg drive
    editDetails()->page()->addSwitch(0, 0, 1, 1, "00", "00", "59", "middle", Qt::AlignCenter);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); // ms 1951 i
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); // ms 1951 i+ii
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);  // rfier vint
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); // rfier modern
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); // t-amp lead
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); // sldn
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);  // 5150 drive
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); // custom
    editDetails()->page()->addComboBox(0, 0, 1, 1, "00", "00", "63");  //type
    editDetails()->page()->addKnob(0, 2, 1, 1, "00", "00", "64");  //bottom
    editDetails()->page()->addKnob(0, 3, 1, 1, "00", "00", "65");  //edge
    editDetails()->page()->addKnob(0, 4, 1, 1, "00", "00", "68");  //pre-lo
    editDetails()->page()->addKnob(0, 5, 1, 1, "00", "00", "69");  //pre-hi
    editDetails()->page()->addKnob(0, 6, 1, 1, "00", "00", "6A");  //character
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); // bogner
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);  // orange
    editDetails()->page()->addStackField();

    editDetails()->addPage();
}
