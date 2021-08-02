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

#include "menuPage_assign12.h"

menuPage_assign12::menuPage_assign12(QWidget *parent)
    : menuPage(parent)
{
    //setLSB("09", "10");
    //editDetails()->patchPos(2592, 32, "09", "10");    //192 sysx file offset and data length of chorus parameters x2,
    setEditPages();
}

void menuPage_assign12::updateSignal()
{
   updateSwitch("03", "00", "2D", "System");
}

void menuPage_assign12::setEditPages()
{
    editDetails()->page()->newGroupBox(tr("Global Assign 12"));
    editDetails()->page()->newGroupBox(tr(""));
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "2D", "System", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Assign 12 Target");
    editDetails()->page()->addTarget(0, 0, 1, 3, "03", "00", "2E", "target", "System", 120); // Target
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->newGroupBox("Assign 12 Source");
    editDetails()->page()->addComboBox(1, 0, 1, 1, "03", "00", "34", "System");                    // Source
    editDetails()->page()->addComboBox(1, 1, 1, 1, "03", "00", "35", "System");                    // Source Mode
    editDetails()->page()->addRange(1, 2, 1, 2, "03", "00", "36", "System");         // min/max range widget
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addGroupBox(0, 0, 2, 1);

    editDetails()->addPage();
}
