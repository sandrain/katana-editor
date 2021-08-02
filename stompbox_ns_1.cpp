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

#include "stompbox_ns_1.h"

stompbox_ns_1::stompbox_ns_1(QWidget *parent)
    : stompBox(parent)
{
    /* NS_1 */
    setImage(":/images/ns_1.png");
    setLSB("06", "00");
    setKnob1("06", "00", "64");
    setKnob2("06", "00", "65");
    setButton("06", "00", "63");
    editDetails()->patchPos(1912, 8, "06", "63");
    setEditPages();
}

void stompbox_ns_1::updateSignal()
{
    updateKnob1("06", "00", "64");
    updateKnob2("06", "00", "65");
    updateButton("06", "00", "63");
    updateSwitch("06", "00", "63");
}

void stompbox_ns_1::setEditPages()
{
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "06", "00", "63", "middle", Qt::AlignCenter);
    //editDetails()->page()->addComboBox(1, 0, 1, 1, "06", "00", "66", "large");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Noise Suppressor");
    editDetails()->page()->addKnob(0, 0, 1, 1, "06", "00", "64");
    editDetails()->page()->addKnob(0, 1, 1, 1, "06", "00", "65");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->addPage();
}
