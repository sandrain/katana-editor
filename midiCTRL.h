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

#ifndef midiCTRL_H
#define midiCTRL_H
#include <vector>
#include <QThread>
#include <QString>
#include <QStringList>
#include <QList>


class midiCTRLDestroyer;

class midiCTRL: public QThread
{
    Q_OBJECT

public:
    midiCTRL();
    static midiCTRL* Instance();
    void callbackMsg(QString rxData);
    QList<QString> getMidiInDevices();
    //static QString sysxBuffer;
    void receiveMidi(int midiInPort);

protected :
    friend class midiCTRLDestroyer;
    virtual ~midiCTRL() { }

signals:
    void errorSignal(QString windowTitle, QString errorMsg);
    void replyMsg(QString sysxInMsg);
    void midiFinished();
    void started();
    //void setStatusSymbol(int value);
    //void setStatusProgress(int value);
    void setStatusMessage(QString message);
    void setStatusdBugMessage(QString dBug);

private:
    static midiCTRL* _instance;
    static midiCTRLDestroyer _destroyer;
    void queryMidiInDevices();
    //QList<QString> midiOutDevices;
    QList<QString> midiInDevices;
    void run();
    void receiveMsg(int midiInPort);
    //static QString msgType;
    //static bool dataReceive;
    //static int bytesTotal;
    //static int bytesReceived;
    int midiInPort;
    //QString sysxInMsg;
    //int dataSize;
    //int h;
    //QString reBuild;
    QString hex;
    //bool midi;
    //int count;
};

#endif // midiCTRL_H
