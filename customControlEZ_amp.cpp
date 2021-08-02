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

#include "customControlEZ_amp.h"
#include "MidiTable.h"
#include "SysxIO.h"
#include "Preferences.h"

customControlEZ_amp::customControlEZ_amp(QWidget *parent,
                                         QString hex1, QString hex2, QString hex3,
                                         QString background, QString direction, int lenght)
    : QWidget(parent)
{
    Q_UNUSED(direction);
    Q_UNUSED(lenght);
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    this->hex1 = hex1;
    this->hex2 = hex2;
    this->hex3 = hex3;
    this->hex4 = hex1;
    this->hex5 = hex2;

    this->use = background;

    this->label_1 = new customControlLabel(this);
    this->label_1->setAlignment(Qt::AlignCenter);

    this->label_2 = new customControlLabel(this);
    this->label_2->setAlignment(Qt::AlignCenter);

    this->label_3 = new customControlLabel(this);
    this->label_3->setAlignment(Qt::AlignCenter);

    this->label_4 = new customControlLabel(this);
    this->label_4->setAlignment(Qt::AlignCenter);

    int x = hex3.toInt(&ok, 16);
    if (this->use.contains("Preamp_"))
    {
        this->hex_1 = QString::number(x+5, 16).toUpper();
        this->hex_2 = QString::number(x+4, 16).toUpper();
        this->label_1->setText(tr("SOFT"));
        this->label_2->setText(tr("HARD"));
        this->label_3->setText(tr("BACKING"));
        this->label_4->setText(tr("SOLO"));
    }
    else if (this->use == ("Preamp"))
    {
        this->hex_1 = "52";
        this->hex_2 = "53";
        this->label_1->setText(tr("SOFT"));
        this->label_2->setText(tr("HARD"));
        this->label_3->setText(tr("BACKING"));
        this->label_4->setText(tr("SOLO"));
    }
    else if (this->use.contains("Speaker_"))
    {
        this->hex_1 = QString::number(x, 16).toUpper();
        this->hex_2 = QString::number(x+11, 16).toUpper();
        this->label_1->setText(tr("SMALL"));
        this->label_2->setText(tr("LARGE"));
        this->label_3->setText(tr("   LIVE"));
        this->label_4->setText(tr("RECORDING"));
    }
    else if (this->use == ("Speaker"))
    {
        this->hex_1 = "61";
        this->hex_2 = "60";
        this->label_1->setText(tr("SMALL"));
        this->label_2->setText(tr("LARGE"));
        this->label_3->setText(tr("   LIVE"));
        this->label_4->setText(tr("RECORDING"));
    }
    else if (this->use == "Ambience")
    {
        this->hex_1 = QString::number(x+5, 16).toUpper();
        this->hex_2 = QString::number(x, 16).toUpper();
        this->label_1->setText(tr("SMALL"));
        this->label_2->setText(tr("LARGE"));
        this->label_3->setText(tr("DRY"));
        this->label_4->setText(tr("WET"));
    }
    else
    {
        this->hex_1 = QString::number(x+3, 16).toUpper();
        this->hex_2 = QString::number(x+2, 16).toUpper();
        this->label_1->setText(tr("SOFT"));
        this->label_2->setText(tr("HARD"));
        this->label_3->setText(tr("BACKING"));
        this->label_4->setText(tr("SOLO"));
    };

    //this->label_4->drawRotatedText(90);

    if(hex_1.length() < 2) hex_1.prepend("0");
    if(hex_2.length() < 2) hex_2.prepend("0");

    this->v_slider = new QSlider(Qt::Vertical, this);
    this->v_slider->setMinimum(0);
    this->v_slider->setMaximum(100);
    this->v_slider->setMinimumHeight(160*ratio);
    this->v_slider->setMinimumWidth(30*ratio);

    this->h_slider = new QSlider(Qt::Horizontal, this);
    this->h_slider->setMinimum(0);
    this->h_slider->setMaximum(100);
    this->h_slider->setMinimumWidth(160*ratio);
    this->h_slider->setMinimumHeight(30*ratio);

    QHBoxLayout *h_slider_Layout = new QHBoxLayout;
    h_slider_Layout->setMargin(0);
    h_slider_Layout->addSpacing(100*ratio);
    h_slider_Layout->addWidget(this->h_slider, 0, Qt::AlignCenter);
    h_slider_Layout->addStretch(0);

    this->frame = new customEZ_amp(this);
    this->frame->setMinimumSize(QSize(200*ratio, 200*ratio));

    QHBoxLayout *knobLayout = new QHBoxLayout;
    knobLayout->setMargin(0);
    knobLayout->setSpacing(10*ratio);
    knobLayout->addWidget(this->v_slider, 0, Qt::AlignCenter);
    knobLayout->addWidget(this->label_3, 0, Qt::AlignCenter);
    knobLayout->setMargin(0);
    knobLayout->addWidget(this->frame, 0, Qt::AlignCenter);
    knobLayout->addWidget(this->label_4, 0, Qt::AlignCenter);
    knobLayout->addStretch(0);

    QVBoxLayout *frameLayout = new QVBoxLayout;
    frameLayout->setMargin(0);
    frameLayout->setSpacing(0);
    frameLayout->addWidget(this->label_2, 0, Qt::AlignCenter);
    frameLayout->addLayout(knobLayout);
    frameLayout->addWidget(this->label_1, 0, Qt::AlignCenter);
    frameLayout->setSpacing(10*ratio);
    frameLayout->addLayout(h_slider_Layout);

    this->setLayout(frameLayout);

    QObject::connect(this->parent(), SIGNAL( dialogUpdateSignal() ),
                     this, SLOT( dialogUpdateSignal() ));

    //QObject::connect(this, SIGNAL( updateSignal() ),
    // this->parent(), SIGNAL( updateSignal() ));

    QObject::connect(this->v_slider, SIGNAL(valueChanged(int)),
                     this, SLOT(y_axisChanged(int)));

    QObject::connect(this->h_slider, SIGNAL(valueChanged(int)),
                     this, SLOT(x_axisChanged(int)));

    QObject::connect(this, SIGNAL( graphUpdateSignal(QString, QString) ),
                     this->frame, SLOT( updateSlot(QString, QString) ));

    QObject::connect(this->frame, SIGNAL(y_axisChanged(int)),
                     this, SLOT(y_axisChanged(int) ));

    QObject::connect(this->frame, SIGNAL(x_axisChanged(int)),
                     this, SLOT(x_axisChanged(int) ));
}

void customControlEZ_amp::paintEvent(QPaintEvent *)
{
    /*DRAWS RED BACKGROUND FOR DEBUGGING PURPOSE */
    /*QPixmap image(":images/dragbar.png");

    QRectF target(0.0, 0.0, this->width(), this->height());
    QRectF source(0.0, 0.0, this->width(), this->height());

    QPainter painter(this);
    painter.drawPixmap(target, image, source);*/
}

void customControlEZ_amp::dialogUpdateSignal()
{
    SysxIO *sysxIO = SysxIO::Instance();
    unsigned int value;

    value = sysxIO->getSourceValue("Structure", this->hex1, this->hex2, this->hex_2);
    if(use.contains("Speaker")){ value = value*5; };
    if(use=="Preamp"){ value=value*5; };
    if(use=="Speaker"){ value=value*2; };
    this->v_slider->setValue(value);
    QString data_1 = QString::number(value, 16).toUpper();
    if(data_1.length() < 2) data_1.prepend("0");

    value = sysxIO->getSourceValue("Structure", this->hex4, this->hex5, this->hex_1);
    if(use=="Ambience"){ value=value*100/70; };
    if(use=="Preamp"){ value=value-20; };
    this->h_slider->setValue(value);
    QString data_2 = QString::number(value, 16).toUpper();
    if(data_2.length() < 2) data_2.prepend("0");

    emit graphUpdateSignal( data_1, data_2);
}

void customControlEZ_amp::y_axisChanged(int iy_axis)
{
    if(iy_axis != y_data)
    {
        y_data = iy_axis;
        this->v_slider->setValue(y_data);
        QString data_1 = QString::number(y_data, 16).toUpper();
        if(data_1.length() < 2) data_1.prepend("0");
        QString data_2 = QString::number(x_data, 16).toUpper();
        if(data_2.length() < 2) data_2.prepend("0");
        emit graphUpdateSignal( data_1, data_2);
        if(this->window()->isActiveWindow()) { dataChanged(); };
    };
}

void customControlEZ_amp::x_axisChanged(int ix_axis)
{
    if(ix_axis != x_data)
    {
        x_data = ix_axis;
        this->h_slider->setValue(x_data);
        QString data_1 = QString::number(y_data, 16).toUpper();
        if(data_1.length() < 2) data_1.prepend("0");
        QString data_2 = QString::number(x_data, 16).toUpper();
        if(data_2.length() < 2) data_2.prepend("0");
        emit graphUpdateSignal( data_1, data_2);
        if(this->window()->isActiveWindow()) { dataChanged(); };
    };
}

void customControlEZ_amp::dataChanged()
{
    SysxIO *sysxIO = SysxIO::Instance();
    bool ok;
    if(this->use.contains("_") && this->frame->underMouse() ) // if from Preamp_A or _B or Speaker_A or _B
    {  // Check if the effect enable and custom setting has not already been enabled, if not then do so.
        QString hex_m = "30", hex_n = "31", hex_o = "15";
        QList<QString> hex_p; hex_p.append("01"); hex_p.append("15");
        if(this->use == "Preamp_A") { hex_m = "50"; hex_n = "51"; hex_o = "19"; hex_p.clear(); hex_p.append("01"); hex_p.append("19"); };
        if(this->use == "Preamp_B") { hex_m = "00"; hex_n = "01"; hex_o = "19"; hex_p.clear(); hex_p.append("01"); hex_p.append("19"); };
        if(this->use == "Speaker_A"){ hex_m = "5D"; hex_n = "5D"; hex_o = "09"; hex_p.clear(); hex_p.append("09"); };
        if(this->use == "Speaker_B"){ hex_m = "0D"; hex_n = "0D"; hex_o = "09"; hex_p.clear(); hex_p.append("09"); };
        int enabled = sysxIO->getSourceValue("Structure", this->hex1, this->hex2, hex_m);
        int custom = sysxIO->getSourceValue("Structure", this->hex1, this->hex2, hex_n);
        if((enabled >! 0 || custom != hex_o.toInt(&ok, 16))) { sysxIO->setFileSource("Structure", this->hex1, this->hex2, hex_m, hex_p); };
    };
    int x = hex3.toInt(&ok, 16);
    QString hex_addr = QString::number(x, 16).toUpper();
    QString hex_addr2 = QString::number(x+10, 16).toUpper();;
    QString hex;
    QList<QString> hexString1;
    QList<QString> hexString2;
    QList<QString> hexString3;

    if(use.contains("Speaker_"))
    {
        hex = QString::number(x_data, 16).toUpper();
        if (hex.size() < 2) hex.prepend("0");  // mic level 64~00
        hexString1.append(hex);

        hex = QString::number(100-x_data, 16).toUpper();
        if (hex.size() < 2) hex.prepend("0");  // direct mix 00~64
        hexString1.append(hex);

        int data = ((y_data)/10);
        hex = QString::number(data, 16).toUpper();
        if (hex.size() < 2) hex.prepend("0");  // spkr size 00~0A
        hexString2.append(hex);

        data = ((y_data)/5);
        hex = QString::number(data, 16).toUpper();
        if (hex.size() < 2) hex.prepend("0");  // spkr color low 00~14
        hexString2.append(hex);

        data = ((y_data)/8)+7;
        hex = QString::number(data, 16).toUpper();
        if (hex.size() < 2) hex.prepend("0");  // spkr color high 00~14
        hexString2.append(hex);

        data = ((y_data)/33);
        hex = QString::number(data, 16).toUpper();
        if (hex.size() < 2) hex.prepend("0");  // spkr number 00~03
        hexString2.append(hex);

        if(this->toggle == 1)
        {
            sysxIO->setFileSource("Structure", this->hex1, this->hex2, hex_addr, hexString1);
            this->toggle = 2;
        } else {
            sysxIO->setFileSource("Structure", this->hex1, this->hex2, hex_addr2, hexString2);
            this->toggle = 1;
        };

    }
    else if(this->use=="Preamp")
    {
        hex = QString::number(x_data+20, 16).toUpper();
        if (hex.size() < 2) hex.prepend("0");  // preamp drive 20~120 int
        hexString1.append(hex);
        hex = QString::number(y_data/5, 16).toUpper();
        if (hex.size() < 2) hex.prepend("0");  // t-comp
        hexString1.append(hex);

        int data = (y_data); if(data >50){ data=50; };
        data = (data/4)+8;
        if(data>20){ data=20; };
        hex = QString::number(data, 16).toUpper();
        if (hex.size() < 2) hex.prepend("0");  // EQ lo mid gain 08~20 int
        hexString2.append(hex);

        data = (100-y_data); if(data<50){ data=50; };
        data=data/12; if(data>12) { data=12; };
        data=data+20;
        hex = QString::number(data, 16).toUpper();
        if (hex.size() < 2) hex.prepend("0");  // FX1 EQ lo mid gain 20~32 int
        hexString3.append(hex);

        if(this->toggle == 1)
        {
            sysxIO->setFileSource("Structure", "00", "00", "52", hexString1); //preamp drive + tcomp
            this->toggle = 2;
        }
        else if(this->toggle == 2)
        {
            sysxIO->setFileSource("Structure", "01", "00", "35", hexString2); //EQ lo mid gain
            this->toggle = 3;
        }
        else
        {
            sysxIO->setFileSource("Structure", "02", "00", "00", hexString3); //FX1 EQ lo mid gain
            this->toggle = 1;
        };
    }
    else if(this->use=="Speaker")
    {

        int data = ((y_data*9)/100);
        if(data>8){ data=8; };
        if(data<1){ data=1; };
        hex = QString::number(data, 16).toUpper();
        if (hex.size() < 2) hex.prepend("0");
        hexString1.append(hex);                   //sp cab type

        data = (y_data/25);
        hex = QString::number(data, 16).toUpper(); //mic type
        if (hex.size() < 2) hex.prepend("0");
        hexString1.append(hex);

        data = (y_data/50);
        if(data>1) { data=1; };
        hex = QString::number(data, 16).toUpper(); //mic dis
        if (hex.size() < 2) hex.prepend("0");
        hexString1.append(hex);

        data = (y_data*11/100);
        if(data>10) { data=10; };
        hex = QString::number(data, 16).toUpper(); //mic pos
        if (hex.size() < 2) hex.prepend("0");
        hexString1.append(hex);

        hex = QString::number(x_data, 16).toUpper(); //mic level
        if (hex.size() < 2) hex.prepend("0");
        hexString1.append(hex);

        hex = QString::number(100-x_data, 16).toUpper(); //direct mix
        if (hex.size() < 2) hex.prepend("0");
        hexString1.append(hex);

        sysxIO->setFileSource("Structure", "00", "00", "5D", hexString1);
    }
    else if(use == "Ambience")
    {
        hex = QString::number(x_data*70/100, 16).toUpper();
        if (hex.size() < 2) hex.prepend("0");               // reverb/chorus/delay level 0~70 int
        hexString1.append(hex);

        int data = ((y_data*99/100));
        hex = QString::number(data, 16).toUpper();
        if (hex.size() < 2) hex.prepend("0");               // reverb time 00~63
        hexString2.append(hex);

        hex = QString::number(y_data/15, 16).toUpper();
        if (hex.size() < 2) hex.prepend("0");               // delay time
        hexString3.append(hex);
        hex = QString::number(y_data, 16).toUpper();
        if (hex.size() < 2) hex.prepend("0");               // delay time
        hexString3.append(hex);

        if(this->toggle == 1)
        {
            sysxIO->setFileSource("Structure", "06", "00", "07", hexString1); //chorus level
            this->toggle = 2;
        }
        else if(this->toggle == 2)
        {
            sysxIO->setFileSource("Structure", "06", "00", "18", hexString1); //reverb level
            this->toggle = 3;
        }
        else if(this->toggle == 3)
        {
            sysxIO->setFileSource("Structure", "05", "00", "66", hexString1); //delay level
            this->toggle = 4;
        }
        else if(this->toggle == 4)
        {
            sysxIO->setFileSource("Structure", "06", "00", "02", hexString2); //chorus rate
            this->toggle = 5;
        }
        else if(this->toggle == 5)
        {
            sysxIO->setFileSource("Structure", "06", "00", "12", hexString2); //reverb time
            this->toggle = 6;
        }
        else
        {
            sysxIO->setFileSource("Structure", "05", "00", "62", hexString3); //delay time
            this->toggle = 1;
        };
    }
    else
    {

        int data = ((y_data*4)/10)+30;
        hex = QString::number(data, 16).toUpper();
        if (hex.size() < 2) hex.prepend("0");
        hexString1.append(hex);

        if(this->use.contains("Preamp_")){ hexString1.append("32"); hexString1.append("32"); };

        data = ((y_data*3)/10)+50;
        hex = QString::number(data, 16).toUpper();
        if (hex.size() < 2) hex.prepend("0");
        hexString1.append(hex);

        hex = QString::number(y_data, 16).toUpper();
        if (hex.size() < 2) hex.prepend("0");
        hexString1.append(hex);

        hex = QString::number(x_data, 16).toUpper();
        if (hex.size() < 2) hex.prepend("0");
        hexString1.append(hex);

        sysxIO->setFileSource("Structure", this->hex1, this->hex2, hex_addr, hexString1);
    };
    emit updateSignal();
}

