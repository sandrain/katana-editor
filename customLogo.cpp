/****************************************************************************
**
** Copyright (C) 2007~2020 Colin Willcocks.
** All rights reserved.
** This file is part of "Katana Fx FloorBoard".
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
#include "customLogo.h"
#include "Preferences.h"

customLogo::customLogo(bool active, QPoint ledPos, QWidget *parent,
                                 QString imagePath)
                                     : QWidget(parent)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    this->active = active;
    this->imagePath = ":/images/katana_on.png";
    QSize imageSize = QPixmap(imagePath).size();
    this->ledSize = QSize(imageSize.width(), imageSize.height());
    this->ledPos = ledPos;

    setOffset(0);
    setGeometry(ledPos.x(), ledPos.y(), ledSize.width()*ratio, ledSize.height()*ratio);
    setValue(active);
    //this->timer = new QTimer(this);
    //connect(timer, SIGNAL(timeout()), this, SLOT(fadeOut()));
    //timer->start(4000);
    fadeIn();
}

void customLogo::paintEvent(QPaintEvent *)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    QRectF target(0.0 , 0.0, ledSize.width()*ratio, ledSize.height()*ratio);
    QRectF source(0.0, yOffset, ledSize.width(), ledSize.height());
    QPixmap image(imagePath);
    //image.setMask(image.mask());

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.drawPixmap(target, image, source);
}

void customLogo::setOffset(signed int imageNr)
{
    this->yOffset = imageNr*ledSize.height();
    this->update();
}

void customLogo::setValue(bool value)
{
    this->active = value;
    if(active)
    {
        //this->show();
        this->fadeIn();
    }
    else
    {
        fadeOut();
        //this->hide();
    };
}

void customLogo::changeValue(bool value)
{
        setValue(value);
}

void customLogo::fadeIn()
{
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
    a->setDuration(2000);
    a->setStartValue(0.5);
    a->setEndValue(1);
    a->setEasingCurve(QEasingCurve::Linear);
    a->start(QPropertyAnimation::DeleteWhenStopped);
    connect(a,SIGNAL(finished()),this,SLOT(fadeOut()));
}

void customLogo::fadeOut()
{
    QGraphicsOpacityEffect *eff = new QGraphicsOpacityEffect(this);
    this->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
    a->setDuration(2000);
    a->setStartValue(1);
    a->setEndValue(0.5);
    a->setEasingCurve(QEasingCurve::Linear);
    a->start(QPropertyAnimation::DeleteWhenStopped);
    connect(a,SIGNAL(finished()),this,SLOT(fadeIn()));
}
