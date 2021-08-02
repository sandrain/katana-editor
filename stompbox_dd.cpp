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

#include "stompbox_dd.h"

stompbox_dd::stompbox_dd(QWidget *parent)
    : stompBox(parent)
{
	/* DELAY */
    setImage(":/images/dd.png");
    setLSB("05", "00");
    setKnob1("05", "00", "62");
    setKnob2("05", "00", "64");
    setComboBox("05", "00", "61");
    setButton("05", "00", "60");
    editDetails()->patchPos(1624, 42, "05", "00");
    setEditPages();
}

void stompbox_dd::updateSignal()
{
    updateKnob1("05", "00", "62");
    updateKnob2("05", "00", "64");
    updateComboBox("05", "00", "61");
    updateButton("05", "00", "60");
    updateSwitch("05", "00", "60");
}

void stompbox_dd::setEditPages()
{
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "05", "00", "60", "middle", Qt::AlignCenter);
	editDetails()->page()->newStackControl(0);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "05", "00", "61", "large");
	editDetails()->page()->addStackControl();
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
	
    editDetails()->page()->insertStackField(0, 0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Option Select");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "12", "00", "07", "large");
    editDetails()->page()->addComboBox(1, 0, 1, 1, "12", "00", "08", "large");
    editDetails()->page()->addComboBox(2, 0, 1, 1, "12", "00", "09", "large");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);

	// SINGLE 
	editDetails()->page()->newStackField(0);
	editDetails()->page()->newGroupBox("Delay");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "62", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "64");
    editDetails()->page()->addGroupBox(1, 0, 2, 1);

	editDetails()->page()->newGroupBox("Filter");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "65");
    editDetails()->page()->addGroupBox(1, 1, 2, 1);

	editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "66");
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "67");
    editDetails()->page()->addGroupBox(1, 2, 2, 1);
	editDetails()->page()->addStackField();

	// PAN 
	editDetails()->page()->newStackField(0);
	editDetails()->page()->newGroupBox("Delay");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "62", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "64");
    editDetails()->page()->addKnob(0, 2, 1, 1, "05", "00", "68");
    editDetails()->page()->addGroupBox(0, 0, 2, 1);

	editDetails()->page()->newGroupBox("Filter");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "6C");
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

	editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "66");
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "67");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
	editDetails()->page()->addStackField();

	// STEREO 
	editDetails()->page()->newStackField(0);
	editDetails()->page()->newGroupBox("Delay");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "62", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "64");
    editDetails()->page()->addGroupBox(0, 0, 2, 1);

	editDetails()->page()->newGroupBox("Filter");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "6C");
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

	editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "66");
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "67");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
	editDetails()->page()->addStackField();

	// DUAL SERIES 
	editDetails()->page()->newStackField(0);
	editDetails()->page()->newGroupBox("Delay 1");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "69", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "6B");
	editDetails()->page()->addGroupBox(0, 0, 1, 1);

	editDetails()->page()->newGroupBox("Delay 2");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "6E", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "70");
	editDetails()->page()->addGroupBox(1, 0, 1, 1);

	editDetails()->page()->newGroupBox("Filter 1");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "6C");
	editDetails()->page()->addGroupBox(0, 1, 1, 1);

	editDetails()->page()->newGroupBox("Filter 2");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "71");
	editDetails()->page()->addGroupBox(1, 1, 1, 1);

	editDetails()->page()->newGroupBox("Level 1");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "6D");
	editDetails()->page()->addGroupBox(0, 2, 1, 1);

	editDetails()->page()->newGroupBox("Level 2");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "72");
	editDetails()->page()->addGroupBox(1, 2, 1, 1);

	editDetails()->page()->newGroupBox("Direct");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "67");
	editDetails()->page()->addGroupBox(0, 3, 2, 1);
	editDetails()->page()->addStackField();

	// DUAL PARALLEL 
	editDetails()->page()->newStackField(0);
	editDetails()->page()->newGroupBox("Delay 1");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "69", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "6B");
	editDetails()->page()->addGroupBox(0, 0, 1, 1);

	editDetails()->page()->newGroupBox("Delay 2");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "6E", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "70");
	editDetails()->page()->addGroupBox(1, 0, 1, 1);

	editDetails()->page()->newGroupBox("Filter 1");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "6C");
	editDetails()->page()->addGroupBox(0, 1, 1, 1);

	editDetails()->page()->newGroupBox("Filter 2");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "71");
	editDetails()->page()->addGroupBox(1, 1, 1, 1);

	editDetails()->page()->newGroupBox("Level 1");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "6D");
	editDetails()->page()->addGroupBox(0, 2, 1, 1);

	editDetails()->page()->newGroupBox("Level 2");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "72");
	editDetails()->page()->addGroupBox(1, 2, 1, 1);

	editDetails()->page()->newGroupBox("Direct");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "67");
	editDetails()->page()->addGroupBox(0, 3, 2, 1);
	editDetails()->page()->addStackField();

	// DUAL L/R 
	editDetails()->page()->newStackField(0);
	editDetails()->page()->newGroupBox("Delay 1");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "69", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "6B");
	editDetails()->page()->addGroupBox(0, 0, 1, 1);

	editDetails()->page()->newGroupBox("Delay 2");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "6E", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "70");
	editDetails()->page()->addGroupBox(1, 0, 1, 1);

	editDetails()->page()->newGroupBox("Filter 1");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "6C");
	editDetails()->page()->addGroupBox(0, 1, 1, 1);

	editDetails()->page()->newGroupBox("Filter 2");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "71");
	editDetails()->page()->addGroupBox(1, 1, 1, 1);

	editDetails()->page()->newGroupBox("Level 1");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "6D");
	editDetails()->page()->addGroupBox(0, 2, 1, 1);

	editDetails()->page()->newGroupBox("Level 2");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "72");
	editDetails()->page()->addGroupBox(1, 2, 1, 1);

	editDetails()->page()->newGroupBox("Direct");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "67");
	editDetails()->page()->addGroupBox(0, 3, 2, 1);
	editDetails()->page()->addStackField();

	// REVERSE 
	editDetails()->page()->newStackField(0);
	editDetails()->page()->newGroupBox("Delay");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "62", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "64");
    editDetails()->page()->addGroupBox(0, 0, 2, 1);

	editDetails()->page()->newGroupBox("Filter");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "65");
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

	editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "66");
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "67");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
	editDetails()->page()->addStackField();

	// ANALOG 
	editDetails()->page()->newStackField(0);
	editDetails()->page()->newGroupBox("Delay");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "62", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "64");
    editDetails()->page()->addGroupBox(0, 0, 2, 1);

	editDetails()->page()->newGroupBox("Filter");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "65");
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

	editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "66");
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "67");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
	editDetails()->page()->addStackField();

	// TAPE 
	editDetails()->page()->newStackField(0);
	editDetails()->page()->newGroupBox("Delay");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "62", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "64");
    editDetails()->page()->addGroupBox(0, 0, 2, 1);

	editDetails()->page()->newGroupBox("Filter");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "65");
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

	editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "66");
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "67");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
	editDetails()->page()->addStackField();


	// MODULATE 
	editDetails()->page()->newStackField(0);
	editDetails()->page()->newGroupBox("Delay");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "62", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "64");
	editDetails()->page()->addGroupBox(0, 0, 1, 2);

	editDetails()->page()->newGroupBox("Modulate");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "73");
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "74");
	editDetails()->page()->addGroupBox(1, 0, 1, 2);

	editDetails()->page()->newGroupBox("Filter");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "65");
	editDetails()->page()->addGroupBox(0, 2, 2, 1);

	editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "66");
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "67");
	editDetails()->page()->addGroupBox(0, 3, 2, 1);
	editDetails()->page()->addStackField();

    // SDE-3000
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Delay");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "62", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "64");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Modulation");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "10", "00", "49");
    editDetails()->page()->addComboBox(0, 1, 1, 1, "10", "00", "4A");
    editDetails()->page()->addComboBox(0, 2, 1, 1, "10", "00", "4D");
    editDetails()->page()->addComboBox(1, 0, 1, 1, "10", "00", "4B");
    editDetails()->page()->addComboBox(1, 1, 1, 1, "10", "00", "4C");
    editDetails()->page()->addKnob(0, 3, 2, 1, "05", "00", "73");
    editDetails()->page()->addKnob(0, 4, 2, 1, "05", "00", "74");
    editDetails()->page()->addGroupBox(1, 0, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "66");
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "67");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
    editDetails()->page()->addStackField();

    editDetails()->addPage();
}
