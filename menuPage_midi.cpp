/****************************************************************************
**
** Copyright (C) 2007~2020 Colin Willcocks. 
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

#include "menuPage_midi.h"

menuPage_midi::menuPage_midi(QWidget *parent)
    : menuPage(parent)
{
  //setLSB("00", "00");
	setEditPages();
}

void menuPage_midi::updateSignal()
{

}

void menuPage_midi::setEditPages()
{
    //editDetails()->page()->newGroupBox(tr("Effects Option Colour"));
   /* editDetails()->page()->addGRYButton(0, 0, 1, 1, "12", "00", "10");        //Booster GRY
    editDetails()->page()->addGRYButton(0, 1, 1, 1, "12", "00", "11");        //MOD GRY
    editDetails()->page()->addGRYButton(0, 2, 1, 1, "12", "00", "12");        //Delay1 GRY
    editDetails()->page()->addGRYButton(0, 3, 1, 1, "12", "00", "13");        //FX GRY
    editDetails()->page()->addGRYButton(0, 4, 1, 1, "12", "00", "14");        //REV/DD2 GRY*/
    //editDetails()->page()->addGroupBox(0, 0, 1, 2);

    //editDetails()->page()->newGroupBox("");
    editDetails()->page()->newGroupBox("Booster");
    editDetails()->page()->addGRYButton(0, 0, 1, 1, "12", "00", "10");        //Booster GRY
    editDetails()->page()->addComboBox(1, 0, 1, 1, "12", "00", "01", "large");
    editDetails()->page()->addComboBox(2, 0, 1, 1, "12", "00", "02", "large");
    editDetails()->page()->addComboBox(3, 0, 1, 1, "12", "00", "03", "large");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("MOD");
    editDetails()->page()->addGRYButton(0, 0, 1, 1, "12", "00", "11");        //MOD GRY
    editDetails()->page()->addComboBox(1, 0, 1, 1, "12", "00", "04", "large");
    editDetails()->page()->addComboBox(2, 0, 1, 1, "12", "00", "05", "large");
    editDetails()->page()->addComboBox(3, 0, 1, 1, "12", "00", "06", "large");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
   // editDetails()->page()->addGroupBox(1, 0, 2, 1);

   // editDetails()->page()->newGroupBox("");
    editDetails()->page()->newGroupBox("Delay 1");
    editDetails()->page()->addGRYButton(0, 0, 1, 1, "12", "00", "12");        //Delay1 GRY
    editDetails()->page()->addComboBox(1, 0, 1, 1, "12", "00", "07", "large");
    editDetails()->page()->addComboBox(2, 0, 1, 1, "12", "00", "08", "large");
    editDetails()->page()->addComboBox(3, 0, 1, 1, "12", "00", "09", "large");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);

    editDetails()->page()->newGroupBox("FX");
    editDetails()->page()->addGRYButton(0, 0, 1, 1, "12", "00", "13");        //FX GRY
    editDetails()->page()->addComboBox(1, 0, 1, 1, "12", "00", "0A", "large");
    editDetails()->page()->addComboBox(2, 0, 1, 1, "12", "00", "0B", "large");
    editDetails()->page()->addComboBox(3, 0, 1, 1, "12", "00", "0C", "large");
    editDetails()->page()->addGroupBox(0, 3, 1, 1);
    //editDetails()->page()->addGroupBox(1, 0, 8, 1);

    //editDetails()->page()->newGroupBox("");
    editDetails()->page()->newGroupBox("Reverb/Delay 2 Mode");
    editDetails()->page()->addGRYButton(0, 0, 1, 1, "12", "00", "14");        //REV/DD2 GRY
    editDetails()->page()->addComboBox(1, 0, 1, 1, "12", "00", "1C", "large");
    editDetails()->page()->addComboBox(2, 0, 1, 1, "12", "00", "1D", "large");
    editDetails()->page()->addComboBox(3, 0, 1, 1, "12", "00", "1E", "large");
    editDetails()->page()->addGroupBox(0, 4, 1, 1);

    editDetails()->page()->newGroupBox("Reverb");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "12", "00", "0D", "large");
    editDetails()->page()->addComboBox(0, 1, 1, 1, "12", "00", "0E", "large");
    editDetails()->page()->addComboBox(0, 2, 1, 1, "12", "00", "0F", "large");
    editDetails()->page()->addGroupBox(1, 0, 1, 2);

    editDetails()->page()->newGroupBox("Delay 2");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "12", "00", "07", "large");
    editDetails()->page()->addComboBox(0, 1, 1, 1, "12", "00", "08", "large");
    editDetails()->page()->addComboBox(0, 2, 1, 1, "12", "00", "09", "large");
    editDetails()->page()->addGroupBox(1, 2, 1, 3);
    //editDetails()->page()->addGroupBox(1, 4, 2, 1);



   /* editDetails()->page()->newGroupBox("Use of the Panel Knobs will limit the availble effects options");
    editDetails()->page()->newGroupBox("AMP PANEL KNOBS.  Caution: Changes are automatically saved to the KATANA.");
    editDetails()->page()->addKnob(0, 0, 1, 1, "00", "04", "20", "System","bottom", 120); // amp type
    editDetails()->page()->addKnob(0, 1, 1, 1, "00", "04", "21", "System"); // gain
    editDetails()->page()->addKnob(0, 2, 1, 1, "00", "04", "22", "System"); // volume
    editDetails()->page()->addKnob(0, 3, 1, 1, "00", "04", "23", "System"); // bass
    editDetails()->page()->addKnob(0, 4, 1, 1, "00", "04", "24", "System"); // mid
    editDetails()->page()->addKnob(0, 5, 1, 1, "00", "04", "25", "System"); // treble
    editDetails()->page()->addKnob(0, 6, 1, 1, "00", "04", "26", "System"); // presence
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("EFFECTS PANEL KNOBS.  Caution: Changes are automatically saved to the KATANA.");
    editDetails()->page()->addKnob(0, 0, 1, 1, "00", "04", "27", "System","bottom", 120); // booster/mod
    editDetails()->page()->addKnob(0, 1, 1, 1, "00", "04", "28", "System","bottom", 120); // delay/fx
    editDetails()->page()->addKnob(0, 2, 1, 1, "00", "04", "29", "System","bottom", 120); // reverb
    editDetails()->page()->addGroupBox(1, 0, 1, 1);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("AMP PANEL KNOBS.  Caution: Changes are automatically saved to the KATANA.");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "00", "04", "10", "System"); // led
    editDetails()->page()->addComboBox(0, 1, 1, 1, "00", "04", "11", "System"); // led
    editDetails()->page()->addComboBox(0, 2, 1, 1, "00", "04", "12", "System"); // led
    editDetails()->page()->addComboBox(0, 3, 1, 1, "00", "04", "13", "System"); // led
    editDetails()->page()->addComboBox(0, 4, 1, 1, "00", "04", "14", "System"); // led
    editDetails()->page()->addComboBox(0, 5, 1, 1, "00", "04", "15", "System"); // led
    editDetails()->page()->addGroupBox(1, 0, 1, 1);     */


    editDetails()->addPage();
}
