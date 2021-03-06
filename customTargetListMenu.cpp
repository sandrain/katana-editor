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

#include "customTargetListMenu.h"
#include "MidiTable.h"
#include "SysxIO.h"
#include "Preferences.h"

customTargetListMenu::customTargetListMenu(QWidget *parent, 
                                           QString hex1, QString hex2, QString hex3, QString hexMsb,
                                           QString hexLsb, QString direction)
    : QWidget(parent)
{
    bool ok;
    target = false;
    if(hex1.toInt(&ok, 16)<11){ target = true; };
    Preferences *preferences = Preferences::Instance();

    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);
    const double fratio = preferences->getPreferences("Window", "Font", "ratio").toDouble(&ok);

    QFont Sfont( "Arial", 12*fratio, QFont::Bold);
    this->controlListComboBox = new customComboBox(this);
    this->controlListComboBox->setObjectName("smallcombo");
    this->controlListComboBox->setFont(Sfont);
    this->hex1 = hex1;
    this->hex2 = hex2;
    this->hex3 = hex3;
    this->hexMsb = hexMsb;
    this->hexLsb = hexLsb;
    if (!direction.contains("System")) {this->area = "Structure"; }
    else {this->area = direction; };

    setComboBox();

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
        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->setMargin(0);
        mainLayout->setSpacing(0);
        mainLayout->addStretch(0);
        mainLayout->addWidget(this->controlListComboBox, 0, Qt::AlignCenter);

        this->setLayout(mainLayout);
        this->setFixedHeight(20*ratio);

    }
    else
    {
        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->setMargin(0);
        mainLayout->setSpacing(0);
        mainLayout->addStretch(0);
        mainLayout->addWidget(this->controlListComboBox, 0, Qt::AlignCenter);

        this->setLayout(mainLayout);
        this->setFixedHeight(20*ratio);

    };

    QObject::connect(this, SIGNAL( updateSignal() ), this->parent(), SIGNAL( updateSignal() ));

    QObject::connect(this->controlListComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(valueChanged(int)));

    QObject::connect(this->controlListComboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(currentIndexChanged(int)));

    QObject::connect(this->controlListComboBox, SIGNAL(highlighted(int)), this, SLOT(valueChanged(int)));

    QObject::connect(this->parent(), SIGNAL(updateSignal()), this, SLOT(comboUpdateSignal()));

    QObject::connect(this, SIGNAL( updateTarget(QString, QString, QString) ),
                     this->parent(), SIGNAL( updateTarget(QString, QString, QString) ));
}

void customTargetListMenu::paintEvent(QPaintEvent *)
{
    /*DRAWS RED BACKGROUND FOR DeBugGING PURPOSE */
    /*QPixmap image(":images/dragbar.png");

    QRectF target(0.0, 0.0, this->width(), this->height());
    QRectF source(0.0, 0.0, this->width(), this->height());

    QPainter painter(this);
    painter.drawPixmap(target, image, source);*/
}

void customTargetListMenu::setComboBox()
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    MidiTable *midiTable = MidiTable::Instance();
    Midi items;
    Midi item_0;
    if(target){ item_0 = midiTable->getMidiMap("Structure", "08", "00", "31", "00"); }
    else{ item_0 = midiTable->getMidiMap("Structure", hex1, "00", hex3); };

    items = item_0;

    QString longestItem = "";
    int itemcount;
    int itemSize = items.level.size();
    int itemTotal = 0;
    for(itemcount=0;itemcount<itemSize;itemcount++ )
    {
        QString item;
        QString desc = items.level.at(itemcount).name;
        if(!desc.isEmpty())
        {
            item = desc;
        }
        else
        {
            item = "Out of Range";
        };
        if(longestItem.size() < item.size()) longestItem = item;
        this->controlListComboBox->addItem(item);
    };
    if(target){
        Midi item_1;
        Midi item_2;
        Midi item_3;
        Midi item_4;

        item_1 = midiTable->getMidiMap("Structure", "08", "00", "31", "01");
        item_2 = midiTable->getMidiMap("Structure", "08", "00", "31", "02");
        item_3 = midiTable->getMidiMap("Structure", "08", "00", "31", "03");
        item_4 = midiTable->getMidiMap("Structure", "08", "00", "31", "04");

        itemTotal = itemTotal + itemcount;
        items = item_1;
        itemSize = items.level.size();
        for(itemcount=0;itemcount<itemSize;itemcount++ )
        {
            QString item;
            QString desc = items.level.at(itemcount).name;
            if(!desc.isEmpty())
            {
                item = desc;
            }
            else
            {
                item = "Out of Range";
            };
            if(longestItem.size() < item.size()) longestItem = item;
            this->controlListComboBox->addItem(item);
        };

        itemTotal = itemTotal + itemcount;
        items = item_2;
        itemSize = items.level.size();
        for(itemcount=0;itemcount<itemSize;itemcount++ )
        {
            QString item;
            QString desc = items.level.at(itemcount).name;
            if(!desc.isEmpty())
            {
                item = desc;
            }
            else
            {
                item = "Out of Range";
            };
            if(longestItem.size() < item.size()) longestItem = item;
            this->controlListComboBox->addItem(item);
        };

        itemTotal = itemTotal + itemcount;
        items = item_3;
        itemSize = items.level.size();
        for(itemcount=0;itemcount<itemSize;itemcount++ )
        {
            QString item;
            QString desc = items.level.at(itemcount).name;
            if(!desc.isEmpty())
            {
                item = desc;
            }
            else
            {
                item = "Out of Range";
            };
            if(longestItem.size() < item.size()) longestItem = item;
            this->controlListComboBox->addItem(item);
        };

        itemTotal = itemTotal + itemcount;
        items = item_4;
        itemSize = items.level.size();
        for(itemcount=0;itemcount<itemSize;itemcount++ )
        {
            QString item;
            QString desc = items.level.at(itemcount).name;
            if(!desc.isEmpty())
            {
                item = desc;
            }
            else
            {
                item = "Out of Range";
            };
            if(longestItem.size() < item.size()) longestItem = item;
            this->controlListComboBox->addItem(item);
        };
    };
    itemTotal = itemTotal + itemcount;

    int maxWidth = QFontMetrics( this->font() ).width( longestItem );
    if(maxWidth < 30*ratio) { maxWidth = 30*ratio; };

    this->controlListComboBox->setFixedWidth(200*ratio);

    this->controlListComboBox->setFixedHeight(20*ratio);
    this->controlListComboBox->setEditable(false);
    this->controlListComboBox->setFrame(false);
    this->controlListComboBox->setMaxVisibleItems(itemTotal);
}

void customTargetListMenu::valueChanged(int index)
{
    QString valueHex = QString::number(index, 16).toUpper();
    if(valueHex.length() < 2) valueHex.prepend("0");

    SysxIO *sysxIO = SysxIO::Instance();
    MidiTable *midiTable = MidiTable::Instance();
    bool ok;
    if(midiTable->isData(this->area, hex1, hex2, hex3))
    {
        int maxRange = QString("7F").toInt(&ok, 16) + 1;
        int value = valueHex.toInt(&ok, 16);
        int dif = value/maxRange;
        QString valueHex1 = QString::number(dif, 16).toUpper();
        if(valueHex1.length() < 2) valueHex1.prepend("0");
        QString valueHex2 = QString::number(value - (dif * maxRange), 16).toUpper();
        if(valueHex2.length() < 2) valueHex2.prepend("0");

        sysxIO->setFileSource(this->area, hex1, hex2, hex3, valueHex1, valueHex2);
    }
    else
    {
        sysxIO->setFileSource(this->area, hex1, hex2, hex3, valueHex);
    };

    int value;
    //QString valueStr;

    value = sysxIO->getSourceValue(this->area, this->hex1, this->hex2, this->hex3);        // read target value as integer.
    valueHex = QString::number(value, 16).toUpper();                        // convert to hex qstring.
    if(valueHex.length() < 2) valueHex.prepend("0");
    //valueStr = midiTable->getValue(this->area, hex1, hex2, hex3, valueHex);  // lookup the target values

    value = valueHex.toInt(&ok, 16);
    QString hex4;
    QString hex5;
    int dif = value/128;
    hex4 = QString::number(dif, 16).toUpper();
    if(hex4.length() < 2) hex4.prepend("0");
    hex5 = QString::number(value - (dif * 128), 16).toUpper();
    if(hex5.length() < 2) hex5.prepend("0");

    //convert valueStr to 7-bit hex4, hex5
    Midi items;
    if(target){ items = midiTable->getMidiMap(this->area, hex1, hex2, hex3, hex4, hex5); }
    else{ items = midiTable->getMidiMap(this->area, hex1, hex2, hex3, valueHex); };
    this->hexMsb = items.desc;
    this->hexLsb = items.customdesc;

    emit updateTarget(hexMsb, hex2, hexLsb);                       // hexMsb & hexLsb are lookup address for label value
    //emit updateTarget(hexMsb, hex2, hexLsb);
    emit updateSignal();
}

void customTargetListMenu::dialogUpdateSignal(QString valueStr)
{
    bool ok;
    int index = valueStr.toInt(&ok, 16);
    this->controlListComboBox->setCurrentIndex(index);
}

void customTargetListMenu::comboUpdateSignal()
{
    SysxIO *sysxIO = SysxIO::Instance();
    int index = sysxIO->getSourceValue(this->area, this->hex1, this->hex2, this->hex3);
    this->controlListComboBox->setCurrentIndex(index);
}

