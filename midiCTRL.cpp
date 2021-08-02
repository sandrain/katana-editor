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

#include "midiCTRL.h"
#include "midiCTRLDestroyer.h"
#include "SysxIO.h"
#include "globalVariables.h"
#include "RtMidi.h"
#include "Preferences.h"
#include "MidiTable.h"

const std::string clientName = "FxFloorBoard";
midiCTRL* midiCTRL::_instance = 0;// initialize pointer
midiCTRLDestroyer midiCTRL::_destroyer;

midiCTRL::midiCTRL()
{
    SysxIO *sysxIO = SysxIO::Instance();
    QObject::connect(this, SIGNAL(replyMsg(QString)),	sysxIO, SLOT(receiveMidi(QString)));
    QObject::connect(this, SIGNAL(midiFinished()), sysxIO, SLOT(finishedMidi()));
    QObject::connect(this, SIGNAL(setStatusdBugMessage(QString)), sysxIO, SIGNAL(setStatusdBugMessage(QString)));
}

midiCTRL* midiCTRL::Instance()
{
    /* Multi-threading safe */
    if (!_instance)  // is it the first call?
    {
        _instance = new midiCTRL; // create sole instance
        _destroyer.SetmidiCTRL(_instance);
    };
    return _instance; // address of sole instance
}

/*********************** queryMidiInDevices() ******************************
 * Retrieves all MIDI In devices installed on your system and stores them
 * as a QList of QStrings and device id's.
 *************************************************************************/
void midiCTRL::queryMidiInDevices()
{
    RtMidiIn *midiin2 = new RtMidiIn();
    std::string portName;
    unsigned int inPorts;
    try
    {
        inPorts = midiin2->getPortCount();      /* Check inputs. */
        for ( unsigned int i=0; i<inPorts; i++ )
        {
            portName = midiin2->getPortName(i);
            QString name = QString::fromStdString(portName);
            if(name.contains("KATANA")) { name.remove(0, name.indexOf("KATANA")); };
#if defined(Q_OS_WIN)
            name.chop(2);
#endif            
            this->midiInDevices.append(name);
        };
    }
    catch (RtMidiError &error)
    {
        emit errorSignal(tr("Controller Midi Input Error"), tr("port availability error"));
        emit setStatusdBugMessage(QString::fromStdString( error.getMessage()));
        this->midiInDevices.append("Device access error");
        goto cleanup;
    };
    if (inPorts < 1)
    { this->midiInDevices.push_back(tr("no midi device available")); };
    // Clean up
cleanup:
    delete midiin2;
}

QList<QString> midiCTRL::getMidiInDevices()
{
    queryMidiInDevices();
    return this->midiInDevices;
}


/*********************** receiveMsg() **********************************
 * Prepares the sysx message after receiving on the MIDI In device. It
 *  and opens, receives and closes the MIDI device then converts the
 *  message from a std::vector to a QString.
 *************************************************************************/
void midictrlcallback(double deltatime, std::vector<unsigned char> *message, void *userData)
{
    Q_UNUSED(deltatime);
    Q_UNUSED(userData);
    QString rxData;
    midiCTRL *midi2 = new midiCTRL();
    unsigned int nBytes = message->size();
    for ( unsigned int i=0; i<nBytes; i++ )
    {
        int n = ((int)message->at(i));					// convert std::vector to QString
        QString hex = QString::number(n, 16).toUpper();
        if (hex.length() < 2) hex.prepend("0");
        rxData.append(hex);
    };
    midi2->callbackMsg(rxData);
    rxData.clear();
}

void midiCTRL::callbackMsg(QString rxData)
{
    emit replyMsg(rxData);
}

void midiCTRL::receiveMsg(int midiInPort)
{
    Preferences *preferences = Preferences::Instance();  bool ok;
    RtMidiIn *midiin2 = new RtMidiIn();
    try {
        int InPort = preferences->getPreferences("Midi", "MidiIn2", "device").toInt(&ok, 10);	// Get midi in device from preferences.
        int inPorts = midiin2->getPortCount();
        if(inPorts<InPort){ msleep(1000); goto cleanup; };
        midiin2->ignoreTypes(false, true, true);  //don,t ignore sysex messages, but ignore other crap like active-sensing
        if(!midiin2->isPortOpen()) midiin2->openPort(midiInPort);             // open the midi in port
        if(midiin2->isPortOpen()) midiin2->setCallback(&midictrlcallback);    // set the callback
        for(int x=0; x<1000; x++){
        int InPort = preferences->getPreferences("Midi", "MidiIn2", "device").toInt(&ok, 10)-1;	// Get midi in device
        if(InPort!=midiInPort) { goto cleanup; };
        msleep(1000);
        };
        goto cleanup;
    }
    catch (RtMidiError &error)
    {
        emit errorSignal(tr("Midi Controller Input Error"), tr("data error"));
        emit setStatusdBugMessage(QString::fromStdString( error.getMessage()));
        goto cleanup;
    };
cleanup:
    if(midiin2->isPortOpen())
    {
        try{
            midiin2->cancelCallback();
            midiin2->closePort();
        }
        catch (RtMidiError &error)
        {
            emit errorSignal(tr("Midi Controller Input Error"), tr("callback cancel"));
            emit setStatusdBugMessage(QString::fromStdString( error.getMessage()));
        };
    };
    delete midiin2;
    emit midiFinished();
}

/**************************** run() **************************************
 * New QThread that processes the midi message and handles
 *  receiving a reply on the MIDI In device midiIn. If
 * so midiCallback() will handle the receive of the incoming midi message.
 *************************************************************************/
void midiCTRL::run()
{
    receiveMsg(this->midiInPort);
}

/*********************** receiveMidi() ***********************************
 * Starts a new thread that handles the receive of midi messages.
 *************************************************************************/
void midiCTRL::receiveMidi(int midiInPort)
{
    this->midiInPort = midiInPort;
    start();
}


