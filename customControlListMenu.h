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

#ifndef CUSTOMCONTROLLISTMENU_H
#define CUSTOMCONTROLLISTMENU_H

#include <QWidget>
#include <QtWidgets>
#include "customControlLabel.h"
#include "customComboBox.h"

class customControlListMenu : public QWidget
{
    Q_OBJECT

public:
    customControlListMenu(QWidget *parent = 0, 
		QString hex1 = "void",
		QString hex2 = "void",
		QString hex3 = "void",
		QString direction = "bottom");
		
		customComboBox* controlListComboBox;

protected:
	void paintEvent(QPaintEvent *event);

public slots:
	void valueChanged(int index);
	void dialogUpdateSignal();
    void set_index(int index);

signals:
	void updateSignal();
	void updateDisplay(QString text);
	void currentIndexChanged(int index);
    void mouseReleasedSignal();

private:
	void setComboBox();
	customControlLabel* label;
	QString hex1;
	QString hex2;
	QString hex3;
	QString area;
    QString direction;
	int comboWidth;
};

#endif // CUSTOMCONTROLLISTMENU_H
