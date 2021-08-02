/****************************************************************************
**
** Copyright (C) 2016~2018 Colin Willcocks.
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

#include <QtWidgets>
#include "customGRYButton.h"
#include "Preferences.h"

customGRYButton::customGRYButton(int active, QWidget *parent, QString hex1, QString hex2, QString hex3,
                                 QString imagePath)
    : QWidget(parent)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    this->hex1 = hex1;
    this->hex2 = hex2;
    this->hex3 = hex3;
    this->active = active;
    this->imagePath = imagePath;
    QSize imageSize = QPixmap(imagePath).size();
    this->switchSize = QSize(imageSize.width(), imageSize.height()/4);
    this->imageRange = 1;
    this->switchPos = switchPos;
    this->setOffset(0);
    this->setFixedSize(switchSize*ratio);

    QObject::connect(this, SIGNAL( valueChanged(int, QString, QString, QString) ),
                     this->parent(), SLOT( valueChanged(int, QString, QString, QString) ));
}

void customGRYButton::paintEvent(QPaintEvent *)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    QRectF target(0.0 , 0.0, switchSize.width()*ratio, switchSize.height()*ratio);
    QRectF source(0.0, yOffset, switchSize.width(), switchSize.height());
    QPixmap image(imagePath);

    QPainter painter(this);
    painter.drawPixmap(target, image, source);
}

void customGRYButton::setOffset(signed int imageNr)
{
    this->yOffset = imageNr*switchSize.height();
    this->update();
}

void customGRYButton::mousePressEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton )
    {
        this->dragStartPosition = event->pos();
        setFocus();
        setOffset(0);
    };
}

void customGRYButton::mouseReleaseEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton )
    {
        this->value ++;
        if(this->value>2){this->value=0;};

        setOffset(this->value+1);
        emitValue(this->value);

        clearFocus();
    };
}

void customGRYButton::emitValue(int value)
{

    emit valueChanged((int)value, this->hex1, this->hex2, this->hex3);
}

void customGRYButton::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() == Qt::LeftButton) && (event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
    {
        return;
    };
}

void customGRYButton::setValue(int value)
{
    this->value = value;
    if(value>2)
    {
        setOffset(0);
    }
    else
    {
        setOffset(value+1);
    };
    clearFocus();
}
