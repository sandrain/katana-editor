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

#include "stompbox_od.h"

stompbox_od::stompbox_od(QWidget *parent)
    : stompBox(parent)
{
    /* OD/DS */
    setImage(":/images/od.png");
    setLSB("00", "00");
    setKnob1("00", "00", "32");
    setKnob2("00", "00", "37");
    setComboBox("00", "00", "31");
    setButton("00", "00", "30");
    editDetails()->patchPos(108, 30, "00", "30");
    setEditPages();
}

void stompbox_od::updateSignal()
{
    updateKnob1("00", "00", "32");
    updateKnob2("00", "00", "37");
    updateComboBox("00", "00", "31");
    updateButton("00", "00", "30");
    updateSwitch("00", "00", "30");
}

void stompbox_od::setEditPages()
{
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "00", "00", "30", "middle", Qt::AlignCenter);
    editDetails()->page()->newStackControl(0);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "00", "00", "31", "large");
    editDetails()->page()->addStackControl();
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Drive");
    editDetails()->page()->addKnob(0, 0, 1, 1, "00", "00", "32", "turbo");
    editDetails()->page()->addKnob(0, 1, 1, 1, "00", "00", "33");
    editDetails()->page()->addKnob(0, 2, 1, 1, "00", "00", "34");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "00", "00", "37");
    editDetails()->page()->addKnob(0, 1, 1, 1, "00", "00", "38");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);

    editDetails()->page()->newGroupBox("Solo");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "00", "00", "35", "middle", Qt::AlignCenter);
    editDetails()->page()->addKnob(0, 1, 1, 1, "00", "00", "36");
    editDetails()->page()->addGroupBox(1, 0, 1, 1);

    editDetails()->page()->newGroupBox("Option Select");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "12", "00", "01", "large");
    editDetails()->page()->addComboBox(1, 0, 1, 1, "12", "00", "02", "large");
    editDetails()->page()->addComboBox(2, 0, 1, 1, "12", "00", "03", "large");
    editDetails()->page()->addGroupBox(0, 3, 2, 1);

    editDetails()->page()->insertStackField(0, 1, 1, 2, 2);

    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Custom");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "00", "00", "39");
    editDetails()->page()->addKnob(0, 1, 1, 1, "00", "00", "3A");
    editDetails()->page()->addKnob(0, 2, 1, 1, "00", "00", "3B");
    editDetails()->page()->addKnob(0, 3, 1, 1, "00", "00", "3C");
    editDetails()->page()->addKnob(0, 4, 1, 1, "00", "00", "3D");
    editDetails()->page()->addKnob(0, 5, 1, 1, "00", "00", "3E");
    editDetails()->page()->addGroupBox(1, 0, 1, 5);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);editDetails()->page()->addStackField();

    editDetails()->addPage();
}
