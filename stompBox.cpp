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

#include "stompBox.h"
#include "MidiTable.h"
#include "SysxIO.h"
#include "globalVariables.h"
#include "floorBoardDisplay.h"
#include "floorBoard.h"
#include "Preferences.h"

stompBox::stompBox(QWidget *parent, unsigned int id, QString imagePath, QPoint stompPos)
    : QWidget(parent)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    this->id = id;
    this->imagePath = imagePath;
    this->stompSize = QPixmap(":/images/stompbg.png").size()*ratio;
    this->stompPos = stompPos;
    this->setFixedSize(stompSize);
    this->editDialog = new editWindow();
    this->setWhatsThis(tr("StompBox effect<br>most can be dragged to a new chain position by holding down the mouse button while dragging the stompbox,<br>release the mouse button over the new location.<br>a double mouse click will open the effect edit page."));

    this->pathSwitch = new customButton(tr(""), false, QPoint(60*ratio, 60*ratio), this, ":/images/pathswitch.png");
    this->pathSwitch->hide();

    QObject::connect(this, SIGNAL( valueChanged(QString, QString, QString) ), this->parent(), SIGNAL( valueChanged(QString, QString, QString) ));

    QObject::connect(this->editDetails(), SIGNAL( valueUpdate(QString, QString, QString) ), this->parent(), SIGNAL( valueChanged(QString, QString, QString) ));

    QObject::connect(this->parent(), SIGNAL( updateStompOffset(signed int) ), this, SLOT( updatePos(signed int) ));

    QObject::connect(this->parent(), SIGNAL( updateSignal() ), this, SLOT( updateSignal() ));

    QObject::connect( this->editDetails()->swap_Button, SIGNAL(mouseReleased()),this->parent(), SIGNAL( updateSignal() ));

    QObject::connect(this->editDialog, SIGNAL( updateSignal() ), this, SLOT( updateSignal() ));

    QObject::connect(this, SIGNAL( dialogUpdateSignal() ), this->editDialog, SIGNAL( dialogUpdateSignal() ));

    QObject::connect(this->parent(), SIGNAL( updateSignal() ), this->editDialog, SIGNAL( dialogUpdateSignal() ));

    QObject::connect(this, SIGNAL( setEditDialog( editWindow*) ), this->parent(), SLOT( setEditDialog(editWindow*) ));

    QObject::connect(this->pathSwitch, SIGNAL( valueChanged(bool)), this, SLOT( pathSwitchSignal(bool) ));
    
    QObject::connect(this->parent(), SIGNAL( pathUpdateSignal() ), this, SIGNAL( pathUpdateSignal() ));

    QObject::connect(this, SIGNAL( pathUpdateSignal() ), this, SLOT( updateStompPath() ));

    QObject::connect(this, SIGNAL( updateStompBoxes() ), this->parent(), SLOT( updateStompBoxes() ));

    QObject::connect(this, SIGNAL( switchSignal() ), this->parent(), SIGNAL( updateSignal() ));

    QObject::connect(this->parent(), SIGNAL(preamp1_buttonSignal(bool)), this, SLOT(preamp1_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(preamp1_statusSignal(bool)), this->parent(), SIGNAL(preamp1_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(preamp1_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(distortion_buttonSignal(bool)), this, SLOT(distortion_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(distortion_statusSignal(bool)), this->parent(), SIGNAL(distortion_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(distortion_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(ns1_buttonSignal(bool)), this, SLOT(ns1_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(ns1_statusSignal(bool)), this->parent(), SIGNAL(ns1_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(ns1_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(fx1_buttonSignal(bool)), this, SLOT(fx1_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(fx1_statusSignal(bool)), this->parent(), SIGNAL(fx1_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(fx1_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(fx2_buttonSignal(bool)), this, SLOT(fx2_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(fx2_statusSignal(bool)), this->parent(), SIGNAL(fx2_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(fx2_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(reverb_buttonSignal(bool)), this, SLOT(reverb_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(reverb_statusSignal(bool)), this->parent(), SIGNAL(reverb_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(reverb_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(delay_buttonSignal(bool)), this, SLOT(delay_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(delay_statusSignal(bool)), this->parent(), SIGNAL(delay_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(delay_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(sendreturn_buttonSignal(bool)), this, SLOT(sendreturn_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(sendreturn_statusSignal(bool)), this->parent(), SIGNAL(sendreturn_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(sendreturn_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));
    
    QObject::connect(this->parent(), SIGNAL(eq_buttonSignal(bool)), this, SLOT(eq_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(eq_statusSignal(bool)), this->parent(), SIGNAL(eq_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(eq_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(ce_buttonSignal(bool)), this, SLOT(ce_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(ce_statusSignal(bool)), this->parent(), SIGNAL(ce_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(ce_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(pedal_buttonSignal(bool)), this, SLOT(pedal_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(pedal_statusSignal(bool)), this->parent(), SIGNAL(pedal_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(pedal_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(fv_buttonSignal(bool)), this, SLOT(fv_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(fv_statusSignal(bool)), this->parent(), SIGNAL(fv_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(fv_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));
}

void stompBox::paintEvent(QPaintEvent *)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);
    setImage(imagePath);
    QPixmap image(imagePath);
    QRectF target(0.0, 0.0, image.width()*ratio, image.height()*ratio);
    QRectF source(0.0, 0.0, image.width(), image.height());
    this->image = image;

    QPainter painter(this);
    painter.drawPixmap(target, image, source);
}

editWindow* stompBox::editDetails()
{
    return this->editDialog;
}

void stompBox::mousePressEvent(QMouseEvent *event)
{
    emitValueChanged(this->hex1, this->hex2, "00", "void");

    if (event->button() == Qt::LeftButton)
    {
        this->dragStartPosition = event->pos();
    }
    else if (event->button() == Qt::RightButton)
    {
        emit switchSignal();
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void stompBox::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    emit switchSignal();
    this->editDialog->setWindow(this->fxName);
    emit setEditDialog(this->editDialog);
}

void stompBox::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        QPoint mousePoint = event->pos();
        QRect stompRect = this->rect();

        if ( stompRect.contains(mousePoint) )
        {

            QByteArray itemData;
            QDataStream dataStream(&itemData, QIODevice::WriteOnly);
            dataStream << id << stompPos << stompSize << event->pos() - rect().topLeft();

            QMimeData *mimeData = new QMimeData;
            mimeData->setData("application/x-stompbox", itemData);
            mimeData->setText(this->objectName());

            QDrag *drag = new QDrag(this);
            drag->setMimeData(mimeData);
            drag->setHotSpot(event->pos() - rect().topLeft());

            QPixmap screen = QWidget::grab();
            screen.setMask(image.mask());

            QRectF source(0, 0, stompSize.width(), stompSize.height());
            QRectF target(0, 0, stompSize.width(), stompSize.height());

            QPixmap buffer = image;
            buffer = buffer.scaled(stompSize.width(), stompSize.height());
            QPainter painter(&buffer);
            painter.drawPixmap(target, screen, source);
            painter.end();

            drag->setPixmap(buffer);

            hide();

            if (drag->start(Qt::MoveAction) != Qt::MoveAction)
            {
                event->ignore();
                show();
            };

            if(drag->source() != drag->target())
            {
                event->ignore();
                show();
            };
            if (this->id < 20) updateStompPath();
        };
    };
}

void stompBox::preamp1_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 2)
    {
        setDisplayToFxName();
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void stompBox::distortion_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 15)
     {
        setDisplayToFxName();
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void stompBox::ns1_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 13)
    {
        setDisplayToFxName();
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void stompBox::fx1_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 5)
    {
        setDisplayToFxName();
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void stompBox::fx2_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 6)
    {
        setDisplayToFxName();
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void stompBox::reverb_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 9)
    {
        setDisplayToFxName();
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void stompBox::delay_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 7)
    {
        setDisplayToFxName();
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void stompBox::sendreturn_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 1)
    {
        setDisplayToFxName();
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void stompBox::eq_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 4)
    {
        setDisplayToFxName();
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void stompBox::ce_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 8)
    {
        setDisplayToFxName();
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void stompBox::pedal_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 11)
    {
        setDisplayToFxName();
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void stompBox::fv_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 12)
    {
        setDisplayToFxName();
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void stompBox::pathSwitchSignal(bool value)
{
    Q_UNUSED(value);
}

void stompBox::setPos(QPoint newPos)
{
    this->move(newPos);
    this->stompPos = newPos;
    updateStompPath();
}

void stompBox::updatePos(signed int offsetDif)
{
    this->stompPos = this->pos();
    QPoint newPos = stompPos + QPoint(offsetDif, 0);
    this->move(newPos);
    this->stompPos = newPos;
}

void stompBox::setImage(QString imagePath)
{
    this->imagePath = imagePath;
    //this->update();
}

void stompBox::setSize(QSize newSize)
{
    this->stompSize = newSize;
    this->setFixedSize(stompSize);
}

void stompBox::setId(unsigned int id)
{
    this->id = id;
}

unsigned int stompBox::getId()
{
    return this->id;
}

void stompBox::setLSB(QString hex1, QString hex2)
{
    this->editDialog->setLSB(hex1, hex2);
}

void stompBox::setComboBox(QString hex1, QString hex2, QString hex3, QRect geometry)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);
    const double fratio = preferences->getPreferences("Window", "Font", "ratio").toDouble(&ok);

    this->combo_hex1 = hex1;
    this->combo_hex2 = hex2;
    this->combo_hex3 = hex3;
    QString area ="Structure";
    if(hex2 != "00"){area="System"; };
    if(hex3=="41" || hex3=="4D" || hex3=="70")
    {
        geometry = QRect(7*ratio, 30*ratio, 80*ratio, 13*ratio);
    }else{
        geometry = QRect(7*ratio, 78*ratio, 80*ratio, 13*ratio);
    };
    MidiTable *midiTable = MidiTable::Instance();
    Midi items = midiTable->getMidiMap(area, this->combo_hex1, this->combo_hex2, this->combo_hex3);

    this->stompComboBox = new customComboBox(this);
    this->stompComboBox->setObjectName("smallcombo");
    QFont Sfont( "Arial", 8*fratio, QFont::Bold);
    this->stompComboBox->setFont(Sfont);

    int itemcount = 0;
    for(itemcount=0;itemcount<items.level.size();itemcount++ )
    {
        QString item;
        QString desc = items.level.at(itemcount).desc;
        QString customdesc = items.level.at(itemcount).customdesc;
        if(!customdesc.isEmpty())
        {
            item = customdesc;
        }
        else
        {
            item = desc;
        };
        this->stompComboBox->addItem(item);
    };

    this->stompComboBox->setGeometry(geometry);
    this->stompComboBox->setEditable(false);
    this->stompComboBox->setFrame(false);
    this->stompComboBox->setMaxVisibleItems(itemcount);

    QObject::connect(this->stompComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(valueChanged(int)));

}

void stompBox::setComboBoxCurrentIndex(int index)
{
    this->stompComboBox->setCurrentIndex(index);
}

void stompBox::setKnob1(QString hex1, QString hex2, QString hex3)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    QString area ="Structure";
    if(hex2 != "00"){area="System"; };
    MidiTable *midiTable = MidiTable::Instance();
    int range = midiTable->getRange(area, hex1, hex2, hex3);
    knob1 = new customDial(0, 0, range, 1, 10, QPoint(6*ratio, 9*ratio), this, hex1, hex2, hex3);
    this->knob1->setWhatsThis(tr("hold down mouse button and drag up/down for quick adjustment")
                              + "<br>" + tr("use scroll wheel or up/down arrow keys for fine adjustment"));
}

void stompBox::setKnob2(QString hex1, QString hex2, QString hex3)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);
    QString area ="Structure";
    if(hex2 != "00"){area="System"; };
    MidiTable *midiTable = MidiTable::Instance();
    int range = midiTable->getRange(area, hex1, hex2, hex3);
    knob2 = new customDial(0, 0, range, 1, 10, QPoint(53*ratio, 9*ratio), this, hex1, hex2, hex3);
    this->knob2->setWhatsThis(tr("hold down mouse button and drag up/down for quick adjustment")
                              + "<br>" + tr("use scroll wheel or up/down arrow keys for fine adjustment"));
}

void stompBox::setSlider1(QString hex1, QString hex2, QString hex3)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    MidiTable *midiTable = MidiTable::Instance();
    int range = midiTable->getRange("Structure", hex1, hex2, hex3);
    slider1 = new customSlider(0, 0, range, 1, 10, QPoint(8*ratio, 17*ratio), this, hex1, hex2, hex3);
}

void stompBox::setSlider2(QString hex1, QString hex2, QString hex3)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    MidiTable *midiTable = MidiTable::Instance();
    int range = midiTable->getRange("Structure", hex1, hex2, hex3);
    slider2 = new customSlider(0, 0, range, 1, 10, QPoint(24*ratio, 17*ratio), this, hex1, hex2, hex3);
}

void stompBox::setSlider3(QString hex1, QString hex2, QString hex3)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    MidiTable *midiTable = MidiTable::Instance();
    int range = midiTable->getRange("Structure", hex1, hex2, hex3);
    slider3 = new customSlider(0, 0, range, 1, 10, QPoint(40*ratio, 17*ratio), this, hex1, hex2, hex3);
}

void stompBox::setSlider4(QString hex1, QString hex2, QString hex3)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    MidiTable *midiTable = MidiTable::Instance();
    int range = midiTable->getRange("Structure", hex1, hex2, hex3);
    slider4 = new customSlider(0, 0, range, 1, 10, QPoint(56*ratio, 17*ratio), this, hex1, hex2, hex3);
}

void stompBox::setSlider5(QString hex1, QString hex2, QString hex3)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    MidiTable *midiTable = MidiTable::Instance();
    int range = midiTable->getRange("Structure", hex1, hex2, hex3);
    slider5 = new customSlider(0, 0, range, 1, 10, QPoint(79*ratio, 17*ratio), this, hex1, hex2, hex3);
}

void stompBox::setButton(QString hex1, QString hex2, QString hex3)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    button = new customButton(false, QPoint(4*ratio, 110*ratio), this, hex1, hex2, hex3);
    led = new customLed(false, QPoint(41*ratio, 4*ratio), this);

    QObject::connect(button, SIGNAL(valueChanged(bool, QString, QString, QString)),
                     led, SLOT(changeValue(bool)));
}

void stompBox::setButton(QString hex1, QString hex2, QString hex3, QPoint pos, QString imagePath)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    button = new customButton(false, pos, this, hex1, hex2, hex3, imagePath);
    led = new customLed(false, QPoint(41*ratio, 4*ratio), this);

    QObject::connect(button, SIGNAL(valueChanged(bool, QString, QString, QString)),
                     led, SLOT(changeValue(bool)));
}

void stompBox::setSwitch(QString hex1, QString hex2, QString hex3)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    switchbutton = new customSwitch(false, this, hex1, hex2, hex3);
    switchbutton->move(QPoint(15*ratio, 45*ratio));
}

void stompBox::updateComboBox(QString hex1, QString hex2, QString hex3)
{
    Q_UNUSED(hex1);
    Q_UNUSED(hex3);
    QObject::disconnect(this->stompComboBox, SIGNAL(currentIndexChanged(int)), // To prevent sending a signal when loading a patch.
                        this, SLOT(valueChanged(int)));

    QString area ="Structure";
    if(hex2 != "00"){area="System"; };
    SysxIO *sysxIO = SysxIO::Instance();
    setComboBoxCurrentIndex(sysxIO->getSourceValue(area, this->combo_hex1, this->combo_hex2, this->combo_hex3));

    QObject::connect(this->stompComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(valueChanged(int)));
}

void stompBox::updateKnob1(QString hex1, QString hex2, QString hex3)
{
    QString area ="Structure";
    if(hex2 != "00"){area="System"; };
    SysxIO *sysxIO = SysxIO::Instance();
    knob1->setValue(sysxIO->getSourceValue(area, hex1, hex2, hex3));
}

void stompBox::updateKnob2(QString hex1, QString hex2, QString hex3)
{
    QString area ="Structure";
    if(hex2 != "00"){area="System"; };
    SysxIO *sysxIO = SysxIO::Instance();
    knob2->setValue(sysxIO->getSourceValue(area, hex1, hex2, hex3));  
}

void stompBox::updateSlider1(QString hex1, QString hex2, QString hex3)
{
    SysxIO *sysxIO = SysxIO::Instance();
    slider1->setValue(sysxIO->getSourceValue("Structure", hex1, hex2, hex3));
}

void stompBox::updateSlider2(QString hex1, QString hex2, QString hex3)
{
    SysxIO *sysxIO = SysxIO::Instance();
    slider2->setValue(sysxIO->getSourceValue("Structure", hex1, hex2, hex3));
}

void stompBox::updateSlider3(QString hex1, QString hex2, QString hex3)
{
    SysxIO *sysxIO = SysxIO::Instance();
    slider3->setValue(sysxIO->getSourceValue("Structure", hex1, hex2, hex3));
}

void stompBox::updateSlider4(QString hex1, QString hex2, QString hex3)
{
    SysxIO *sysxIO = SysxIO::Instance();
    slider4->setValue(sysxIO->getSourceValue("Structure", hex1, hex2, hex3));
}

void stompBox::updateSlider5(QString hex1, QString hex2, QString hex3)
{
    SysxIO *sysxIO = SysxIO::Instance();
    slider5->setValue(sysxIO->getSourceValue("Structure", hex1, hex2, hex3));
}

void stompBox::updateButton(QString hex1, QString hex2, QString hex3)
{
    QString area ="Structure";
    if(hex2 != "00"){area="System"; };
    SysxIO *sysxIO = SysxIO::Instance();
    int value = sysxIO->getSourceValue(area, hex1, hex2, hex3);
    led->setValue((value==1)?true:false);
    button->setValue((value==1)?true:false);
}

void stompBox::updateSwitch(QString hex1, QString hex2, QString hex3)
{
    QString area ="Structure";
    if(hex2 != "00"){area="System"; };
    SysxIO *sysxIO = SysxIO::Instance();
    int value = sysxIO->getSourceValue(area, hex1, hex2, hex3);
    bool set = false;
    if (value > 0) {set = true; };
    if (this->id == 15 && hex3 == "30"){ emit distortion_statusSignal(set); };
    if (this->id == 4 && hex3 == "30") { emit eq_statusSignal(set); };
    if (this->id == 8 && hex3 == "4E") { emit ce_statusSignal(set); };
    if (this->id == 5 && hex3 == "40") { emit fx1_statusSignal(set); };
    if (this->id == 2 && hex3 == "50") { emit preamp1_statusSignal(set); };
    if (this->id == 13 && hex3 == "63") { emit ns1_statusSignal(set); };
    if (this->id == 1 && hex3 == "55") { emit sendreturn_statusSignal(set); };
    if (this->id == 7 && hex3 == "60") { emit delay_statusSignal(set); };
    if (this->id == 6 && hex3 == "4C") { emit fx2_statusSignal(set); };
    if (this->id == 9 && hex3 == "10") { emit reverb_statusSignal(set); };
    if (this->id == 11 && hex3 == "20") { emit pedal_statusSignal(set); };
}

void stompBox::valueChanged(int value, QString hex1, QString hex2, QString hex3)
{
    QString valueHex = QString::number(value, 16).toUpper();
    if(valueHex.length() < 2) valueHex.prepend("0");
    emitValueChanged(hex1, hex2, hex3, valueHex);
}

void stompBox::valueChanged(bool value, QString hex1, QString hex2, QString hex3)
{
    int valueInt;
    (value)? valueInt=1: valueInt=0;
    QString valueHex = QString::number(valueInt, 16).toUpper();
    if(valueHex.length() < 2) valueHex.prepend("0");
    emitValueChanged(hex1, hex2, hex3, valueHex);
}

void stompBox::valueChanged(int index)
{
    QString valueHex = QString::number(index, 16).toUpper();
    if(valueHex.length() < 2) valueHex.prepend("0");
    emitValueChanged(this->combo_hex1, this->combo_hex2, this->combo_hex3, valueHex);
    QString area ="Structure";
    if(this->combo_hex2 != "00"){area="System"; };
    MidiTable *midiTable = MidiTable::Instance();
    Midi items = midiTable->getMidiMap(area, this->combo_hex1, this->combo_hex2, this->combo_hex3);

    QString desc = items.level.at(index).desc;
    QString customdesc = items.level.at(index).customdesc;
    if(customdesc.isEmpty())
    {
        customdesc = desc;
    };

    this->stompComboBox->setCurrentIndex(index);
    this->stompComboBox->setEditText(desc);
}

void stompBox::emitValueChanged(QString hex1, QString hex2, QString hex3, QString valueHex)
{
    QString area ="Structure";
    if(hex2 != "00"){area="System"; };
    QString valueName, valueStr;
    if(hex1 != "void" && hex2 != "void")
    {
        MidiTable *midiTable = MidiTable::Instance();
        if(valueHex != "void")

        {
            SysxIO *sysxIO = SysxIO::Instance(); bool ok;
            if(midiTable->isData(area, hex1, hex2, hex3))
            {
                int maxRange = QString("7F").toInt(&ok, 16) + 1;
                int value = valueHex.toInt(&ok, 16);
                int dif = value/maxRange;
                QString valueHex1 = QString::number(dif, 16).toUpper();
                if(valueHex1.length() < 2) valueHex1.prepend("0");
                QString valueHex2 = QString::number(value - (dif * maxRange), 16).toUpper();
                if(valueHex2.length() < 2) valueHex2.prepend("0");
                sysxIO->setFileSource(area, hex1, hex2, hex3, valueHex1, valueHex2);
            }
            else
            {
                sysxIO->setFileSource(area, hex1, hex2, hex3, valueHex);
                //emit dialogUpdateSignal();
            };
            Midi items = midiTable->getMidiMap(area, hex1, hex2, hex3);
            valueName = items.customdesc;
            valueStr = midiTable->getValue(area, hex1, hex2, hex3, valueHex);
            emit valueChanged(this->fxName, valueName, valueStr);
            updateSwitch(hex1, hex2, hex3);
        };
    }else{
       updateStompPath();
         emit valueChanged(this->fxName, "", "");
    };
}

void stompBox::setDisplayToFxName()
{
    emit valueChanged(this->fxName, "", "");
}

void stompBox::updateStompPath()
{
    if (this->id == 11) {this->namedata = "PDL";   this->fxName = tr("Pedal Fx");};
    if (this->id == 15) {this->namedata = "OD";   this->fxName = tr("Booster");};
    if (this->id == 5) {this->namedata = "FX1";  this->fxName = tr("Modulation");};
    if (this->id == 2) {this->namedata = "CH_A"; this->fxName = tr("PreAmp");};
    if (this->id == 13) {this->namedata = "NS_1"; this->fxName = tr("Noise Suppressor");};
    if (this->id == 12) {this->namedata = "FV";   this->fxName = tr("Master & Foot Volume");};
    if (this->id == 1) {this->namedata = "LP";   this->fxName = tr("Send/Return");};
    if (this->id == 7) {this->namedata = "DD";   this->fxName = tr("Delay 1");};
    if (this->id == 6) {this->namedata = "FX2";  this->fxName = tr("Effects");};
    if (this->id == 9) {this->namedata = "RV";   this->fxName = tr("Reverb");};
    if (this->id == 4) {this->namedata = "EQ";   this->fxName = tr("Equaliser");};
    if (this->id == 8) {this->namedata = "CE";   this->fxName = tr("Delay 2");};
}

void stompBox::getStompOrder()
{

}
