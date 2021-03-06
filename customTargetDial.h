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

#ifndef CUSTOMTARGETDIAL_H
#define CUSTOMTARGETDIAL_H

#include <QWidget>

class customTargetDial : public QWidget
{
    Q_OBJECT

public:
    customTargetDial(
		double value = 50, 
		double min = 0, 
		double max = 100, 
		double single = 1, 
		double page = 10,
                QPoint dialPos = QPoint(0, 0),
		QWidget *parent = 0,
		QString hex1 = "void",
		QString hex2 = "void",
		QString hex3 = "void",
		QString imagePath = ":/images/stompknob.png",
		unsigned int imageRange = 62,
        QString background = "void",
        QString root = "Structure");
	void setValue(int value);

public slots:
	void knobSignal(QString, QString, QString);

signals:
	void valueChanged(int newValue, QString hex1, QString hex2, QString hex3);

protected:
   void paintEvent(QPaintEvent *event);
   void mouseMoveEvent(QMouseEvent *event);
   void mousePressEvent(QMouseEvent *event);
   void wheelEvent(QWheelEvent *event);
   void keyPressEvent(QKeyEvent *event);

private:
	void setOffset(double value);
	void emitValue(double value);
    QString area;
	QString hex1;
	QString hex2;
	QString hex3;
	double value; 
	double min; 
	double max; 
	double single; 
	double page;
	QString imagePath;
	QSize dialSize;
	unsigned int imageRange;
	QPoint dialPos; 
	signed int xOffset;
	double _lastValue;
	QPoint _startpos;
	QPoint _lastpos;
    QString background;
	double m_value;
};

#endif // CUSTOMTARGETDIAL_H
