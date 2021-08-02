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

#include "customParaEQGraph.h"
#include <QPainter>
#include <QMouseEvent>
#include <QGraphicsPathItem>
#include "Preferences.h"

customParaEQGraph::customParaEQGraph (QWidget *parent) //: /*m_poly(5), */loMid(11), m_iDragNode(-1)
{
    Q_UNUSED(parent);
    QFrame::setFrameShape(QFrame::Panel);
    QFrame::setFrameShadow(QFrame::Sunken);
}

customParaEQGraph::~customParaEQGraph (void)
{
}

void customParaEQGraph::setLowCut ( unsigned short iLowCut )
{
    //if (iLowCut > 9) iLowCut = 9;
    if (m_iLowCut != iLowCut) {
        m_iLowCut  = iLowCut;
        update();
        emit LowCutChanged(LowCut());
    };
}

unsigned short customParaEQGraph::LowCut (void) const
{
    return m_iLowCut;
}

void customParaEQGraph::setLowGain ( unsigned short iLowGain )
{
    //if (iLowGain > 40) iLowGain = 40;
    if (m_iLowGain != iLowGain) {
        m_iLowGain  = iLowGain;
        update();
        emit LowGainChanged(LowGain());
    };
}

unsigned short customParaEQGraph::LowGain (void) const
{
    return m_iLowGain;
}

void customParaEQGraph::setLowMidFreq ( unsigned short iLowMidFreq )
{
    //if (iLowMidFreq > 27) iLowMidFreq = 27;
    if (m_iLowMidFreq != iLowMidFreq) {
        m_iLowMidFreq  = iLowMidFreq;
        update();
        emit LowMidFreqChanged(LowMidFreq());
    };
}

unsigned short customParaEQGraph::LowMidFreq (void) const
{
    return m_iLowMidFreq;
}

void customParaEQGraph::setLowMidQ ( unsigned short iLowMidQ )
{
    //if (iLowMidQ > 5) iLowMidQ = 5;
    if (m_iLowMidQ != iLowMidQ) {
        m_iLowMidQ  = iLowMidQ;
        update();
        emit LowMidQChanged(LowMidQ());
    }
}

unsigned short customParaEQGraph::LowMidQ (void) const
{
    return m_iLowMidQ;
}

void customParaEQGraph::setLowMidGain ( unsigned short iLowMidGain )
{
    //if (iLowMidGain > 40) iLowMidGain = 40;
    if (m_iLowMidGain != iLowMidGain) {
        m_iLowMidGain  = iLowMidGain;
        update();
        emit LowMidGainChanged(LowMidGain());
    }
}

unsigned short customParaEQGraph::LowMidGain (void) const
{
    return m_iLowMidGain;
}

void customParaEQGraph::setHighMidFreq ( unsigned short iHighMidFreq )
{
    //if (iHighMidFreq > 27) iHighMidFreq = 27;
    if (m_iHighMidFreq != iHighMidFreq) {
        m_iHighMidFreq  = iHighMidFreq;
        update();
        emit HighMidFreqChanged(HighMidFreq());
    }
}

unsigned short customParaEQGraph::HighMidFreq (void) const
{
    return m_iHighMidFreq;
}

void customParaEQGraph::setHighMidQ ( unsigned short iHighMidQ )
{
    //if (iHighMidQ > 5) iHighMidQ = 5;
    if (m_iHighMidQ != iHighMidQ) {
        m_iHighMidQ  = iHighMidQ;
        update();
        emit HighMidQChanged(HighMidQ());
    }
}

unsigned short customParaEQGraph::HighMidQ (void) const
{
    return m_iHighMidQ;
}


void customParaEQGraph::setHighMidGain ( unsigned short iHighMidGain )
{
    //if (iHighMidGain > 40) iHighMidGain = 40;
    if (m_iHighMidGain != iHighMidGain) {
        m_iHighMidGain  = iHighMidGain;
        update();
        emit HighMidGainChanged(HighMidGain());
    }
}

unsigned short customParaEQGraph::HighMidGain (void) const
{
    return m_iHighMidGain;
}

void customParaEQGraph::setHighGain ( unsigned short iHighGain )
{
    //if (iHighGain > 40) iHighGain = 40;
    if (m_iHighGain != iHighGain) {
        m_iHighGain  = iHighGain;
        update();
        emit HighGainChanged(HighGain());
    };
}

unsigned short customParaEQGraph::HighGain (void) const
{
    return m_iHighGain;
}

void customParaEQGraph::setHighCut ( unsigned short iHighCut )
{
    //if (iHighCut > 9) iHighCut = 9;
    if (m_iHighCut != iHighCut) {
        m_iHighCut  = iHighCut;
        update();
        emit HighCutChanged(HighCut());
    };
}

unsigned short customParaEQGraph::HighCut (void) const
{
    return m_iHighCut;
}

void customParaEQGraph::setLevel ( unsigned short iLevel )
{
    //if (iLevel > 40) iLevel = 40;
    if (m_iLevel != iLevel) {
        m_iLevel  = iLevel;
        update();
        emit LevelChanged(Level());
    };
}

unsigned short customParaEQGraph::Level (void) const
{
    return m_iLevel;
}


void customParaEQGraph::paintEvent ( QPaintEvent *pPaintEvent )
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    QPixmap image = QPixmap(":/images/EQ_graph.png");
    QRectF target(0.0, 0.0, image.width()*110/100*ratio, image.height()*45/100*ratio);
    QRectF source(0.0, 0.0, image.width(), image.height());

    QPainter painter(this);
    painter.drawPixmap(target, image, source);

    int h  = height()/ratio;
    int w  = width()/ratio;

    int lc  = (m_iLowCut*22)+8;
    int lg  = h-(m_iLowGain*3)-12;

    int lmf = (m_iLowMidFreq*25)+5;
    int lmq = (w/3)-(m_iLowMidQ*40);
    int lmg = h/2-(m_iLowMidGain)+17;

    int hmf = (m_iHighMidFreq*25)+5;
    int hmq = (w/3)-(m_iHighMidQ*40);
    int hmg = h/2-(m_iHighMidGain)+17;

    int hg  = h - (m_iHighGain*3)-12;
    int hc  = w/2 + (m_iHighCut*25)-10;

    int lev = h/10 - (m_iLevel*2)-10;

    QLinearGradient grad(0, 0, w << 1, h << 1);
    grad.setColorAt(0.0f, Qt::yellow);
    grad.setColorAt(1.0f, Qt::black);
    painter.setPen(Qt::gray);
    painter.setBrush(grad);


    loMid.putPoints(0, 11,
                    lc-50,   h+lev+63,
                    lc,      lg+lev,

                    lmf-lmq, 110,
                    lmf,     (h-(m_iLowMidGain*7)+(lmg/2)),
                    lmf+lmq, 110,
                    w/2,       (h*50/78)+lev,
                    hmf-hmq, 110,
                    hmf,     (h-(m_iHighMidGain*7)+(hmg/2)),
                    hmf+hmq, 110,

                    hc,      hg+lev,
                    hc+60,   h+lev+63);

    QPainterPath loHiCut;
    loHiCut.moveTo(loMid.at(0));
    loHiCut.lineTo(loMid.at(1));
    loHiCut.quadTo(loMid.at(1), loMid.at(5));
    loHiCut.quadTo(loMid.at(5), loMid.at(9));
    loHiCut.lineTo(loMid.at(10));
    loHiCut.quadTo(loMid.at(9), loMid.at(5));
    loHiCut.quadTo(loMid.at(5), loMid.at(1));
    loHiCut.lineTo(loMid.at(0));

    QPainterPath para_1;
    para_1.moveTo(loMid.at(2));
    para_1.cubicTo(loMid.at(2), loMid.at(3), loMid.at(4));
    para_1.cubicTo(loMid.at(4), loMid.at(3), loMid.at(2));

    QPainterPath para_2;
    para_2.moveTo(loMid.at(6));
    para_2.cubicTo(loMid.at(6), loMid.at(7), loMid.at(8));
    para_2.cubicTo(loMid.at(8), loMid.at(7), loMid.at(6));
    
    painter.setPen(Qt::black);
    QPen pen;
    pen.setWidth(3);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QPainterPath graph;
    graph.moveTo(0, h);
    bool intersect = false;
    int x = 0;
    signed int a = 0;
    signed int b = 0;
    signed int c = 0;
    for(x=0; x<700; ++x)
    {
        for( a=0; a<220&&intersect!=true; ++a)
        {
            intersect = loHiCut.intersects(QRectF(x, a, 1, 1));
        };
        intersect = false;

        for( b=0; b<220&&intersect!=true; ++b)
        {
            intersect = para_1.intersects(QRectF(x, b, 1, 1));
        };
        if(b>=220){b=110;};
        intersect = false;

        for( c=0; c<220&&intersect!=true; ++c)
        {
            intersect = para_2.intersects(QRectF(x, c, 1, 1));
        };
        if(c>=220) {c=110;};
        intersect = false;
        graph.lineTo(x, a+b+c-220 );
        x=x+5;
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
void customParaEQGraph::mousePressEvent ( QMouseEvent *pMouseEvent )
{
    Q_UNUSED(pMouseEvent);
}


void customParaEQGraph::mouseMoveEvent ( QMouseEvent *pMouseEvent )
{
    Q_UNUSED(pMouseEvent);
}


void customParaEQGraph::mouseReleaseEvent ( QMouseEvent *pMouseEvent )
{
    Q_UNUSED(pMouseEvent);
}

int customParaEQGraph::nodeIndex ( const QPoint& pos ) const
{
    Q_UNUSED(pos);
    return -1;
}

void customParaEQGraph::dragNode ( const QPoint& pos )
{
    Q_UNUSED(pos);
}

void customParaEQGraph::updateSlot(  QString hex_1, QString hex_2, QString hex_3,
                                     QString hex_4, QString hex_5, QString hex_6,
                                     QString hex_7, QString hex_8, QString hex_9,
                                     QString hex_10, QString hex_11)
{
    this->hex_1 = hex_1;
    this->hex_2 = hex_2;
    this->hex_3 = hex_3;
    this->hex_4 = hex_4;
    this->hex_5 = hex_5;
    this->hex_6 = hex_6;
    this->hex_7 = hex_7;
    this->hex_8 = hex_8;
    this->hex_9 = hex_9;
    this->hex_10 = hex_10;
    this->hex_11 = hex_11;
    bool ok;
    setLowCut(this->hex_1.toShort(&ok, 16));
    setLowGain(this->hex_2.toShort(&ok, 16));
    setLowMidFreq(this->hex_3.toShort(&ok, 16));
    setLowMidQ(this->hex_4.toShort(&ok, 16));
    setLowMidGain(this->hex_5.toShort(&ok, 16));
    setHighMidFreq(this->hex_6.toShort(&ok, 16));
    setHighMidQ(this->hex_7.toShort(&ok, 16));
    setHighMidGain(this->hex_8.toShort(&ok, 16));
    setHighGain(this->hex_9.toShort(&ok, 16));
    setHighCut(this->hex_10.toShort(&ok, 16));
    setLevel(this->hex_11.toShort(&ok, 16));

}






