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

#include "stompbox_pdl.h"

stompbox_pdl::stompbox_pdl(QWidget *parent)
    : stompBox(parent)
{
    /* PDL */
    setImage(":/images/pdl.png");
    setLSB("06", "00");
    setComboBox("11", "00", "11");
    setButton("06", "00", "20");
    editDetails()->patchPos(0, 1, "06", "20");
    setEditPages();
}

void stompbox_pdl::updateSignal()
{
    updateComboBox("11", "00", "11");
    updateButton("06", "00", "20");
    updateSwitch("06", "00", "20");
}

void stompbox_pdl::setEditPages()
{
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "06", "00", "20", "middle", Qt::AlignCenter); // pdl fx on/off
    editDetails()->page()->newStackControl(0);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "11", "00", "11", "large");  // pdl fx type
    editDetails()->page()->addStackControl();
    editDetails()->page()->insertStackField(0, 2, 0, 1, 1);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);


    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("WAH");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "06", "00", "26", "large");  // wah type
    editDetails()->page()->addKnob(0, 1, 1, 1, "06", "00", "27");               // pdl position
    editDetails()->page()->addKnob(0, 2, 1, 1, "06", "00", "28");               // pdl min
    editDetails()->page()->addKnob(0, 3, 1, 1, "06", "00", "29");               // pdl max
    editDetails()->page()->addKnob(0, 4, 1, 1, "06", "00", "2A");               // effect level
    editDetails()->page()->addKnob(0, 5, 1, 1, "06", "00", "2B");               // direct mix
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Pedal Bend");
    editDetails()->page()->addKnob(0, 1, 1, 1, "06", "00", "22");           // pitch
    editDetails()->page()->addKnob(0, 2, 1, 1, "06", "00", "23");           // pdl position
    editDetails()->page()->addKnob(0, 3, 1, 1, "06", "00", "24");           // effect level
    editDetails()->page()->addKnob(0, 4, 1, 1, "06", "00", "25");           // direct mix
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("EVH WAH95");
    editDetails()->page()->addKnob(0, 1, 1, 1, "11", "00", "12");           // pitch
    editDetails()->page()->addKnob(0, 2, 1, 1, "11", "00", "13");           // pdl position
    editDetails()->page()->addKnob(0, 3, 1, 1, "11", "00", "14");           // effect level
    editDetails()->page()->addKnob(0, 4, 1, 1, "11", "00", "15");           // direct mix
    editDetails()->page()->addKnob(0, 5, 1, 1, "11", "00", "16");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addStackField();

    editDetails()->addPage();
}
