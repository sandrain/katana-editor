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

#include "customKnobTarget.h"
#include "MidiTable.h"
#include "SysxIO.h"
#include "Preferences.h"


customKnobTarget::customKnobTarget(QWidget *parent, 
                                   QString hex1, QString hex2, QString hex3,
                                   QString hexMsb, QString hexLsb, QString background, QString root)
    : QWidget(parent)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    target = false;
    if(hex1.toInt(&ok, 16)<11){ target = true; };

    this->hex1 = hex1;
    this->hex2 = hex2;
    this->hex3 = hex3;
    this->hexMsb = hexMsb;
    this->hexLsb = hexLsb;
    this->background = background;
    this->area = root;

    MidiTable *midiTable = MidiTable::Instance();

    QPoint bgPos = QPoint((5*ratio)-(6*ratio), (4*ratio)-(7*ratio)); // Correction needed y-3.
    QPoint knobPos = QPoint(5*ratio, 4*ratio); // Correction needed y+1 x-1.
    QLabel *newBackGround = new QLabel(this);

    if (this->background == "target")
    {
        if(target){
            this->range = midiTable->getRange("Structure", "08", "00", "31");
            this->rangeMin = midiTable->getRangeMinimum("Structure", "08", "00", "31"); }
        else{
            this->range = midiTable->getRange("Structure", hex1, "00", hex3);
            this->rangeMin = midiTable->getRangeMinimum("Structure", hex1, "00", hex3); };
    }
    else
    {
        this->range = midiTable->getRange("Structure", hexMsb, hex2, hexLsb);
        this->rangeMin = midiTable->getRangeMinimum("Structure", hexMsb, hex2, hexLsb);
    };

    newBackGround->setPixmap(QPixmap(":/images/knobbgn.png").scaled(49*ratio,50*ratio, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    newBackGround->move(bgPos);

    QString imagePath(":/images/knob.png");
    unsigned int imageRange = 100;
    this->knob = new customTargetDial(0, rangeMin, range, 1, 10, knobPos, this, hex1, hex2, hex3, imagePath, imageRange, background, root);
    this->setFixedSize(newBackGround->pixmap()->size() - QSize(0, 4)); // Correction needed h-4.


    QObject::connect(this, SIGNAL( updateSignal() ),
                     this->parent(), SIGNAL( updateSignal() ));

    QObject::connect(this, SIGNAL( updateDisplayTarget(QString) ),
                     this->parent(), SIGNAL( updateDisplayTarget(QString) ));

    QObject::connect(this, SIGNAL( updateDisplayMin(QString) ),
                     this->parent(), SIGNAL( updateDisplayMin(QString) ));

    QObject::connect(this, SIGNAL( updateDisplayMax(QString) ),
                     this->parent(), SIGNAL( updateDisplayMax(QString) ));

    QObject::connect(this, SIGNAL( updateTarget(QString, QString, QString) ),
                     this->parent(), SIGNAL( updateTarget(QString, QString, QString) ));

    QObject::connect(this, SIGNAL( updateHex(QString, QString, QString) ),
                     this, SLOT( knobSignal(QString, QString, QString) ));

    QObject::connect(this->parent(), SIGNAL( updateHex(QString, QString, QString) ),
                     this, SIGNAL( updateHex(QString, QString, QString) ));

}

void customKnobTarget::paintEvent(QPaintEvent *)
{
    /*DRAWS RED BACKGROUND FOR DEBUGGING PURPOSE */
    /*QPixmap image(":images/dragbar.png");

    QRectF target(0.0, 0.0, this->width(), this->height());
    QRectF source(0.0, 0.0, this->width(), this->height());

    QPainter painter(this);
    painter.drawPixmap(target, image, source);*/
}

void customKnobTarget::knobSignal(QString hexMsb, QString hex2, QString hexLsb)
{
    Q_UNUSED(hex2);
    if (background != "target")
    {
        this->hexMsb = hexMsb;
        this->hexLsb = hexLsb;
    };
}

void customKnobTarget::setValue(int value)
{
    if(this->background=="Target" || target)
    { this->knob->setValue(value); };        // update regular Assigns and pedal Assign Target knob only
}

void customKnobTarget::valueChanged(int value, QString hex1, QString hex2, QString hex3)
{
    if (background != "target")
    {
        this->knobSignal(hexMsb, hex2, hexLsb);
        this->hexMsb = hexMsb;
        this->hex2 = hex2;
        this->hexLsb = hexLsb;
    };

    /* QMessageBox *msgBox = new QMessageBox();
            msgBox->setWindowTitle("data log");
            msgBox->setIcon(QMessageBox::Information);
            msgBox->setText("Area="+area+"<br>Hex1="+hex1+"<br>Hex2="+hex2+"<br>Hex3="+hex3);
            msgBox->setStandardButtons(QMessageBox::Ok);
            msgBox->exec();*/

    MidiTable *midiTable = MidiTable::Instance();

    QString valueHex = QString::number(value, 16).toUpper();
    if(valueHex.length() < 2) valueHex.prepend("0");

    SysxIO *sysxIO = SysxIO::Instance(); bool ok;

    if(midiTable->isData(this->area, hex1, hex2, hex3))
    {;
        int value = valueHex.toInt(&ok, 16);
        int dif = value/128;
        QString valueHex1 = QString::number(dif, 16).toUpper();
        if(valueHex1.length() < 2) valueHex1.prepend("0");
        QString valueHex2 = QString::number(value - (dif * 128), 16).toUpper();
        if(valueHex2.length() < 2) valueHex2.prepend("0");
        sysxIO->setFileSource(this->area, hex1, hex2, hex3, valueHex1, valueHex2);
    }
    else
    {
        sysxIO->setFileSource(this->area, hex1, hex2, hex3, valueHex);
    };

    QString valueStr;
    if (this->background == "target"){
        if(target){valueStr = midiTable->getValue("Structure", "08", "00", "31", valueHex); }
        else{valueStr = midiTable->getValue("Structure", hex1, "00", hex3, valueHex); };
        emit updateDisplayTarget(valueStr);                                                       // updates display values
    }
    else if (this->background == "min")
    {
        valueStr = midiTable->getValue("Structure", hexMsb, hex2, hexLsb, valueHex);
        emit updateDisplayMin(valueStr);
    } else if (this->background == "max")
    {
        valueStr = midiTable->getValue("Structure", hexMsb, hex2, hexLsb, valueHex);
        emit updateDisplayMax(valueStr);
    };                                                  // updates display values
    emit updateSignal();

    if (this->background == "target")   // get the currently selected target value & set min/max address
    {
        value = sysxIO->getSourceValue(this->area, this->hex1, this->hex2, this->hex3);        // read target value as integer.
        valueHex = QString::number(value, 16).toUpper();                        // convert to hex qstring.
        if(valueHex.length() < 2) valueHex.prepend("0");
        if(target){ valueStr = midiTable->getValue("Structure", "08", "00", "31", valueHex); }  // lookup the target values
        else{ valueStr = midiTable->getValue("Structure", hex1, "00", hex3, valueHex); };

        value = valueHex.toInt(&ok, 16);                                              //convert valueStr to 7bit hex4, hex5
        int dif = value/128;
        QString hex4 = QString::number(dif, 16).toUpper();
        if(hex4.length() < 2) hex4.prepend("0");
        QString hex5 = QString::number(value - (dif * 128), 16).toUpper();
        if(hex5.length() < 2) hex5.prepend("0");

        Midi items;
        if(target){ items = midiTable->getMidiMap("Structure", "08", "00", "31", hex4, hex5); }
        else{ items = midiTable->getMidiMap("Structure", hex1, "00", hex3, valueHex); };
        this->hexMsb = items.desc;
        this->hexLsb = items.customdesc;

        emit updateTarget(hexMsb, hex2, hexLsb);                       // hexMsb & hexLsb are lookup address for label value
        emit updateTarget(hexMsb, hex2, hexLsb);
    };                                                             // updates on knob value change
}

