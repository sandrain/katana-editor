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

#ifndef CUSTOMASSIGN_H
#define CUSTOMASSIGN_H

#include <QWidget>
#include <QTimer>

class customAssign : public QWidget
{
    Q_OBJECT

public:
    customAssign(
		bool active = false,
		QPoint ledPos = QPoint(0, 0), 
		QWidget *parent = 0,
    QString imagePath = ":/images/assignBar.png");
	void setValue(bool value);

public slots:
	void changeValue(bool value);

private slots:
    void fadeIn();
    void fadeOut();

protected:
   void paintEvent(QPaintEvent *event);

private:
  void setOffset(signed int imageNr);
	bool active;
	QString imagePath;
	QSize ledSize;
	QPoint ledPos; 
	signed int yOffset;
  QTimer* timer;
};

#endif // CUSTOMSTRUCTURE_H
