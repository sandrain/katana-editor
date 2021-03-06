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

#ifndef SUMMARYDIALOGSYSTEM_H
#define SUMMARYDIALOGSYSTEM_H

#include <QDialog>
#include <QMainWindow>
#include <QTextEdit>
#include <QTextDocument>
#include <QLabel>
#include <QPrintDialog>
#include <QPrinter>
#include "SysxIO.h"

class QListWidget;
class QListWidgetItem;
class QStackedWidget;

class summaryDialogSystem : public QWidget
{
    Q_OBJECT

public:
    summaryDialogSystem(QWidget *parent = 0);
    ~summaryDialogSystem();
    QTextEdit *textDialog;
    QTextDocument *document;
    //QPrintDialog *dialog;
    QPushButton *cancelButton;
    QPushButton *printButton;
    QPushButton *saveAsButton;

public slots:
    void systemReply(QString replyMsg);
    void cancel();
    void printFile();
    void saveAs();
    void makeList();
    void makeMidiMapList();

signals:  
    void setStatusSymbol(int value);
    void setStatusProgress(int value);
    void setStatusMessage(QString message);

private:
    QString text;
    int start;
    int finish;
    QString address1;
    QString address2;
};

#endif //summaryDialogSystem
