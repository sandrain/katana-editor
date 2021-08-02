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

#include "menuPage_assign9.h"

menuPage_assign9::menuPage_assign9(QWidget *parent)
    : menuPage(parent)
{
    //setLSB("08", "30");
    //editDetails()->patchPos(2374, 32, "08", "30");
    setEditPages();
}

void menuPage_assign9::updateSignal()
{
   updateSwitch("03", "00", "00", "System");
}

void menuPage_assign9::setEditPages()
{
    editDetails()->page()->newGroupBox(tr("Global Assign 9"));
    editDetails()->page()->newGroupBox(tr(""));
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "00", "System", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->newGroupBox("Assign 9 Target");
    editDetails()->page()->addTarget(0, 0, 1, 3, "03", "00", "01", "target", "System", 120); // Target
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->newGroupBox("Assign 9 Source");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "03", "00", "07", "System");            // Source
    editDetails()->page()->addComboBox(0, 1, 1, 1, "03", "00", "08", "System");            // Source Mode
    editDetails()->page()->addRange(0, 2, 1, 2, "03", "00", "09", "System");        // min/max range widget
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addGroupBox(0, 0, 2, 1);

    editDetails()->addPage();
}
