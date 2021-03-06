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

#include <QtWidgets>
#include "customLed.h"
#include "Preferences.h"

customLed::customLed(bool active, QPoint ledPos, QWidget *parent,
					 QString imagePath)
    : QWidget(parent)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

	this->active = active;
	this->imagePath = imagePath;
    QSize imageSize = QPixmap(imagePath).size();
    this->ledSize = QSize(imageSize.width()*ratio, (imageSize.height()/2)*ratio);
	this->ledPos = ledPos;

	setOffset(0);
    setGeometry(ledPos.x(), ledPos.y(), ledSize.width()*ratio, ledSize.height()*ratio);
}

void customLed::paintEvent(QPaintEvent *)
{
    QRectF target(0.0 , 0.0, ledSize.width(), ledSize.height());
    QRectF source(0.0, yOffset, ledSize.width(), ledSize.height());
	QPixmap image(imagePath);
    image = image.scaled(ledSize.width(), ledSize.height()*2, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    //image.setMask(image.mask());

	QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.drawPixmap(target, image, source);
}

void customLed::setOffset(signed int imageNr)
{
    this->yOffset = imageNr*ledSize.height();
    this->update();
}

void customLed::setValue(bool value)
{
    this->active = value;
    if(active)
    {
        setOffset(1);
    }
    else
    {
        setOffset(0);
    };
}

void customLed::changeValue(bool value)
{
    setValue(value);
}
