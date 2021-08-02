/****************************************************************************
**
** Copyright (C) 2007~2019 Colin Willcocks.
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

#include "stompbox_fv.h"

stompbox_fv::stompbox_fv(QWidget *parent)
    : stompBox(parent)
{
    /* FV */
    setImage(":/images/fv.png");
    setLSB("06", "00");
    editDetails()->patchPos(0, 1, "06", "20");
    setEditPages();
}

void stompbox_fv::updateSignal()
{

}

void stompbox_fv::setEditPages()
{    
    editDetails()->page()->newGroupBox("Foot Volume");
    editDetails()->page()->addKnob(0, 1, 1, 1, "06", "00", "33");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Master");
    editDetails()->page()->addKnob(0, 0, 1, 1, "07", "00", "10");
    editDetails()->page()->addComboBox(1, 0, 1, 1, "07", "00", "18", "large");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);

    editDetails()->addPage();
}
