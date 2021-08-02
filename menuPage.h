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

#ifndef MENUPAGE_H
#define MENUPAGE_H

#include <QtWidgets>
#include "MidiTable.h"
#include "editWindow.h"
#include "editPage.h"
#include "customButton.h"
#include "customComboBox.h"

class menuPage : public QWidget
{
    Q_OBJECT

public:
    menuPage(
            QWidget *parent = 0,
            unsigned int id = 0,
            QString imagePath = ":/images/menuPage_stomp.png",
            QPoint stompPos = QPoint(100, 24));
    void setPos(QPoint newPos);
    void setImage(QString imagePath);
    void setSize(QSize newSize);
    void setId(unsigned int id);

    void setLSB(QString hex1, QString hex2);
    unsigned int getId();
    void updateSwitch(QString hex1, QString hex2, QString hex3, QString area);
    virtual void setEditPages() {}
    editWindow* editDetails();

public slots:
    void updatePos(signed int offsetDif);
    void valueChanged(int value, QString hex1, QString hex2, QString hex3);
    void valueChanged(bool value, QString hex1, QString hex2, QString hex3);
    virtual void updateSignal() {}
    void setDisplayToFxName();
    void master_ButtonSignal(bool value);
    void ez_edit_ButtonSignal(bool value);
    void system_ButtonSignal(bool value);
    void midi_ButtonSignal(bool value);
    void systemReply(QString);
    void assign1_ButtonSignal(bool value);
    void assign2_ButtonSignal(bool value);
    void assign3_ButtonSignal(bool value);
    void assign4_ButtonSignal(bool value);
    void assign5_ButtonSignal(bool value);
    void assign6_ButtonSignal(bool value);
    void assign7_ButtonSignal(bool value);
    void assign8_ButtonSignal(bool value);
    void assign9_ButtonSignal(bool value);
    void assign10_ButtonSignal(bool value);
    void assign11_ButtonSignal(bool value);
    void assign12_ButtonSignal(bool value);
    void assign13_ButtonSignal(bool value);
    void assign14_ButtonSignal(bool value);
    void assign15_ButtonSignal(bool value);
    void assign16_ButtonSignal(bool value);

signals:
    void valueChanged(QString fxName, QString valueName, QString valueStr);
    void currentIndexChanged(int index);
    void dialogUpdateSignal();
    void setEditDialog(editWindow* editDialog);
    //void notConnectedSignal();
    void setStatusSymbol(int value);
    void setStatusProgress(int value);
    void setStatusMessage(QString message);
    void systemUpdateSignal();
    void assign1_statusSignal(bool value);
    void assign2_statusSignal(bool value);
    void assign3_statusSignal(bool value);
    void assign4_statusSignal(bool value);
    void assign5_statusSignal(bool value);
    void assign6_statusSignal(bool value);
    void assign7_statusSignal(bool value);
    void assign8_statusSignal(bool value);
    void assign9_statusSignal(bool value);
    void assign10_statusSignal(bool value);
    void assign11_statusSignal(bool value);
    void assign12_statusSignal(bool value);
    void assign13_statusSignal(bool value);
    void assign14_statusSignal(bool value);
    void assign15_statusSignal(bool value);
    void assign16_statusSignal(bool value);

protected:
    void paintEvent(QPaintEvent *event);

private:

    void emitValueChanged(QString hex1, QString hex2, QString hex3, QString valueHex);
    editWindow *editDialog;
    QString imagePath;
    QSize stompSize;
    QPoint stompPos;
    unsigned int id;

    QPoint dragStartPosition;
    QPixmap image;
    QString hex1;
    QString hex2;
    QString hex3;
    QString fxName;
    QString area_mode;
    customButton *menuButton;
};

#endif // MENUPAGE_H
