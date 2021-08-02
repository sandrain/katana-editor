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

#include "customControlMidiTable.h"
#include "MidiTable.h"
#include "SysxIO.h"
#include "Preferences.h"

customControlMidiTable::customControlMidiTable(QWidget *parent,
                                               QString hex1, QString hex2, QString hex3,
                                               QString direction, QString list)
    : QWidget(parent)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);
    const double fratio = preferences->getPreferences("Window", "Font", "ratio").toDouble(&ok);
    QFont Sfont( "Arial", 9*fratio, QFont::Bold);

    this->hex1 = hex1;
    this->hex2 = hex2;
    this->hex3 = hex3;
    this->text = direction;
    this->list = list;

    if(list == "label")
    {
        this->label = new customControlLabel(this);
        this->label->setAlignment(Qt::AlignCenter);
        this->label->setUpperCase(true);

        this->display = new QLineEdit();
        this->display->setObjectName("editdisplay");
        this->display->setFixedWidth(800*ratio);
        this->display->setFixedHeight(16*ratio);
        this->display->setAlignment(Qt::AlignCenter);
        this->display->setFont(Sfont);

        QVBoxLayout *labelLayout = new QVBoxLayout;
        labelLayout->setMargin(0);
        labelLayout->setSpacing(0);
        labelLayout->addStretch(0);
        labelLayout->addWidget(this->label, 0, Qt::AlignCenter);
        labelLayout->addWidget(this->display, 0, Qt::AlignCenter);
        this->setLayout(labelLayout);
        this->setFixedHeight(28*ratio);
    }
    else
    {
        setComboBox();
    };
}

void customControlMidiTable::paintEvent(QPaintEvent *)
{
    /*DRAWS RED BACKGROUND FOR DeBugGING PURPOSE */
    /*QPixmap image(":images/dragbar.png");

        QRectF target(0.0, 0.0, this->width(), this->height());
        QRectF source(0.0, 0.0, this->width(), this->height());

        QPainter painter(this);
        painter.drawPixmap(target, image, source);*/
}

void customControlMidiTable::setComboBox()
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);
    const double fratio = preferences->getPreferences("Window", "Font", "ratio").toDouble(&ok);
    QFont Sfont( "Arial", 12*fratio, QFont::Bold);
    QFont Lfont( "Arial", 9*fratio, QFont::Bold);
    comboList = list.split(" ");
    comboList.removeLast();

    this->label = new customControlLabel(this);
    this->controlMidiComboBox = new customComboBox(this);
    this->controlMidiComboBox->setObjectName("largecombo");
    this->controlMidiComboBox->setFixedWidth(80*ratio);
    this->controlMidiComboBox->setFixedHeight(25*ratio);
    this->controlMidiComboBox->setEditable(false);
    this->controlMidiComboBox->setFrame(false);
    this->controlMidiComboBox->setMaxVisibleItems(200);
    this->controlMidiComboBox->addItems(comboList);
    this->controlMidiComboBox->setFont(Sfont);

    this->label->setUpperCase(true);
    this->label->setFont(Lfont);
    this->label->setText(this->text);
    this->label->setAlignment(Qt::AlignLeft);

    QVBoxLayout *comboLayout = new QVBoxLayout;
    comboLayout->setMargin(0);
    comboLayout->setSpacing(0);
    comboLayout->addStretch(0);
    comboLayout->addWidget(this->label, 0, Qt::AlignCenter);
    comboLayout->addWidget(this->controlMidiComboBox, 0, Qt::AlignCenter);
    this->setLayout(comboLayout);

    QObject::connect(this->parent()->parent(), SIGNAL( dialogUpdateSignal() ), this, SLOT( dialogUpdateSignal() ));

    QObject::connect(this, SIGNAL( updateSignal() ), this->parent(), SIGNAL( updateSignal() ));

    QObject::connect(this->controlMidiComboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(currentIndexChanged(int)));

    QObject::connect(this->controlMidiComboBox, SIGNAL(highlighted(int)), this, SIGNAL(currentIndexChanged(int)));

    if(text != "KATANA Patch") {
      QObject::connect(this->controlMidiComboBox, SIGNAL(currentIndexChanged(int)), this->parent(), SLOT(changedIndex(int)));

      QObject::connect(this->controlMidiComboBox, SIGNAL(highlighted(int)), this->parent(), SLOT(changedIndex(int)));
        } else {
      QObject::connect(this->controlMidiComboBox, SIGNAL(currentIndexChanged(int)), this->parent(), SLOT(valueChanged(int)));

      QObject::connect(this->controlMidiComboBox, SIGNAL(highlighted(int)), this->parent(), SLOT(valueChanged(int)));
     };
}

void customControlMidiTable::valueChanged(int index)
{
    Q_UNUSED(index);
}

void customControlMidiTable::dialogUpdateSignal()
{
    if(text == "KATANA Patch")
    {
        SysxIO *sysxIO = SysxIO::Instance();
        int index = sysxIO->getSourceValue("MidiT", hex1, hex2, hex3);
        this->controlMidiComboBox->setCurrentIndex(index);
        this->valueChanged(index);
    };
}






