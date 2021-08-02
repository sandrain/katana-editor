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

#include "stompbox_cs.h"

stompbox_cs::stompbox_cs(QWidget *parent)
    : stompBox(parent)
{
	/* COMP */
	setImage(":/images/cs.png");
	setLSB("00", "00");
   // setKnob1("00", "00", "22");
    //setKnob2("00", "00", "25");
    //setComboBox("00", "00", "21");
    //setButton("00", "00", "20");
    //editDetails()->patchPos(86, 12, "00", "20");
    setEditPages();
}

void stompbox_cs::updateSignal()
{
    //updateKnob1("00", "00", "22");
    //updateKnob2("00", "00", "25");
    //updateComboBox("00", "00", "21");
    //updateButton("00", "00", "20");
    //updateSwitch("00", "00", "20");
}

void stompbox_cs::setEditPages()
{
   /* editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "00", "00", "20", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "00", "00", "21", "large");
	editDetails()->page()->addGroupBox(0, 0, 1, 1);

	editDetails()->page()->newGroupBox("Compressor");
    editDetails()->page()->addKnob(0, 0, 1, 1, "00", "00", "22");
    editDetails()->page()->addKnob(0, 1, 1, 1, "00", "00", "23");
    editDetails()->page()->addKnob(0, 2, 1, 1, "00", "00", "24");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

	editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addKnob(0, 0, 1, 1, "00", "00", "25");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
*/
	editDetails()->addPage();
}
