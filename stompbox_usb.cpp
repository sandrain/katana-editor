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

#include "stompbox_usb.h"

stompbox_usb::stompbox_usb(QWidget *parent)
    : stompBox(parent)
{
    /* USB */
    this->setImage(":/images/usb.png");
    setLSB("void", "void");
    setEditPages();
}

void stompbox_usb::updateSignal()
{

}

void stompbox_usb::setEditPages()
{


    editDetails()->page()->newGroupBox(tr("USB-System - Note* Changes are automatically written to system settings"));
    editDetails()->page()->addComboBox(0, 0, 1, 1, "00", "00", "50", "System"); //usb I/O mode
    editDetails()->page()->addLabel(0, 1, 1, 1, "     ");
    editDetails()->page()->addComboBox(0, 2, 1, 1, "00", "00", "55", "System"); //usb monitor cmd
    editDetails()->page()->addKnob(0, 3, 1, 1, "00", "00", "51", "System"); //usb input level
    editDetails()->page()->addKnob(0, 4, 1, 1, "00", "00", "52", "System"); //usb mix level
    editDetails()->page()->addKnob(0, 5, 1, 1, "00", "00", "53", "System"); //usb mix level
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->addPage();
}
