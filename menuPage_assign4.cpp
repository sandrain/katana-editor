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

#include "menuPage_assign4.h"

menuPage_assign4::menuPage_assign4(QWidget *parent)
    : menuPage(parent)
{
    setLSB("09", "10");
    editDetails()->patchPos(2592, 32, "09", "10");    //192 sysx file offset and data length of chorus parameters x2,
    setEditPages();
}

void menuPage_assign4::updateSignal()
{
   updateSwitch("09", "00", "10", "Structure");
}

void menuPage_assign4::setEditPages()
{
    editDetails()->page()->newGroupBox(tr("Patch Assign 4"));
    editDetails()->page()->newGroupBox(tr(""));
    editDetails()->page()->addSwitch(0, 0, 1, 1, "09", "00", "10", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Assign 4 Target");
    editDetails()->page()->addTarget(0, 0, 1, 3, "09", "00", "11", "target","target", 120); // Target
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->newGroupBox("Assign 4 Source");
    editDetails()->page()->addComboBox(1, 0, 1, 1, "09", "00", "17");                    // Source
    editDetails()->page()->addComboBox(1, 1, 1, 1, "09", "00", "18");                    // Source Mode
    editDetails()->page()->addRange(1, 2, 1, 2, "09", "00", "19", "Structure");         // min/max range widget
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addGroupBox(0, 0, 2, 1);

    editDetails()->addPage();
}
