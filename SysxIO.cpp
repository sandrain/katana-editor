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

#include <QApplication>
#include <QMessageBox>
#include "SysxIO.h"
#include "SysxIODestroyer.h"
#include "midiCTRL.h"
#include "midiIO.h"
#include "Preferences.h"
#include "sysxWriter.h"
#include "MidiTable.h"
#include "globalVariables.h"
#include <QtWidgets>
#include <QWidget>
//#include <process.h>

// Platform-dependent sleep routines.
#ifdef Q_OS_WIN
#include <windows.h>
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds )
#else // Unix variants
#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif


SysxIO::SysxIO() 
{
    this->setConnected(false);
    this->setDevice(false);
    this->setDeviceReady(true);
    this->setSyncStatus(false);
    this->setNoError(true);
    this->setBank(0);
    this->setPatch(0);
    this->setLoadedBank(0);
    this->setLoadedPatch(0);
    this->changeCount = 0;
    this->fileReady=false;
}

SysxIO* SysxIO::_instance = 0;// initialize pointer
SysxIODestroyer SysxIO::_destroyer;

SysxIO* SysxIO::Instance() 
{
    /* Multi-threading safe */
    if (!_instance )  // is it the first call?
    {
        _instance = new SysxIO; // create sole instance
        _destroyer.SetSysxIO(_instance);
    };
    return _instance; // address of sole instance
}

void SysxIO::setFileSource(QString area, SysxData fileSource)
{ 
    if (area == "System")
    {
        this->systemSource = fileSource;
    }
    else
    {
        this->fileSource = fileSource;
    };
}

void SysxIO::setFileSource(QString area, QByteArray data)
{
    if (area == "System")
    {
        this->systemSource.address.clear();
        this->systemSource.hex.clear();
        QByteArray assigns;
        QFile file("assign.kat");
        if (file.open(QIODevice::ReadOnly))
        {
          assigns = file.readAll();
         }else{
            QFile file(":global.syx");
            if (file.open(QIODevice::ReadOnly))
            { assigns = file.readAll();    };
        };
        data.replace(systemSize, 141, assigns);
    }else
    {
        this->fileSource.address.clear();
        this->fileSource.hex.clear();
    };

    //QString errorList;
    QList<QString> sysxBuffer;
    int dataSize = 0; int offset = 0;
    for(int i=0;i<data.size();i++)
    {
        unsigned char byte = (char)data[i];
        unsigned int n = (int)byte;
        QString hex = QString::number(n, 16).toUpper();
        if (hex.length() < 2) hex.prepend("0");
        sysxBuffer.append(hex);

        unsigned char nextbyte = (char)data[i+1];
        unsigned int nextn = (int)nextbyte;
        QString nexthex = QString::number(nextn, 16).toUpper();
        if (nexthex.length() < 2) nexthex.prepend("0");
        if(offset >= checksumOffset && nexthex != "F7")
        {
            dataSize += n;
        };
        /*if(nexthex == "F7")
        {
            QString checksum = hex;

            if(getCheckSum(dataSize) != checksum)
            {
                QString errorString;
                errorString.append(tr("Address") + ": ");
                errorString.append(sysxBuffer.at(sysxAddressOffset) + " ");
                errorString.append(sysxBuffer.at(sysxAddressOffset + 1) + " ");
                errorString.append(sysxBuffer.at(sysxAddressOffset + 2) + " ");
                errorString.append(sysxBuffer.at(sysxAddressOffset + 3) + " - ");
                errorString.append(tr("checksum") + " (" + checksum + ") ");
                errorString.append(tr("should have been") + " (" + getCheckSum(dataSize) + ")");
                errorString.append("\n");
                errorList.append(errorString);

                sysxBuffer = correctSysxMsg(sysxBuffer);
            };
        };*/
        offset++;

        if(hex == "F7")
        {
            if (area == "System")
            {
                this->systemSource.address.append( sysxBuffer.at(sysxAddressOffset + 1) + sysxBuffer.at(sysxAddressOffset + 2) );
                this->systemSource.hex.append(sysxBuffer);
            }
            else
            {
                this->fileSource.address.append( sysxBuffer.at(sysxAddressOffset + 2) + sysxBuffer.at(sysxAddressOffset + 3) );
                this->fileSource.hex.append(sysxBuffer);
            };
            sysxBuffer.clear();
            dataSize = 0;
            offset = 0;
        };
    };
   /* if(!errorList.isEmpty())
    {
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle(tr("KATANA Fx FloorBoard - Checksum Error"));
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setTextFormat(Qt::RichText);
        QString msgText;
        msgText.append("<font size='+1'><b>");
        msgText.append(tr("The file opened contains one or more incorrect checksums."));
        msgText.append("<b></font><br>");
        msgText.append(tr("The incorrect values have been corrected where possible.<br>"
                          "If correction was impossible then some settings might have been reset to default."));
        msgBox->setText(msgText);
        msgBox->setInformativeText(tr("Be aware of possible inconsistencies in this patch!"));
        msgBox->setDetailedText(errorList);
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->exec();
    };*/
}

void SysxIO::setFileSource(QString area, QString data)
{
    if (area == "System")
    {
        this->systemSource.address.clear();
        this->systemSource.hex.clear();
        QByteArray assigns;
        QFile file("assign.kat");
        if (file.open(QIODevice::ReadOnly))
        {
          assigns = file.readAll();
         }else{
            QFile file(":global.syx");
            if (file.open(QIODevice::ReadOnly))
            { assigns = file.readAll();    };
        };
        QString sysxBuffer;
        for(int i=0;i<assigns.size();i++)
        {
            unsigned char byte = (char)assigns[i];
            unsigned int n = (int)byte;
            QString hex = QString::number(n, 16).toUpper();
            if (hex.length() < 2) hex.prepend("0");
            sysxBuffer.append(hex);
        };
        data.append(sysxBuffer);
    }
    else
    {
        this->fileSource.address.clear();
        this->fileSource.hex.clear();
    };
     //QString texts;
    QList<QString> sysxBuffer;
    for(int i=0;i<data.size();i++)
    {

        QString hex = data.mid(i, 2);
        sysxBuffer.append(hex);
        //texts.append(hex);
        i++;

        if(hex == "F7")
        {
            if (area == "System")
            {
                this->systemSource.address.append( sysxBuffer.at(sysxAddressOffset + 1) + sysxBuffer.at(sysxAddressOffset + 2) );
                this->systemSource.hex.append(sysxBuffer);
            }
            else
            {
                this->fileSource.address.append( sysxBuffer.at(sysxAddressOffset + 2) + sysxBuffer.at(sysxAddressOffset + 3) );
                this->fileSource.hex.append(sysxBuffer);
                //texts.append(sysxBuffer.at(sysxAddressOffset + 2) + sysxBuffer.at(sysxAddressOffset + 3));
            };

            /*QMessageBox *msgBox = new QMessageBox();
            msgBox->setWindowTitle(QObject::tr("data received"));
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->setTextFormat(Qt::RichText);
            QString msgText;
            msgText.append(QObject::tr("<br> data = "));
            msgText.append(texts);
            msgBox->setText(msgText);
            msgBox->setStandardButtons(QMessageBox::Ok);
            msgBox->exec();
            texts.clear();*/

            sysxBuffer.clear();
        };
    };
}

void SysxIO::setFileSource(QString area, QString hex1, QString hex2, QString hex3, QString hex4)
{
    if(hex4.size()>2){
        QString hex5=hex4.at(0);
        hex5.append(hex4.at(1));
        hex4.truncate(2);
        this->setFileSource(area, hex1, hex2, hex3, hex5, hex4);
    }else{
        if (area != "System")  {area = "Structure";};
        MidiTable *midiTable = MidiTable::Instance();
        bool ok;
        QString sourceHex1 = hex1;
        QString sourceHex3 = hex3;

        int index = sourceHex3.toInt(&ok, 16) + sysxDataOffset;
        QString address;
        address.append(sourceHex1);
        address.append(hex2);
        QList<QString> sysxList;
        if(area == "System")
        {
            sysxList = this->systemSource.hex.at(this->systemSource.address.indexOf(address));
        }
        else
        {
            sysxList = this->fileSource.hex.at(this->fileSource.address.indexOf(address));
        };
        sysxList.replace(index, hex4);

        int dataSize = 0;
        for(int i=sysxList.size() - 3; i>=checksumOffset;i--)
        {
            dataSize += sysxList.at(i).toInt(&ok, 16);
        };
        sysxList.replace(sysxList.size() - 2, getCheckSum(dataSize));

        if(area == "System")
        {
            this->systemSource.hex.replace(this->systemSource.address.indexOf(address), sysxList);
        }
        else
        {
            this->fileSource.hex.replace(this->fileSource.address.indexOf(address), sysxList);
        };
        if(area=="System" && hex1=="03")
        {
            writeGlobalAssign();   //save global Assigns
        }
        else
        {
            QString sysxMsg = midiTable->dataChange(area, hex1, hex2, hex3, hex4);

            if(this->isConnected() && this->deviceReady())
            {
                this->setDeviceReady(false);

                emit setStatusSymbol(2);
                emit setStatusMessage(tr("Sending"));

                QObject::connect(this, SIGNAL(sysxReply(QString)),
                                 this, SLOT(resetDevice(QString)));

                this->sendSysx(sysxMsg);
            }
            else if(this->isConnected())
            {
                this->sendSpooler.append(sysxMsg);
            };
        };
    };
}

void SysxIO::setFileSource(QString area, QString hex1, QString hex2, QString hex3, QString hex4, QString hex5)
{
    MidiTable *midiTable = MidiTable::Instance();
    bool ok;
    if (area != "System")  {area = "Structure";};
    QString sourceHex1 = hex1;
    QString sourceHex3 = hex3;

    QString address;
    address.append(sourceHex1);
    address.append(hex2);

    QList<QString> sysxList;
    if (area == "System")
    {
        sysxList = this->systemSource.hex.at(this->systemSource.address.indexOf(address));
    }
    else
    {
        sysxList = this->fileSource.hex.at(this->fileSource.address.indexOf(address));
    };
    int index = sourceHex3.toInt(&ok, 16) + sysxDataOffset;
    sysxList.replace(index, hex4);
    sysxList.replace(index + 1, hex5);

    int dataSize = 0;
    for(int i=sysxList.size() - 3; i>=checksumOffset;i--)
    {
        dataSize += sysxList.at(i).toInt(&ok, 16);
    };
    sysxList.replace(sysxList.size() - 2, getCheckSum(dataSize));

    if (area == "System")
    {
        this->systemSource.hex.replace(this->systemSource.address.indexOf(address), sysxList);
    }
    else
    {
        this->fileSource.hex.replace(this->fileSource.address.indexOf(address), sysxList);
    };

    if(area=="System" && hex1=="03")
    {
        writeGlobalAssign();   //save global Assigns
    }
    else
    {
        QString sysxMsg = midiTable->dataChange(area, hex1, hex2, hex3, hex4, hex5);

        if(this->isConnected() && this->deviceReady())
        {
            this->setDeviceReady(false);

            emit setStatusSymbol(2);
            emit setStatusMessage(tr("Sending"));

            QObject::connect(this, SIGNAL(sysxReply(QString)),
                             this, SLOT(resetDevice(QString)));

            this->sendSysx(sysxMsg);
        }
        else if(this->isConnected())
        {
            this->sendSpooler.append(sysxMsg);
        };
    };
}

void SysxIO::setFileSource(QString area, QString hex1, QString hex2, QString hex3, QList<QString> hexData)
{
    QString sysxMsg;
    bool ok;

    QString address;
    address.append(hex1);
    address.append(hex2);

    int pointerOffset2 = hex3.toInt(&ok, 16);
    if (area != "System")
    {
        QList<QString> sysxList = this->fileSource.hex.at(this->fileSource.address.indexOf(address));

        for(int i=0; i<hexData.size();++i)
        {
            sysxList.replace(i + (sysxDataOffset + pointerOffset2), hexData.at(i));
        };
        this->fileSource.hex.replace(this->fileSource.address.indexOf(address), sysxList);

        sysxMsg = "F04100000060126000";
        sysxMsg.append(hex1);
        sysxMsg.append(hex3);
    }
    else
    {
        QList<QString> sysxList = this->systemSource.hex.at(this->systemSource.address.indexOf(address));
        for(int i=0; i<hexData.size();++i)
        {
            sysxList.replace(i + (sysxDataOffset + pointerOffset2), hexData.at(i));

        };
        this->systemSource.hex.replace(this->systemSource.address.indexOf(address), sysxList);

        sysxMsg = "F041000000601200";
        sysxMsg.append(hex1);
        sysxMsg.append(hex2);
        sysxMsg.append(hex3);
    };
    for(int i=0;i<hexData.size();++i)
    {
        sysxMsg.append(hexData.at(i));
    };
    int dataSize = 0;
    QString temp;
    for(int i=checksumOffset; i<sysxMsg.size()-1;i++)
    {dataSize += sysxMsg.mid(i*2, 2).toInt(&ok, 16); };
    sysxMsg.append(getCheckSum(dataSize));
    sysxMsg.append("F7");

    if(this->isConnected() && this->deviceReady())
    {
        this->setDeviceReady(false);

        emit setStatusSymbol(2);
        emit setStatusMessage(tr("Sending"));

        QObject::connect(this, SIGNAL(sysxReply(QString)),
                         this, SLOT(resetDevice(QString)));

        this->sendSysx(sysxMsg);
    }
    else if(this->isConnected())
    {
        this->sendSpooler.append(sysxMsg);
    };
}

QList<QString> SysxIO::getSourceItems(QString area, QString hex1, QString hex2)
{
    QList<QString> items = this->getFileSource(area, hex1, hex2);
    return items;
}

int SysxIO::getSourceValue(QString area, QString hex1, QString hex2, QString hex3)
{
    MidiTable *midiTable = MidiTable::Instance();
    bool ok;
    int value;
    if(area == "MidiT")
    {
        QList<QString> items = this->getSourceItems("System", hex1, hex2);
        int maxRange = QString("7F").toInt(&ok, 16) + 1;
        int listindex = sysxDataOffset + QString(hex3).toInt(&ok, 16);
        int valueData1 = items.at(listindex).toInt(&ok, 16);
        int valueData2 = items.at(listindex + 1).toInt(&ok, 16);
        value = (valueData1 * maxRange) + valueData2;
    }else{
        if (area != "System")  {area = "Structure";};
        QList<QString> items = this->getSourceItems(area, hex1, hex2);
        if(midiTable->isData(area, hex1, hex2, hex3))
        {
            int maxRange = QString("7F").toInt(&ok, 16) + 1;
            int listindex = sysxDataOffset + QString(hex3).toInt(&ok, 16);
            int valueData1 = items.at(listindex).toInt(&ok, 16);
            int valueData2 = items.at(listindex + 1).toInt(&ok, 16);
            value = (valueData1 * maxRange) + valueData2;
        }
        else
        {
            value = items.at(sysxDataOffset + QString(hex3).toInt(&ok, 16)).toInt(&ok, 16);
        };
    };
    return value;
}

/************************ resetDevice() ******************************
* Reset the device after sending a sysexmesage.
* And starts to processes the spooler if the device is free.
**********************************************************************/
void SysxIO::resetDevice(QString replyMsg) 
{
    Q_UNUSED(replyMsg);
    if(this->sendSpooler.size() == 0)
    {
        QObject::disconnect(this, SIGNAL(sysxReply(QString)),
                            this, SLOT(resetDevice(QString)));

        this->setDeviceReady(true);	// Free the device after finishing interaction.
    }
    else
    {
        processSpooler();
    };
}

/************************ processSpooler() ******************************
* Send message that are in the spooler due to that the device was busy.
* And eliminates multiple messages where only the value changes.
**********************************************************************/
void SysxIO::processSpooler() 
{
    QString sysxMsg;
    for(int i=0; i<this->sendSpooler.size(); ++i)
    {
        if(i + 1 < this->sendSpooler.size())
        {
            QString currentMsg = this->sendSpooler.at(i).left(2 * 3);
            QString nextMsg = this->sendSpooler.at(i + 1).left(2 * 3);
            if(currentMsg != nextMsg)
            {
                sysxMsg.append(this->sendSpooler.at(i));
            };
        }
        else
        {
            sysxMsg.append(this->sendSpooler.at(i));
        };
    };
    this->sendSysx(sysxMsg);
    this->sendSpooler.clear();
}

void SysxIO::setFileName(QString fileName)
{
    this->fileName = fileName;
}

QString SysxIO::getFileName()
{
    return this->fileName;
}

SysxData SysxIO::getFileSource()
{
    return this->fileSource;
}

SysxData SysxIO::getSystemSource()
{
    return this->systemSource;
}

QList<QString> SysxIO::getFileSource(QString area, QString hex1, QString hex2)
{
    QString address;
    address.append(hex1);
    address.append(hex2);
    QList<QString> sysxMsg;
    if (area == "System")
    {
        if(this->systemSource.address.indexOf(address) == -1)
        {
            sysxWriter file;
            file.setFile(":system.syx");  // Read the default sysex file so whe don't start empty handed.
            if(file.readFile())
            {
                setFileSource(area, file.getSystemSource());
            };
        };
        sysxMsg = this->systemSource.hex.at( this->systemSource.address.indexOf(address) );

    }
    else
    {
        area = "Structure";
        if(this->fileSource.address.indexOf(address) == -1)
        {
            sysxWriter file;
            file.setFile(":default.syx");  // Read the default sysex file so whe don't start empty handed.
            if(file.readFile())
            {
                setFileSource(area, file.getFileSource());
            };
        };
        sysxMsg = this->fileSource.hex.at( this->fileSource.address.indexOf(address) );


    };
    return sysxMsg;
}

QString SysxIO::getCheckSum(int dataSize)
{
    /*bool ok;
    QString base = "80";
    int sum = dataSize % base.toInt(&ok, 16);
    if(sum!=0) sum = base.toInt(&ok, 16) - sum;
    QString checksum = QString::number(sum, 16).toUpper();
    if(checksum.length()<2) checksum.prepend("0");
    return checksum;*/
    MidiTable *midiTable = MidiTable::Instance();
    return midiTable->getCheckSum(dataSize);
}

QList<QString> SysxIO::correctSysxMsg(QList<QString> sysxMsg)
{
    QString address1 = sysxMsg.at(sysxAddressOffset + 2);
    QString address2 = sysxMsg.at(sysxAddressOffset + 3);

    bool ok;

    //MidiTable *midiTable = MidiTable::Instance();
    for(int i=sysxDataOffset;i<sysxMsg.size() - 3;i++)
    {
        if(i==sysxDataOffset + 1) i++; // is reserved memmory address on the KATANA so we skip it.

        QString address3 = QString::number(i - sysxDataOffset, 16).toUpper();
        if(address3.length()<2) address3.prepend("0");
        /*
        int range = midiTable->getRange("Structure", address1, address2, address3);

        if(midiTable->isData("Structure", address1, address2, address3))
        {
            int maxRange = QString("7F").toInt(&ok, 16) + 1; // index starts at 0 -> 0-127 = 128 entry's.
            int value1 = sysxMsg.at(i).toInt(&ok, 16);
            int value2 = sysxMsg.at(i + 1).toInt(&ok, 16);
            int value = (value1 * maxRange) + value2;

            if(value > range)
            {
                value = 0;//(int)(range / 2);
                int dif = (int)(value/maxRange);
                QString valueHex1 = QString::number(dif, 16).toUpper();
                if(valueHex1.length() < 2) valueHex1.prepend("0");
                QString valueHex2 = QString::number(value - (dif * maxRange), 16).toUpper();
                if(valueHex2.length() < 2) valueHex2.prepend("0");

                sysxMsg.replace(i, valueHex1);
                sysxMsg.replace(i + 1, valueHex2);
            };

            i++;
        }
        else
        {
            if(sysxMsg.at(i).toInt(&ok, 16) > range)
            {
                int value = 0;//(int)(range / 2);
                QString valueHex = QString::number(value, 16).toUpper();
                if(valueHex.length() < 2) valueHex.prepend("0");
                sysxMsg.replace(i, valueHex);
            };
        };  */
    };

    int dataSize = 0;
    for(int i=sysxMsg.size() - 1; i>=checksumOffset;i--)
    {
        dataSize += sysxMsg.at(i).toInt(&ok, 16);
    };
    sysxMsg.replace(sysxMsg.size() - 1, getCheckSum(dataSize));

    return sysxMsg;
}

/***************************** isConnected() ******************************
* Connection status that's globaly accesible.
****************************************************************************/
bool SysxIO::isConnected()
{
    return this->connected;
}

void SysxIO::setConnected(bool connected)
{
    this->connected = connected;
    emit setStatusMessage(tr("Ready"));
}

/***************************** deviceReady() ******************************
* Midi busy or READY status that's globaly accesible. 
* To prevent multiple messages sent at once.
****************************************************************************/
bool SysxIO::deviceReady()
{
    return this->status;
}

void SysxIO::setDeviceReady(bool status)
{
    this->status = status;
}

/***************************** isDevice() **********************************
* Flag that hold if the current sysex data we are editing is from file or DEVICE.
****************************************************************************/
bool SysxIO::isDevice()
{
    return this->isdevice;
}

void SysxIO::setDevice(bool isdevice)
{
    this->isdevice = isdevice;
}

/***************************** getSyncStatus() **********************************
* Flag that hold if the sysex data we are editing is synchronized with what's 
* on the device.
****************************************************************************/
bool SysxIO::getSyncStatus()
{
    return this->syncStatus;
}

void SysxIO::setSyncStatus(bool syncStatus)
{
    this->syncStatus = syncStatus;
}

void SysxIO::setBank(int bank)
{
    this->bank = bank;
}

void SysxIO::setPatch(int patch)
{
    this->patch = patch;
}

int SysxIO::getBank(){
    return this->bank;
}

int SysxIO::getPatch(){
    return this->patch;
}

void SysxIO::setLoadedBank(int bank)
{
    this->loadedBank = bank;
}

void SysxIO::setLoadedPatch(int patch)
{
    this->loadedPatch = patch;
}

int SysxIO::getLoadedBank(){
    return this->loadedBank;
}

int SysxIO::getLoadedPatch(){
    return this->loadedPatch;
}

/*********************** getRequestName() ***********************************
* Set the name for check of the patch that we are going to load.
***************************************************************************/
void SysxIO::setRequestName(QString requestName)
{
    this->requestName = requestName;
}

/*********************** returnRequestName() ***********************************
* Return the name for check of the patch that should have been loaded.
***************************************************************************/
QString SysxIO::getRequestName()
{
    return this->requestName;
}

/*********************** getPatchChangeMsg() *****************************
* Formats the midi message for bank changing (temp buffer) and returns it.
*************************************************************************/
QString SysxIO::getPatchChangeMsg(int bank, int patch)
{
    int bankOffset = ((bank - 1) * patchPerBank) + (patch);
    int bankSize = 100; // Size of items that go in a bank ( != patch address which equals 127 ).
    int bankMsbNum = (int)(bankOffset / bankSize);
    int programChangeNum = bankOffset - (bankSize * bankMsbNum);
    QString bankMsb = QString::number(bankMsbNum, 16);
    QString programChange = QString::number(programChangeNum, 16).toUpper();

    if (bankMsb.length() < 2) bankMsb.prepend("0");
    if (programChange.length() < 2) programChange.prepend("0");

    QString midiMsg;                     // universal bank change method
    midiMsg.append("B000"+bankMsb);
    midiMsg.append("B02000");
    midiMsg.append("C0"+programChange);
   /* midiMsg.append("F0410000000033120001000000");
    midiMsg.append(programChange);
    midiMsg.append("00F7");*/
    return midiMsg;
}

/***************************** sendMidi() ***********************************
* Sends a midi message over the midiOut device sellected in the preferences.
****************************************************************************/
void SysxIO::sendMidi(QString midiMsg)
{
    if(isConnected())
    {
        Preferences *preferences = Preferences::Instance(); bool ok;
        int midiOutPort = preferences->getPreferences("Midi", "MidiOut", "device").toInt(&ok, 10);	// Get midi out device from preferences.
        //int midiInPort = preferences->getPreferences("Midi", "MidiIn", "device").toInt(&ok, 10);	// Get midi out device from preferences.
        QString device = preferences->getPreferences("Midi", "Device", "bool");

        midiIO *midi = new midiIO();
        if(device=="true")
        {
            QList<QString> midiOutDevices = midi->getMidiOutDevices();
            if ( midiOutDevices.contains("KATANA") )
            {
                midiOutPort = midiOutDevices.indexOf("KATANA");
            };
        };

        midi->sendMidi(midiMsg, midiOutPort);
        //midi->sendSysxMsg(midiMsg, midiOutPort, midiInPort);
        /*DeBugGING OUTPUT */
        if(preferences->getPreferences("Midi", "DBug", "bool")=="true")
        {
            QString dBug =("      ");
            dBug.append(midiMsg);
            emit setStatusdBugMessage(dBug);
        }
    };
}

/***************************** receiveMidi() *******************************
* Receives possible replied sysex message on sendSysex.
****************************************************************************/
void SysxIO::receiveMidi(QString midiMsg)
{
    emit midiReply(midiMsg);
}

/***************************** finishedMidi() *******************************
* Receives possible replied sysex message on sendSysex.
****************************************************************************/
void SysxIO::finishedMidi()
{
    emit rxRestart(1);
}

/***************************** readMidiInput() ******************************
* check midi input port for any midi messages
****************************************************************************/
void SysxIO::readMidiInput()
{
    emit rxRestart(0);
    Preferences *preferences = Preferences::Instance();  bool ok;
    int midiInPort = preferences->getPreferences("Midi", "MidiIn2", "device").toInt(&ok, 10);	// Get midi in device from preferences.
    if(midiInPort>0)
    {
        midiCTRL *midiRx = new midiCTRL();
        midiRx->receiveMidi(midiInPort-1);
    };
}

/***************************** finishedSending() *************************************
* Signals that we are finished sending a midi message, so we can go one with our life.
*************************************************************************************/
void SysxIO::finishedSending()
{
    emit isFinished();
    emit setStatusSymbol(1);
    emit setStatusProgress(0);
    emit setStatusMessage(tr("Ready"));

    //this->namePatchChange();
}

/***************************** requestPatchChange() *************************
* Send a patch change request. Result will be checked with checkPatchChange.
****************************************************************************/
void SysxIO::requestPatchChange(int bank, int patch)
{
    this->bankChange = bank;
    this->patchChange = patch;

    QObject::connect(this, SIGNAL(isFinished()),	// Connect the result of the request
                     this, SLOT(namePatchChange()));				// to returnPatchName function.

    QString midiMsg = getPatchChangeMsg(bank, patch);
    emit setStatusMessage(tr("Patch change"));
    this->sendMidi(midiMsg);
}

/***************************** namePatchChange() *************************
* Get the name of the patch we are switching to and check it with the 
* one requested.
****************************************************************************/
void SysxIO::namePatchChange()
{	
    QObject::disconnect(this, SIGNAL(isFinished()),
                        this, SLOT(namePatchChange()));
    QObject::disconnect(SIGNAL(patchName(QString)));

    QObject::connect(this, SIGNAL(patchName(QString)),
                     this, SLOT(checkPatchChange(QString)));

    //this->requestPatchName(0, 0);
}

/***************************** checkPatchChange() *************************namePatchChange()
* Emits a signal if the patch change was confirmed else it will retry until
* the maximum retry's has been reached.
****************************************************************************/
void SysxIO::checkPatchChange(QString name)
{	
    Q_UNUSED(name);
    QObject::disconnect(this, SIGNAL(patchName(QString)),
                        this, SLOT(checkPatchChange(QString)));

    emit isChanged();
    this->changeCount = 0;
}

/***************************** sendSysx() ***********************************
* Sends a sysex message over the midiOut device sellected in the preferences.
*****************************************************************************/
void SysxIO::sendSysx(QString sysxMsg)
{
    Preferences *preferences = Preferences::Instance();  bool ok;
    int midiOutPort = preferences->getPreferences("Midi", "MidiOut", "device").toInt(&ok, 10);	// Get midi out device from preferences.
    int midiInPort = preferences->getPreferences("Midi", "MidiIn", "device").toInt(&ok, 10);	// Get midi in device from preferences.
    QString device = preferences->getPreferences("Midi", "Device", "bool");

    midiIO *midi = new midiIO();
    if(device=="true")
    {
        QList<QString> midiInDevices = midi->getMidiInDevices();
        QList<QString> midiOutDevices = midi->getMidiOutDevices();
        if ( midiInDevices.contains("KATANA") )
        {
            midiInPort = midiInDevices.indexOf("KATANA");
        };
        if ( midiOutDevices.contains("KATANA") )
        {
            midiOutPort = midiOutDevices.indexOf("KATANA");
        };
    };
    midi->sendSysxMsg(sysxMsg, midiOutPort, midiInPort);

}

/***************************** receiveSysx() *******************************
* Receives possible replied sysex message on sendSysex.
****************************************************************************/
void SysxIO::receiveSysx(QString sysxMsg)
{
    QString reBuild;
    if((sysxMsg.size()/2 == 2928) || (sysxMsg.size()/2 == systemReplySize) || (sysxMsg.contains("F07E0006024133")))    //if((sysxMsg.size()/2 == 2332)
    {
        emit sysxReply(sysxMsg);
    } else if(sysxMsg.size()/2==2332)
    {
        QString msgText;
        msgText.append(QObject::tr("<br>Received data indicates this Katana is running Firmware 1"));
        msgText.append(QObject::tr("<br>please download FxFloorboard for Firmware 1 or"));
        msgText.append(QObject::tr("<br>update your Katana firmware to System version 3"));
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle("Incorrect Firmware detected");
        msgBox->setIcon(QMessageBox::Information);
        msgBox->setText(msgText);
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->exec();

    } else if(sysxMsg.size()/2==2790)
    {
        QString msgText;
        msgText.append(QObject::tr("<br>Received data indicates this Katana is running Firmware 2"));
        msgText.append(QObject::tr("<br>please download FxFloorboard for Firmware 2 or"));
        msgText.append(QObject::tr("<br>update your Katana firmware to System version 3"));
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle("Incorrect Firmware detected");
        msgBox->setIcon(QMessageBox::Information);
        msgBox->setText(msgText);
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->exec();

    }
    else if(sysxMsg.size()/2==2925)
    {
        QString msgText;
        msgText.append(QObject::tr("<br>Received data indicates this Katana is running Firmware 3"));
        msgText.append(QObject::tr("<br>please download FxFloorboard for Firmware 3 or"));
        msgText.append(QObject::tr("<br>upgrade your Katana firmware to System version 4"));
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle("Incorrect Firmware detected");
        msgBox->setIcon(QMessageBox::Information);
        msgBox->setText(msgText);
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->exec();

    }
    else if(sysxMsg.size()/2<2000 && sysxMsg.size()/2>1400)
       {
           QString msgText;
           msgText.append(QObject::tr("<br>Received data indicates this Katana is a MK2 model"));
           msgText.append(QObject::tr("<br>please download FxFloorboard for MK2"));
           msgText.append(QObject::tr("<br>or use an original MK1 Katana"));
           QMessageBox *msgBox = new QMessageBox();
           msgBox->setWindowTitle("Incorrect Firmware detected");
           msgBox->setIcon(QMessageBox::Information);
           msgBox->setText(msgText);
           msgBox->setStandardButtons(QMessageBox::Ok);
           msgBox->exec();

       }else
    {
        QString sysxEOF;
        QString hex;
        int msgLength = sysxMsg.length()/2;
        for(int i=0;i<msgLength*2;++i)
        {
            hex.append(sysxMsg.mid(i*2, 2));
            sysxEOF = sysxMsg.mid((i*2)+4, 2);
            if (sysxEOF == "F7")
            {
                if(hex.mid(0, 14)=="F0410000000033") {hex.replace(0, 14, "F04100000060"); }; //replace Katana header with GT-100. size = 2913 after
                int dataSize = 0; bool ok;
                for(int h=checksumOffset;h<hex.size()-1;++h)
                { dataSize += hex.mid(h*2, 2).toInt(&ok, 16); };
                QString base = "80";                       // checksum calculate.
                unsigned int sum = dataSize % base.toInt(&ok, 16);
                if(sum!=0) { sum = base.toInt(&ok, 16) - sum; };
                QString checksum = QString::number(sum, 16).toUpper();
                if(checksum.length()<2) {checksum.prepend("0");};
                hex.append(checksum);
                hex.append("F7");
                reBuild.append(hex);
                hex.clear();
                sysxEOF.clear();
                i=i+2;
            };
        };

        emit sysxReply(reBuild);
    };
   /*    Preferences *preferences = Preferences::Instance(); // Load the preferences.
    if(preferences->getPreferences("Midi", "DBug", "bool")=="true")
    {
        if (sysxMsg.size() > 0){
            QString snork;
            for(int i=0;i<sysxMsg.size();++i)
            {
                snork.append(sysxMsg.mid(i, 2));
                snork.append(" ");
                i++;
            };
            snork.replace("F7", "F7<br>");
            QString msgText;
            msgText.append(QObject::tr("<br>ReceiveSyx data size received = ")+(QString::number(sysxMsg.size()/2, 10)));

            QMessageBox *msgBox = new QMessageBox();
            msgBox->setWindowTitle(msgText);
            msgBox->setIcon(QMessageBox::Information);
            msgBox->setText(snork);
            msgBox->setStandardButtons(QMessageBox::Ok);
            msgBox->exec();
        };
    }; */
}

/***************************** requestPatchName() ***************************
* Send a patch name request. Result will be send with the returnPatchName 
* function. 
****************************************************************************/
void SysxIO::requestPatchName(int bank, int patch)
{
    QObject::disconnect(this, SIGNAL(sysxReply(QString)),
                        this, SLOT(returnPatchName(QString)));

    QObject::connect(this, SIGNAL(sysxReply(QString)),	// Connect the result of the request
                     this, SLOT(returnPatchName(QString)));			// to returnPatchName function.

    /* Patch name request. */
    MidiTable *midiTable = MidiTable::Instance();
    QString sysxMsg = midiTable->nameRequest(bank, patch);
    sendSysx(sysxMsg);
}

/***************************** returnPatchName() ***************************
* Emits a signal with the retrieved patch name.
****************************************************************************/
void SysxIO::returnPatchName(QString sysxMsg)
{
    QObject::disconnect(this, SIGNAL(sysxReply(QString)),
                        this, SLOT(returnPatchName(QString)));

    QString name;
    if(sysxMsg.size()/2 == 29)
    {
        int dataStartOffset = sysxNameOffset;   //pointer to start of names in patch file at xxxx bytes.
        for(int i=dataStartOffset*2; i<(dataStartOffset*2)+(nameLength*2);++i)   //read the length of name string.
        {
            QString hexStr = sysxMsg.mid(i, 2);
            if(hexStr == "7E")
            {
                name.append((QChar)(0x2192));
            }
            else if (hexStr == "7F")
            {
                name.append((QChar)(0x2190));
            }
            else
            {
                bool ok;
                name.append( (char)(hexStr.toInt(&ok, 16)) );
            };
            i++;
        };
    }
    else if (sysxMsg.size()/2 > 0 && sysxMsg.size()/2 != 29)
    {name = tr("bad data");}
    else {name = tr("no reply"); };
    emit patchName(name);
}


/***************************** requestPatch() ******************************
* Send a patch request. Result will be send directly with receiveSysx signal
****************************************************************************/
void SysxIO::requestPatch(int bank, int patch)
{
    /* Patch request. */
    MidiTable *midiTable = MidiTable::Instance();
    QString sysxMsg = midiTable->patchRequest(bank, patch);
    sendSysx(sysxMsg);
}

/***************************** errorSignal() ******************************
* Displays all midi related error messages.
****************************************************************************/
void SysxIO::errorSignal(QString errorType, QString errorMsg)
{
    setNoError(false);
    emit setStatusdBugMessage(errorType + "  " + errorMsg);
    this->errorType = "";
    this->errorMsg = "";
}

void SysxIO::errorReturn(QString errorType, QString errorMsg)
{
    setNoError(false);
    emit setStatusdBugMessage(errorType + "  " + errorMsg);
    this->errorType = "";
    this->errorMsg = "";
}

/***************************** noError() ******************************
* Error flag set on midi error to prevent (double) connexion faillure 
* messages and a midi messages.
************************************************************************/
bool SysxIO::noError()
{
    return this->noerror;
}

void SysxIO::setNoError(bool status)
{
    this->noerror = status;
}

/***************************** CurrentPatchName() ******************************
* This is to make it possible to verify the patch name that we are trying to receive 
* corresponds in case we had a name change that was not yet written permanantly
* to the device. (See floorBoardDisplay.cpp for more info)
************************************************************************/
void SysxIO::setCurrentPatchName(QString patchName)
{
    this->currentName = patchName;
}

QString SysxIO::getCurrentPatchName()
{
    return this->currentName;
}

/***************************** emit() *********************************
* Added to make status update possible from static methods and classes 
* like the CALLBACK in midiIO.
************************************************************************/
void SysxIO::emitStatusSymbol(int value)
{
    emit setStatusSymbol(value);
}

void SysxIO::emitStatusProgress(int value)
{
    emit setStatusProgress(value);
}

void SysxIO::emitStatusMessage(QString message)
{
    emit setStatusMessage(message);
}
void SysxIO::emitStatusdBugMessage(QString dBug)
{
    emit setStatusdBugMessage(dBug);
}

void SysxIO::systemWrite()
{
    setDeviceReady(false);			// Reserve the device for interaction.
    QString sysxMsg;
    QList< QList<QString> > systemData = getSystemSource().hex; // Get the loaded system data.
    for(int i=0;i<systemData.size();++i)
    {
        QList<QString> data = systemData.at(i);
        for(int x=0;x<data.size();++x)
        {
            QString hex;
            hex = data.at(x);
            if (hex.length() < 2) hex.prepend("0");
            sysxMsg.append(hex);
        };
    };
    sendSysx(sysxMsg);	// Send the data.
    setDeviceReady(true);
}

void SysxIO::systemDataRequest()
{
    emit setStatusProgress(100);
    //QString replyMsg;
    if (isConnected())
    {
        emit setStatusSymbol(2);
        emit setStatusMessage(tr("Request System data"));
        setDeviceReady(false); // Reserve the device for interaction.
        QObject::disconnect(this, SIGNAL(sysxReply(QString)));
        QObject::connect(this, SIGNAL(sysxReply(QString)), this, SLOT(systemReply(QString)));
        sendSysx(systemRequest); // KATANA System area data Request.

        emit setStatusProgress(0);
    }
    else
    {
        QString snork = tr("Ensure connection is active and retry");
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle(deviceType + tr(" not connected !!"));
        msgBox->setIcon(QMessageBox::Information);
        msgBox->setText(snork);
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->exec();
    };
}

void SysxIO::systemReply(QString replyMsg)
{
    QObject::disconnect(this, SIGNAL(sysxReply(QString)), this, SLOT(systemReply(QString)));
    setDeviceReady(true); // Free the device after finishing interaction.
    if(this->noError())
    {
        if(replyMsg.size()/2 == systemReplySize)
        {
            /* TRANSLATE SYSX MESSAGE FORMAT to 128 byte data blocks */
            QString header = "F0410000006012";
            QString footer ="00F7";

            QString part1 = replyMsg.mid(24, 256); //from 12, copy 128 bits (values are doubled for QString)
            part1.prepend("00000000").prepend(header).append(footer);

            QString part2 = replyMsg.mid(280, 226);
            QString part2B = replyMsg.mid(534, 30);
            part2.prepend("00000100").prepend(header).append(part2B).append(footer);

            QString part3 = replyMsg.mid(564, 256);
            part3.prepend("00000200").prepend(header).append(footer);

            QString part4 = replyMsg.mid(820, 196);
            part4.prepend("00000300").prepend(header).append(footer);

            QString part5 = replyMsg.mid(1044, 150);
            part5.prepend("00000400").prepend(header).append(footer);

            QString part6 = replyMsg.mid(1222, 82);
            part6.prepend("00020000").prepend(header).append(footer);

            replyMsg.clear();
            replyMsg.append(part1).append(part2).append(part3).append(part4).append(part5).append(part6);

            QString reBuild;       /* Add correct checksum to patch strings */
            QString sysxEOF;
            QString hex;
            int msgLength = replyMsg.length()/2;
            for(int i=0;i<msgLength*2;++i)
            {
                hex.append(replyMsg.mid(i*2, 2));
                sysxEOF = (replyMsg.mid((i*2)+4, 2));
                if (sysxEOF == "F7")
                {
                    int dataSize = 0; bool ok;
                    for(int h=checksumOffset;h<hex.size()-1;++h)
                    { dataSize += hex.mid(h*2, 2).toInt(&ok, 16); };
                    QString base = "80";                       // checksum calculate.
                    unsigned int sum = dataSize % base.toInt(&ok, 16);
                    if(sum!=0) { sum = base.toInt(&ok, 16) - sum; };
                    QString checksum = QString::number(sum, 16).toUpper();
                    if(checksum.length()<2) {checksum.prepend("0");};
                    hex.append(checksum);
                    hex.append("F7");
                    reBuild.append(hex);
                    hex = "";
                    sysxEOF = "";
                    i=i+2;
                };
            };
            replyMsg = reBuild.simplified().toUpper();
            this->setFileSource("System", replyMsg);		// Set the source to the data received.
            this->setFileName(tr("System Data from ") + deviceType);	// Set the file name to KATANA system for the display.
            this->setDevice(true);				// Patch received from the device so this is set to true.
            this->setSyncStatus(true);			// We can't be more in sync than right now! :)
            //emit systemUpdateSignal();
        }
    }
    else
    {
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle(deviceType + tr(" KATANA data or connection Error !!"));
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setTextFormat(Qt::RichText);
        QString msgText;
        msgText.append("<font size='+1'><b>");
        msgText.append(tr("The Boss ") + deviceType + tr(" Amp was not found<br>"));
        msgText.append(tr(" or an error occured during data transfer. <br>"));
        msgText.append("<b></font><br>");
        msgText.append(tr(" default system data will be as shown on screen<br>"));
        msgText.append(tr("and the KATANA system data might not be in sync<br>"));
        msgBox->setText(msgText);
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->exec();
    };

    emit setStatusMessage(tr("Ready"));
}

void SysxIO::writeToBuffer() 
{
    setDeviceReady(false);			// Reserve the device for interaction.
    QObject::disconnect(this, SIGNAL(isChanged()),
                        this, SLOT(writeToBuffer()));

    QString sysxMsg;
    QList< QList<QString> > patchData = getFileSource().hex; // Get the loaded patch data.

    emit setStatusSymbol(2);
    emit setStatusMessage(tr("Sync to ")+deviceType);

    QString addr1 = QString::number(96, 16).toUpper();  // temp address
    QString addr2 = QString::number(0, 16).toUpper();

    for(int i=0;i<patchData.size();++i)
    {
        QList<QString> data = patchData.at(i);
        for(int x=0;x<data.size();++x)
        {
            QString hex;
            if(x == sysxAddressOffset)
            {
                hex = addr1;
            }
            else if(x == sysxAddressOffset + 1)
            {
                hex = addr2;
            }
            else
            {
                hex = data.at(x);
            };
            if (hex.length() < 2) hex.prepend("0");
            sysxMsg.append(hex);
        };
    };
    setSyncStatus(true);		// In advance of the actual data transfer we set it already to sync.

    QObject::connect(this, SIGNAL(sysxReply(QString)),	// Connect the result signal
                     this, SLOT(resetDevice(QString)));					// to a slot that will reset the device after sending.
    sendSysx(sysxMsg);	// Send the data.

    emit setStatusProgress(33); // time wasting sinusidal statusbar progress
    SLEEP(150);
    emit setStatusProgress(66);
    SLEEP(150);
    emit setStatusProgress(100);
    SLEEP(150);
    emit setStatusProgress(75);
    SLEEP(150);
    emit setStatusProgress(42);
    SLEEP(150);
    emit setStatusMessage(tr("Ready"));
    setDeviceReady(true);
}

void SysxIO::relayUpdateSignal()
{
    emit updateSignal();
}

void SysxIO::writeGlobalAssign()
{
    QFile file("assign.kat");
    if (file.open(QIODevice::WriteOnly))
    {
        QList<QString> data = getSourceItems("System", "03", "00");
        QByteArray out;
        unsigned int count=0;
            for (QList<QString>::iterator code = data.begin(); code != data.end(); ++code)
            {
                QString str(*code);
                bool ok;
                unsigned int n = str.toInt(&ok, 16);
                out[count] = (char)n;
                count++;
            };
        file.write(out);
    };
}
