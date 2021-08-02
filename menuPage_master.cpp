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

#include "menuPage_master.h"
#include "Preferences.h"

menuPage_master::menuPage_master(QWidget *parent)
    : menuPage(parent)
{
    setLSB("07", "00");
    editDetails()->patchPos(2028, 20, "07", "10");
    //editDetails()->patchPos(2124, 16, "07", "40");
    setEditPages();
}

void menuPage_master::updateSignal()
{

}

void menuPage_master::setEditPages()
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const int model = preferences->getPreferences("Midi", "Katana", "model").toInt(&ok);

//**************************************************************************************************************************


    editDetails()->page()->newGroupBox("Booster");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "12", "00", "30");                  // Booster
    editDetails()->page()->addGroupBox(0, 0, 1, 1);

    editDetails()->page()->newGroupBox("Delay1/Delay2");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "12", "00", "31");                     // Delay 1/2
    editDetails()->page()->addGroupBox(0, 1, 1, 1);

    editDetails()->page()->newGroupBox("Reverb");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "12", "00", "32");                     // Reverb
    editDetails()->page()->addGroupBox(0, 2, 1, 1);

    editDetails()->page()->newGroupBox("MOD/FX");
    editDetails()->page()->addComboBox(0, 0, 1, 1, "12", "00", "33");                     // MOD/FX Chorus
    editDetails()->page()->addComboBox(1, 0, 1, 1, "12", "00", "34");                     //
    editDetails()->page()->addComboBox(2, 0, 1, 1, "12", "00", "35");                     //
    editDetails()->page()->addComboBox(3, 0, 1, 1, "12", "00", "36");                     //
    editDetails()->page()->addComboBox(4, 0, 1, 1, "12", "00", "37");                     //
    editDetails()->page()->addComboBox(5, 0, 1, 1, "12", "00", "38");                     //
    editDetails()->page()->addComboBox(6, 0, 1, 1, "12", "00", "39");                     //
    editDetails()->page()->addComboBox(7, 0, 1, 1, "12", "00", "3A");                     //
    editDetails()->page()->addComboBox(8, 0, 1, 1, "12", "00", "3B");                     //
    editDetails()->page()->addComboBox(9, 0, 1, 1, "12", "00", "3C");                     //
    editDetails()->page()->addComboBox(0, 1, 1, 1, "12", "00", "3D");                     //
    editDetails()->page()->addComboBox(1, 1, 1, 1, "12", "00", "3E");                     //
    editDetails()->page()->addComboBox(2, 1, 1, 1, "12", "00", "3F");                     //
    editDetails()->page()->addComboBox(3, 1, 1, 1, "12", "00", "40");                     //
    editDetails()->page()->addComboBox(4, 1, 1, 1, "12", "00", "41");                     //
    editDetails()->page()->addComboBox(5, 1, 1, 1, "12", "00", "42");                     //
    editDetails()->page()->addComboBox(6, 1, 1, 1, "12", "00", "43");                     //
    editDetails()->page()->addComboBox(7, 1, 1, 1, "12", "00", "44");                     //
    editDetails()->page()->addComboBox(8, 1, 1, 1, "12", "00", "45");                     //
    editDetails()->page()->addComboBox(9, 1, 1, 1, "12", "00", "46");                     //
    editDetails()->page()->addComboBox(0, 2, 1, 1, "12", "00", "47");                     //
    editDetails()->page()->addComboBox(1, 2, 1, 1, "12", "00", "48");                     //
    editDetails()->page()->addComboBox(2, 2, 1, 1, "12", "00", "49");                     //
    editDetails()->page()->addComboBox(3, 2, 1, 1, "12", "00", "4A");                     //
    editDetails()->page()->addComboBox(4, 2, 1, 1, "12", "00", "4B");                     //
    editDetails()->page()->addComboBox(5, 2, 1, 1, "12", "00", "4C");                     //
    editDetails()->page()->addComboBox(6, 2, 1, 1, "12", "00", "4D");                      //
    editDetails()->page()->addComboBox(7, 2, 1, 1, "12", "00", "4E");                     //
    editDetails()->page()->addComboBox(8, 2, 1, 1, "12", "00", "4F");                     //
    editDetails()->page()->addComboBox(9, 2, 1, 1, "12", "00", "50");                     //
    editDetails()->page()->addGroupBox(1, 0, 1, 3);


editDetails()->addPage("00", "00", "2E", "00", "Tables");
//*************************************************************************************************************

editDetails()->page()->newGroupBox("EXP Assign");
editDetails()->page()->newStackControl(0);
editDetails()->page()->addComboBox(0, 0, 1, 1, "12", "00", "1F", "large");          //   EXP PEDAL FUNCTION
editDetails()->page()->addStackControl();
editDetails()->page()->insertStackField(0, 0, 1, 1, 1);
editDetails()->page()->addGroupBox(0, 0, 1, 1);

editDetails()->page()->newStackField(0);                                            // Volume
editDetails()->page()->addStackField();

editDetails()->page()->newStackField(0);                                            // Foot Volume
editDetails()->page()->addStackField();

editDetails()->page()->newStackField(0);                                            //FV/Wah
editDetails()->page()->addStackField();

editDetails()->page()->newStackField(0);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "00", "21","20", 80);       // booster
editDetails()->page()->addStackField();

editDetails()->page()->newStackField(0);
editDetails()->page()->newStackControl(1);
editDetails()->page()->addComboBox(1, 0, 1, 1, "00", "00", "36", "Tables");          //MOD
editDetails()->page()->addStackControl();
editDetails()->page()->insertStackField(1, 0, 1, 2, 1);
editDetails()->page()->addStackField();

editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "03", "2B","2A", 80);        //Chorus
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "04", "2D","2C", 80);        // Flanger
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "05", "2F","2E", 80);        // Phaser
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "06", "31","30", 80);        // UniVibe
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "07", "33","32", 80);        // Tremolo
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "08", "35","34", 80);        // Vibrato
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "09", "37","36", 80);        // Rotary
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "0A", "39","38", 80);        // Ring Mod
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "0B", "3B","3A", 80);        // Slow Gear
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "0C", "3D","3C", 80);        // Slicer
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "0D", "3F","3E", 80);        // Compressor
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "0E", "41","40", 80);        // Limiter
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "0F", "43","42", 80);        // T Wah
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "10", "45","44", 80);        // Auto Wah
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "11", "47","46", 80);        // Pedal Wah
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "12", "49","48", 80);        // Graphic EQ
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "13", "4B","4A", 80);        // Parametric EQ
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "14", "4D","4C", 80);        // Guitar Sim
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "15", "4F","4E", 80);        // Acoustic Sim
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "16", "51","50", 80);        // Acoustic Proc
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "17", "53","52", 80);        // Wave Synth
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "18", "55","54", 80);        // Octave
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "19", "58","56", 80);        // Pitch Shifter
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "1A", "5C","5A", 80);        // Harmonist
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "1B", "5F","5E", 80);        // Humanizer
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "1C", "61","60", 80);        // Phaser 90E
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "1D", "63","62", 80);        // Flanger 117E
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "1E", "65","64", 80);        // Wah 95E
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "1F", "68","66", 80);        // DC-30
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "6A", "6C","6B", 80);        // Heavy Oct
editDetails()->page()->addStackField();


editDetails()->page()->newStackField(0);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "01", "24","22", 80);       //delay 1
editDetails()->page()->addStackField();

editDetails()->page()->newStackField(0);
editDetails()->page()->newStackControl(1);
editDetails()->page()->addComboBox(1, 0, 1, 1, "00", "00", "37", "Tables");          // FX
editDetails()->page()->addStackControl();
editDetails()->page()->insertStackField(1, 0, 1, 2, 1);
editDetails()->page()->addStackField();

editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "03", "2B","2A", 80);        //Chorus
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "04", "2D","2C", 80);        // Flanger
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "05", "2F","2E", 80);        // Phaser
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "06", "31","30", 80);        // UniVibe
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "07", "33","32", 80);        // Tremolo
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "08", "35","34", 80);        // Vibrato
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "09", "37","36", 80);        // Rotary
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "0A", "39","38", 80);        // Ring Mod
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "0B", "3B","3A", 80);        // Slow Gear
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "0C", "3D","3C", 80);        // Slicer
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "0D", "3F","3E", 80);        // Compressor
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "0E", "41","40", 80);        // Limiter
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "0F", "43","42", 80);        // T Wah
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "10", "45","44", 80);        // Auto Wah
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "11", "47","46", 80);        // Pedal Wah
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "12", "49","48", 80);        // Graphic EQ
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "13", "4B","4A", 80);        // Parametric EQ
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "14", "4D","4C", 80);        // Guitar Sim
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "15", "4F","4E", 80);        // Acoustic Sim
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "16", "51","50", 80);        // Acoustic Proc
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "17", "53","52", 80);        // Wave Synth
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "18", "55","54", 80);        // Octave
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "19", "58","56", 80);        // Pitch Shifter
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "1A", "5C","5A", 80);        // Harmonist
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "1B", "5F","5E", 80);        // Humanizer
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "1C", "61","60", 80);        // Phaser 90E
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "1D", "63","62", 80);        // Flanger 117E
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "1E", "65","64", 80);        // Wah 95E
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "1F", "68","66", 80);        // DC-30
editDetails()->page()->addStackField();
editDetails()->page()->newStackField(1);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "6A", "6C","6B", 80);        // Heavy Octave
editDetails()->page()->addStackField();

editDetails()->page()->newStackField(0);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "01", "24","22", 80);       //delay 2
editDetails()->page()->addStackField();

editDetails()->page()->newStackField(0);
editDetails()->page()->addTarget(0, 0, 1, 3, "13", "00", "02", "28","26", 80);       //reverb
editDetails()->page()->addStackField();

editDetails()->addPage("00", "00", "2E", "01", "Tables");


if(model>0){
//******************************************************************************************************************************
    editDetails()->page()->newGroupBox("GAFC EXP1 Assign");
    editDetails()->page()->newStackControl(0);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "12", "00", "20", "large");                     // GAFC EXP1 PEDAL FUNCTION
    editDetails()->page()->addStackControl();
    editDetails()->page()->insertStackField(0, 0, 1, 2, 1);
    editDetails()->page()->addGroupBox(1, 2, 1, 1);

    editDetails()->page()->newStackField(0);                                            // Volume
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);                                            // Foot Volume
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);                                            //FV/Wah
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "00", "21","20", 80);       // booster
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);
    editDetails()->page()->newStackControl(1);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "00", "00", "38", "Tables");          //MOD
    editDetails()->page()->addStackControl();
    editDetails()->page()->insertStackField(1, 0, 1, 2, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "03", "2B","2A", 80);        //Chorus
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "04", "2D","2C", 80);        // Flanger
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "05", "2F","2E", 80);        // Phaser
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "06", "31","30", 80);        // UniVibe
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "07", "33","32", 80);        // Tremolo
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "08", "35","34", 80);        // Vibrato
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "09", "37","36", 80);        // Rotary
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "0A", "39","38", 80);        // Ring Mod
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "0B", "3B","3A", 80);        // Slow Gear
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "0C", "3D","3C", 80);        // Slicer
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "0D", "3F","3E", 80);        // Compressor
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "0E", "41","40", 80);        // Limiter
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "0F", "43","42", 80);        // T Wah
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "10", "45","44", 80);        // Auto Wah
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "11", "47","46", 80);        // Pedal Wah
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "12", "49","48", 80);        // Graphic EQ
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "13", "4B","4A", 80);        // Parametric EQ
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "14", "4D","4C", 80);        // Guitar Sim
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "15", "4F","4E", 80);        // Acoustic Sim
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "16", "51","50", 80);        // Acoustic Proc
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "17", "53","52", 80);        // Wave Synth
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "18", "55","54", 80);        // Octave
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "19", "58","56", 80);        // Pitch Shifter
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "1A", "5C","5A", 80);        // Harmonist
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "1B", "5F","5E", 80);        // Humanizer
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "1C", "61","60", 80);        // Phaser 90E
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "1D", "63","62", 80);        // Flanger 117E
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "1E", "65","64", 80);        // Wah 95E
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "1F", "68","66", 80);        // DC-30
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "6A", "6C","6B", 80);        // Heavy Octave
    editDetails()->page()->addStackField();


    editDetails()->page()->newStackField(0);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "01", "24","22", 80);       //delay 1
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);
    editDetails()->page()->newStackControl(1);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "00", "00", "39", "Tables");          // FX
    editDetails()->page()->addStackControl();
    editDetails()->page()->insertStackField(1, 0, 1, 2, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "03", "2B","2A", 80);        //Chorus
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "04", "2D","2C", 80);        // Flanger
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "05", "2F","2E", 80);        // Phaser
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "06", "31","30", 80);        // UniVibe
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "07", "33","32", 80);        // Tremolo
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "08", "35","34", 80);        // Vibrato
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "09", "37","36", 80);        // Rotary
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "0A", "39","38", 80);        // Ring Mod
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "0B", "3B","3A", 80);        // Slow Gear
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "0C", "3D","3C", 80);        // Slicer
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "0D", "3F","3E", 80);        // Compressor
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "0E", "41","40", 80);        // Limiter
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "0F", "43","42", 80);        // T Wah
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "10", "45","44", 80);        // Auto Wah
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "11", "47","46", 80);        // Pedal Wah
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "12", "49","48", 80);        // Graphic EQ
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "13", "4B","4A", 80);        // Parametric EQ
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "14", "4D","4C", 80);        // Guitar Sim
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "15", "4F","4E", 80);        // Acoustic Sim
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "16", "51","50", 80);        // Acoustic Proc
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "17", "53","52", 80);        // Wave Synth
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "18", "55","54", 80);        // Octave
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "19", "58","56", 80);        // Pitch Shifter
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "1A", "5C","5A", 80);        // Harmonist
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "1B", "5F","5E", 80);        // Humanizer
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "1C", "61","60", 80);        // Phaser 90E
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "1D", "63","62", 80);        // Flanger 117E
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "1E", "65","64", 80);        // Wah 95E
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "1F", "68","66", 80);        // DC-30
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "1F", "68","66", 80);        // Heavy Octave
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "01", "24","22", 80);       //delay 2
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);
    editDetails()->page()->addTarget(0, 0, 1, 3, "14", "00", "02", "28","26", 80);       //reverb
    editDetails()->page()->addStackField();

editDetails()->addPage("00", "00", "2E", "02", "Tables");

//************************************************************************************************************************
    editDetails()->page()->newGroupBox("GAFC EXP2 Assign");
    editDetails()->page()->newStackControl(0);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "12", "00", "21", "large");          // GAFC EXP2 PEDAL FUNCTION
    editDetails()->page()->addStackControl();
    editDetails()->page()->insertStackField(0, 0, 1, 2, 1);
    editDetails()->page()->addGroupBox(2, 2, 1, 1);

    editDetails()->page()->newStackField(0);                                            // Volume
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);                                            // Foot Volume
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);                                            //FV/Wah
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "00", "21","20", 80);       // booster
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);
    editDetails()->page()->newStackControl(1);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "00", "00", "3A", "Tables");          //MOD
    editDetails()->page()->addStackControl();
    editDetails()->page()->insertStackField(1, 0, 1, 2, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "03", "2B","2A", 80);        //Chorus
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "04", "2D","2C", 80);        // Flanger
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "05", "2F","2E", 80);        // Phaser
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "06", "31","30", 80);        // UniVibe
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "07", "33","32", 80);        // Tremolo
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "08", "35","34", 80);        // Vibrato
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "09", "37","36", 80);        // Rotary
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "0A", "39","38", 80);        // Ring Mod
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "0B", "3B","3A", 80);        // Slow Gear
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "0C", "3D","3C", 80);        // Slicer
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "0D", "3F","3E", 80);        // Compressor
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "0E", "41","40", 80);        // Limiter
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "0F", "43","42", 80);        // T Wah
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "10", "45","44", 80);        // Auto Wah
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "11", "47","46", 80);        // Pedal Wah
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "12", "49","48", 80);        // Graphic EQ
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "13", "4B","4A", 80);        // Parametric EQ
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "14", "4D","4C", 80);        // Guitar Sim
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "15", "4F","4E", 80);        // Acoustic Sim
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "16", "51","50", 80);        // Acoustic Proc
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "17", "53","52", 80);        // Wave Synth
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "18", "55","54", 80);        // Octave
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "19", "58","56", 80);        // Pitch Shifter
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "1A", "5C","5A", 80);        // Harmonist
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "1B", "5F","5E", 80);        // Humanizer
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "1C", "61","60", 80);        // Phaser 90E
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "1D", "63","62", 80);        // Flanger 117E
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "1E", "65","64", 80);        // Wah 95E
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "1F", "68","66", 80);        // DC-30
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "6A", "6C","6B", 80);        // Heavy Octave
    editDetails()->page()->addStackField();


    editDetails()->page()->newStackField(0);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "01", "24","22", 80);       //delay 1
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);
    editDetails()->page()->newStackControl(1);
    editDetails()->page()->addComboBox(1, 0, 1, 1, "00", "00", "3B", "Tables");          // FX
    editDetails()->page()->addStackControl();
    editDetails()->page()->insertStackField(1, 0, 1, 2, 1);
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "03", "2B","2A", 80);        //Chorus
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "04", "2D","2C", 80);        // Flanger
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "05", "2F","2E", 80);        // Phaser
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "06", "31","30", 80);        // UniVibe
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "07", "33","32", 80);        // Tremolo
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "08", "35","34", 80);        // Vibrato
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "09", "37","36", 80);        // Rotary
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "0A", "39","38", 80);        // Ring Mod
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "0B", "3B","3A", 80);        // Slow Gear
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "0C", "3D","3C", 80);        // Slicer
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "0D", "3F","3E", 80);        // Compressor
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "0E", "41","40", 80);        // Limiter
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "0F", "43","42", 80);        // T Wah
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "10", "45","44", 80);        // Auto Wah
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "11", "47","46", 80);        // Pedal Wah
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "12", "49","48", 80);        // Graphic EQ
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "13", "4B","4A", 80);        // Parametric EQ
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "14", "4D","4C", 80);        // Guitar Sim
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "15", "4F","4E", 80);        // Acoustic Sim
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "16", "51","50", 80);        // Acoustic Proc
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "17", "53","52", 80);        // Wave Synth
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "18", "55","54", 80);        // Octave
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "19", "58","56", 80);        // Pitch Shifter
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "1A", "5C","5A", 80);        // Harmonist
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "1B", "5F","5E", 80);        // Humanizer
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "1C", "61","60", 80);        // Phaser 90E
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "1D", "63","62", 80);        // Flanger 117E
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "1E", "65","64", 80);        // Wah 95E
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "1F", "68","66", 80);        // DC-30
    editDetails()->page()->addStackField();
    editDetails()->page()->newStackField(1);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "6A", "6C","6B", 80);        // Heavy Octave
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "01", "24","22", 80);       //delay 2
    editDetails()->page()->addStackField();

    editDetails()->page()->newStackField(0);
    editDetails()->page()->addTarget(0, 0, 1, 3, "15", "00", "02", "28","26", 80);       //reverb
    editDetails()->page()->addStackField();
    editDetails()->addPage("00", "00", "2E", "03", "Tables");
    };
}



