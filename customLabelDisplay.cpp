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

#include <QPainter>
#include "customLabelDisplay.h"
#include "Preferences.h"

customLabelDisplay::customLabelDisplay(QRect geometry, QWidget *parent)
    : QWidget(parent)
{
    this->geometry = geometry;
    this->font.setFamily("void");
    this->setGeometry(geometry);
    this->setLabelPosition();
}

void customLabelDisplay::paintEvent(QPaintEvent *)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);
    const double fratio = preferences->getPreferences("Window", "Font", "ratio").toDouble(&ok);

    QFont Sfont( "Arial", 10*fratio, QFont::Bold);
    this->font = Sfont;
    this->mainLabel->setFont(Sfont);
    /* Set the default font. */
    if(this->font.family() == "void")
    {
        this->font = this->mainLabel->font();
        this->mainPal = this->mainLabel->palette();
        this->subPal = this->subLabelLeft->palette();
    }
    else
    {
        this->mainLabel->setFont(this->font);
    };

    while(this->mainLabel->geometry().width() < QFontMetrics(this->mainLabel->font()).width(this->mainLabel->text()))
    {
        /* Change the font size to make it fit... */
        QFont tmpfont = this->mainLabel->font();
        int fontSize = (tmpfont.pixelSize()*ratio)-1;


         if (fontSize <= 1)
        {
            break;
            fontSize = 1;
        };
        tmpfont.setPixelSize(fontSize);
        this->mainLabel->setFont(tmpfont);
    };
	
	/* This paints the background with the border. */
	QPen border;
	border.setWidth(1);
	border.setColor(QColor(150,150,150));

	QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
	painter.setPen(border);
    painter.setBrush(QColor(34,24,25));
	//painter.drawRoundRect(QRectF(0.0, 0.0, geometry.width()-1, geometry.height()-1), 8, 8);
	//painter.drawRect(QRect(0.0, 0.0, geometry.width()-1, geometry.height()-1));


	/* Draw a custom path. This to have a constant border 
	   radius independant of the rectangle size. */
	int radius = 5; // Set the border radius.
	QPainterPath roundRectPath;
	roundRectPath.moveTo(geometry.width()-1, radius);
	roundRectPath.arcTo((geometry.width()-1) - radius*2, 0.0, radius*2, radius*2, 0.0, 90.0);
	roundRectPath.lineTo(radius, 0.0);
	roundRectPath.arcTo(0.0, 0.0, radius*2, radius*2, 90.0, 90.0);
	roundRectPath.lineTo(0.0, (geometry.height()-1) - radius);
	roundRectPath.arcTo(0.0, (geometry.height()-1) - (radius*2), radius*2, radius*2, 180.0, 90.0);
	roundRectPath.lineTo((geometry.width()-1) - radius, geometry.height()-1);
	roundRectPath.arcTo((geometry.width()-1) - (radius*2), (geometry.height()-1) - (radius*2), radius*2, radius*2, 270.0, 90.0);
	roundRectPath.closeSubpath();
	painter.drawPath(roundRectPath);
}

void customLabelDisplay::setLabelPosition(bool invert)
{
    Q_UNUSED(invert);
	  Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);
    const double fratio = preferences->getPreferences("Window", "Font", "ratio").toDouble(&ok);

    int height = this->geometry.height();
    int width = this->geometry.width();
    int marginWidth = 5*ratio;
    int marginHeight = 2*ratio;

    QRect subGeometry, mainGeometry;
    subGeometry = QRect(marginWidth, marginHeight, width - (marginWidth * 2), (height / 2) - marginHeight);
    mainGeometry = QRect(marginWidth, 0, width - (marginWidth * 2), height - marginHeight);

    this->mainLabel = new QLabel(this);
    this->mainLabel->setObjectName("displaySmall");
    QFont Mfont( "Arial", 10*fratio, QFont::Bold);
    this->mainLabel->setFont(Mfont);
    this->mainLabel->setAlignment(Qt::AlignCenter);
    this->mainLabel->setGeometry(mainGeometry);

    this->subLabelLeft = new QLabel(this);
    this->subLabelLeft->setObjectName("displaySmall");
    QFont SLfont( "Arial", 10*fratio, QFont::Bold);
    this->subLabelLeft->setFont(SLfont);
    this->subLabelLeft->setAlignment(Qt::AlignLeft);
    this->subLabelLeft->setGeometry(subGeometry);

    this->subLabelRight = new QLabel(this);
    this->subLabelRight->setObjectName("displaySmall");
    QFont SRfont( "Arial", 10*fratio, QFont::Bold);
    this->subLabelRight->setFont(SRfont);
    this->subLabelRight->setAlignment(Qt::AlignRight);
    this->subLabelRight->setGeometry(subGeometry);
}


void customLabelDisplay::setMainText(QString mainText, Qt::Alignment alignment)
{
	this->mainLabel->setText(mainText);
	this->mainLabel->setAlignment(alignment);
}

void customLabelDisplay::setSubText(QString subTextLeft, QString subTextRight)
{
	this->subLabelLeft->setText(subTextLeft);
	this->subLabelRight->setText(subTextRight);
}

void customLabelDisplay::clearAll()
{
	this->mainLabel->clear();
	this->subLabelLeft->clear();
	this->subLabelRight->clear();
}

void customLabelDisplay::setMainObjectName(QString name)
{
	this->mainLabel->setObjectName(name);
}

void customLabelDisplay::setSubObjectName(QString name)
{
	this->subLabelLeft->setObjectName(name);
	this->subLabelRight->setObjectName(name);
}

void customLabelDisplay::setAllColor(QColor color)
{
	QString red, green, blue;;
	red = QString::number(color.red(), 10);
	green = QString::number(color.green(), 10);
	blue = QString::number(color.blue(), 10);
	this->mainLabel->setStyleSheet("color: rgb("+ red +","+ green +","+ blue +");");
	this->subLabelLeft->setStyleSheet("color: rgb("+ red +","+ green +","+ blue +");");
	this->subLabelRight->setStyleSheet("color: rgb("+ red +","+ green +","+ blue +");");
}

void customLabelDisplay::resetAllColor()
{
	this->mainLabel->setPalette(this->mainPal);
	this->subLabelLeft->setPalette(this->subPal);
	this->subLabelRight->setPalette(this->subPal);
}

