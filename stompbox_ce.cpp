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

#include "stompbox_ce.h"

stompbox_ce::stompbox_ce(QWidget *parent)
    : stompBox(parent)
{
	/* Chorus */
	setImage(":/images/ce.png");
    setLSB("05", "00");
    setKnob1("10", "00", "50");
    setKnob2("10", "00", "52");
    setComboBox("10", "00", "4F");
    setButton("10", "00", "4E");
    editDetails()->patchPos(1624, 42, "05", "00");
	setEditPages();
}

void stompbox_ce::updateSignal()
{
    updateKnob1("10", "00", "50");
    updateKnob2("10", "00", "52");
    updateComboBox("10", "00", "4F");
    updateButton("10", "00", "4E");
    updateSwitch("10", "00", "4E");
}

void stompbox_ce::setEditPages()
{
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "10", "00", "4E", "middle", Qt::AlignCenter);
    editDetails()->page()->newStackControl(0);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "10", "00", "4F", "large");
    editDetails()->page()->addStackControl();
    editDetails()->page()->addGroupBox(0, 0, 2, 1);

    editDetails()->page()->insertStackField(0, 0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Option Select");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "12", "00", "07", "large");
    editDetails()->page()->addComboBox(1, 0, 1, 1, "12", "00", "08", "large");
    editDetails()->page()->addComboBox(2, 0, 1, 1, "12", "00", "09", "large");
    editDetails()->page()->addGroupBox(0, 3, 1, 1);

    // SINGLE
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Delay");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "50", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "52");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Filter");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "53");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "54");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "55");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // PAN
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Delay");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "50", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "52");
    editDetails()->page()->addKnob(0, 2, 1, 1, "10", "00", "56");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Filter");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "53");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "54");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "55");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // STEREO
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Delay");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "50", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "52");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Filter");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "5A");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "54");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "55");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // DUAL SERIES
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Delay 1");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "57", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "59");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Delay 2");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "5C", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "5E");
    editDetails()->page()->addGroupBox(1, 0, 1, 1);

    editDetails()->page()->newGroupBox("Filter 1");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "5A");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Filter 2");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "5F");
    editDetails()->page()->addGroupBox(1, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level 1");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "5B");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);

    editDetails()->page()->newGroupBox("Level 2");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "60");
    editDetails()->page()->addGroupBox(1, 2, 1, 1);

    editDetails()->page()->newGroupBox("Direct");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "55");
    editDetails()->page()->addGroupBox(0, 3, 2, 1);
    editDetails()->page()->addStackField();

    // DUAL PARALLEL
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Delay 1");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "57", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "59");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Delay 2");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "5C", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "5E");
    editDetails()->page()->addGroupBox(1, 0, 1, 1);

    editDetails()->page()->newGroupBox("Filter 1");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "5A");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Filter 2");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "5F");
    editDetails()->page()->addGroupBox(1, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level 1");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "5B");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);

    editDetails()->page()->newGroupBox("Level 2");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "60");
    editDetails()->page()->addGroupBox(1, 2, 1, 1);

    editDetails()->page()->newGroupBox("Direct");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "55");
    editDetails()->page()->addGroupBox(0, 3, 2, 1);
    editDetails()->page()->addStackField();

    // DUAL L/R
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Delay 1");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "57", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "59");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Delay 2");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "5C", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "5E");
    editDetails()->page()->addGroupBox(1, 0, 1, 1);

    editDetails()->page()->newGroupBox("Filter 1");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "5A");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Filter 2");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "5F");
    editDetails()->page()->addGroupBox(1, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level 1");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "5B");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);

    editDetails()->page()->newGroupBox("Level 2");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "60");
    editDetails()->page()->addGroupBox(1, 2, 1, 1);

    editDetails()->page()->newGroupBox("Direct");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "55");
    editDetails()->page()->addGroupBox(0, 3, 2, 1);
    editDetails()->page()->addStackField();

    // REVERSE
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Delay");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "50", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "52");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Filter");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "53");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "54");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "55");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // ANALOG
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Delay");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "50", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "52");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Filter");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "53");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "54");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "55");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // TAPE
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Delay");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "50", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "52");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Filter");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "53");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "54");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "55");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();


    // MODULATE
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Delay");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "50", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "52");
    editDetails()->page()->addGroupBox(0, 0, 1, 2);

    editDetails()->page()->newGroupBox("Modulate");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "61");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "62");
    editDetails()->page()->addGroupBox(1, 0, 1, 2);

    editDetails()->page()->newGroupBox("Filter");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "53");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "54");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "55");
    editDetails()->page()->addGroupBox(0, 3, 2, 1);
    editDetails()->page()->addStackField();

    // SDE-3000
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Delay");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "50", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "52");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Modulation");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "10", "00", "63");
    editDetails()->page()->addComboBox(0, 1, 1, 1, "10", "00", "64");
    editDetails()->page()->addComboBox(0, 2, 1, 1, "10", "00", "67");
    editDetails()->page()->addComboBox(1, 0, 1, 1, "10", "00", "65");
    editDetails()->page()->addComboBox(1, 1, 1, 1, "10", "00", "66");
    editDetails()->page()->addKnob(0, 3, 2, 1, "10", "00", "61");
    editDetails()->page()->addKnob(0, 4, 2, 1, "10", "00", "62");
    editDetails()->page()->addGroupBox(1, 0, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "54");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "55");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
    editDetails()->page()->addStackField();

    editDetails()->addPage();
  	
}
