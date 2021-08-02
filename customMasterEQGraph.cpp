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

#include "customMasterEQGraph.h"
#include <QPainter>
#include <QMouseEvent>
#include <QGraphicsPathItem>
#include "Preferences.h"

customMasterEQGraph::customMasterEQGraph (QWidget *parent)
{
    Q_UNUSED(parent);
	QFrame::setFrameShape(QFrame::Panel);
        QFrame::setFrameShadow(QFrame::Sunken);

}

customMasterEQGraph::~customMasterEQGraph (void)
{

}

void customMasterEQGraph::setLowGain ( unsigned short iLowGain )
{
     //if (iLowGain > 24) iLowGain = 24;
        if (m_iLowGain != iLowGain) {
                m_iLowGain  = iLowGain;
                update();
                emit LowGainChanged(LowGain());
        };
}

unsigned short customMasterEQGraph::LowGain (void) const
{
        return m_iLowGain;
}


void customMasterEQGraph::setMidFreq ( unsigned short iMidFreq )
{
     //if (iMidFreq > 27) iMidFreq = 27;
        if (m_iMidFreq != iMidFreq) {
                m_iMidFreq  = iMidFreq;
                update();
                emit MidFreqChanged(MidFreq());
        };
}

unsigned short customMasterEQGraph::MidFreq (void) const
{
        return m_iMidFreq;
}


void customMasterEQGraph::setMidQ ( unsigned short iMidQ )
{
    //if (iMidQ > 5) iMidQ = 5;
        if (m_iMidQ != iMidQ) {
                m_iMidQ  = iMidQ;
                update();
                emit MidQChanged(MidQ());
        }
}

unsigned short customMasterEQGraph::MidQ (void) const
{
        return m_iMidQ;
}


void customMasterEQGraph::setMidGain ( unsigned short iMidGain )
{
    //if (iMidGain > 24) iMidGain = 24;
        if (m_iMidGain != iMidGain) {
                m_iMidGain  = iMidGain;
                update();
                emit MidGainChanged(MidGain());
        }
}

unsigned short customMasterEQGraph::MidGain (void) const
{
        return m_iMidGain;
}

void customMasterEQGraph::setHighGain ( unsigned short iHighGain )
{
    //if (iHighGain > 24) iHighGain = 24;
        if (m_iHighGain != iHighGain) {
                m_iHighGain  = iHighGain;
                update();
                emit HighGainChanged(HighGain());
        };
}

unsigned short customMasterEQGraph::HighGain (void) const
{
        return m_iHighGain;
}

void customMasterEQGraph::setLevel ( unsigned short iLevel )
{
    //if (iLevel > 100) iLevel = 100;
        if (m_iLevel != iLevel) {
                m_iLevel  = iLevel;
                update();
                emit LevelChanged(Level());
        };
}

unsigned short customMasterEQGraph::Level (void) const
{
        return m_iLevel;
}


void customMasterEQGraph::paintEvent ( QPaintEvent *pPaintEvent )
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    QPixmap image = QPixmap(":images/EQ_graph.png");
    QRectF target(0.0, 0.0, image.width()*85/100, image.height()*43/100);
    QRectF source(0.0, 0.0, image.width(), image.height());

    QPainter painter(this);
    painter.drawPixmap(target, image, source);

    int h   =     height();
    int w   =     width();
    
    int lg  = h - (m_iLowGain*2)+5;
    
    int mf  =     (m_iMidFreq*12) + 12;
    int mq  = h - (m_iMidQ*18)-80;
    int mg  = h - (m_iMidGain)-29;
    
    int hg  = h - (m_iHighGain*2)+5;
    
    int lev = 80 - (m_iLevel);

    QLinearGradient grad(0, 0, w << 1, h << 1);
    grad.setColorAt(0.0f, Qt::yellow);
    grad.setColorAt(1.0f, Qt::black);

    painter.setBrush(grad);
                // horizonal,    vertical

    poly.putPoints(0,           11,
                   0,           h,                       // [0]
                   0,          (lg)+lev-93,              // [1]
                   (w/10)*2,    lg+lev-93,               // [2]
                   (w/10)*4,    lev+52,                  // [3]
                   mf-mq,       mg+m_iMidGain-40,        // [4]
                   mf,         (270)-(m_iMidGain*6)-40,  // [5]
                   mf+mq,       mg+m_iMidGain-40,        // [6]
                   (w/10)*6,    lev+52,                  // [7]
                   (w/10)*8,   (hg+lev-93),              // [8]
                   w+5,          (hg)+lev-93,            // [9]
                   w+5,           h  );                  // [10]

    QPainterPath loHi;
    loHi.moveTo(poly.at(1));
    loHi.cubicTo(poly.at(1), poly.at(2), poly.at(3));
    loHi.lineTo(poly.at(7));
    loHi.cubicTo(poly.at(7), poly.at(8), poly.at(9));
    loHi.cubicTo(poly.at(9), poly.at(8), poly.at(7));
    loHi.lineTo(poly.at(3));
    loHi.cubicTo(poly.at(3), poly.at(2), poly.at(1));

    QPainterPath Mid;
    Mid.moveTo(poly.at(4));
    Mid.cubicTo(poly.at(4), poly.at(5), poly.at(6));
    Mid.cubicTo(poly.at(6), poly.at(5), poly.at(4));

    painter.setPen(Qt::black);
    QPen pen;
    pen.setWidth(3);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    QPainterPath graph;
    graph.moveTo(0, h);
    bool intersect = false;
    int x = 0;
    int a = 0;
    int b = 0;
    int c = 0;
    for(x=0; x<400; ++x)
    {
        for( a=0; a<220&&intersect!=true; ++a)
        {
            intersect = loHi.intersects(QRectF(x, a, 1, 1));
        };
        intersect = false;

        for( b=0; b<220&&intersect!=true; ++b)
        {
            intersect = Mid.intersects(QRectF(x, b, 1, 1));
        };
        if(b>=220){b=110;};
        intersect = false;
        c = a+b-100;
        graph.lineTo(x, c);
        x=x+4;
    };

    graph.lineTo(w, h);
    painter.scale(ratio, ratio);
    painter.setOpacity(0.6);
    painter.setBrush(grad);
    painter.drawPath(graph);
    painter.end();

    QFrame::paintEvent(pPaintEvent);
}


// Mouse interaction.
void customMasterEQGraph::mousePressEvent ( QMouseEvent *pMouseEvent )
{
    Q_UNUSED(pMouseEvent);
}

void customMasterEQGraph::mouseMoveEvent ( QMouseEvent *pMouseEvent )
{
    Q_UNUSED(pMouseEvent);
}


void customMasterEQGraph::mouseReleaseEvent ( QMouseEvent *pMouseEvent )
{
    Q_UNUSED(pMouseEvent);
}

// Draw rectangular point.
QRect customMasterEQGraph::nodeRect ( int iNode ) const
{
        const QPoint& pos = poly.at(iNode);
        return QRect(pos.x() - 4, pos.y() - 4, 8, 8);
}

int customMasterEQGraph::nodeIndex ( const QPoint& pos ) const
{     
    Q_UNUSED(pos);
    return -1;
}

void customMasterEQGraph::dragNode ( const QPoint& pos )
{
    Q_UNUSED(pos);
}

void customMasterEQGraph::updateSlot(  QString hex_1, QString hex_2, QString hex_3,
                               QString hex_4, QString hex_5, QString hex_6 )
{
    this->hex_1 = hex_1;
    this->hex_2 = hex_2;
    this->hex_3 = hex_3;
    this->hex_4 = hex_4;
    this->hex_5 = hex_5;
    this->hex_6 = hex_6;
    bool ok;
    setLowGain(this->hex_1.toShort(&ok, 16));
    setMidFreq(this->hex_2.toShort(&ok, 16));
    setMidQ(this->hex_3.toShort(&ok, 16));
    setMidGain(this->hex_4.toShort(&ok, 16));
    setHighGain(this->hex_5.toShort(&ok, 16));
    setLevel(this->hex_6.toShort(&ok, 16));

}

