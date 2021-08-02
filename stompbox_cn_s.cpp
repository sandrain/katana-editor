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

#include "stompbox_cn_s.h"

stompbox_cn_s::stompbox_cn_s(QWidget *parent)
    : stompBox(parent)
{
    /* CHAIN SPLIT */
    this->setImage(":/images/cn_s.png");
    setLSB("06", "40");
   // setComboBox("06", "00", "40", QRect(8, 78, 79, 13));
  //  editDetails()->patchPos(1842, 20, "06", "40");
    setEditPages();
}

void stompbox_cn_s::updateSignal()
{
   // updateComboBox("06", "00", "40");
   // updateSwitch("06", "00", "40");
   // updateSwitch("06", "00", "41");
}

void stompbox_cn_s::setEditPages()
{
 /*   editDetails()->page()->newGroupBox(tr("Channel Divide"));
    editDetails()->page()->newStackControl(0);
    editDetails()->page()->addComboBox(2, 0, 1, 1, "06", "00", "40", "large");   //mode
    editDetails()->page()->addStackControl();
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->insertStackField(0, 2, 0, 1, 1);

    editDetails()->page()->newStackField(0, Qt::AlignCenter);
    editDetails()->page()->newGroupBox(tr("Channel Select"));
    editDetails()->page()->addComboBox(0, 0, 1, 1, "06", "00", "41", "large"); // channel select
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0, Qt::AlignCenter);
    editDetails()->page()->newGroupBox(tr("Channel A"));
    editDetails()->page()->newStackControl(1);
    editDetails()->page()->addComboBox(0, 0, 1, 1, "06", "00", "42"); //ch-a dynamic
    editDetails()->page()->addStackControl();
    editDetails()->page()->insertStackField(1, 1, 0, 1, 1);
    editDetails()->page()->newStackControl(2);
    editDetails()->page()->addComboBox(2, 0, 1, 1, "06", "00", "44"); //ch-a filter
    editDetails()->page()->addStackControl();
    editDetails()->page()->insertStackField(2, 3, 0, 1, 1);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox(tr("Channel B"));
    editDetails()->page()->newStackControl(3);
    editDetails()->page()->addComboBox(0, 0, 1, 1, "06", "00", "46"); //ch-b dynamic
    editDetails()->page()->addStackControl();
    editDetails()->page()->insertStackField(3, 1, 0, 1, 1);
    editDetails()->page()->newStackControl(4);
    editDetails()->page()->addComboBox(2, 0, 1, 1, "06", "00", "48"); //ch-b filter
    editDetails()->page()->addStackControl();
    editDetails()->page()->insertStackField(4, 3, 0, 1, 1);
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addKnob(0, 0, 1, 1, "06", "00", "43");   //ch-a sense
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addKnob(0, 0, 1, 1, "06", "00", "43");   //ch-a sense
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(2);
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);
    editDetails()->page()->addKnob(0, 0, 1, 1, "06", "00", "45");   //cutoff freq
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(2);
    editDetails()->page()->addKnob(0, 0, 1, 1, "06", "00", "45");   //cutoff freq
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(3);
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);
    editDetails()->page()->addKnob(0, 0, 1, 1, "06", "00", "47");   //ch-b sense
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(3);
    editDetails()->page()->addKnob(0, 0, 1, 1, "06", "00", "47");   //ch-b sense
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(4);
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(4);
    editDetails()->page()->addKnob(0, 0, 1, 1, "06", "00", "49");   //cutoff freq
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(4);
    editDetails()->page()->addKnob(0, 0, 1, 1, "06", "00", "49");   //cutoff freq
    editDetails()->page()->addStackField(); */

    editDetails()->addPage();
}
