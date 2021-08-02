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

#include "stompbox_fx2.h"

stompbox_fx2::stompbox_fx2(QWidget *parent)
    : stompBox(parent)
{
    /* FX2 */
    setImage(":/images/fx2.png");
    setLSB("03", "00");
    setComboBox("03", "00", "4D", QRect(8, 31, 79, 13));
    setButton("03", "00", "4C");
    editDetails()->patchPos(1020, 586, "03", "00");
    setEditPages();
}

void stompbox_fx2::updateSignal()
{
    updateComboBox("03", "00", "4D");
    updateButton("03", "00", "4C");
    updateSwitch("03", "00", "4C");
}

void stompbox_fx2::setEditPages()
{
    editDetails()->page()->newGroupBox("Select");
    editDetails()->page()->newStackControl(0);
    editDetails()->page()->addComboBox(0, 0, 1, 1, "03", "00", "4D", "large");
    editDetails()->page()->addStackControl();
    editDetails()->page()->addGroupBox(0, 0, 2, 1);
    editDetails()->page()->insertStackField(0, 0, 1, 3, 1);

    editDetails()->page()->newGroupBox("Option Select");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "12", "00", "0A", "large");
    editDetails()->page()->addComboBox(1, 0, 1, 1, "12", "00", "0B", "large");
    editDetails()->page()->addComboBox(2, 0, 1, 1, "12", "00", "0C", "large");
    editDetails()->page()->addGroupBox(2, 0, 1, 1);



    // T-Wah
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter); //on/off
    editDetails()->page()->addComboBox(1, 0, 1, 1, "03", "00", "58", "bottom", Qt::AlignRight); // mode
    editDetails()->page()->addComboBox(2, 0, 1, 2, "03", "00", "59");  //polarity
    editDetails()->page()->addGroupBox(1, 0, 1, 1);

    editDetails()->page()->newGroupBox("Touch Wah");
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "5A"); //sense
    editDetails()->page()->addKnob(0, 2, 1, 1, "03", "00", "5B");  //freq
    editDetails()->page()->addKnob(0, 3, 1, 1, "03", "00", "5C");  //peak
    editDetails()->page()->addGroupBox(1, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "5D"); //direct mix
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "5E"); //effect level
    editDetails()->page()->addGroupBox(1, 2, 1, 1);
    editDetails()->page()->addStackField();

    // Auto Wah
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "03", "00", "60");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Auto Wah");
    editDetails()->page()->addKnob(0, 3, 1, 1, "03", "00", "61");
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "62");
    editDetails()->page()->addKnob(0, 2, 1, 1, "03", "00", "63", "normal","bottom", 120);
    editDetails()->page()->addKnob(0, 4, 1, 1, "03", "00", "64");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "65");
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "66");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // Sub-Wah
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "03", "00", "68", "bottom", Qt::AlignRight);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Sub Wah");
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "69" );
    editDetails()->page()->addKnob(0, 2, 1, 1, "03", "00", "6A");
    editDetails()->page()->addKnob(0, 3, 1, 1, "03", "00", "6B");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "6C");
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "6D");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // Advanced Compressor
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "03", "00", "6F");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Advanced Compressor");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "70");
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "71");
    editDetails()->page()->addKnob(0, 2, 1, 1, "03", "00", "72");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "73");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // Limiter
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "03", "00", "75");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Limitter");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "76");
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "77");
    editDetails()->page()->addKnob(0, 2, 1, 1, "03", "00", "78");
    editDetails()->page()->addKnob(0, 3, 1, 1, "03", "00", "79");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "7A");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    //DISTORTION
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "03", "00", "4E");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Drive");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "4F", "turbo");
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "50");
    editDetails()->page()->addKnob(0, 2, 1, 1, "03", "00", "51");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "03", "00", "54");
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "55");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);

    editDetails()->page()->newGroupBox("Solo");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "52", "middle", Qt::AlignCenter);
    editDetails()->page()->addKnob(0, 1, 1, 1, "03", "00", "53");
    editDetails()->page()->addGroupBox(0, 3, 1, 1);
    editDetails()->page()->addStackField();

    // Graphic EQ
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addGraphicEQ(0, 1, 2, 4, "03", "00", "7C", "geq_slider");
    editDetails()->page()->addGroupBox(0, 0, 2, 4);
    editDetails()->page()->addStackField();

    // Parametric EQ
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addParaEQ(0, 1, 2, 4, "04", "00", "08");
    editDetails()->page()->addGroupBox(0, 0, 2, 4);
    editDetails()->page()->addStackField();

    // Tone Modify
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "04", "00", "14");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Tone Modify");
    editDetails()->page()->addKnob(0, 3, 1, 1, "04", "00", "15");
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "16");
    editDetails()->page()->addKnob(0, 2, 1, 1, "04", "00", "17");;
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "04", "00", "18");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // Guitar Sim
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "04", "00", "1A");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Guitar Simulator");
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "1B");
    editDetails()->page()->addKnob(0, 2, 1, 1, "04", "00", "1C");
    editDetails()->page()->addKnob(0, 3, 1, 1, "04", "00", "1E");;
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "04", "00", "1D");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();


    // Slow Gear
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Slow Gear");
    editDetails()->page()->addKnob(0, 0, 1, 1, "04", "00", "20");
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "21");
    editDetails()->page()->addKnob(0, 2, 1, 1, "04", "00", "22");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->addStackField();


    // DeFretter
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Defretter");
    editDetails()->page()->addKnob(0, 0, 1, 1, "04", "00", "24");
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "25");
    editDetails()->page()->addKnob(0, 2, 1, 1, "04", "00", "26");
    editDetails()->page()->addKnob(0, 3, 1, 1, "04", "00", "27");
    editDetails()->page()->addKnob(0, 4, 1, 1, "04", "00", "28");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "04", "00", "29");
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "2A");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();


    // Wave Synth
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "04", "00", "2C");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Wave Synth");
    editDetails()->page()->addKnob(0, 0, 1, 1, "04", "00", "2D");
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "2E");
    editDetails()->page()->addKnob(0, 2, 1, 1, "04", "00", "2F");
    editDetails()->page()->addKnob(0, 3, 1, 1, "04", "00", "30");
    editDetails()->page()->addKnob(0, 4, 1, 1, "04", "00", "31");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "04", "00", "32");
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "33");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();


    // Sitar Sim
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Sitar");
    editDetails()->page()->addKnob(0, 0, 1, 1, "04", "00", "35");
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "36");
    editDetails()->page()->addKnob(0, 2, 1, 1, "04", "00", "37");
    editDetails()->page()->addKnob(0, 3, 1, 1, "04", "00", "38");
    editDetails()->page()->addKnob(0, 4, 1, 1, "04", "00", "39");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "04", "00", "3A");
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "3B");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // Octaver
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Octave", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addComboBox(0, 0, 1, 3, "04", "00", "3D");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "04", "00", "3E");
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "3F");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();


    // Pitch Shifter
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->newStackControl(1);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "04", "00", "41");
    editDetails()->page()->addStackControl();
    editDetails()->page()->addGroupBox(0, 0, 3, 1);

    editDetails()->page()->insertStackField(1, 0, 1, 3, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->newGroupBox("Pitch Shifter");
    editDetails()->page()->newGroupBox("Voice 1");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "04", "00", "42");
    editDetails()->page()->addKnob(1, 0, 1, 1, "04", "00", "45", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 2, 1, "04", "00", "43");
    editDetails()->page()->addKnob(0, 2, 2, 1, "04", "00", "44");
    editDetails()->page()->addGroupBox(0, 0, 3, 1);
    editDetails()->page()->addKnob(1, 1, 1, 1, "04", "00", "4E");
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "04", "00", "47");
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "4F");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->newGroupBox("Pitch Shifter");
    editDetails()->page()->newGroupBox("Voice 1");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "04", "00", "42");
    editDetails()->page()->addKnob(1, 0, 1, 1, "04", "00", "45", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 2, 1, "04", "00", "43");
    editDetails()->page()->addKnob(0, 2, 2, 1, "04", "00", "44");
    editDetails()->page()->addGroupBox(0, 0, 3, 1);
    editDetails()->page()->addKnob(1, 1, 1, 1, "04", "00", "4E");

    editDetails()->page()->newGroupBox("Voice 2");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "04", "00", "48");
    editDetails()->page()->addKnob(1, 0, 1, 1, "04", "00", "4B", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 2, 1, "04", "00", "49");
    editDetails()->page()->addKnob(0, 2, 2, 1, "04", "00", "4A");
    editDetails()->page()->addGroupBox(4, 0, 1, 1);
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "04", "00", "47");
    editDetails()->page()->addKnob(2, 0, 1, 1, "04", "00", "4D");
    editDetails()->page()->addKnob(1, 1, 1, 1, "04", "00", "4F");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->newGroupBox("Pitch Shifter");
    editDetails()->page()->newGroupBox("Voice 1");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "04", "00", "42");
    editDetails()->page()->addKnob(1, 0, 1, 1, "04", "00", "45", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 2, 1, "04", "00", "43");
    editDetails()->page()->addKnob(0, 2, 2, 1, "04", "00", "44");
    editDetails()->page()->addGroupBox(0, 0, 3, 1);
    editDetails()->page()->addKnob(1, 1, 1, 1, "04", "00", "4E");

    editDetails()->page()->newGroupBox("Voice 2");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "04", "00", "48");
    editDetails()->page()->addKnob(1, 0, 1, 1, "04", "00", "4B", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 2, 1, "04", "00", "49");
    editDetails()->page()->addKnob(0, 2, 2, 1, "04", "00", "4A");
    editDetails()->page()->addGroupBox(4, 0, 1, 1);
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "04", "00", "47");
    editDetails()->page()->addKnob(2, 0, 1, 1, "04", "00", "4D");
    editDetails()->page()->addKnob(1, 1, 1, 1, "04", "00", "4F");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
    editDetails()->page()->addStackField();

    // Harmonizer
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->newStackControl(1);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "04", "00", "51");
    editDetails()->page()->addStackControl();
    editDetails()->page()->addComboBox(2, 0, 1, 1, "07", "00", "18"); //master key
    editDetails()->page()->addGroupBox(0, 0, 2, 1);
    editDetails()->page()->insertStackField(1, 0, 1, 3, 1);
    editDetails()->page()->newGroupBox("User Harmony 1");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "04", "00", "5C");
    editDetails()->page()->addComboBox(0, 1, 1, 1, "04", "00", "5D");
    editDetails()->page()->addComboBox(0, 2, 1, 1, "04", "00", "5E");
    editDetails()->page()->addComboBox(0, 3, 1, 1, "04", "00", "5F");
    editDetails()->page()->addComboBox(0, 4, 1, 1, "04", "00", "60");
    editDetails()->page()->addComboBox(0, 5, 1, 1, "04", "00", "61");
    editDetails()->page()->addComboBox(0, 6, 1, 1, "04", "00", "62");
    editDetails()->page()->addComboBox(0, 7, 1, 1, "04", "00", "63");
    editDetails()->page()->addComboBox(0, 8, 1, 1, "04", "00", "64");
    editDetails()->page()->addComboBox(0, 9, 1, 1, "04", "00", "65");
    editDetails()->page()->addComboBox(0, 10, 1, 1, "04", "00", "66");
    editDetails()->page()->addComboBox(0, 11, 1, 1, "04", "00", "67");
    editDetails()->page()->addGroupBox(3, 0, 1, 3);
    editDetails()->page()->newGroupBox("User Harmony 2");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "04", "00", "68");
    editDetails()->page()->addComboBox(0, 1, 1, 1, "04", "00", "69");
    editDetails()->page()->addComboBox(0, 2, 1, 1, "04", "00", "6A");
    editDetails()->page()->addComboBox(0, 3, 1, 1, "04", "00", "6B");
    editDetails()->page()->addComboBox(0, 4, 1, 1, "04", "00", "6C");
    editDetails()->page()->addComboBox(0, 5, 1, 1, "04", "00", "6D");
    editDetails()->page()->addComboBox(0, 6, 1, 1, "04", "00", "6E");
    editDetails()->page()->addComboBox(0, 7, 1, 1, "04", "00", "6F");
    editDetails()->page()->addComboBox(0, 8, 1, 1, "04", "00", "70");
    editDetails()->page()->addComboBox(0, 9, 1, 1, "04", "00", "71");
    editDetails()->page()->addComboBox(0, 10, 1, 1, "04", "00", "72");
    editDetails()->page()->addComboBox(0, 11, 1, 1, "04", "00", "73");
    editDetails()->page()->addGroupBox(4, 0, 1, 3);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->newGroupBox("Harmonist");
    editDetails()->page()->newGroupBox("Voice 1");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "04", "00", "52");
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "53", "normal","right", 120);
    editDetails()->page()->addGroupBox(0, 0, 2, 1);
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "5A", "normal","right", 50);
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "04", "00", "55");
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "5B");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->newGroupBox("Harmonist");
    editDetails()->page()->newGroupBox("Voice 1");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "04", "00", "52");
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "53", "normal","right", 120);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->newGroupBox("Voice 2");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "04", "00", "56");
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "57", "normal","right", 120);
    editDetails()->page()->addGroupBox(2, 0, 1, 1);
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "5A", "normal","right", 50);
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "04", "00", "55");
    editDetails()->page()->addKnob(1, 0, 1, 1, "04", "00", "59");
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "5B");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->newGroupBox("Harmonist");
    editDetails()->page()->newGroupBox("Voice 1");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "04", "00", "52");
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "53", "normal","right", 120);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->newGroupBox("Voice 2");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "04", "00", "56");
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "57", "normal","right", 120);
    editDetails()->page()->addGroupBox(2, 0, 1, 1);
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "5A", "normal","right", 50);
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "04", "00", "55");
    editDetails()->page()->addKnob(1, 0, 1, 1, "04", "00", "59");
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "5B");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
    editDetails()->page()->addStackField();


    // Sound Hold
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Sound Hold");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "04", "00", "75", "middle", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addKnob(0, 2, 1, 1, "04", "00", "76");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "04", "00", "77");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();


    // Acoustic Processor
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "04", "00", "79");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Acoustic Processor");
    editDetails()->page()->addKnob(0, 0, 1, 1, "04", "00", "7A");
    editDetails()->page()->addKnob(0, 1, 1, 1, "04", "00", "7B");
    editDetails()->page()->addKnob(0, 2, 1, 1, "04", "00", "7C", "normal","bottom", 80);
    editDetails()->page()->addKnob(0, 3, 1, 1, "04", "00", "7D");
    editDetails()->page()->addKnob(0, 4, 1, 1, "04", "00", "7E");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "04", "00", "7F");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();


    // Phaser
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "05", "00", "01");
    editDetails()->page()->addGroupBox(0, 0, 2, 1);

    editDetails()->page()->newGroupBox("Phaser");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "02", "normal","right", 120);
    editDetails()->page()->addKnob(1, 0, 1, 1, "05", "00", "06", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "04");
    editDetails()->page()->addKnob(0, 2, 1, 1, "05", "00", "05");
    editDetails()->page()->addKnob(0, 3, 1, 1, "05", "00", "03", "normal","right", 80);
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "07");
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "08");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
    editDetails()->page()->addStackField();


    // Flanger
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 2, 1);

    editDetails()->page()->newGroupBox("Flanger");
    editDetails()->page()->addKnob(0, 0, 1, 3, "05", "00", "0A", "normal","right", 120);
    editDetails()->page()->addKnob(1, 0, 1, 1, "05", "00", "0B");
    editDetails()->page()->addKnob(1, 1, 1, 1, "05", "00", "0C");
    editDetails()->page()->addKnob(1, 2, 1, 1, "05", "00", "0D");
    //editDetails()->page()->addKnob(1, 3, 1, 1, "05", "00", "0E");
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Filter");
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "0F");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "10");
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "11");
    editDetails()->page()->addGroupBox(0, 3, 2, 1);
    editDetails()->page()->addStackField();


    // Tremolo
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Tremelo");
    editDetails()->page()->newGroupBox("Wave");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "13");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "14", "normal","right", 120);
    editDetails()->page()->addKnob(0, 2, 1, 1, "05", "00", "15");
    editDetails()->page()->addKnob(0, 3, 1, 1, "05", "00", "16");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->addStackField();


    // Rotary 1
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 3, "05", "00", "18");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Rotary 1");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "19", "normal","right", 120);
    editDetails()->page()->addKnob(1, 0, 1, 1, "05", "00", "1A", "normal","right", 120);
    //editDetails()->page()->addKnob(0, 1, 2, 1, "05", "00", "1B");
    //editDetails()->page()->addKnob(0, 2, 2, 1, "05", "00", "1C");
    editDetails()->page()->addKnob(0, 3, 2, 1, "05", "00", "1D");
    editDetails()->page()->addKnob(0, 4, 2, 1, "05", "00", "1E");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->addStackField();

    // Uni-V
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Uni-V");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "20", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "21");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "22");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // Panner
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Pan");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "05", "00", "24");
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "25");
    editDetails()->page()->newGroupBox("Wave");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "26");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addKnob(0, 3, 1, 1, "05", "00", "27", "normal","right", 120);
    editDetails()->page()->addKnob(0, 5, 1, 1, "05", "00", "28");
    editDetails()->page()->addKnob(0, 6, 1, 1, "05", "00", "29");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();


    // Slicer
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Slicer");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "05", "00", "2B");
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "2C", "normal","right", 120);
    editDetails()->page()->addKnob(0, 2, 1, 1, "05", "00", "2D");
    editDetails()->page()->addKnob(0, 3, 1, 1, "05", "00", "2E");
    editDetails()->page()->addKnob(0, 4, 1, 1, "05", "00", "2F");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->addStackField();

    // Vibrato
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Vibrato");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "31", "normal","right", 120);
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "32");

    editDetails()->page()->newGroupBox("Trigger");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "05", "00", "33", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 2, 1, 1);

    //editDetails()->page()->addKnob(0, 3, 1, 1, "05", "00", "34");
    editDetails()->page()->addKnob(0, 4, 1, 1, "05", "00", "35");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->addStackField();


    // Ring Modulator
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "05", "00", "37");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Ring Modulator");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "38");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "39");
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "3A");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();


    // Humanizer
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "05", "00", "3C");
    editDetails()->page()->addGroupBox(0, 0, 2, 1);

    editDetails()->page()->newGroupBox("Humanizer");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "05", "00", "3D");
    editDetails()->page()->addComboBox(1, 0, 1, 1, "05", "00", "3E");
    editDetails()->page()->addKnob(0, 1, 1, 3, "05", "00", "40", "normal","right", 120);
    editDetails()->page()->addKnob(1, 1, 1, 1, "05", "00", "3F");
    editDetails()->page()->addKnob(1, 2, 1, 1, "05", "00", "41");
    editDetails()->page()->addKnob(1, 3, 1, 1, "05", "00", "42");
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "43");
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
    editDetails()->page()->addStackField();


    // 2CE
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 2, 1);

    editDetails()->page()->newGroupBox("2x2 Chorus");
    editDetails()->page()->newGroupBox("Low");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "48", "normal","bottom", 63);
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "46", "normal","bottom", 120);
    editDetails()->page()->addKnob(0, 2, 1, 1, "05", "00", "47");
    editDetails()->page()->addGroupBox(1, 0, 1, 1);

    editDetails()->page()->newGroupBox("High");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "4C", "normal","bottom", 63);
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "4A", "normal","bottom", 120);
    editDetails()->page()->addKnob(0, 2, 1, 1, "05", "00", "4B");

    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addKnob(0, 1, 2, 1, "05", "00", "45", "normal","right", 63);
    editDetails()->page()->addGroupBox(0, 1, 2, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "4D"); //high
    editDetails()->page()->addKnob(1, 0, 1, 1, "05", "00", "4E"); //direct
    editDetails()->page()->addKnob(2, 0, 1, 1, "05", "00", "49"); //low
    editDetails()->page()->addGroupBox(0, 2, 2, 1);
    editDetails()->page()->addStackField();



    // Sub Delay
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Sub Delay");
    editDetails()->page()->newStackControl(1);
    editDetails()->page()->addComboBox(0, 0, 1, 1, "05", "00", "4F");
    editDetails()->page()->addStackControl();
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "50", "normal","right", 120);
    editDetails()->page()->addKnob(0, 2, 1, 1, "05", "00", "52");
    editDetails()->page()->addKnob(0, 3, 1, 1, "05", "00", "53");
    editDetails()->page()->insertStackField(1, 0, 4, 1, 1);
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "54");
    editDetails()->page()->addKnob(0, 1, 1, 1, "05", "00", "55");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addKnob(0, 0, 1, 1, "05", "00", "56");
    editDetails()->page()->addStackField();

    // AC Guitar Sim
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("AC Guitar Simulator");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "1F"); //high
    editDetails()->page()->addKnob(0, 2, 1, 1, "10", "00", "20"); //body
    editDetails()->page()->addKnob(0, 3, 1, 1, "10", "00", "21"); //low
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "23"); //level
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // Rotary 2
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addComboBox(1, 0, 1, 3, "10", "00", "26");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Rotary 2");
    editDetails()->page()->addKnob(0, 0, 2, 1, "10", "00", "27", "normal","right", 120); //rate slow
    editDetails()->page()->addKnob(2, 0, 2, 1, "10", "00", "28", "normal","right", 120); //rate fast
    editDetails()->page()->addKnob(0, 1, 2, 1, "10", "00", "29"); //rise
    editDetails()->page()->addKnob(2, 1, 2, 1, "10", "00", "2A"); //fall
    editDetails()->page()->addKnob(0, 2, 2, 1, "10", "00", "25"); //balance
    editDetails()->page()->addKnob(2, 2, 2, 1, "10", "00", "2B"); //depth
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "2C"); //effect
    editDetails()->page()->addKnob(2, 0, 1, 1, "10", "00", "2D"); //direct
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // Tera Echo
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Tera Echo");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "10", "00", "2F"); //mode
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "30", "normal","right", 120); //s time
    editDetails()->page()->addKnob(0, 2, 1, 1, "10", "00", "31"); //feedback
    editDetails()->page()->addKnob(0, 3, 1, 1, "10", "00", "32"); //tone
    editDetails()->page()->addComboBox(0, 4, 1, 1, "10", "00", "35"); //hold
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "33"); //effect level
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "34"); //direct mix
    editDetails()->page()->addGroupBox(0, 2, 1, 1);

    editDetails()->page()->addStackField();


    //OVERTONE
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter); //on/off
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Over Tone");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "37"); //detune
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "38"); //tone
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "39"); //lower level
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "3A"); //upper level
    editDetails()->page()->addKnob(0, 2, 1, 1, "10", "00", "3B"); //direct mix
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();

    // Phaser 90E
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Phaser 90E");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "10", "00", "43");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "44", "normal","right", 80);
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->addStackField();

    // Flanger 117E
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter);
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Flanger 117E");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "45");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "46");
    editDetails()->page()->addKnob(0, 2, 1, 1, "10", "00", "47");
    editDetails()->page()->addKnob(0, 3, 1, 1, "10", "00", "48");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);
    editDetails()->page()->addStackField();

    // Wah 95E
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter); //on/off
    editDetails()->page()->addGroupBox(1, 0, 1, 1);

    editDetails()->page()->newGroupBox("WAH95E");
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "76"); //pedal pos
    editDetails()->page()->addKnob(0, 2, 1, 1, "10", "00", "77");  //min
    editDetails()->page()->addKnob(0, 3, 1, 1, "10", "00", "78");  //max
    editDetails()->page()->addGroupBox(1, 1, 1, 1);

    editDetails()->page()->newGroupBox("Level");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "79"); //effect
    editDetails()->page()->addKnob(0, 1, 1, 1, "10", "00", "7A"); //direct
    editDetails()->page()->addGroupBox(1, 2, 1, 1);
    editDetails()->page()->addStackField();

    // DC30
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Effect");
    editDetails()->page()->addSwitch(0, 0, 1, 1, "03", "00", "4C", "middle", Qt::AlignCenter); //on/off
    editDetails()->page()->newStackControl(1);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "10", "00", "7B", "bottom", Qt::AlignRight); // mode
    editDetails()->page()->addStackControl();
    editDetails()->page()->addGroupBox(1, 0, 1, 1);

    editDetails()->page()->insertStackField(1, 1, 1, 1, 1);

    editDetails()->page()->newGroupBox("Common");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "7C"); // input
    editDetails()->page()->addKnob(0, 1, 1, 1, "11", "00", "02"); // tone
    editDetails()->page()->addComboBox(0, 2, 1, 1, "11", "00", "03", "bottom", Qt::AlignRight); // output
    editDetails()->page()->addGroupBox(1, 2, 1, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->newGroupBox("Chorus");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "7D"); //sense
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->newGroupBox("Echo");
    editDetails()->page()->addKnob(0, 0, 1, 1, "10", "00", "7E"); // repeat rate
    editDetails()->page()->addKnob(0, 1, 1, 1, "11", "00", "00");  //intensity
    editDetails()->page()->addKnob(0, 2, 1, 1, "11", "00", "01");  //volume
    editDetails()->page()->addGroupBox(0, 0, 1, 1);
    editDetails()->page()->addStackField();

    //Heavy Octave
    editDetails()->page()->newStackField(0);
    editDetails()->page()->newGroupBox("Direct", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addKnob(0, 0, 1, 1, "11", "00", "1C");
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Octave -1", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addKnob(0, 0, 1, 1, "11", "00", "1A");
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Octave -2", Qt::AlignTop | Qt::AlignHCenter);
    editDetails()->page()->addKnob(0, 0, 1, 1, "11", "00", "1B");
    editDetails()->page()->addGroupBox(0, 2, 1, 1);
    editDetails()->page()->addStackField();


    editDetails()->addPage();
}
