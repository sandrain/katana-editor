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

#include "stompbox_fx1.h"

stompbox_fx1::stompbox_fx1(QWidget *parent)
    : stompBox(parent)
{
    /* FX1 */
    setImage(":/images/fx1.png");
    setLSB("01", "00");
    setComboBox("01", "00", "41", QRect(8, 35, 79, 13));
    setButton("01", "00", "40");
    editDetails()->patchPos(432, 588, "01", "40");
    setEditPages();
}

void stompbox_fx1::updateSignal()
{
    updateComboBox("01", "00", "41");
    updateButton("01", "00", "40");
    updateSwitch("01", "00", "40");
}

void stompbox_fx1::setEditPages()
{
    editDetails()->page()->newGroupBox("Select");
    editDetails()->page()->newStackControl(0);
    editDetails()->page()->addComboBox(0, 0, 1, 1, "01", "00", "41", "large");
    editDetails()->page()->addStackControl();
    editDetails()->page()->addGroupBox(0, 0, 2, 1);
    editDetails()->page()->insertStackField(0, 0, 1, 3, 1);

    editDetails()->page()->newGroupBox("Option Select");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "12", "00", "04", "large");
    editDetails()->page()->addComboBox(1, 0, 1, 1, "12", "00", "05", "large");
    editDetails()->page()->addComboBox(2, 0, 1, 1, "12", "00", "06", "large");
    editDetails()->page()->addGroupBox(2, 0, 1, 1);



    // T-Wah
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter); //on/off
    editDetails()->page()->addComboBox(1, 0, 1, 1, "01", "00", "4C", "bottom", Qt::AlignRight); // mode
    editDetails()->page()->addComboBox(2, 0, 1, 2, "01", "00", "4D");  //polarity
    editDetails()->page()->addGroupBox(1, 0, 1, 1);

    editDetails()->page()->newGroupBox("Touch Wah");
    editDetails()->page()->addKnob(0, 1, 1, 1, "01", "00", "4E"); //sense
    editDetails()->page()->addKnob(0, 2, 1, 1, "01", "00", "4F");  //freq
    editDetails()->page()->addKnob(0, 3, 1, 1, "01", "00", "50");  //peak
    editDetails()->page()->addGroupBox(1, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "01", "00", "52"); //direct mix
    editDetails()->page()->addKnob(0, 1, 1, 1, "01", "00", "51"); //effect level
    editDetails()->page()->addGroupBox(1, 2, 1, 1);
    editDetails()->page()->addStackField();

    // Auto Wah
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "01", "00", "54");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Auto Wah");
    editDetails()->page()->addKnob(0, 3, 1, 1, "01", "00", "55");
    editDetails()->page()->addKnob(0, 1, 1, 1, "01", "00", "56");
    editDetails()->page()->addKnob(0, 2, 1, 1, "01", "00", "57", "normal","bottom", 120);
    editDetails()->page()->addKnob(0, 4, 1, 1, "01", "00", "58");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "01", "00", "59");
    editDetails()->page()->addKnob(0, 1, 1, 1, "01", "00", "5A");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // Sub-Wah
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "01", "00", "5C", "bottom", Qt::AlignRight);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Sub Wah");
    editDetails()->page()->addKnob(0, 1, 1, 1, "01", "00", "5D" );
    editDetails()->page()->addKnob(0, 2, 1, 1, "01", "00", "5E");
    editDetails()->page()->addKnob(0, 3, 1, 1, "01", "00", "5F");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "01", "00", "60");
    editDetails()->page()->addKnob(0, 1, 1, 1, "01", "00", "61");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // Advanced Compressor
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "01", "00", "63");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Advanced Compressor");
    editDetails()->page()->addKnob(0, 0, 1, 1, "01", "00", "64");
    editDetails()->page()->addKnob(0, 1, 1, 1, "01", "00", "65");
    editDetails()->page()->addKnob(0, 2, 1, 1, "01", "00", "66");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "01", "00", "67");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // Limiter
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "01", "00", "69");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Limitter");
    editDetails()->page()->addKnob(0, 0, 1, 1, "01", "00", "6A");
    editDetails()->page()->addKnob(0, 1, 1, 1, "01", "00", "6B");
    editDetails()->page()->addKnob(0, 2, 1, 1, "01", "00", "6C");
    editDetails()->page()->addKnob(0, 3, 1, 1, "01", "00", "6D");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "01", "00", "6E");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    //DISTORTION
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "01", "00", "42");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Drive");
    editDetails()->page()->addKnob(0, 0, 1, 1, "01", "00", "43", "turbo");
    editDetails()->page()->addKnob(0, 1, 1, 1, "01", "00", "44");
    editDetails()->page()->addKnob(0, 2, 1, 1, "01", "00", "45");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "01", "00", "48");
    editDetails()->page()->addKnob(0, 1, 1, 1, "01", "00", "49");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);

    editDetails()->page()->newGroupBox("Solo");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "46", "middle", Qt::AlignCenter);
    editDetails()->page()->addKnob(0, 1, 1, 1, "01", "00", "47");
    editDetails()->page()->addGroupBox(0, 3, 1, 1);
    editDetails()->page()->addStackField();

    // Graphic EQ
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addGraphicEQ(0, 1, 2, 4, "01", "00", "70", "geq_slider");
    editDetails()->page()->addGroupBox(0, 0, 2, 4);
    editDetails()->page()->addStackField();

    // Parametric EQ
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addParaEQ(0, 1, 2, 4, "01", "00", "7C");
    editDetails()->page()->addGroupBox(0, 0, 2, 4);
    editDetails()->page()->addStackField();

    // Tone Modify
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "02", "00", "08");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Tone Modify");
    editDetails()->page()->addKnob(0, 3, 1, 1, "02", "00", "09");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "0A");
    editDetails()->page()->addKnob(0, 2, 1, 1, "02", "00", "0B");;
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "0C");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // Guitar Sim
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "02", "00", "0E");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Guitar Simulator");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "0F");
    editDetails()->page()->addKnob(0, 2, 1, 1, "02", "00", "10");
    editDetails()->page()->addKnob(0, 3, 1, 1, "02", "00", "12");;
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "11");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();


    // Slow Gear
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Slow Gear");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "14");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "15");
    editDetails()->page()->addKnob(0, 2, 1, 1, "02", "00", "16");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->addStackField();


    // DeFretter
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Defretter");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "18");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "19");
    editDetails()->page()->addKnob(0, 2, 1, 1, "02", "00", "1A");
    editDetails()->page()->addKnob(0, 3, 1, 1, "02", "00", "1B");
    editDetails()->page()->addKnob(0, 4, 1, 1, "02", "00", "1C");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "1D");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "1E");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();


    // Wave Synth
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "02", "00", "20");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Wave Synth");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "21");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "22");
    editDetails()->page()->addKnob(0, 2, 1, 1, "02", "00", "23");
    editDetails()->page()->addKnob(0, 3, 1, 1, "02", "00", "24");
    editDetails()->page()->addKnob(0, 4, 1, 1, "02", "00", "25");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "26");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "27");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();


    // Sitar Sim
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Sitar");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "29");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "2A");
    editDetails()->page()->addKnob(0, 2, 1, 1, "02", "00", "2B");
    editDetails()->page()->addKnob(0, 3, 1, 1, "02", "00", "2C");
    editDetails()->page()->addKnob(0, 4, 1, 1, "02", "00", "2D");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "2E");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "2F");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // Octaver
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Octave", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addComboBox(0, 0, 1, 3, "02", "00", "31");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "32");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "33");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();


    // Pitch Shifter
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->newStackControl(1);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "02", "00", "35");
    editDetails()->page()->addStackControl();
    editDetails()->page()->addGroupBox(0, 0, 3, 1);

    editDetails()->page()->insertStackField(1, 0, 1, 3, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->newGroupBox("Pitch Shifter");
    editDetails()->page()->newGroupBox("Voice 1");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "02", "00", "36");
    editDetails()->page()->addKnob(1, 0, 1, 1, "02", "00", "39", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 2, 1, "02", "00", "37");
    editDetails()->page()->addKnob(0, 2, 2, 1, "02", "00", "38");
    editDetails()->page()->addGroupBox(0, 0, 3, 1);
    editDetails()->page()->addKnob(1, 1, 1, 1, "02", "00", "42");
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "3B");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "43");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->newGroupBox("Pitch Shifter");
    editDetails()->page()->newGroupBox("Voice 1");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "02", "00", "36");
    editDetails()->page()->addKnob(1, 0, 1, 1, "02", "00", "39", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 2, 1, "02", "00", "37");
    editDetails()->page()->addKnob(0, 2, 2, 1, "02", "00", "38");
    editDetails()->page()->addGroupBox(0, 0, 3, 1);
    editDetails()->page()->addKnob(1, 1, 1, 1, "02", "00", "42");

    editDetails()->page()->newGroupBox("Voice 2");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "02", "00", "3C");
    editDetails()->page()->addKnob(1, 0, 1, 1, "02", "00", "3F", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 2, 1, "02", "00", "3D");
    editDetails()->page()->addKnob(0, 2, 2, 1, "02", "00", "3E");
    editDetails()->page()->addGroupBox(4, 0, 1, 1);
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "3B");
    editDetails()->page()->addKnob(2, 0, 1, 1, "02", "00", "41");
    editDetails()->page()->addKnob(1, 1, 1, 1, "02", "00", "43");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->newGroupBox("Pitch Shifter");
    editDetails()->page()->newGroupBox("Voice 1");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "02", "00", "36");
    editDetails()->page()->addKnob(1, 0, 1, 1, "02", "00", "39", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 2, 1, "02", "00", "37");
    editDetails()->page()->addKnob(0, 2, 2, 1, "02", "00", "38");
    editDetails()->page()->addGroupBox(0, 0, 3, 1);
    editDetails()->page()->addKnob(1, 1, 1, 1, "02", "00", "42");

    editDetails()->page()->newGroupBox("Voice 2");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "02", "00", "3C");
    editDetails()->page()->addKnob(1, 0, 1, 1, "02", "00", "3F", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 2, 1, "02", "00", "3D");
    editDetails()->page()->addKnob(0, 2, 2, 1, "02", "00", "3E");
    editDetails()->page()->addGroupBox(4, 0, 1, 1);
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "3B");
    editDetails()->page()->addKnob(2, 0, 1, 1, "02", "00", "41");
    editDetails()->page()->addKnob(1, 1, 1, 1, "02", "00", "43");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
    editDetails()->page()->addStackField();

    // Harmonizer
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->newStackControl(1);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "02", "00", "45");
    editDetails()->page()->addStackControl();
    editDetails()->page()->addComboBox(2, 0, 1, 1, "07", "00", "18");
    editDetails()->page()->addGroupBox(0, 0, 2, 1);
    editDetails()->page()->insertStackField(1, 0, 1, 3, 1);
    editDetails()->page()->newGroupBox("User Harmony 1");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "02", "00", "50");
    editDetails()->page()->addComboBox(0, 1, 1, 1, "02", "00", "51");
    editDetails()->page()->addComboBox(0, 2, 1, 1, "02", "00", "52");
    editDetails()->page()->addComboBox(0, 3, 1, 1, "02", "00", "53");
    editDetails()->page()->addComboBox(0, 4, 1, 1, "02", "00", "54");
    editDetails()->page()->addComboBox(0, 5, 1, 1, "02", "00", "55");
    editDetails()->page()->addComboBox(0, 6, 1, 1, "02", "00", "56");
    editDetails()->page()->addComboBox(0, 7, 1, 1, "02", "00", "57");
    editDetails()->page()->addComboBox(0, 8, 1, 1, "02", "00", "58");
    editDetails()->page()->addComboBox(0, 9, 1, 1, "02", "00", "59");
    editDetails()->page()->addComboBox(0, 10, 1, 1, "02", "00", "5A");
    editDetails()->page()->addComboBox(0, 11, 1, 1, "02", "00", "5B");
    editDetails()->page()->addGroupBox(3, 0, 1, 3);
    editDetails()->page()->newGroupBox("User Harmony 2");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "02", "00", "5C");
    editDetails()->page()->addComboBox(0, 1, 1, 1, "02", "00", "5D");
    editDetails()->page()->addComboBox(0, 2, 1, 1, "02", "00", "5E");
    editDetails()->page()->addComboBox(0, 3, 1, 1, "02", "00", "5F");
    editDetails()->page()->addComboBox(0, 4, 1, 1, "02", "00", "60");
    editDetails()->page()->addComboBox(0, 5, 1, 1, "02", "00", "60");
    editDetails()->page()->addComboBox(0, 6, 1, 1, "02", "00", "62");
    editDetails()->page()->addComboBox(0, 7, 1, 1, "02", "00", "63");
    editDetails()->page()->addComboBox(0, 8, 1, 1, "02", "00", "64");
    editDetails()->page()->addComboBox(0, 9, 1, 1, "02", "00", "65");
    editDetails()->page()->addComboBox(0, 10, 1, 1, "02", "00", "66");
    editDetails()->page()->addComboBox(0, 11, 1, 1, "02", "00", "67");
    editDetails()->page()->addGroupBox(4, 0, 1, 3);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->newGroupBox("Harmonist");
    editDetails()->page()->newGroupBox("Voice 1");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "02", "00", "46");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "47", "normal","right", 120);
    editDetails()->page()->addGroupBox(0, 0, 2, 1);
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "4E", "normal","right", 50);
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "49");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "4F");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->newGroupBox("Harmonist");
    editDetails()->page()->newGroupBox("Voice 1");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "02", "00", "46");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "47", "normal","right", 120);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->newGroupBox("Voice 2");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "02", "00", "4A");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "4B", "normal","right", 120);
    editDetails()->page()->addGroupBox(2, 0, 1, 1);
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "4E", "normal","right", 50);
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "49");
    editDetails()->page()->addKnob(1, 0, 1, 1, "02", "00", "4D");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "4F");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->newGroupBox("Harmonist");
    editDetails()->page()->newGroupBox("Voice 1");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "02", "00", "46");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "47", "normal","right", 120);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->newGroupBox("Voice 2");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "02", "00", "4A");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "4B", "normal","right", 120);
    editDetails()->page()->addGroupBox(2, 0, 1, 1);
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "4E", "normal","right", 50);
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "49");
    editDetails()->page()->addKnob(1, 0, 1, 1, "02", "00", "4D");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "4F");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
    editDetails()->page()->addStackField();


    // Sound Hold
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Sound Hold");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "02", "00", "69", "middle", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addKnob(0, 2, 1, 1, "02", "00", "6A");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "6B");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();


    // Acoustic Processor
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "02", "00", "6D");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Acoustic Processor");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "6E");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "6F");
    editDetails()->page()->addKnob(0, 2, 1, 1, "02", "00", "70", "normal","bottom", 80);
    editDetails()->page()->addKnob(0, 3, 1, 1, "02", "00", "71");
    editDetails()->page()->addKnob(0, 4, 1, 1, "02", "00", "72");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "73");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();


    // Phaser
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "02", "00", "75");
    editDetails()->page()->addGroupBox(0, 0, 2, 1);

    editDetails()->page()->newGroupBox("Phaser");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "76", "normal","right", 120);
    editDetails()->page()->addKnob(1, 0, 1, 1, "02", "00", "7A", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "78");
    editDetails()->page()->addKnob(0, 2, 1, 1, "02", "00", "79");
    editDetails()->page()->addKnob(0, 3, 1, 1, "02", "00", "77", "normal","bottom", 80);

    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "02", "00", "7B");
    editDetails()->page()->addKnob(0, 1, 1, 1, "02", "00", "7C");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
    editDetails()->page()->addStackField();


    // Flanger
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 2, 1);

    editDetails()->page()->newGroupBox("Flanger");
    editDetails()->page()->addKnob(0, 0, 1, 3, "02", "00", "7E", "normal","right", 120);
    editDetails()->page()->addKnob(1, 0, 1, 1, "02", "00", "7F");
    editDetails()->page()->addKnob(1, 1, 1, 1, "03", "00", "00");
    editDetails()->page()->addKnob(1, 2, 1, 1, "03", "00", "01");
    //editDetails()->page()->addKnob(1, 3, 1, 1, "03", "00", "02");
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Filter");
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "03");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "04");
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "05");
    editDetails()->page()->addGroupBox(0, 3, 2, 1);
    editDetails()->page()->addStackField();


    // Tremolo
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Tremelo");
    editDetails()->page()->newGroupBox("Wave");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "07");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "08", "normal","right", 120);
    editDetails()->page()->addKnob(0, 2, 1, 1, "03", "00", "09");
    editDetails()->page()->addKnob(0, 3, 1, 1, "03", "00", "0A");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->addStackField();


    // Rotary
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 3, "03", "00", "0C");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Rotary");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "0D", "normal","right", 120);
    editDetails()->page()->addKnob(1, 0, 1, 1, "03", "00", "0E", "normal","right", 120);
    //editDetails()->page()->addKnob(0, 1, 2, 1, "03", "00", "0F");
    //editDetails()->page()->addKnob(0, 2, 2, 1, "03", "00", "10");
    editDetails()->page()->addKnob(0, 3, 2, 1, "03", "00", "11");
    editDetails()->page()->addKnob(0, 4, 2, 1, "03", "00", "12");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->addStackField();

    // Uni-V
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Uni-V");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "14", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "15");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "16");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // Panner
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Pan");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "03", "00", "18");
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "19");
    editDetails()->page()->newGroupBox("Wave");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "1A");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addKnob(0, 3, 1, 1, "03", "00", "1B", "normal","right", 120);
    editDetails()->page()->addKnob(0, 5, 1, 1, "03", "00", "1C");
    editDetails()->page()->addKnob(0, 6, 1, 1, "03", "00", "1D");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();


    // Slicer
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Slicer");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "03", "00", "1F");
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "20", "normal","right", 120);
    editDetails()->page()->addKnob(0, 2, 1, 1, "03", "00", "21");
    editDetails()->page()->addKnob(0, 3, 1, 1, "03", "00", "22");
    editDetails()->page()->addKnob(0, 4, 1, 1, "03", "00", "23");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->addStackField();

    // Vibrato
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Vibrato");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "25", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "26");

    editDetails()->page()->newGroupBox("Trigger");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "27", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 2, 1, 1);

    //editDetails()->page()->addKnob(0, 3, 1, 1, "03", "00", "28");
    editDetails()->page()->addKnob(0, 4, 1, 1, "03", "00", "29");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->addStackField();


    // Ring Modulator
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "03", "00", "2B");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Ring Modulator");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "2C");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "2D");
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "2E");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();


    // Humanizer
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "03", "00", "30");
    editDetails()->page()->addGroupBox(0, 0, 2, 1);

    editDetails()->page()->newGroupBox("Humanizer");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "03", "00", "31");
    editDetails()->page()->addComboBox(1, 0, 1, 1, "03", "00", "32");
    editDetails()->page()->addKnob(0, 1, 1, 3, "03", "00", "33", "normal","right", 120);
    editDetails()->page()->addKnob(1, 1, 1, 1, "03", "00", "34");
    editDetails()->page()->addKnob(1, 2, 1, 1, "03", "00", "35");
    editDetails()->page()->addKnob(1, 3, 1, 1, "03", "00", "36");
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "37");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
    editDetails()->page()->addStackField();


    // 2CE
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 2, 1);

    editDetails()->page()->newGroupBox("2x2 Chorus");
    editDetails()->page()->newGroupBox("Low");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "3C", "normal","bottom", 63);
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "3A", "normal","bottom", 120);
    editDetails()->page()->addKnob(0, 2, 1, 1, "03", "00", "3B");
    editDetails()->page()->addGroupBox(1, 0, 1, 1);

    editDetails()->page()->newGroupBox("High");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "40", "normal","bottom", 63);
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "3E", "normal","bottom", 120);
    editDetails()->page()->addKnob(0, 2, 1, 1, "03", "00", "3F");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addKnob(0, 1, 2, 1, "03", "00", "39", "normal","right", 63);
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "41"); // high
    editDetails()->page()->addKnob(1, 0, 1, 1, "03", "00", "42"); //direct
    editDetails()->page()->addKnob(2, 0, 1, 1, "03", "00", "3D"); //low
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
    editDetails()->page()->addStackField();



    // Sub Delay
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Sub Delay");
    editDetails()->page()->newStackControl(1);
    editDetails()->page()->addComboBox(0, 0, 1, 1, "03", "00", "43");
    editDetails()->page()->addStackControl();
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "44", "normal","right", 120);
    editDetails()->page()->addKnob(0, 2, 1, 1, "03", "00", "46");
    editDetails()->page()->addKnob(0, 3, 1, 1, "03", "00", "47");
    editDetails()->page()->insertStackField(1, 0, 4, 1, 1);
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "48");
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "49");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "4A");
    editDetails()->page()->addStackField();

    // AC Guitar Sim
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("AC Guitar Simulator");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "10"); //high
    editDetails()->page()->addKnob(0, 2, 1, 1, "10", "00", "11"); //body
    editDetails()->page()->addKnob(0, 3, 1, 1, "10", "00", "12"); //low
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "14"); //level
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // Rotary 2
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 3, "10", "00", "17"); //speed sel
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Rotary 2");
    editDetails()->page()->addKnob(0, 0, 2, 1, "10", "00", "18", "normal","right", 120); //rate slow
    editDetails()->page()->addKnob(2, 0, 2, 1, "10", "00", "19", "normal","right", 120); //rate fast
    editDetails()->page()->addKnob(0, 1, 2, 1, "10", "00", "1A"); //rise
    editDetails()->page()->addKnob(2, 1, 2, 1, "10", "00", "1B"); //fall
    editDetails()->page()->addKnob(0, 2, 2, 1, "10", "00", "16"); //balance
    editDetails()->page()->addKnob(2, 2, 2, 1, "10", "00", "1C"); //depth
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "1D"); //effect
    editDetails()->page()->addKnob(2, 0, 1, 1, "10", "00", "1E"); //direct
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0); editDetails()->page()->addStackField();

    // Phaser 90E
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Phaser 90E");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "10", "00", "3D");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "3E", "normal","right", 80);
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->addStackField();

    // Flanger 117E
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Flanger 117E");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "3F");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "40");
    editDetails()->page()->addKnob(0, 2, 1, 1, "10", "00", "41");
    editDetails()->page()->addKnob(0, 3, 1, 1, "10", "00", "42");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->addStackField();

    // Wah 95E
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter); //on/off
    editDetails()->page()->addGroupBox(1, 0, 1, 1);

    editDetails()->page()->newGroupBox("WAH95E");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "68"); //pedal pos
    editDetails()->page()->addKnob(0, 2, 1, 1, "10", "00", "69");  //min
    editDetails()->page()->addKnob(0, 3, 1, 1, "10", "00", "6A");  //max
    editDetails()->page()->addGroupBox(1, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "6B"); //effect
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "6C"); //direct
    editDetails()->page()->addGroupBox(1, 2, 1, 1);
    editDetails()->page()->addStackField();

    // DC30
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "01", "00", "40", "middle", Qt::AlignCenter); //on/off
    editDetails()->page()->newStackControl(1);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "10", "00", "6D", "bottom", Qt::AlignRight); // mode
    editDetails()->page()->addStackControl();
    editDetails()->page()->addGroupBox(1, 0, 1, 1);

    editDetails()->page()->insertStackField(1, 1, 1, 1, 1);

    editDetails()->page()->newGroupBox("Common");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "6E"); // input
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "74"); // tone
    editDetails()->page()->addComboBox(0, 2, 1, 1, "10", "00", "75", "bottom", Qt::AlignRight); // output
    editDetails()->page()->addGroupBox(1, 2, 1, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->newGroupBox("Chorus");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "6F"); //sense
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->newGroupBox("Echo");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "70"); //repeat rate
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "72");  //intensity
    editDetails()->page()->addKnob(0, 2, 1, 1, "10", "00", "73");  //volume
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addStackField();

    //Heavy Octave
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Direct", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addKnob(0, 0, 1, 1, "11", "00", "19");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Octave -1", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addKnob(0, 0, 1, 1, "11", "00", "17");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Octave -2", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addKnob(0, 0, 1, 1, "11", "00", "18");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();



    editDetails()->addPage();
}
