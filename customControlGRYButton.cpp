/****************************************************************************
**
** Copyright (C) 2007~2018 Colin Willcocks.
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

#include "customControlGRYButton.h"
#include "MidiTable.h"
#include "SysxIO.h"
#include "Preferences.h"

customControlGRYButton::customControlGRYButton(QWidget *parent,
                                         QString hex1, QString hex2, QString hex3,
                                         QString direction)
    : QWidget(parent)
{
    this->label = new customControlLabel(this);
    this->hex1 = hex1;
    this->hex2 = hex2;
    this->hex3 = hex3;
    this->area = direction;
    if (this->area != "System"){area = "Structure";};
    MidiTable *midiTable = MidiTable::Instance();
    Midi items = midiTable->getMidiMap(this->area, hex1, hex2, hex3);
    QString labeltxt = items.customdesc;

    this->label->setUpperCase(true);
    this->label->setText(labeltxt);

    QString imagePath(":/images/switchGRY.png");
    this->grybutton = new customGRYButton(false, this, hex1, hex2, hex3, imagePath);

    if(direction == "left")
    {

    }
    else if(direction == "right")
    {

    }
    else if(direction == "top")
    {

    }
    else if(direction == "bottom")
    {
        this->label->setAlignment(Qt::AlignCenter);

        QVBoxLayout *mainLayout = new QVBoxLayout;
        //mainLayout->setMargin(0);
        //mainLayout->setSpacing(5*ratio);
        mainLayout->addWidget(this->label, 0, Qt::AlignCenter);
        mainLayout->addWidget(this->grybutton, 0, Qt::AlignCenter);
        //mainLayout->addStretch(0);
        this->setLayout(mainLayout);
    }
    else if(direction == "middle" || direction == "System")
    {
        this->label->setAlignment(Qt::AlignCenter);

        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->setMargin(0);
        mainLayout->setSpacing(0);
        mainLayout->addStretch(0);
        mainLayout->addWidget(this->label, 0, Qt::AlignCenter);
        mainLayout->addWidget(this->grybutton, 0, Qt::AlignCenter);

        this->setLayout(mainLayout);
    };

    QObject::connect(this->parent(), SIGNAL( dialogUpdateSignal() ),
                     this, SLOT( dialogUpdateSignal() ));

    QObject::connect(this, SIGNAL( updateSignal() ),
                     this->parent(), SIGNAL( updateSignal() ));
}

void customControlGRYButton::paintEvent(QPaintEvent *)
{

}

void customControlGRYButton::valueChanged(int value, QString hex1, QString hex2, QString hex3)
{
    QString valueHex;
    if(value==1)
    {
        valueHex = "01";
    }
    else if(value==2)
    {
        valueHex = "02";
    }
    else
    {
        valueHex = "00";
    };

    SysxIO *sysxIO = SysxIO::Instance();
    sysxIO->setFileSource(this->area, hex1, hex2, hex3, valueHex);
    emit updateSignal();
}

void customControlGRYButton::dialogUpdateSignal()
{
    SysxIO *sysxIO = SysxIO::Instance();
    int value = sysxIO->getSourceValue(this->area, this->hex1, this->hex2, this->hex3);

     this->grybutton->setValue(value);
}
