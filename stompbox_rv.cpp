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

#include "stompbox_rv.h"

stompbox_rv::stompbox_rv(QWidget *parent)
    : stompBox(parent)
{
    /* REVERB */
    setImage(":/images/rv.png");
    setLSB("06", "00");
    setKnob1("06", "00", "12");
    setKnob2("06", "00", "18");
    setComboBox("06", "00", "11");
    setButton("06", "00", "10");
    editDetails()->patchPos(1746, 22, "06", "10");
    setEditPages();
}

void stompbox_rv::updateSignal()
{
    updateKnob1("06", "00", "12");
    updateKnob2("06", "00", "18");
    updateComboBox("06", "00", "11");
    updateButton("06", "00", "10");
    updateSwitch("06", "00", "10");
}

void stompbox_rv::setEditPages()
{
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "06", "00", "10", "middle", Qt::AlignCenter);
    editDetails()->page()->newStackControl(0);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "06", "00", "11", "large");
    editDetails()->page()->addStackControl();
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Option Select");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "12", "00", "0D", "large");
    editDetails()->page()->addComboBox(1, 0, 1, 1, "12", "00", "0E", "large");
    editDetails()->page()->addComboBox(2, 0, 1, 1, "12", "00", "0F", "large");
    editDetails()->page()->addGroupBox(1, 0, 1, 1);

    editDetails()->page()->newGroupBox("Reverb");
    editDetails()->page()->addKnob(0, 0, 1, 1, "06", "00", "13", "normal","right", 60);
    editDetails()->page()->addKnob(0, 1, 1, 1, "06", "00", "12", "normal","right", 80);
    editDetails()->page()->insertStackField(0, 0, 2, 1, 1);
    editDetails()->page()->addKnob(0, 3, 1, 1, "06", "00", "17");
    editDetails()->page()->addGroupBox(0, 1, 1, 2);

    editDetails()->page()->newGroupBox("Filter");
    editDetails()->page()->addKnob(0, 0, 1, 1, "06", "00", "15");
    editDetails()->page()->addKnob(0, 1, 1, 1, "06", "00", "16");
    editDetails()->page()->addGroupBox(1, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "06", "00", "18");
    editDetails()->page()->addKnob(0, 1, 1, 1, "06", "00", "19");
    editDetails()->page()->addGroupBox(1, 2, 1, 1);

    editDetails()->addPage();

    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);
    editDetails()->page()->addKnob(0, 0, 1, 1, "06", "00", "1A"); // spring sensitivity
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
}

