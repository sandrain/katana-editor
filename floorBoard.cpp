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

#include <QList>
#include <QVector>

#include "dragBar.h"
#include "bankTreeList.h"
#include "floorBoardDisplay.h"
#include "floorPanelBar.h"
#include "floorBoard.h"
#include "Preferences.h"
#include "MidiTable.h"
#include "sysxWriter.h"
#include "SysxIO.h"
#include "globalVariables.h"

#include "menuPage_midi.h"
#include "menuPage_system.h"
#include "menuPage_master.h"
#include "menuPage_ez_edit.h"
#include "menuPage_assign1.h"
#include "menuPage_assign2.h"
#include "menuPage_assign3.h"
#include "menuPage_assign4.h"
#include "menuPage_assign5.h"
#include "menuPage_assign6.h"
#include "menuPage_assign7.h"
#include "menuPage_assign8.h"
#include "menuPage_assign9.h"
#include "menuPage_assign10.h"
#include "menuPage_assign11.h"
#include "menuPage_assign12.h"
#include "menuPage_assign13.h"
#include "menuPage_assign14.h"
#include "menuPage_assign15.h"
#include "menuPage_assign16.h"

#include "stompbox_fx1.h"
#include "stompbox_ch_a.h"
#include "stompbox_lp.h"
#include "stompbox_od.h"
#include "stompbox_fx2.h"
#include "stompbox_dd.h"
#include "stompbox_rv.h"
#include "stompbox_fv.h"
#include "stompbox_ns_1.h"
#include "stompbox_eq.h"
#include "stompbox_ce.h"
#include "stompbox_pdl.h"


floorBoard::floorBoard(QWidget *parent,
                       QString imagePathFloor,
                       QString imagePathStompBG,
                       QString imagePathInfoBar,
                       unsigned int marginStompBoxesTop,
                       unsigned int marginStompBoxesBottom,
                       unsigned int marginStompBoxesWidth,
                       unsigned int panelBarOffset,
                       unsigned int borderWidth,
                       QPoint pos)
    : QWidget(parent)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    this->imagePathFloor = imagePathFloor;
    this->imagePathStompBG = imagePathStompBG;
    this->imagePathInfoBar = imagePathInfoBar;
    
    marginStompBoxesTop = 235*ratio;
    marginStompBoxesBottom = 105*ratio;
    marginStompBoxesWidth = 35*ratio;
    panelBarOffset = 5*ratio;
    borderWidth = 3*ratio;

    this->marginStompBoxesTop = marginStompBoxesTop;
    this->marginStompBoxesBottom = marginStompBoxesBottom;
    this->marginStompBoxesWidth = marginStompBoxesWidth;
    this->panelBarOffset = panelBarOffset;
    this->borderWidth = borderWidth;
    this->pos = pos;

    bankTreeList *bankList = new bankTreeList(this);

    setFloorBoard();

    floorBoardDisplay *display = new floorBoardDisplay(this);
    display->setPos(displayPos);

    floorPanelBar *panelBar = new floorPanelBar(this);
    panelBar->setPos(panelBarPos);

    dragBar *bar = new dragBar(this);
    bar->setDragBarSize(QSize(4*ratio, panelBar->height()*ratio ));
    bar->setDragBarMinOffset(2*ratio, 8*ratio);
    bar->setDragBarMaxOffset(offset - panelBarOffset + (5*ratio));

    initStomps();
    initMenuPages();
    this->editDialog = new editWindow(this);
    this->editDialog->hide();
    this->oldDialog = this->editDialog;

    SysxIO *sysxIO = SysxIO::Instance();
    QObject::connect(sysxIO, SIGNAL(updateSignal()), this, SIGNAL(updateSignal()));

    QObject::connect(this, SIGNAL( resizeSignal(QRect) ), bankList, SLOT( updateSize(QRect) ) );
    QObject::connect(display, SIGNAL(connectedSignal()), bankList, SLOT(connectedSignal()));
    QObject::connect(this, SIGNAL(valueChanged(QString, QString, QString)), display, SLOT(setValueDisplay(QString, QString, QString)));
    QObject::connect(panelBar, SIGNAL(resizeSignal(int)), this, SLOT(setWidth(int)));
    QObject::connect(panelBar, SIGNAL(collapseSignal()), this, SLOT(setCollapse()));
    QObject::connect(this, SIGNAL(setCollapseState(bool)), panelBar, SIGNAL(collapseState(bool)));
    QObject::connect(this, SIGNAL(setDisplayPos(QPoint)), display, SLOT(setPos(QPoint)));
    QObject::connect(this, SIGNAL(setFloorPanelBarPos(QPoint)), panelBar, SLOT(setPos(QPoint)));
    QObject::connect(this->parent(), SIGNAL(updateSignal()), this, SIGNAL(updateSignal()));
    QObject::connect(this, SIGNAL(updateSignal()), this, SLOT(updateStompBoxes()));
    QObject::connect(bankList, SIGNAL(patchSelectSignal(int, int)), display, SLOT(patchSelectSignal(int, int)));
    QObject::connect(bankList, SIGNAL(patchLoadSignal(int, int)), display, SLOT(patchLoadSignal(int, int)));
    QObject::connect(this->parent(), SIGNAL(loadnames()), bankList, SLOT(updateLoaderNames()));
    QObject::connect(display, SIGNAL(loadnames()), bankList, SLOT(updateLoaderNames()));

    QObject::connect(panelBar, SIGNAL(showDragBar(QPoint)), this, SIGNAL(showDragBar(QPoint)));
    QObject::connect(panelBar, SIGNAL(hideDragBar()), this, SIGNAL(hideDragBar()));

    QObject::connect(this, SIGNAL(chain_1_buttonSignal(bool)), this, SLOT(chain_1_Set(bool)));
    QObject::connect(this, SIGNAL(chain_2_buttonSignal(bool)), this, SLOT(chain_2_Set(bool)));
    QObject::connect(this, SIGNAL(chain_3_buttonSignal(bool)), this, SLOT(chain_3_Set(bool)));
    QObject::connect(this, SIGNAL(chain_4_buttonSignal(bool)), this, SLOT(chain_4_Set(bool)));
    QObject::connect(this, SIGNAL(chain_5_buttonSignal(bool)), this, SLOT(chain_5_Set(bool)));
    QObject::connect(this, SIGNAL(chain_6_buttonSignal(bool)), this, SLOT(chain_6_Set(bool)));
    QObject::connect(this, SIGNAL(updateSignal()), this, SLOT(chainSet()));

    QString collapseState = preferences->getPreferences("Window", "Collapsed", "bool");
    QString width = preferences->getPreferences("Window", "Collapsed", "width");
    QString defaultwidth = preferences->getPreferences("Window", "Collapsed", "defaultwidth");
    if(width.isEmpty()){ width = defaultwidth; }

    this->l_floorSize = QSize(width.toInt(&ok, 10), floorSize.height());

    if(preferences->getPreferences("Window", "Restore", "sidepanel")=="true")
    {
        if(collapseState=="true")
        {
            this->setSize(l_floorSize);
            this->colapseState = true;
            emit setCollapseState(true);
        }
        else
        {
            this->setSize(minSize);
            this->colapseState = false;
            emit setCollapseState(false);
        };
    }
    else
    {
        this->setSize(minSize);
        this->colapseState = false;
        emit setCollapseState(false);
    };
    emit updateSignal();
}

floorBoard::~floorBoard()
{
    //emit notConnected();
    Preferences *preferences = Preferences::Instance();
    if(preferences->getPreferences("Window", "Restore", "sidepanel")=="true")
    {
        preferences->setPreferences("Window", "Collapsed", "width", QString::number(this->l_floorSize.width(), 10));
    }
    else
    {
        preferences->setPreferences("Window", "Collapsed", "width", "");
    };
    preferences->setPreferences("Window", "Collapsed", "bool", QString(this->colapseState?"true":"false"));
    preferences->savePreferences();
}

void floorBoard::paintEvent(QPaintEvent *)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    QRectF target(pos.x(), pos.y(), floorSize.width()*ratio, floorSize.height()*ratio);
    QRectF source(0.0, 0.0, floorSize.width(), floorSize.height());

    QPainter painter(this);
    painter.drawPixmap(target, image, source);
    this->setAcceptDrops(true);
}

void floorBoard::setFloorBoard() 
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    QPixmap imageFloor(imagePathFloor);
    QPixmap imagestompBG(imagePathStompBG);
    QPixmap imageInfoBar(imagePathInfoBar);
    QPixmap buffer = imageFloor;
    QPainter painter(&buffer);

    this->offset = imageFloor.width()*ratio - imageInfoBar.width()*ratio;
    this->infoBarWidth = imageInfoBar.width();
    this->stompSize = imagestompBG.size()*ratio;
    this->infoBarHeight = imageInfoBar.height()*ratio;

    initSize(imageFloor.size());
    this->maxSize = floorSize;
    this->minSize = QSize(imageInfoBar.width() + borderWidth + panelBarOffset, imageFloor.height());

    // Draw InfoBar
    QRectF sourceInfoBar(0.0, 0.0, imageInfoBar.width(), imageInfoBar.height());
    QRectF targetInfoBar(offset, 0.0, imageInfoBar.width(), imageInfoBar.height());
    QRectF targetInfoBar2(offset, imageInfoBar.height()-4, imageInfoBar.width(), imageInfoBar.height());
    QRectF targetInfoBar3(offset, (imageInfoBar.height()*2)-8, imageInfoBar.width(), imageInfoBar.height()/2);
    painter.drawPixmap(targetInfoBar, imageInfoBar, sourceInfoBar);
    painter.drawPixmap(targetInfoBar2, imageInfoBar, sourceInfoBar);
    painter.drawPixmap(targetInfoBar3, imageInfoBar, sourceInfoBar);

    // Draw LiberianBar
    QRectF sourceLiberianBar(0.0, 0.0, imageInfoBar.width(), imageInfoBar.height());
    QRectF targetLiberianBar(offset, (imageFloor.height()*ratio - imageInfoBar.height()*ratio) - (2*ratio), imageInfoBar.width()*ratio, imageInfoBar.height()*ratio);
    QRectF targetLiberianBar2(offset, (imageFloor.height()*ratio - (imageInfoBar.height()*2*ratio)+(2*ratio)), imageInfoBar.width()*ratio, imageInfoBar.height()*ratio);
    painter.drawPixmap(targetLiberianBar, imageInfoBar, sourceLiberianBar);
    painter.drawPixmap(targetLiberianBar2, imageInfoBar, sourceLiberianBar);

    // Draw stomp boxes background
    QRectF source(0.0, 0.0, imagestompBG.width(), imagestompBG.height());
    for(int i=0;i<fxPos.size();i++)
    {
        QRectF target(fxPos.at(i).x()/ratio, fxPos.at(i).y()/ratio, imagestompBG.width(), imagestompBG.height());
        painter.drawPixmap(target, imagestompBG, source);
    };
    painter.end();

    this->baseImage = buffer;
    this->image = buffer;
    this->floorHeight = imageFloor.height()*ratio;

    QPoint newPanelBarPos = QPoint(offset - panelBarOffset, borderWidth);
    this->panelBarPos = newPanelBarPos;

    QPoint newDisplayPos = QPoint(offset, 0);
    this->displayPos = newDisplayPos;

    QPoint newLiberainPos = QPoint(offset, floorHeight);
    this->liberainPos = newLiberainPos;

    QRect newBankListRect = QRect(borderWidth, borderWidth, offset - panelBarOffset - (borderWidth*2), floorHeight - (borderWidth*2));
    emit resizeSignal(newBankListRect);
}

void floorBoard::dragEnterEvent(QDragEnterEvent *event)
{
    QByteArray data = event->mimeData()->data("text/uri-list");
    QString uri(data);

    if ((event->mimeData()->hasFormat("application/x-stompbox")) ||
            (uri.contains(".syx", Qt::CaseInsensitive) &&
             event->answerRect().intersects(this->geometry())))
    {
        if (children().contains(event->source()))
        {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        };
    } else {
        event->ignore();
    };
}

void floorBoard::dragMoveEvent(QDragMoveEvent *event)
{
    QByteArray data = event->mimeData()->data("text/uri-list");
    QString uri(data);

    if (( event->mimeData()->hasFormat("application/x-stompbox")) ||
            (uri.contains(".syx", Qt::CaseInsensitive) &&
             event->answerRect().intersects(this->geometry())))
    {
        if (children().contains(event->source()))
        {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            //event->acceptProposedAction();
        };
    } else {
        event->ignore();
    };
}

void floorBoard::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-stompbox"))
    {
        QByteArray itemData = event->mimeData()->data("application/x-stompbox");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        int stompId;
        QPoint stompPos;
        //QSize stompSize;
        QPoint topLeftOffset;
        dataStream >> stompId >> stompPos >> stompSize >> topLeftOffset;
        QPoint dragPoint = (event->pos() - topLeftOffset) + QPoint(stompSize.width()/2, stompSize.height()/2);
        int stompSpacing = fxPos.at(1).x() - (fxPos.at(0).x() + stompSize.width());

        int destIndex = -1; // Set to out of range by default.
        int orgIndex = fx.indexOf(stompId);
        for(int x=0;x<fx.size();x++)
        {
            QRect dropRect = QRect(fxPos.at(x).x() - stompSpacing - (stompSize.width()/2), fxPos.at(x).y(), stompSize.width() + stompSpacing, stompSize.height());
            QRect lastDropRect = QRect(fxPos.at(x).x() + (stompSize.width()/2), fxPos.at(x).y(), stompSize.width() + stompSpacing, stompSize.height());
            if( dropRect.contains(dragPoint) )
            {
                destIndex = fx.indexOf(fx.at(x));
            }
            else if( (x == (int)fx.size()-1 ||
                      ( x == (int)(fx.size()/2)-1 && fx.at(stompId) != fx.size()/2 ) )
                     && lastDropRect.contains(dragPoint))
            {
                destIndex = fx.indexOf(fx.at(x)) + 1;
            };
        };


        if((destIndex > -1 && destIndex < fx.size() + 1))
            // Make sure we are not dropping the stomp out of range.
        {
            if( orgIndex < destIndex )
            {
                destIndex = destIndex - 1;
                for(int i=orgIndex;i<destIndex ;i++)
                {
                    setStompPos(fx.at(i + 1), i);
                };
                setStompPos(stompId, destIndex );
            }
            else if( orgIndex > destIndex )
            {
                for(int i=orgIndex;i>destIndex;i--)
                {
                    setStompPos(fx.at(i - 1), i);
                };
                setStompPos(stompId, destIndex);
            };
            QString text;
            if(orgIndex != destIndex) // Prevent sending data when stomp was dropped in the same place.
            {
                SysxIO *sysxIO = SysxIO::Instance();
                QList<QString> hexData;
                for(int index=0;index<fx.size();index++)
                {
                    QString fxHexValue = QString::number(fx.at(index), 16).toUpper();
                    if(fxHexValue.length() < 2) fxHexValue.prepend("0");
                    hexData.append(fxHexValue);
                    text.append(fxHexValue+"  ");
                };
            hexData.prepend("11");

            hexData.append("12");
            hexData.append("00");
            hexData.append("03");            
            hexData.append("0A");
            hexData.append("13");
            hexData.append("0E");
            hexData.append("10");

             sysxIO->setFileSource("Structure", "07", "00", "20", hexData);
             updateStompBoxes();
             chainSet();
             this->chain_1_statusSignal(false);
             this->chain_2_statusSignal(false);
             this->chain_3_statusSignal(false);

             if(!chain_message){
             QMessageBox *msgBox = new QMessageBox();
             msgBox->isTopLevel();
             msgBox->setWindowTitle(tr("Custom Chain order"));
             msgBox->setIcon(QMessageBox::Warning);
             msgBox->setTextFormat(Qt::RichText);
             QString msgText;
             msgText.append(tr("To save custom chain order,"));
             msgText.append(tr("<br>press and hold Katana panel channel button"));
             msgBox->setText(msgText);
             msgBox->show();
             QTimer::singleShot(3000, msgBox, SLOT(deleteLater()));
             chain_message = true;
             };
            };
        }
        else
        {
            event->ignore();
        };
    }
    else
    {
        if(event->mimeData()->hasFormat("text/uri-list"))
        {
            QByteArray data = event->mimeData()->data("text/uri-list");
            QString uri(data);
            if(uri.contains(".syx", Qt::CaseInsensitive))
            {
                QString removeFromStart = "file:///";
                QString removeFromEnd = ".syx";
                QString filePath = uri;
                filePath.replace(0, filePath.indexOf(removeFromStart) + removeFromStart.length(), "");
                filePath.truncate(filePath.indexOf(removeFromEnd) + removeFromEnd.length());
                filePath.replace("%20", " ");

                SysxIO *sysxIO = SysxIO::Instance();
                sysxIO->setFileName(filePath);
                sysxIO->setSyncStatus(false);
                sysxIO->setDevice(false);

                sysxWriter file;
                file.setFile(filePath);
                if(file.readFile())
                {
                    emit updateSignal();
                }
                else
                {
                    event->ignore();
                };
            }
            else
            {
                event->ignore();
            };
        }
        else
        {
            event->ignore();
        };
    };
}

void floorBoard::chain_1_Set(bool value)
{
    Q_UNUSED(value);
    SysxIO *sysxIO = SysxIO::Instance();
    sysxIO->setFileSource("Structure", "12", "00", "00", "00");
    QList<QString> hexData;
    hexData << "11" << "0B" << "02" << "0D" << "0C" << "04" << "01" << "0F" << "05" << "07" << "06" << "08" << "09" << "12" << "00" << "03" << "0A" << "13" << "0E" << "10";
    if(sysxIO->getSourceValue("Structure", "12", "00", "17")==1){hexData.removeOne("01"); hexData.insert(hexData.indexOf("12"), "01"); };  // S/R loop
    if(sysxIO->getSourceValue("Structure", "12", "00", "18")==0){hexData.removeOne("04"); hexData.insert(hexData.indexOf("02"), "04"); };  // EQ
    if(sysxIO->getSourceValue("Structure", "11", "00", "10")==1){hexData.removeOne("0B"); hexData.insert(hexData.indexOf("0D"), "0B"); };  // PDL
    sysxIO->setFileSource("Structure", "07", "00", "20", hexData);
    updateStompBoxes();
    chainSet();
}

void floorBoard::chain_2_Set(bool value)
{
    Q_UNUSED(value);
    SysxIO *sysxIO = SysxIO::Instance();
    sysxIO->setFileSource("Structure", "12", "00", "00", "01");
    QList<QString> hexData;
    hexData << "11"  << "0B" << "0F" << "05" << "02" << "0D" << "0C" << "04" << "01" << "07" << "06" << "08" << "09" << "12" << "00" << "03" << "0A" << "13"  << "0E"<< "10";
    if(sysxIO->getSourceValue("Structure", "12", "00", "17")==1){hexData.removeOne("01"); hexData.insert(hexData.indexOf("12"), "01"); };  // S/R loop
    if(sysxIO->getSourceValue("Structure", "12", "00", "18")==0){hexData.removeOne("04"); hexData.insert(hexData.indexOf("02"), "04"); };  // EQ
    if(sysxIO->getSourceValue("Structure", "11", "00", "10")==1){hexData.removeOne("0B"); hexData.insert(hexData.indexOf("0D"), "0B"); };  // PDL
    sysxIO->setFileSource("Structure", "07", "00", "20", hexData);
    updateStompBoxes();
    chainSet();
}

void floorBoard::chain_3_Set(bool value)
{
    Q_UNUSED(value);
    SysxIO *sysxIO = SysxIO::Instance();
    sysxIO->setFileSource("Structure", "12", "00", "00", "02");
    QList<QString> hexData;
    hexData << "11" << "0B" << "0F" << "05" << "07" << "06" << "02" << "0D" << "0C" << "04" << "01" << "08" << "09" << "12" << "00" << "03" << "0A" << "13" << "0E" << "10";
    if(sysxIO->getSourceValue("Structure", "12", "00", "17")==1){hexData.removeOne("01"); hexData.insert(hexData.indexOf("12"), "01"); };  // S/R loop
    if(sysxIO->getSourceValue("Structure", "12", "00", "18")==0){hexData.removeOne("04"); hexData.insert(hexData.indexOf("02"), "04"); };  // EQ
    if(sysxIO->getSourceValue("Structure", "11", "00", "10")==1){hexData.removeOne("0B"); hexData.insert(hexData.indexOf("0D"), "0B"); };  // PDL
    sysxIO->setFileSource("Structure", "07", "00", "20", hexData);
    updateStompBoxes();
    chainSet();
}

void floorBoard::chain_4_Set(bool value)  // SR loop
{
    Q_UNUSED(value);
    SysxIO *sysxIO = SysxIO::Instance();
    MidiTable *midiTable = MidiTable::Instance();
    QList<QString> fxChain = sysxIO->getFileSource("Structure", "07", "00");
    QList<QString> hexData;
    for(int i= sysxDataOffset+32;i< (sysxDataOffset+32 + 20);i++ )
    {
        hexData.append( midiTable->getMidiMap("Structure", "07", "00", "20", fxChain.at(i)).value );
    };
    int val = 1-sysxIO->getSourceValue("Structure", "12", "00", "18");
    if(sysxIO->getSourceValue("Structure", "12", "00", "17")==0)
    {
      hexData.removeOne("01"); hexData.insert(hexData.indexOf("0E"), "01"); //place at end of usable chain
      sysxIO->setFileSource("Structure", "12", "00", "17", "01");
    }else{
        hexData.removeOne("01"); hexData.insert(hexData.indexOf("02")+(4-val), "01");
             sysxIO->setFileSource("Structure", "12", "00", "17", "00");
    };  // S/R loop chain
    sysxIO->setFileSource("Structure", "07", "00", "20", hexData);
    updateStompBoxes();
    chainSet();
}

void floorBoard::chain_5_Set(bool value)  //EQ amp in - amp out
{
    Q_UNUSED(value);
    SysxIO *sysxIO = SysxIO::Instance();
    MidiTable *midiTable = MidiTable::Instance();
    QList<QString> fxChain = sysxIO->getFileSource("Structure", "07", "00");
    QList<QString> hexData;
    for(int i= sysxDataOffset+32;i< (sysxDataOffset+32 + 20);i++ )
    {
        hexData.append( midiTable->getMidiMap("Structure", "07", "00", "20", fxChain.at(i)).value );
    };
    if(sysxIO->getSourceValue("Structure", "12", "00", "18")==1)
    {
      hexData.removeOne("04"); hexData.insert(hexData.indexOf("02"), "04");
      sysxIO->setFileSource("Structure", "12", "00", "18", "00");
    }else{
        hexData.removeOne("04"); hexData.insert(hexData.indexOf("0C")+1, "04");
             sysxIO->setFileSource("Structure", "12", "00", "18", "01");
    };  // EQ chain
    sysxIO->setFileSource("Structure", "07", "00", "20", hexData);
    updateStompBoxes();
    chainSet();
}

void floorBoard::chain_6_Set(bool value)  //PDL
{
    Q_UNUSED(value);
    SysxIO *sysxIO = SysxIO::Instance();
    MidiTable *midiTable = MidiTable::Instance();
    QList<QString> fxChain = sysxIO->getFileSource("Structure", "07", "00");
    QList<QString> hexData;
    for(int i= sysxDataOffset+32;i< (sysxDataOffset+32 + 20);i++ )
    {
        hexData.append( midiTable->getMidiMap("Structure", "07", "00", "20", fxChain.at(i)).value );
    };
    if(sysxIO->getSourceValue("Structure", "11", "00", "10")==1)
    {
      hexData.removeOne("0B"); hexData.insert(hexData.indexOf("11")+1, "0B");
      sysxIO->setFileSource("Structure", "11", "00", "10", "00");
    }else{
        hexData.removeOne("0B"); hexData.insert(hexData.indexOf("0D"), "0B");
             sysxIO->setFileSource("Structure", "11", "00", "10", "01");
    };  // EQ chain
    sysxIO->setFileSource("Structure", "07", "00", "20", hexData);
    updateStompBoxes();
    chainSet();
}

void floorBoard::chainSet()
{
    SysxIO *sysxIO = SysxIO::Instance();
    this->chain_1_statusSignal(false);
    this->chain_2_statusSignal(false);
    this->chain_3_statusSignal(false);
    this->chain_4_statusSignal(false);
    this->chain_5_statusSignal(false);
    this->chain_6_statusSignal(false);
    int val = sysxIO->getSourceValue("Structure", "12", "00", "00");
    if(val==0){this->chain_1_statusSignal(true);};
    if(val==1){this->chain_2_statusSignal(true);};
    if(val==2){this->chain_3_statusSignal(true);};
    if(sysxIO->getSourceValue("Structure", "12", "00", "17")==1){this->chain_4_statusSignal(true);};
    if(sysxIO->getSourceValue("Structure", "12", "00", "18")==0){this->chain_5_statusSignal(true);};
    if(sysxIO->getSourceValue("Structure", "11", "00", "10")==0){this->chain_6_statusSignal(true);};
}

void floorBoard::initSize(QSize floorSize)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);
    //const int model = preferences->getPreferences("Midi", "Katana", "model").toInt(&ok);
    unsigned int pads=10;
    //if(model==0){pads=9;};
    
    this->floorSize = floorSize;
    this->l_floorSize = floorSize;
    QList<QPoint> fxPos;

    unsigned int spacingH = ( (floorSize.width()*ratio - offset - (marginStompBoxesWidth*2)) - (stompSize.width()*((pads*pads/5)-11)) ) /pads;
    for(unsigned int i=0;i<20;i++)
    {
        unsigned int y = marginStompBoxesTop;
        unsigned int x = marginStompBoxesWidth + (( stompSize.width() + spacingH/4 ) * i)-pads;
        if(i>pads+1){ x = 1800*ratio; };
        fxPos.append(QPoint(offset + x, y - (this->infoBarHeight/2)));
    };

    this->fxPos = fxPos;
}

QPoint floorBoard::getStompPos(int id)
{
    return fxPos.at(id);
}

void floorBoard::setCollapse()
{
    if(floorSize.width() > minSize.width())
    {
        this->l_floorSize = floorSize;
        this->setSize(minSize);
        this->colapseState = false;
        emit setCollapseState(false);
    }
    else
    {
        this->setSize(l_floorSize);
        emit setCollapseState(true);
        this->colapseState = true;
    };
}

void floorBoard::setSize(QSize newSize)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    unsigned int oldOffset = offset;
    this->offset = newSize.width()*ratio - infoBarWidth*ratio;
    QSize oldFloorSize = this->floorSize;
    this->floorSize = newSize;

    for(int i=0;i<fxPos.size();i++)
    {
        fxPos[i] = fxPos[i] + QPoint(offset - oldOffset, 0);
    };
    emit updateStompOffset(offset - oldOffset);

    QPoint newPanelBarPos = QPoint(offset - panelBarOffset, panelBarPos.y());
    emit setFloorPanelBarPos(newPanelBarPos);
    this->panelBarPos = newPanelBarPos;

    QPoint newDisplayPos = QPoint(offset, displayPos.y());
    emit setDisplayPos(newDisplayPos);
    this->displayPos = newDisplayPos;

    QPixmap buffer = QPixmap(floorSize);
    QPainter painter(&buffer);

    // Redraw new resized floor
    QRectF source(maxSize.width() - floorSize.width(), 0.0, floorSize.width(), floorSize.height());
    QRectF target(0.0, 0.0, floorSize.width(), floorSize.height());
    painter.drawPixmap(target, baseImage, source);

    painter.end();

    this->image = buffer;

    QRect newBankListRect = QRect(borderWidth, borderWidth, offset - panelBarOffset - (borderWidth*2), floorHeight - (borderWidth*2));
    emit resizeSignal(newBankListRect);

    emit sizeChanged(floorSize, oldFloorSize);
    this->centerEditDialog();
}

void floorBoard::setWidth(int dist)
{
    QSize newSize;
    if(floorSize.width() + dist < minSize.width())
    {
        newSize = minSize;
        this->colapseState = false;
        emit setCollapseState(false);
    }
    else if(floorSize.width() + dist > maxSize.width())
    {
        newSize = maxSize;
        this->l_floorSize = newSize;
        this->colapseState = true;
        emit setCollapseState(true);
    }
    else
    {
        newSize = QSize(floorSize.width() + dist, floorSize.height());
        this->l_floorSize = newSize;
        this->colapseState = true;
        emit setCollapseState(true);
    };
    this->setSize(newSize);
}

void floorBoard::initStomps()
{
    QList<signed int> fx;
    fx << 11 << 16 << 5 << 2 << 13 << 12 << 4 << 1 << 7 << 6 << 8 << 9;
        //0B    0F   05   02    0D    0C   04   01   07   06   08   09
//fx name=pdl  od   fx1  pre_a  ns1   fv   eq   lp   dd   fx2  ce   rv      cn_m1_s2 cs ch_b fx3 ns2 cn_m usb
    this->fx = fx;

    QVector<QString> initStompNames(20);
    this->stompNames = initStompNames.toList();;

    QVector<stompBox *> initStompBoxes(20);
    this->stompBoxes = initStompBoxes.toList();;

    MidiTable *midiTable = MidiTable::Instance();
    Midi midimap = midiTable->getMidiMap("Structure", "07", "00", "20");
    QList<int> fxID;
    QList<QString> fxNAMES;
    QString fxName;
    for(int i=0;i<20;i++)
    {
        fxName=midimap.level.at(i).name;
        if(fxName=="LP" || fxName=="CH_A" || fxName=="FX1" || fxName=="FX2" || fxName=="DD" || fxName=="PDL"
                || fxName=="RV" || fxName=="FV" || fxName=="NS_1" || fxName=="OD" || fxName=="EQ" || fxName=="CE")
        {
            bool ok;
            fxID.append(midimap.level.at(i).value.toInt(&ok, 16));
            fxNAMES.append(fxName);
        };
    };

    /* PDL */
    stompBox *pdl = new stompbox_pdl(this);
    pdl->setId( fxID.at(fxNAMES.indexOf("PDL")) );
    pdl->setPos(this->getStompPos(pdl->getId()));
    this->stompBoxes.replace(pdl->getId(), pdl);
    this->stompNames.replace(pdl->getId(), "PDL");

    // OD/DS
    stompBox *od = new stompbox_od(this);
    od->setId( fxID.at(fxNAMES.indexOf("OD")) );
    od->setPos(this->getStompPos(od->getId()));
    this->stompBoxes.replace(od->getId(), od);
    this->stompNames.replace(od->getId(), "OD");

    // FX1
    stompBox *fx1 = new stompbox_fx1(this);
    fx1->setId( fxID.at(fxNAMES.indexOf("FX1")) );
    fx1->setPos(this->getStompPos(fx1->getId()));
    this->stompBoxes.replace(fx1->getId(), fx1);
    this->stompNames.replace(fx1->getId(), "FX1");

    // CH_A
    stompBox *ch_a = new stompbox_ch_a(this);
    ch_a->setId( fxID.at(fxNAMES.indexOf("CH_A")) );
    ch_a->setPos(this->getStompPos(ch_a->getId()));
    this->stompBoxes.replace(ch_a->getId(), ch_a);
    this->stompNames.replace(ch_a->getId(), "CH_A");

    // NS_1
    stompBox *ns_1 = new stompbox_ns_1(this);
    ns_1->setId( fxID.at(fxNAMES.indexOf("NS_1")) );
    ns_1->setPos(this->getStompPos(ns_1->getId()));
    this->stompBoxes.replace(ns_1->getId(), ns_1);
    this->stompNames.replace(ns_1->getId(), "NS_1");

    // VOLUME
    stompBox *fv = new stompbox_fv(this);
    fv->setId( fxID.at(fxNAMES.indexOf("FV")) );
    fv->setPos(this->getStompPos(fv->getId()));
    this->stompBoxes.replace(fv->getId(), fv);
    this->stompNames.replace(fv->getId(), "FV");

    // EQ
    stompBox *eq = new stompbox_eq(this);
    eq->setId( fxID.at(fxNAMES.indexOf("EQ")) );
    eq->setPos(this->getStompPos(eq->getId()));
    this->stompBoxes.replace(eq->getId(), eq);
    this->stompNames.replace(eq->getId(), "EQ");

    // LOOP
    stompBox *lp = new stompbox_lp(this);
    lp->setId( fxID.at(fxNAMES.indexOf("LP")) );
    lp->setPos(this->getStompPos(lp->getId()));
    this->stompBoxes.replace(lp->getId(), lp);
    this->stompNames.replace(lp->getId(), "LP");

    // Delay
    stompBox *dd = new stompbox_dd(this);
    dd->setId( fxID.at(fxNAMES.indexOf("DD")) );
    dd->setPos(this->getStompPos(dd->getId()));
    this->stompBoxes.replace(dd->getId(), dd);
    this->stompNames.replace(dd->getId(), "DD");

    // FX 2
    stompBox *fx2 = new stompbox_fx2(this);
    fx2->setId( fxID.at(fxNAMES.indexOf("FX2")) );
    fx2->setPos(this->getStompPos(fx2->getId()));
    this->stompBoxes.replace(fx2->getId(), fx2);
    this->stompNames.replace(fx2->getId(), "FX2");

    // DD2
    stompBox *ce = new stompbox_ce(this);
    ce->setId( fxID.at(fxNAMES.indexOf("CE")) );
    ce->setPos(this->getStompPos(ce->getId()));
    this->stompBoxes.replace(ce->getId(), ce);
    this->stompNames.replace(ce->getId(), "CE");

    // REVERB
    stompBox *rv = new stompbox_rv(this);
    rv->setId( fxID.at(fxNAMES.indexOf("RV")) );
    rv->setPos(this->getStompPos(rv->getId()));
    this->stompBoxes.replace(rv->getId(), rv);
    this->stompNames.replace(rv->getId(), "RV");
}

void floorBoard::setStomps(QList<QString> stompOrder)
{
    for(int i=0;i<stompOrder.size();i++)
    {
        QString name = stompOrder.at(i);
        setStompPos(name, i);
        this->fx.replace(i, stompNames.indexOf(name));
    };
}

void floorBoard::setStompPos(QString name, int order)
{
    this->stompBoxes.at(stompNames.indexOf(name))->setPos(this->getStompPos(order));
    this->fx.replace(order, stompNames.indexOf(name));
}

void floorBoard::setStompPos(int index, int order)
{
    this->stompBoxes.at(index)->setPos(this->getStompPos(order));
    this->fx.replace(order, index);
}

void floorBoard::updateStompBoxes()
{
    SysxIO *sysxIO = SysxIO::Instance();
    QString area = "Structure";
    QList<QString> fxChain = sysxIO->getFileSource(area, "07", "00");


    MidiTable *midiTable = MidiTable::Instance();
    QList<QString> stompOrder;
    for(int i= sysxDataOffset+32;i< (sysxDataOffset+32 + 20);i++ )
    {
        stompOrder.append( midiTable->getMidiMap("Structure", "07", "00", "20", fxChain.at(i)).name );
    };
    stompOrder.removeOne("CS");
    stompOrder.removeOne("CH_B");
    stompOrder.removeOne("FX3");
    stompOrder.removeOne("NS_2");
    stompOrder.removeOne("USB");
    stompOrder.removeOne("CN_S");
    stompOrder.removeOne("CN_M1_S2");
    stompOrder.removeOne("CN_M");

    setStomps(stompOrder);
}

void floorBoard::setEditDialog(editWindow* editDialog)
{
    this->oldDialog = this->editDialog;
    this->editDialog = editDialog;
    this->editDialog->setParent(this);
    Preferences *preferences = Preferences::Instance();
    if(preferences->getPreferences("Window", "Single", "bool")!="true")
    {
        this->editDialog->setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    };
    this->centerEditDialog();
    this->editDialog->pageUpdateSignal();
    this->editDialog->show();
}

void floorBoard::centerEditDialog()
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    if(preferences->getPreferences("Window", "Single", "bool")=="true")
    {
        int x = this->displayPos.x() + (((this->floorSize.width()*ratio - this->displayPos.x()) - this->editDialog->width() + (10*ratio)) / 2);
        int y = this->pos.y() + ((((this->floorSize.height()*ratio) + (75*ratio)) - this->editDialog->height()) / 2);
        this->editDialog->move(x, y);
    };
}

void floorBoard::initMenuPages()
{
    QVector<menuPage *> initMenuPages(20);
    this->menuPages = initMenuPages.toList();;

    /* MENU_PAGES */
    menuPage *ez_edit = new menuPage_ez_edit(this);
    ez_edit->setId(31);

    menuPage *system = new menuPage_system(this);
    system->setId(30);

    menuPage *midi = new menuPage_midi(this);
    midi->setId(29);

    menuPage *master = new menuPage_master(this);
    master->setId(20);

    menuPage *assign1 = new menuPage_assign1(this);
    assign1->setId(21);

    menuPage *assign2 = new menuPage_assign2(this);
    assign2->setId(22);

    menuPage *assign3 = new menuPage_assign3(this);
    assign3->setId(23);

    menuPage *assign4 = new menuPage_assign4(this);
    assign4->setId(24);

    menuPage *assign5 = new menuPage_assign5(this);
    assign5->setId(25);

    menuPage *assign6 = new menuPage_assign6(this);
    assign6->setId(26);

    menuPage *assign7 = new menuPage_assign7(this);
    assign7->setId(27);

    menuPage *assign8 = new menuPage_assign8(this);
    assign8->setId(28);

    menuPage *assign9 = new menuPage_assign9(this);
    assign9->setId(33);

    menuPage *assign10 = new menuPage_assign10(this);
    assign10->setId(34);

    menuPage *assign11 = new menuPage_assign11(this);
    assign11->setId(35);

    menuPage *assign12 = new menuPage_assign12(this);
    assign12->setId(36);

    menuPage *assign13 = new menuPage_assign13(this);
    assign13->setId(37);

    menuPage *assign14 = new menuPage_assign14(this);
    assign14->setId(38);

    menuPage *assign15 = new menuPage_assign15(this);
    assign15->setId(39);

    menuPage *assign16 = new menuPage_assign16(this);
    assign16->setId(40);
}

void floorBoard::menuButtonSignal()
{
    emit updateSignal();
    Preferences *preferences = Preferences::Instance();
    if(preferences->getPreferences("Window", "Single", "bool")=="true")
    {
        this->oldDialog->hide();
        this->editDialog->show();
    };
}





