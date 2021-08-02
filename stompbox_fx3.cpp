/****************************************************************************
**
** Copyright (C) 2007~2015 Colin Willcocks.
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

#include "stompbox_fx3.h"

stompbox_fx3::stompbox_fx3(QWidget *parent)
    : stompBox(parent)
{
    /* fx3 */
    setImage(":/images/fx3.png");
    setLSB("06", "00");
   // setComboBox("06", "00", "70", QRect(8, 31, 79, 13));
   // editDetails()->patchPos(1938, 82, "06", "70");
    setEditPages();
}

void stompbox_fx3::updateSignal()
{
  //  updateComboBox("06", "00", "70");
}

void stompbox_fx3::setEditPages()
{
  /*  editDetails()->page()->newGroupBox("Select");
    editDetails()->page()->newStackControl(0);
    editDetails()->page()->addComboBox(0, 0, 1, 1, "06", "00", "70", "large");
    editDetails()->page()->addStackControl();
    editDetails()->page()->addGroupBox(0, 0, 2, 1);
    editDetails()->page()->insertStackField(0, 0, 1, 1, 1);

    //S-Bend
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("S-Bend");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "06", "00", "71", "bottom", Qt::AlignRight); // sbend pitch
    editDetails()->page()->addKnob(0, 1, 1, 1, "06", "00", "72"); // rise time
    editDetails()->page()->addKnob(0, 2, 1, 1, "06", "00", "73"); // fall time
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addStackField();

    //laser beam
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Laser Beam");
    editDetails()->page()->addKnob(0, 0, 1, 1, "06", "00", "74");
    editDetails()->page()->addKnob(0, 1, 1, 1, "06", "00", "75");
    editDetails()->page()->addKnob(0, 2, 1, 1, "06", "00", "76");
    editDetails()->page()->addKnob(0, 3, 1, 1, "06", "00", "77");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->addStackField();

    //ring mod
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Ring Mod");
    editDetails()->page()->addKnob(0, 0, 1, 1, "06", "00", "78");
    editDetails()->page()->addKnob(0, 1, 1, 1, "06", "00", "79");
    editDetails()->page()->addKnob(0, 2, 1, 1, "06", "00", "7A");
    editDetails()->page()->addKnob(0, 3, 1, 1, "06", "00", "7B");
    editDetails()->page()->addKnob(0, 4, 1, 1, "06", "00", "7C");
    editDetails()->page()->addKnob(0, 5, 1, 1, "06", "00", "7D");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->addStackField();

    //TWIST
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Twist");
    editDetails()->page()->addKnob(0, 0, 1, 1, "06", "00", "7E");
    editDetails()->page()->addKnob(0, 1, 1, 1, "06", "00", "7F");
    editDetails()->page()->addKnob(0, 2, 1, 1, "07", "00", "00");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->addStackField();

    //WARP
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Warp");
    editDetails()->page()->addKnob(0, 0, 1, 1, "07", "00", "01");
    editDetails()->page()->addKnob(0, 1, 1, 1, "07", "00", "02");
    editDetails()->page()->addKnob(0, 2, 1, 1, "07", "00", "03");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->addStackField();

    //Feedbacker
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Feedbacker");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "07", "00", "04", "bottom", Qt::AlignRight);
    editDetails()->page()->addKnob(0, 1, 1, 1, "07", "00", "05");
    editDetails()->page()->addKnob(0, 2, 1, 1, "07", "00", "06");
    editDetails()->page()->addKnob(0, 3, 1, 1, "07", "00", "07");
    editDetails()->page()->addKnob(0, 4, 1, 1, "07", "00", "08");
    editDetails()->page()->addKnob(0, 5, 1, 1, "07", "00", "09");
    editDetails()->page()->addKnob(0, 6, 1, 1, "07", "00", "0A");
    editDetails()->page()->addKnob(0, 7, 1, 1, "07", "00", "0B");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->addStackField();  */

    editDetails()->addPage();
}
