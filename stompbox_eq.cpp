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

#include "stompbox_eq.h"

stompbox_eq::stompbox_eq(QWidget *parent)
    : stompBox(parent)
{
    /* EQ */
    setImage(":/images/eq.png");
    setLSB("01", "00");
    setSlider1("01", "00", "32");
    setSlider2("01", "00", "35");
    setSlider3("01", "00", "38");
    setSlider4("01", "00", "39");
    setSlider5("01", "00", "3B");
    setButton("01", "00", "30");
    editDetails()->patchPos(400, 24, "01", "30");
    setEditPages();
}

void stompbox_eq::updateSignal()
{
    updateSlider1("01", "00", "32");
    updateSlider2("01", "00", "35");
    updateSlider3("01", "00", "38");
    updateSlider4("01", "00", "39");
    updateSlider5("01", "00", "3B");
    updateButton("01", "00", "30");
    updateSwitch("01", "00", "30");
}

void stompbox_eq::setEditPages()
{
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "30", "middle", Qt::AlignCenter);
    editDetails()->page()->newStackControl(0);
    editDetails()->page()->addComboBox(1, 1, 1, 1, "11", "00", "04"); // type
    editDetails()->page()->addStackControl();
    editDetails()->page()->insertStackField(0, 1, 2, 3, 1);
    editDetails()->page()->addGroupBox(0, 0, 2, 4);

    editDetails()->page()->newStackField(0);
    editDetails()->page()->addParaEQ(0, 1, 2, 4, "01", "00", "31");
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);
    editDetails()->page()->addGraphicEQ(0, 1, 2, 4, "11", "00", "05", "geq_slider");
    editDetails()->page()->addStackField();

	editDetails()->addPage();

}
