/****************************************************************************
**
** Copyright (C) 2007~2018 Colin Willcocks.
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

#ifndef CUSTOMGRYBUTTON_H
#define CUSTOMGRYBUTTON_H

#include <QWidget>

class customGRYButton : public QWidget
{
    Q_OBJECT

public:
    customGRYButton(
        int active = 0,
		QWidget *parent = 0,
		QString hex1 = "void",
		QString hex2 = "void",
		QString hex3 = "void",
        QString imagePath = ":/images/switchGRY.png");
    void setValue(int value);

signals:
    void valueChanged(int newValue, QString hex1, QString hex2, QString hex3);

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

private:
	void setOffset(signed int imageNr);
    void emitValue(int value);

	QString hex1;
	QString hex2;
	QString hex3;
    int value;
	bool active;
    int m_value;
	QString imagePath;
	QSize switchSize;
	unsigned int imageRange;
	QPoint switchPos; 
	QPoint dragStartPosition;
	signed int yOffset;
};

#endif // CUSTOMSWITCH_H
