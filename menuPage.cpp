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

#include "menuPage.h"
#include "MidiTable.h"
#include "SysxIO.h"
#include "globalVariables.h"
#include "floorBoardDisplay.h"
#include "floorBoard.h"
#include "Preferences.h"
#include <QTimer>

menuPage::menuPage(QWidget *parent, unsigned int id, QString imagePath, QPoint stompPos)
    : QWidget(parent)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    this->id = id;
    this->imagePath = imagePath;
    this->stompSize = QPixmap(imagePath).size()*ratio;
    stompPos = QPoint(100*ratio, 24*ratio);
    this->setWhatsThis(tr("Deep editing of the selected effect<br>pressing this button will open an edit page<br>allowing detailed setting of this effects parameters."));


    this->setFixedSize(stompSize);

    this->editDialog = new editWindow();

    this->menuButton = new customButton(tr(""), false, QPoint(0, 0), this, ":/images/menu_pushbutton.png");

    QObject::connect(this, SIGNAL( valueChanged(QString, QString, QString) ), this->parent(), SIGNAL( valueChanged(QString, QString, QString) ));

    QObject::connect(this->parent(), SIGNAL( updateStompOffset(signed int) ), this, SLOT( updatePos(signed int) ));

    QObject::connect(this->parent(), SIGNAL( updateSignal() ), this, SLOT( updateSignal() ));

    QObject::connect(this, SIGNAL( systemUpdateSignal() ), this->parent()->parent(), SIGNAL( updateSignal() ));

    QObject::connect(this->editDialog, SIGNAL( updateSignal() ), this, SLOT( updateSignal() ));

    QObject::connect(this, SIGNAL( dialogUpdateSignal() ), this->editDialog, SIGNAL( dialogUpdateSignal() ));

    QObject::connect(this->parent(), SIGNAL( updateSignal() ), this->editDialog, SIGNAL( dialogUpdateSignal() ));

    QObject::connect(this->editDialog, SIGNAL( updateSignal() ), this, SLOT( setDisplayToFxName() ));

    QObject::connect(this, SIGNAL( setEditDialog(editWindow*) ), this->parent(), SLOT( setEditDialog(editWindow*) ));

    QObject::connect(this->parent(), SIGNAL(midi_buttonSignal(bool)), this, SLOT(midi_ButtonSignal(bool) ));
    QObject::connect(this->parent(), SIGNAL(midi_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(master_buttonSignal(bool)), this, SLOT(master_ButtonSignal(bool) ));
    QObject::connect(this->parent(), SIGNAL(master_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(ez_edit_buttonSignal(bool)), this, SLOT(ez_edit_ButtonSignal(bool) ));
    QObject::connect(this->parent(), SIGNAL(ez_edit_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(system_buttonSignal(bool)), this, SLOT(system_ButtonSignal(bool) ));
    QObject::connect(this->parent(), SIGNAL(system_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(assign1_buttonSignal(bool)), this, SLOT(assign1_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(assign1_statusSignal(bool)), this->parent(), SIGNAL(assign1_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(assign1_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(assign2_buttonSignal(bool)), this, SLOT(assign2_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(assign2_statusSignal(bool)), this->parent(), SIGNAL(assign2_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(assign2_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(assign3_buttonSignal(bool)), this, SLOT(assign3_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(assign3_statusSignal(bool)), this->parent(), SIGNAL(assign3_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(assign3_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(assign4_buttonSignal(bool)), this, SLOT(assign4_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(assign4_statusSignal(bool)), this->parent(), SIGNAL(assign4_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(assign4_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(assign5_buttonSignal(bool)), this, SLOT(assign5_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(assign5_statusSignal(bool)), this->parent(), SIGNAL(assign5_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(assign5_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(assign6_buttonSignal(bool)), this, SLOT(assign6_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(assign6_statusSignal(bool)), this->parent(), SIGNAL(assign6_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(assign6_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(assign7_buttonSignal(bool)), this, SLOT(assign7_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(assign7_statusSignal(bool)), this->parent(), SIGNAL(assign7_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(assign7_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(assign8_buttonSignal(bool)), this, SLOT(assign8_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(assign8_statusSignal(bool)), this->parent(), SIGNAL(assign8_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(assign8_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(assign9_buttonSignal(bool)), this, SLOT(assign9_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(assign9_statusSignal(bool)), this->parent(), SIGNAL(assign9_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(assign9_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(assign10_buttonSignal(bool)), this, SLOT(assign10_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(assign10_statusSignal(bool)), this->parent(), SIGNAL(assign10_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(assign10_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(assign11_buttonSignal(bool)), this, SLOT(assign11_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(assign11_statusSignal(bool)), this->parent(), SIGNAL(assign11_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(assign11_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(assign12_buttonSignal(bool)), this, SLOT(assign12_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(assign12_statusSignal(bool)), this->parent(), SIGNAL(assign12_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(assign12_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(assign13_buttonSignal(bool)), this, SLOT(assign13_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(assign13_statusSignal(bool)), this->parent(), SIGNAL(assign13_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(assign13_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(assign14_buttonSignal(bool)), this, SLOT(assign14_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(assign14_statusSignal(bool)), this->parent(), SIGNAL(assign14_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(assign14_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(assign15_buttonSignal(bool)), this, SLOT(assign15_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(assign15_statusSignal(bool)), this->parent(), SIGNAL(assign15_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(assign15_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    QObject::connect(this->parent(), SIGNAL(assign16_buttonSignal(bool)), this, SLOT(assign16_ButtonSignal(bool) ));
    QObject::connect(this, SIGNAL(assign16_statusSignal(bool)), this->parent(), SIGNAL(assign16_statusSignal(bool)));
    QObject::connect(this->parent(), SIGNAL(assign16_buttonSignal(bool)), this->parent(), SLOT(menuButtonSignal()));

    SysxIO *sysxIO = SysxIO::Instance();
    QObject::connect(this, SIGNAL(setStatusSymbol(int)), sysxIO, SIGNAL(setStatusSymbol(int)));
    QObject::connect(this, SIGNAL(setStatusProgress(int)), sysxIO, SIGNAL(setStatusProgress(int)));
    QObject::connect(this, SIGNAL(setStatusMessage(QString)), sysxIO, SIGNAL(setStatusMessage(QString)));

}


void menuPage::paintEvent(QPaintEvent *)
{
     Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    QRectF target(0.0, 0.0, stompSize.width(), stompSize.height());
    QRectF source(0.0, 0.0, stompSize.width(), stompSize.height());
    QPixmap image(imagePath);
    image = image.scaled(stompSize.width()*ratio, stompSize.height()*ratio, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    this->image = image;
    QPainter painter(this);
    painter.drawPixmap(target, image, source);
}

editWindow* menuPage::editDetails()
{
    return this->editDialog;
}

void menuPage::system_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    SysxIO *sysxIO = SysxIO::Instance();
    if(this->id == 30) //&& sysxIO->deviceReady())
    {
        emit setStatusMessage(tr("Opening Page..."));
        emit setStatusSymbol(3);
        emitValueChanged(this->hex1, this->hex2, "00", "void");
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
        if (sysxIO->isConnected())
        {
            sysxIO->setDeviceReady(false); // Reserve the device for interaction.
            QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)));
            QObject::connect(sysxIO, SIGNAL(sysxReply(QString)), this, SLOT(systemReply(QString)));
            emit setStatusProgress(100);
            emit setStatusSymbol(2);
            emit setStatusMessage(tr("Request System data"));
            sysxIO->sendSysx(systemRequest); // KATANA System area data Request.
        }
        else
        {
            QString snork = tr("Ensure connection is active and retry<br>");
            snork.append(tr("System data not transferred, current settings are to be used<br>"));
            QMessageBox *msgBox = new QMessageBox();
            msgBox->setWindowTitle(deviceType + tr(" midi connection not found!!"));
            msgBox->setIcon(QMessageBox::Information);
            msgBox->setText(snork);
            msgBox->setStandardButtons(QMessageBox::Ok);
            msgBox->exec();
            emit setStatusMessage(tr("Not Connected"));
            emit setStatusSymbol(0);

        };
    };
}

void menuPage::midi_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if(this->id == 29)
    {
        emitValueChanged(this->hex1, this->hex2, "00", "void");
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);      
    };
}

void menuPage::master_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 20)
    {
        emitValueChanged(this->hex1, this->hex2, "00", "void");
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void menuPage::ez_edit_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 31)
    {
        emitValueChanged(this->hex1, this->hex2, "00", "void");
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void menuPage::assign1_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 21)
    {
        emitValueChanged(this->hex1, this->hex2, "00", "void");
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void menuPage::assign2_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 22)
    {
        emitValueChanged(this->hex1, this->hex2, "00", "void");
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void menuPage::assign3_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 23)
    {
        emitValueChanged(this->hex1, this->hex2, "00", "void");
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void menuPage::assign4_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 24)
    {
        emitValueChanged(this->hex1, this->hex2, "00", "void");
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void menuPage::assign5_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 25)
    {
        emitValueChanged(this->hex1, this->hex2, "00", "void");
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void menuPage::assign6_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 26)
    {
        emitValueChanged(this->hex1, this->hex2, "00", "void");
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void menuPage::assign7_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 27)
    {
        emitValueChanged(this->hex1, this->hex2, "00", "void");
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void menuPage::assign8_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 28)
    {
        emitValueChanged(this->hex1, this->hex2, "00", "void");
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void menuPage::assign9_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 33)
    {
        emitValueChanged(this->hex1, this->hex2, "00", "void");
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void menuPage::assign10_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 34)
    {
        emitValueChanged(this->hex1, this->hex2, "00", "void");
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void menuPage::assign11_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 35)
    {
        emitValueChanged(this->hex1, this->hex2, "00", "void");
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void menuPage::assign12_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 36)
    {
        emitValueChanged(this->hex1, this->hex2, "00", "void");
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void menuPage::assign13_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 37)
    {
        emitValueChanged(this->hex1, this->hex2, "00", "void");
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void menuPage::assign14_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 38)
    {
        emitValueChanged(this->hex1, this->hex2, "00", "void");
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void menuPage::assign15_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 39)
    {
        emitValueChanged(this->hex1, this->hex2, "00", "void");
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void menuPage::assign16_ButtonSignal(bool value)
{
    Q_UNUSED(value);
    if (this->id == 40)
    {
        emitValueChanged(this->hex1, this->hex2, "00", "void");
        this->editDialog->setWindow(this->fxName);
        emit setEditDialog(this->editDialog);
    };
}

void menuPage::systemReply(QString replyMsg)
{
    SysxIO *sysxIO = SysxIO::Instance();
    QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)), this, SLOT(systemReply(QString)));
    sysxIO->setDeviceReady(true); // Free the device after finishing interaction.

    if(sysxIO->noError())
    {
        if(replyMsg.size()/2 == systemReplySize)
        {
            /* TRANSLATE SYSX MESSAGE FORMAT to 128 byte data blocks */
            QString header = "F0410000006012";
            QString footer ="00F7";

            QString part1 = replyMsg.mid(24, 256); //from 12, copy 128 bits (values are doubled for QString)
            part1.prepend("00000000").prepend(header).append(footer);

            QString part2 = replyMsg.mid(280, 226);
            QString part2B = replyMsg.mid(534, 30);
            part2.prepend("00000100").prepend(header).append(part2B).append(footer);

            QString part3 = replyMsg.mid(564, 256);
            part3.prepend("00000200").prepend(header).append(footer);

            QString part4 = replyMsg.mid(820, 196);
            part4.prepend("00000300").prepend(header).append(footer);

            QString part5 = replyMsg.mid(1044, 150);
            part5.prepend("00000400").prepend(header).append(footer);

            QString part6 = replyMsg.mid(1222, 82);
            part6.prepend("00020000").prepend(header).append(footer);

            replyMsg = "";
            replyMsg.append(part1).append(part2).append(part3).append(part4).append(part5).append(part6);

            QString reBuild;       /* Add correct checksum to patch strings */
            QString sysxEOF;
            QString hex;
            int msgLength = replyMsg.length()/2;
            for(int i=0;i<msgLength*2;++i)
            {
                hex.append(replyMsg.mid(i*2, 2));
                sysxEOF = (replyMsg.mid((i*2)+4, 2));
                if (sysxEOF == "F7")
                {
                    int dataSize = 0; bool ok;
                    for(int h=checksumOffset;h<hex.size()-1;++h)
                    { dataSize += hex.mid(h*2, 2).toInt(&ok, 16); };
                    QString base = "80";                       // checksum calculate.
                    unsigned int sum = dataSize % base.toInt(&ok, 16);
                    if(sum!=0) { sum = base.toInt(&ok, 16) - sum; };
                    QString checksum = QString::number(sum, 16).toUpper();
                    if(checksum.length()<2) {checksum.prepend("0");};
                    hex.append(checksum);
                    hex.append("F7");
                    reBuild.append(hex);
                    hex = "";
                    sysxEOF = "";
                    i=i+2;
                };
            };
            replyMsg = reBuild.simplified().toUpper();
            sysxIO->setFileSource("System", replyMsg);		// Set the source to the data received.
            sysxIO->setFileName(tr("System Data from ") + deviceType);	// Set the file name to KATANA system for the display.
            sysxIO->setDevice(true);				// Patch received from the device so this is set to true.
            sysxIO->setSyncStatus(true);			// We can't be more in sync than right now! :)
            emit systemUpdateSignal();
        }
        else
        {
            QMessageBox *msgBox = new QMessageBox();
            msgBox->setWindowTitle(deviceType + tr(" Fx FloorBoard connection Error !!"));
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->setTextFormat(Qt::RichText);
            QString msgText;
            msgText.append("<font size='+1'><b>");
            msgText.append(tr("The Boss ") + deviceType + tr(" System data was not transfered !!."));
            msgText.append("<b></font><br>");
            msgBox->setText(msgText);
            msgBox->show();
            QTimer::singleShot(100, msgBox, SLOT(deleteLater()));
        };
    };
    emit setStatusMessage(tr("Ready"));
}

void menuPage::setPos(QPoint newPos)
{
    this->move(newPos);
    //this->stompPos = newPos;
}

void menuPage::updatePos(signed int offsetDif)
{
    this->stompPos = this->pos();
    QPoint newPos = stompPos + QPoint(offsetDif, 0);
    this->move(newPos);
    //this->stompPos = newPos;
}

void menuPage::setImage(QString imagePath)
{
    this->imagePath = imagePath;
    this->update();
}

void menuPage::setSize(QSize newSize)
{
    this->stompSize = newSize;
    this->setFixedSize(stompSize);
}

void menuPage::setId(unsigned int id)
{
    this->id = id;
}

unsigned int menuPage::getId()
{
    return this->id;
}

void menuPage::setLSB(QString hex1, QString hex2)
{
    this->hex1 = hex1;
    this->hex2 = hex2;
    this->editDialog->setLSB(hex1, hex2);
}

void menuPage::updateSwitch(QString hex1, QString hex2, QString hex3, QString area)
{
 SysxIO *sysxIO = SysxIO::Instance();
    if(!area.contains("System")){area="Structure";};
    int value = sysxIO->getSourceValue(area, hex1, hex2, hex3);
    bool set = false;
    if (value >= 1) {set = true; };
    if(this->id == 21) { emit assign1_statusSignal(set); };
    if(this->id == 22) { emit assign2_statusSignal(set); };
    if(this->id == 23) { emit assign3_statusSignal(set); };
    if(this->id == 24) { emit assign4_statusSignal(set); };
    if(this->id == 25) { emit assign5_statusSignal(set); };
    if(this->id == 26) { emit assign6_statusSignal(set); };
    if(this->id == 27) { emit assign7_statusSignal(set); };
    if(this->id == 28) { emit assign8_statusSignal(set); };
    if(this->id == 33) { emit assign9_statusSignal(set); };
    if(this->id == 34) { emit assign10_statusSignal(set); };
    if(this->id == 35) { emit assign11_statusSignal(set); };
    if(this->id == 36) { emit assign12_statusSignal(set); };
    if(this->id == 37) { emit assign13_statusSignal(set); };
    if(this->id == 38) { emit assign14_statusSignal(set); };
    if(this->id == 39) { emit assign15_statusSignal(set); };
    if(this->id == 40) { emit assign16_statusSignal(set); };
}


void menuPage::valueChanged(int value, QString hex1, QString hex2, QString hex3)
{
    MidiTable *midiTable = MidiTable::Instance();
    QString area;
    if(this->id == 30 || this->id == 29) {area = "System";} else {area = "Structure";};
    QString valueHex = QString::number(value, 16).toUpper();
    if(valueHex.length() < 2) valueHex.prepend("0");

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
    };

    emitValueChanged(hex1, hex2, hex3, valueHex);
}

void menuPage::valueChanged(bool value, QString hex1, QString hex2, QString hex3)
{
    int valueInt;
    (value)? valueInt=1: valueInt=0;
    QString valueHex = QString::number(valueInt, 16).toUpper();
    if(valueHex.length() < 2) valueHex.prepend("0");

    SysxIO *sysxIO = SysxIO::Instance();
    QString area;
    if(this->id == 30 || this->id == 29) {area = "System";} else {area = "Structure";};
    sysxIO->setFileSource(area, hex1, hex2, hex3, valueHex);


    emitValueChanged(hex1, hex2, hex3, valueHex);
    //emit updateSignal();
}


void menuPage::emitValueChanged(QString hex1, QString hex2, QString hex3, QString valueHex)
{
    QString area;
    if(this->id == 30 || this->id == 29) {area = "System";} else {area = "Structure";};
    QString valueName, valueStr;
    if(hex1 != "void" && hex2 != "void")
    {
        MidiTable *midiTable = MidiTable::Instance();
        if(valueHex != "void")
        {
            Midi items = midiTable->getMidiMap(area, hex1, hex2, hex3);
            valueName = items.desc;
            //this->fxName = midiTable->getMidiMap("Structure", hex1, hex2, hex3).name;
            valueStr = midiTable->getValue(area, hex1, hex2, hex3, valueHex);
            //emit dialogUpdateSignal();
        }
        else
        {
            if (this->id == 31){this->fxName = tr("EZ-Tone Creator");                 this->area_mode = "Structure";};
            if (this->id == 30){this->fxName = tr("System settings");                 this->area_mode = "System";   };
            if (this->id == 29){this->fxName = tr("Green-Red-Yellow Option Select");  this->area_mode = "Structure";};
            if (this->id == 20){this->fxName = tr("Knob & Pedal Assign");             this->area_mode = "Structure";};
            if (this->id == 21){this->fxName = tr("Patch - MIDI Assign 1");           this->area_mode = "Structure";};
            if (this->id == 22){this->fxName = tr("Patch - MIDI Assign 2");           this->area_mode = "Structure";};
            if (this->id == 23){this->fxName = tr("Patch - MIDI Assign 3");           this->area_mode = "Structure";};
            if (this->id == 24){this->fxName = tr("Patch - MIDI Assign 4");           this->area_mode = "Structure";};
            if (this->id == 25){this->fxName = tr("Patch - MIDI Assign 5");           this->area_mode = "Structure";};
            if (this->id == 26){this->fxName = tr("Patch - MIDI Assign 6");           this->area_mode = "Structure";};
            if (this->id == 27){this->fxName = tr("Patch - MIDI Assign 7");           this->area_mode = "Structure";};
            if (this->id == 28){this->fxName = tr("Patch - MIDI Assign 8");           this->area_mode = "Structure";};
            if (this->id == 33){this->fxName = tr("Global - MIDI Assign 9");          this->area_mode = "Structure";};
            if (this->id == 34){this->fxName = tr("Global - MIDI Assign 10");         this->area_mode = "Structure";};
            if (this->id == 35){this->fxName = tr("Global - MIDI Assign 11");         this->area_mode = "Structure";};
            if (this->id == 36){this->fxName = tr("Global - MIDI Assign 12");         this->area_mode = "Structure";};
            if (this->id == 37){this->fxName = tr("Global - MIDI Assign 13");         this->area_mode = "Structure";};
            if (this->id == 38){this->fxName = tr("Global - MIDI Assign 14");         this->area_mode = "Structure";};
            if (this->id == 39){this->fxName = tr("Global - MIDI Assign 15");         this->area_mode = "Structure";};
            if (this->id == 40){this->fxName = tr("Global - MIDI Assign 16");         this->area_mode = "Structure";};
        };
    };

    emit valueChanged(this->fxName, valueName, valueStr);
}

void menuPage::setDisplayToFxName()
{
    emit valueChanged(this->fxName, "", "");
}
