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

#ifndef FLOORBOARD_H
#define FLOORBOARD_H

#include <QWidget>
#include <QPixmap>
#include "stompBox.h"
#include "menuPage.h"
#include "editWindow.h"

class floorBoard : public QWidget
{
    Q_OBJECT

public:
    floorBoard(
            QWidget *parent = 0,
            QString imagePathFloor = ":/images/floor.png",
            QString imagePathStompBG = ":/images/stompbg.png",
            QString imagePathInfoBar = ":/images/infobar.png",
            unsigned int marginStompBoxesTop = 175,
            unsigned int marginStompBoxesBottom = 105,
            unsigned int marginStompBoxesWidth = 35,
            unsigned int panelBarOffset = 5,
            unsigned int borderWidth = 3,
            QPoint pos = QPoint(0, 0));
    ~floorBoard();
    QPoint getStompPos(int id);
    editWindow* editDialog;
    editWindow* oldDialog;
    QList<stompBox*> stompBoxes;

public slots:
    void setWidth(int dist);
    void setCollapse();
    void updateStompBoxes();
    void setEditDialog(editWindow* editDialog);
    void menuButtonSignal();
    void chain_1_Set(bool value);
    void chain_2_Set(bool value);
    void chain_3_Set(bool value);
    void chain_4_Set(bool value);
    void chain_5_Set(bool value);
    void chain_6_Set(bool value);
    void chainSet();
    virtual void updateSwap() {}

signals:
    void valueChanged(QString fxName, QString valueName, QString value);
    void setDisplayPos(QPoint newpos);
    void setFloorPanelBarPos(QPoint newpos);
    void updateStompOffset(signed int offsetDif);
    void sizeChanged(QSize newsize, QSize oldSize);
    void setCollapseState(bool state);
    void resizeSignal(QRect newrect);
    void showDragBar(QPoint newpos);
    void hideDragBar();
    void updateSignal();
    void pathUpdateSignal();
    void preamp1_buttonSignal(bool value);
    void distortion_buttonSignal(bool value);
    void ns1_buttonSignal(bool value);
    void eq_buttonSignal(bool value);
    void ce_buttonSignal(bool value);
    void fx1_buttonSignal(bool value);
    void fx2_buttonSignal(bool value);
    void reverb_buttonSignal(bool value);
    void delay_buttonSignal(bool value);
    void sendreturn_buttonSignal(bool value);
    void pedal_buttonSignal(bool value);
    void fv_buttonSignal(bool value);
    void master_buttonSignal(bool value);
    void ez_edit_buttonSignal(bool value);
    void system_buttonSignal(bool value);
    void midi_buttonSignal(bool value);
    void chain_1_buttonSignal(bool value);
    void chain_2_buttonSignal(bool value);
    void chain_3_buttonSignal(bool value);
    void chain_4_buttonSignal(bool value);
    void chain_5_buttonSignal(bool value);
    void chain_6_buttonSignal(bool value);
    void preamp1_statusSignal(bool value);
    void distortion_statusSignal(bool value);
    void ns1_statusSignal(bool value);
    void eq_statusSignal(bool value);
    void ce_statusSignal(bool value);
    void fx1_statusSignal(bool value);
    void fx2_statusSignal(bool value);
    void reverb_statusSignal(bool value);
    void delay_statusSignal(bool value);
    void sendreturn_statusSignal(bool value);
    void pedal_statusSignal(bool value);
    void fv_statusSignal(bool value);
    void master_statusSignal(bool value);
    void system_statusSignal(bool value);
    void midi_statusSignal(bool value);
    void chain_1_statusSignal(bool value);
    void chain_2_statusSignal(bool value);
    void chain_3_statusSignal(bool value);
    void chain_4_statusSignal(bool value);
    void chain_5_statusSignal(bool value);
    void chain_6_statusSignal(bool value);
    void assign1_buttonSignal(bool value);
    void assign1_statusSignal(bool value);
    void assign2_buttonSignal(bool value);
    void assign2_statusSignal(bool value);
    void assign3_buttonSignal(bool value);
    void assign3_statusSignal(bool value);
    void assign4_buttonSignal(bool value);
    void assign4_statusSignal(bool value);
    void assign5_buttonSignal(bool value);
    void assign5_statusSignal(bool value);
    void assign6_buttonSignal(bool value);
    void assign6_statusSignal(bool value);
    void assign7_buttonSignal(bool value);
    void assign7_statusSignal(bool value);
    void assign8_buttonSignal(bool value);
    void assign8_statusSignal(bool value);
    void assign9_buttonSignal(bool value);
    void assign9_statusSignal(bool value);
    void assign10_buttonSignal(bool value);
    void assign10_statusSignal(bool value);
    void assign11_buttonSignal(bool value);
    void assign11_statusSignal(bool value);
    void assign12_buttonSignal(bool value);
    void assign12_statusSignal(bool value);
    void assign13_buttonSignal(bool value);
    void assign13_statusSignal(bool value);
    void assign14_buttonSignal(bool value);
    void assign14_statusSignal(bool value);
    void assign15_buttonSignal(bool value);
    void assign15_statusSignal(bool value);
    void assign16_buttonSignal(bool value);
    void assign16_statusSignal(bool value);

protected:
    void paintEvent(QPaintEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);

private:
    void initSize(QSize floorSize);
    void setSize(QSize newSize);
    void setFloorBoard();
    void initStomps();
    void initMenuPages();
    void setStomps(QList<QString> stompOrder);
    void setStompPos(QString name, int order);
    void setStompPos(int index, int order);
    void centerEditDialog();
    QString imagePathFloor;
    QString imagePathStompBG;
    QString imagePathInfoBar;
    unsigned int offset;
    unsigned int infoBarWidth;
    unsigned int infoBarHeight;
    unsigned int panelBarOffset;
    unsigned int borderWidth;
    unsigned int floorHeight;
    QSize minSize;
    QSize maxSize;
    QSize l_floorSize;
    QSize floorSize;
    unsigned int marginStompBoxesTop;
    unsigned int marginStompBoxesBottom;
    unsigned int marginStompBoxesWidth;
    QSize stompSize;
    QPixmap baseImage;
    QPixmap image;
    QPoint pos;
    QPoint displayPos;
    QPoint liberainPos;
    QPoint panelBarPos;
    QList<QPoint> fxPos;
    QList<int> fx;
    bool colapseState;
    QList<menuPage*> menuPages;
    QList<QString> stompNames;
    bool chain_message = false;
};

#endif // FLOORBOARD_H
