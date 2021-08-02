/****************************************************************************
**
** Copyright (C) 2007~2015 Colin Willcocks.
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

#include "stompbox_ch_b.h"

stompbox_ch_b::stompbox_ch_b(QWidget *parent)
    : stompBox(parent)
{
    /* CHANNEL B PREAMP */
    setImage(":/images/ch_b.png");
    setLSB("01", "00");
    /*setButton("01", "00", "00");
    setComboBox("01", "00", "01");
    setKnob1("01", "00", "02");
    setKnob2("01", "00", "08");*/
    editDetails()->patchPos(304, 64, "01", "00");
    setEditPages();
}

void stompbox_ch_b::updateSignal()
{
   /* updateButton("01", "00", "00");
    updateComboBox("01", "00", "01");
    updateKnob1("01", "00", "02");
    updateKnob2("01", "00", "08");
    updateSwitch("01", "00", "00");
    updateSwitch("01", "00", "0B");*/
}

void stompbox_ch_b::setEditPages()
{
    // CHANNEL B
 /*   editDetails()->page()->newGroupBox(tr("Channel B"));

    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "00");   // off/on effect
    editDetails()->page()->addGroupBox(0, 0, 2, 1);

    editDetails()->page()->newGroupBox(tr("Solo"));
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "0B");
    editDetails()->page()->addKnob(1, 0, 2, 1, "01", "00", "0C");
    editDetails()->page()->addGroupBox(2, 0, 2, 1);

    editDetails()->page()->newGroupBox(tr("Pre Amp"));
    editDetails()->page()->newStackControl(0);
    editDetails()->page()->addComboBox(0, 0, 1, 1, "01", "00", "01", "large");  // pre type
    editDetails()->page()->addStackControl();
    editDetails()->page()->addComboBox(1, 0, 1, 1, "01", "00", "0A", "bottom", Qt::AlignLeft); // gain sw
    editDetails()->page()->addKnob(0, 1, 2, 1, "01", "00", "02", "turbo");  //gain
    editDetails()->page()->addKnob(0, 2, 2, 1, "01", "00", "03");            // t-comp
    editDetails()->page()->addKnob(0, 3, 2, 1, "01", "00", "04");            // bass
    editDetails()->page()->addKnob(0, 4, 2, 1, "01", "00", "05");           // mid
    editDetails()->page()->addKnob(0, 5, 2, 1, "01", "00", "06");           // treble
    editDetails()->page()->addKnob(0, 6, 2, 1, "01", "00", "07");           //presence
    editDetails()->page()->insertStackField(0, 0, 7, 2, 1);                 // bright switch
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox(tr("Pre Amp"));
    editDetails()->page()->addKnob(1, 0, 3, 1, "01", "00", "08");          //effect level
    editDetails()->page()->addGroupBox(0, 3, 4, 1);

    editDetails()->page()->newGroupBox(tr("Speaker - only active while Output-Select set to Line/Phones"));
    editDetails()->page()->newStackControl(1);
    editDetails()->page()->addComboBox(0, 0, 1, 1, "01", "00", "0D", "large");   // speaker type
    editDetails()->page()->addStackControl();
    editDetails()->page()->addComboBox(0, 1, 1, 1, "01", "00", "0E", "bottom", Qt::AlignRight);  // mic type
    editDetails()->page()->addComboBox(0, 2, 1, 1, "01", "00", "0F");  // mic dis
    editDetails()->page()->addKnob(0, 3, 1, 1, "01", "00", "10");   // mic pos
    editDetails()->page()->addKnob(0, 4, 1, 1, "01", "00", "11");    // mic level
    editDetails()->page()->addKnob(0, 5, 1, 1, "01", "00", "12");   // direct level
    editDetails()->page()->insertStackField(1, 1, 0, 1, 7);
    editDetails()->page()->addSystemOverride(0, 0, 2, 6, "00", "00", "70", "00");
    editDetails()->page()->addGroupBox(2, 1, 2, 1);

    editDetails()->page()->addSystemOverride(0, 0, 4, 7, "00", "00", "41", "00");
    editDetails()->page()->addGroupBox(0, 1, 4, 1);




    // Bright Button stack fields
    editDetails()->page()->newStackField(0);  //natural clean
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "09", "middle", Qt::AlignCenter);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); // full range
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);  //combo crunch
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "09", "middle", Qt::AlignCenter);
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
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "09", "middle", Qt::AlignCenter);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);  //clean twin
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "09", "middle", Qt::AlignCenter);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); //pro crunch
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "09", "middle", Qt::AlignCenter);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); //tweed
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "09", "middle", Qt::AlignCenter);
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
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "09", "middle", Qt::AlignCenter);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);  //bg drive
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "09", "middle", Qt::AlignCenter);
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
    editDetails()->page()->addComboBox(0, 0, 1, 1, "01", "00", "13");
    editDetails()->page()->addKnob(0, 2, 1, 1, "01", "00", "14");
    editDetails()->page()->addKnob(0, 3, 1, 1, "01", "00", "15");
    editDetails()->page()->addKnob(0, 4, 1, 1, "01", "00", "18");
    editDetails()->page()->addKnob(0, 5, 1, 1, "01", "00", "19");
    editDetails()->page()->addKnob(0, 6, 1, 1, "01", "00", "1A");
    editDetails()->page()->addStackField();


    editDetails()->page()->newStackField(1);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addKnob(0, 0, 1, 1, "01", "00", "1B", "normal","right", 40);
    editDetails()->page()->addKnob(0, 1, 1, 1, "01", "00", "1C", "normal","right", 40);
    editDetails()->page()->addKnob(0, 2, 1, 1, "01", "00", "1D", "normal","right", 40);
    editDetails()->page()->addComboBox(0, 3, 1, 1, "01", "00", "1E", "bottom", Qt::AlignHCenter);
    editDetails()->page()->addComboBox(0, 4, 1, 1, "01", "00", "1F", "bottom", Qt::AlignHCenter);
    editDetails()->page()->addStackField();  

    editDetails()->page()->newStackField(0); // bogner
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);  // orange
    editDetails()->page()->addStackField(); */

    editDetails()->addPage();
}
