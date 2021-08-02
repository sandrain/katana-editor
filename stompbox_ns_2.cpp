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

#include "stompbox_ns_2.h"

stompbox_ns_2::stompbox_ns_2(QWidget *parent)
    : stompBox(parent)
{
    /* NS_2 */
    setImage(":/images/ns_2.png");
    setLSB("06", "00");
   /* setKnob1("06", "00", "69");
    setKnob2("06", "00", "6A");
    setButton("06", "00", "68");
    editDetails()->patchPos(1922, 8, "06", "68");*/
    setEditPages();
}

void stompbox_ns_2::updateSignal()
{
  /*  updateKnob1("06", "00", "69");
    updateKnob2("06", "00", "6A");
    updateButton("06", "00", "68");
    updateSwitch("06", "00", "68");*/
}

void stompbox_ns_2::setEditPages()
{
    /*editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "06", "00", "68", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "06", "00", "6B", "large");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Noise Suppressor");
    editDetails()->page()->addKnob(0, 0, 1, 1, "06", "00", "69");
    editDetails()->page()->addKnob(0, 1, 1, 1, "06", "00", "6A");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);*/
    editDetails()->addPage();
}
