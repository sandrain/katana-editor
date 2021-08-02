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
#include <QFile>
#include <QDataStream>
#include <QByteArray>
#include <QMessageBox>
#include "sysxWriter.h"
#include "fileDialog.h"
#include "globalVariables.h"
#include "MidiTable.h"
#include <QJsonArray>
#include <QRegularExpression>

sysxWriter::sysxWriter()
{
    mk2 = false;
}

sysxWriter::~sysxWriter()
{

}

void sysxWriter::setFile(QString fileName)
{
    this->fileName = fileName;
    this->fileSource.address.clear();
    this->fileSource.hex.clear();
}

bool sysxWriter::readFile()
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        data = file.readAll();     // read the pre-selected file,
        QByteArray GT10_default;
        QByteArray GCL_default;
        QByteArray SMF_default;

        QFile file(":default.syx");           // Read the default KATANA sysx file .
        if (file.open(QIODevice::ReadOnly))
        {GT10_default = file.readAll(); };

        QFile GCLfile(":default.gcl");           // Read the default GT-10 GCL file .
        if (GCLfile.open(QIODevice::ReadOnly))
        {GCL_default = GCLfile.readAll(); };

        QFile smffile(":default.mid");           // Read the HexLookupTable for the SMF header file .
        if (smffile.open(QIODevice::ReadOnly))
        {SMF_default = smffile.readAll(); };

        QByteArray SYX_header = GT10_default.mid(0, 7);             // copy header from default.syx
        QByteArray other_SYX_header = GT10_default.mid(0, 3);
        QByteArray GCL_header = GCL_default.mid(3, 20);            // copy header from default.gcl
        QByteArray SMF_header = SMF_default.mid(0, 18);            // copy header from default.mid
        QByteArray SYX_file = data.mid(0, 7);                      // copy *.syx identifiable info from loaded patch
        QByteArray other_SYX_file = data.mid(0, 3);
        QByteArray GCL_file = data.mid(3, 20);
        QByteArray SMF_file = data.mid(0, 18);

        headerType = "false";
        if (SYX_file == SYX_header)                              {headerType = "GT100_SYX"; }
        else if (GCL_file == GCL_header && data.mid(2, 1) == "G"){headerType = "GT10_GXG";  }
        else if (GCL_file == GCL_header && data.mid(2, 1) == "B"){headerType = "GT10B_GXB"; }
        else if (GCL_file == GCL_header)                         {headerType = "GT100_GCL"; }
        else if (SMF_file == SMF_header)                         {headerType = "GT100_SMF"; }
        else if (other_SYX_file == other_SYX_header)             {headerType = "other_SYX"; }
        else if (fileName.contains(".tsl"))                      {headerType = "GT100_TSL"; };


        /***** TEST IF SYSTEM FILE *****************************************/
        if(data.size() == (systemSize) && headerType == "GT100_SYX"){    // if KATANA system file size is correct- load file.
            SysxIO *sysxIO = SysxIO::Instance();
            QString area = "System";
            sysxIO->setFileSource(area, data);
            this->systemSource = sysxIO->getSystemSource();
            return true;
        }
        /***** TEST IF KATANA SYX PATCH FILE *****************************************/
        else if(data.size() == patchSize && headerType == "GT100_SYX"){  //if KATANA patch file size is correct- load file
            SysxIO *sysxIO = SysxIO::Instance();
            QString area = "Structure";
            sysxIO->setFileSource(area, data);
            sysxIO->setFileName(this->fileName);
            this->fileSource = sysxIO->getFileSource();
            return true;
        }
        /***** TEST IF Version 1 KATANA SYX PATCH FILE *****************************************/
        else if(data.size() == 2152 && headerType == "GT100_SYX"){  //if KATANA patch file size is correct- load file
            GT10_default.replace(0, 2115, data.mid(0, 2115));
            SysxIO *sysxIO = SysxIO::Instance();
            QString area = "Structure";
            sysxIO->setFileSource(area, GT10_default);
            sysxIO->setFileName(this->fileName);
            this->fileSource = sysxIO->getFileSource();
            return true;
        }
        /***** TEST IF KATANA SMF PATCH FILE *****************************************/
        else if (headerType == "GT100_SMF")
        {                                        // file contains a .mid type SMF patch file header from Boss Librarian
            QFile file(":default.syx");              // Read the default KATANA sysx file so we don't start empty handed.
            if (file.open(QIODevice::ReadOnly))
            {	GT100_default = file.readAll(); };
            QByteArray temp;                         // TRANSLATION of KATANA SMF PATCHES, data read from smf patch **************

            if ( data.at(37) == char(47) ){    // check if a valid GT-10 file
                headerType = "GT10_SMF";
                convertFromGT10();
                return true;
            };
            if ( data.at(37) == char(48) ){    // check if a valid GT-10B file
                headerType = "GT10B_SMF";
                convertFromGT10B();
                return true;
            };

            index = 1;
            int patchCount = (data.size()-32)/2322;
            if (patchCount>1)
            {
                QString msgText;
                QString patchText;
                QString patchNumber;
                char r;
                this->patchList.clear();
                this->patchList.append(QObject::tr("Select Patch"));
                int a = 43; // locate patch text start position from the start of the file
                for (int h=0;h<patchCount;h++)
                {
                    for (int b=0;b<16;b++)
                    {
                        r = char(data[a+b]);
                        patchText.append(r);
                    };
                    patchNumber = QString::number(h+1, 10).toUpper();
                    msgText.append(patchNumber + " : ");
                    msgText.append(patchText + "   ");
                    this->patchList.append(msgText);
                    patchText.clear();
                    msgText.clear();
                    a=a+2322;                      // offset is set in front of marker
                };

                QString type = "smf";
                fileDialog *dialog = new fileDialog(fileName, patchList, data, GT10_default, type);
                dialog->exec();
                patchIndex(this->index);
            };

            int a=0;
            if (patchCount>1)
            {
                int q=index-1;      // find start of required patch
                a = q*2322;
            };
            temp = data.mid(a+43, 128);            // copy SMF 128 bytes#
            GT100_default.replace(11, 128, temp);             // replace gt100 address "00"#
            temp = data.mid(a+171, 114);           // copy SMF part1#
            temp.append(data.mid(a+301,14));       // copy SMF part2#
            GT100_default.replace(152, 128, temp);            // replace gt100 address "01"#
            temp = data.mid(a+315, 128);           // copy SMF part1#
            GT100_default.replace(293, 128, temp);            // replace gt100 address "02"#
            temp = data.mid(a+443, 100);           // copy SMF part1#
            temp.append(data.mid(a+559,28));       // copy SMF part2#
            GT100_default.replace(434, 128, temp);            // replace gt100 address "03"#
            temp = data.mid(a+587, 128);           // copy SMF part1#
            GT100_default.replace(575, 128, temp);            // replace gt100 address "04"#
            temp = data.mid(a+715, 86);            // copy SMF part1#
            temp.append(data.mid(a+817,42));      // copy SMF part2#
            GT100_default.replace(716, 128, temp);             // replace gt100 address "05"#
            temp = data.mid(a+859, 128);           // copy SMF part1#
            GT100_default.replace(857,128, temp);             // replace gt100 address "06"#
            temp = data.mid(a+987, 72);            // copy SMF part1#
            temp.append(data.mid(a+1075,56));      // copy SMF part2#
            GT100_default.replace(998, 128, temp);            // replace gt100 address "07"#
            temp = data.mid(a+1131, 128);          // copy SMF part1#
            GT100_default.replace(1139,128, temp);            // replace gt100 address "08"#
            temp = data.mid(a+1259, 58);           // copy SMF part1#
            temp.append(data.mid(a+1333,70));      // copy SMF part2#
            GT100_default.replace(1280, 128, temp);           // replace gt100 address "09"#
            temp = data.mid(a+1403, 128);          // copy SMF part1#
            GT100_default.replace(1421,128, temp);            // replace gt100 address "0A"#
            temp = data.mid(a+1531, 44);           // copy SMF part1#
            temp.append(data.mid(a+1591,84));      // copy SMF part2#
            GT100_default.replace(1562, 128, temp);           // replace gt100 address "0B"#
            temp = data.mid(a+1675, 128);          // copy SMF part1#
            GT100_default.replace(1703,128, temp);            // replace gt100 address "0C"#
            temp = data.mid(a+1803, 30);           // copy SMF part1#
            temp.append(data.mid(a+1849,98));      // copy SMF part2#
            GT100_default.replace(1844, 128, temp);           // replace gt100 address "0D"#
            temp = data.mid(a+1947, 128);           // copy SMF part1#
            GT100_default.replace(1985, 128, temp);           // replace gt100 address "0E"#
            temp = data.mid(a+2075, 16);           // copy SMF part1#
            temp.append(data.mid(a+2107,112));      // copy SMF part2  - ver1 was 8
            GT100_default.replace(2126, 128, temp);           // replace gt100 address "0F"  - ver1 was 24
            temp = data.mid(a+2219, 60);           // copy SMF part1#
            GT100_default.replace(2267, 60, temp);           // replace gt100 address "10"
            if (index>0)
            {
                SysxIO *sysxIO = SysxIO::Instance();
                QString area = "Structure";
                sysxIO->setFileSource(area, GT100_default);
                sysxIO->setFileName(this->fileName);
                this->fileSource = sysxIO->getFileSource();
                return true;
            } else {return false; }
        }
        /***** TEST IF KATANA GCL PATCH FILE *****************************************/
        else if (headerType == "GT100_GCL")
        {
            index=1;
            char msb = char(data[34]);     // find patch count msb bit in GCL file at byte 34
            char lsb = char(data[35]);     // find patch count lsb bit in GCL file at byte 35
            bool ok;
            int patchCount;
            patchCount = (256*QString::number(msb, 16).toUpper().toInt(&ok, 16)) + (QString::number(lsb, 16).toUpper().toInt(&ok, 16));
            QByteArray marker;
            if (patchCount>1)
            {
                QString msgText;
                marker = data.mid(170, 2);      //copy marker key to find "0882" which marks the start of each patch block
                QString patchText;
                QString patchNumber;
                this->patchList.clear();
                this->patchList.append(QObject::tr("Select Patch"));
                int a = data.indexOf(marker, 0); // locate patch start position from the start of the file
                a=a+2;                             // offset is set in front of marker
                for (int h=0;h<patchCount;h++)
                {
                    for (int b=0;b<16;b++)
                    {
                        char r = char(data[a+b]);
                        patchText.append(r);
                    };
                    patchNumber = QString::number(h+1, 10).toUpper();
                    msgText.append(patchNumber + " : ");
                    msgText.append(patchText + "   ");
                    this->patchList.append(msgText);
                    patchText.clear();
                    msgText.clear();
                    a = data.indexOf(marker, a); // locate patch start position from the start of the file
                    a=a+2;                      // offset is set in front of marker
                };

                QString type = "gcl";
                fileDialog *dialog = new fileDialog(fileName, patchList, data, GT10_default, type);
                dialog->exec();
                patchIndex(this->index);
            };

            marker = data.mid(170, 2);                 //copy marker key to find "0882" which marks the start of each patch block
            int a = data.indexOf(marker, 0);  // locate patch start position from the start of the file
            a=a+2;                                     // offset is set in front of marker
            if (patchCount>1)
            {
                int q=index-1;
                for (int h=0;h<q;h++)
                {
                    a = data.indexOf(marker, a);          // locate patch start position from the start of the file
                    a=a+2;
                };                                     // offset is set in front of marker
            };
            QByteArray temp;
            temp = data.mid(a, 128);
            GT10_default.replace(11, 128, temp);       //address "00" +
            temp = data.mid(a+128, 128);
            GT10_default.replace(152, 128, temp);      //address "01" +
            temp = data.mid(a+256, 128);
            GT10_default.replace(293, 128, temp);      //address "02" +
            temp = data.mid(a+384, 128);
            GT10_default.replace(434, 128, temp);      //address "03" +
            temp = data.mid(a+512, 128);
            GT10_default.replace(575, 128, temp);      //address "04" +
            temp = data.mid(a+640, 128);
            GT10_default.replace(716, 128, temp);       //address "05" +
            temp = data.mid(a+768, 128);
            GT10_default.replace(857, 128, temp);      //address "06" +
            temp = data.mid(a+896, 128);
            GT10_default.replace(998, 128, temp);      //address "07" +
            temp = data.mid(a+1024, 128);
            GT10_default.replace(1139, 128, temp);     //address "08" +
            temp = data.mid(a+1152, 128);
            GT10_default.replace(1280, 128, temp);     //address "09" +
            temp = data.mid(a+1280, 128);
            GT10_default.replace(1421, 128, temp);     //address "0A" +
            temp = data.mid(a+1408, 128);
            GT10_default.replace(1562, 128, temp);     //address "0B" +
            temp = data.mid(a+1536, 128);
            GT10_default.replace(1703, 128, temp);     //address "0C" +
            temp = data.mid(a+1664, 128);
            GT10_default.replace(1844, 128, temp);     //address "0D" +
            temp = data.mid(a+1792, 128);
            GT10_default.replace(1985, 128, temp);     //address "0E" +
            temp = data.mid(a+1920, 128);
            GT10_default.replace(2126, 128, temp);     //address "0F" +
            temp = data.mid(a+2048, 60);
            GT10_default.replace(2267, 60, temp);     //address "10" +

            if (index>0)
            {
                SysxIO *sysxIO = SysxIO::Instance();
                QString area = "Structure";
                sysxIO->setFileSource(area, GT10_default);
                sysxIO->setFileName(this->fileName);
                this->fileSource = sysxIO->getFileSource();
                return true;
            }
        }
        else if (headerType == "GT100_TSL"){
            convertFromTSL(0);
            return true;
        }
        else if (headerType == "GT10_GXG"){
            convertFromGT10();
            return true;
        }
        else if (headerType == "GT10B_GXB"){
            convertFromGT10B();
            return true;
        }
        else if(headerType == "other_SYX")
        {
            other_SYX_file = data.mid(0, 11);
            QString header;
            for(int x=0; x<10; ++x)
            {
                char r = other_SYX_file.at(x+1);
                QString val = QString::number(r, 16).toUpper();
                if(val.size()<2){val.prepend("0"); };
                header.append(val);
            };
            bool ok;
            int address = header.mid(12, 2).toInt(&ok, 16);
            bool isPatch = false;
            if(address > 5)isPatch = true;
            if(isPatch && header.contains("00002F") )
            {
                headerType = "GT10_SYX";
                convertFromGT10();
                return true;
            } else if(isPatch && header.contains("000030") )
            {
                headerType = "GT10B_SYX";
                convertFromGT10B();
                return true;
            }
            else if(isPatch && header.contains("000006") )
            {
                convertFromGT8();
                return true;
            }
            else if(header.contains("005012") )
            {
                convertFromGT6B();
                return true;
            } else
            {
                QMessageBox *msgBox = new QMessageBox();
                msgBox->setWindowTitle(QObject::tr("Patch type Error!"));
                msgBox->setIcon(QMessageBox::Warning);
                msgBox->setTextFormat(Qt::RichText);
                QString msgText;
                msgText.append("<font size='+1'><b>");
                msgText.append(QObject::tr("This is not a ") + deviceType + QObject::tr(" patch!"));
                msgText.append("<b></font><br>");
                if (header.contains("005012")){
                    msgText.append(QObject::tr("but appears to be a GT-6B patch<br>"));};
                if (header.contains("004612")){
                    msgText.append(QObject::tr("but appears to be a GT-6 patch<br>"));};
                if (header.contains("001B12")){
                    msgText.append(QObject::tr("but appears to be a GT-3 patch<br>"));};
                if (header.contains("000412")){
                    msgText.append(QObject::tr("but appears to be a GT-5 patch<br>"));};

                msgText.append(QObject::tr("Patch data size is ") + (QString::number(data.size(), 10)) + QObject::tr(" bytes, and does not quite meet KATANAFxFloorBoard requirements."));
                msgBox->setText(msgText);
                msgBox->setStandardButtons(QMessageBox::Ok);
                msgBox->exec();
                return false;
            };
        }
        else
        {
            QMessageBox *msgBox = new QMessageBox();
            msgBox->setWindowTitle(QObject::tr("Patch type Error!"));
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->setTextFormat(Qt::RichText);
            QString msgText;
            msgText.append("<font size='+1'><b>");
            msgText.append(QObject::tr("This is not a ") + deviceType + QObject::tr(" patch!"));
            msgText.append("<b></font><br>");
            msgText.append(QObject::tr("Patch data size is ") + (QString::number(data.size(), 10)) + QObject::tr(" bytes, and does not meet KATANAFxFloorBoard requirements."));
            msgBox->setText(msgText);
            msgBox->setStandardButtons(QMessageBox::Ok);
            msgBox->exec();
            return false;
        };
    }
    else
    {
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle(QObject::tr("Patch size Error!"));
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setTextFormat(Qt::RichText);
        QString msgText = "FILE ERROR - not recognised format";
        msgBox->setText(msgText);
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->exec();
        return false;
    };
    return false;
}

void sysxWriter::patchIndex(int listIndex)
{
    Q_UNUSED(listIndex);
    SysxIO *sysxIO = SysxIO::Instance();
    this->index=sysxIO->patchListValue;
}

void sysxWriter::writeLoaderFile(QString fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        SysxIO *sysxIO = SysxIO::Instance();
        QByteArray loader=sysxIO->data;
        loader.chop(1);
        QByteArray assign;
        assign.append(",").append(char(34)).append("hex").append(char(34)).append(":").append(char(34));
        QList<QString> data = sysxIO->getSourceItems("System", "03", "00");
        QByteArray out;
        int count=0;
        for (QList<QString>::iterator code = data.begin(); code != data.end(); ++code)
        {
            out.append(data.at(count));
            count++;
        };
        if(loader.contains(assign))
        {
            loader.replace(loader.lastIndexOf(assign)+8, 282, out);
            loader.append(char(34)).append("}");
        }else{
            loader.append(assign).append(out).append(char(34)).append("}");
        };
        file.write(loader);
    };
}

void sysxWriter::writeSystemFile(QString fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        SysxIO *sysxIO = SysxIO::Instance();
        this->systemSource = sysxIO->getSystemSource();

        QByteArray out;
        unsigned int count=0;
        for (QList< QList<QString> >::iterator dev = systemSource.hex.begin(); dev != systemSource.hex.end(); ++dev)
        {
            QList<QString> data(*dev);
            for (QList<QString>::iterator code = data.begin(); code != data.end(); ++code)
            {
                QString str(*code);
                bool ok;
                int n = str.toInt(&ok, 16);
                out[count] = char(n);
                count++;
            };
        };
        file.write(out);
    };
}

void sysxWriter::writeSYX(QString fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        SysxIO *sysxIO = SysxIO::Instance();
        this->fileSource = sysxIO->getFileSource();

        QByteArray out;
        unsigned int count=0;
        for (QList< QList<QString> >::iterator dev = fileSource.hex.begin(); dev != fileSource.hex.end(); ++dev)
        {
            QList<QString> data(*dev);
            for (QList<QString>::iterator code = data.begin(); code != data.end(); ++code)
            {
                QString str(*code);
                bool ok;
                int n = str.toInt(&ok, 16);
                out[count] = char(n);
                count++;
            };
        };
        file.write(out);
    };
}

void sysxWriter::writeSMF(QString fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        SysxIO *sysxIO = SysxIO::Instance();
        this->fileSource = sysxIO->getFileSource();

        QByteArray out;
        unsigned int count=0;
        for (QList< QList<QString> >::iterator dev = fileSource.hex.begin(); dev != fileSource.hex.end(); ++dev)
        {
            QList<QString> data(*dev);
            for (QList<QString>::iterator code = data.begin(); code != data.end(); ++code)
            {
                QString str(*code);
                bool ok;
                int n = str.toInt(&ok, 16);
                out[count] = char(n);
                count++;
            };
        };
        // TRANSLATION of KATANA PATCHES, data read from syx patch **************
        QByteArray temp;                                               // and replace syx headers with mid data and new addresses**************
        QFile hexfile(":default.mid");   // use a QByteArray of hex numbers from a lookup table.
        if (hexfile.open(QIODevice::ReadOnly))
        { GT100_default = hexfile.readAll(); };
        int a = 0;
        temp = out.mid(11, 128);
        temp.append(out.mid(152, 114));
        GT100_default.replace(a+43, 242, temp);         // replace SMF address "0000"#

        temp = out.mid(266, 14);
        temp.append(out.mid(293,128));
        temp.append(out.mid(434,100));
        GT100_default.replace(a+301, 242, temp);        // replace SMF address "0172"#

        temp = out.mid(534, 28);
        temp.append(out.mid(575,128));
        temp.append(out.mid(716,86));
        GT100_default.replace(a+559, 242, temp);        // replace SMF address "0346"#

        temp = out.mid(802, 42);
        temp.append(out.mid(857, 128));
        temp.append(out.mid(998, 72));
        GT100_default.replace(a+817, 242, temp);        // replace SMF address "0542"#

        temp = out.mid(1070, 56);
        temp.append(out.mid(1139, 128));
        temp.append(out.mid(1280, 58));
        GT100_default.replace(a+1075, 242, temp);       // replace SMF address "0748"#

        temp = out.mid(1338, 70);
        temp.append(out.mid(1421,128));
        temp.append(out.mid(1562,44));
        GT100_default.replace(a+1333, 242, temp);       // replace SMF address "093A"#

        temp = out.mid(1606,84);
        temp.append(out.mid(1703,128));
        temp.append(out.mid(1844,30));
        GT100_default.replace(a+1591, 242, temp);       // replace SMF address "0B2C"#

        temp = out.mid(1874, 98);
        temp.append(out.mid(1985,128));
        temp.append(out.mid(2126,16));
        GT100_default.replace(a+1849, 242, temp);       // replace SMF address "0D1E"#

        temp = out.mid(2142,112);
        temp.append(out.mid(2267,60));
        GT100_default.replace(a+2107, 172, temp);         // replace SMF address "0F10"#

        QByteArray header(GT100_default.mid(0,29));
        QByteArray footer(GT100_default.mid(2351,4));
        GT100_default.remove(0,29);
        GT100_default.remove(2351,4);

        this->fileSource.address.clear();
        this->fileSource.hex.clear();

        QList<QString> sysxBuffer;
        int dataSize = 0; int offset = 0;
        for(int i=0;i<GT100_default.size();i++)
        {
            char byte = char(GT100_default[i]);
            int n = int(byte);
            QString hex = QString::number(n, 16).toUpper();
            if (hex.length() < 2) hex.prepend("0");
            sysxBuffer.append(hex);

            char nextbyte = char(GT100_default[i+1]);
            int nextn = int(nextbyte);
            QString nexthex = QString::number(nextn, 16).toUpper();
            if (nexthex.length() < 2) nexthex.prepend("0");
            if(offset >= checksumOffset+3 && nexthex != "F7")   // smf offset is 8 bytes + previous byte
            {
                dataSize += n;
            };
            if(nexthex == "F7")
            {
                QString checksum;
                bool ok;
                int dataSize = 0;
                for(int i=checksumOffset+3;i<sysxBuffer.size()-1;++i)
                { dataSize += sysxBuffer.at(i).toInt(&ok, 16); };
                QString base = "80";
                int sum = dataSize % base.toInt(&ok, 16);
                if(sum!=0) sum = base.toInt(&ok, 16) - sum;
                checksum = QString::number(sum, 16).toUpper();
                if(checksum.length()<2) checksum.prepend("0");
                sysxBuffer.replace(sysxBuffer.size() - 1, checksum);
            };
            offset++;

            if(hex == "F7")
            {
                this->fileSource.address.append( sysxBuffer.at(sysxAddressOffset + 5) + sysxBuffer.at(sysxAddressOffset + 6) );
                this->fileSource.hex.append(sysxBuffer);
                sysxBuffer.clear();
                dataSize = 0;
                offset = 0;
            };
        };

        GT100_default.clear();
        count=0;
        for (QList< QList<QString> >::iterator dev = fileSource.hex.begin(); dev != fileSource.hex.end(); ++dev)
        {
            QList<QString> data(*dev);
            for (QList<QString>::iterator code = data.begin(); code != data.end(); ++code)
            {
                QString str(*code);
                bool ok;
                int n = str.toInt(&ok, 16);
                GT100_default[count] = char(n);
                count++;
            };
        };
        GT100_default.prepend(header);           // place smf header after checksum is added
        GT100_default.append(footer);             // place smf footer after "F7" EOF
        file.write(GT100_default);
    };
}

void sysxWriter::writeGCL(QString fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        SysxIO *sysxIO = SysxIO::Instance();
        this->fileSource = sysxIO->getFileSource();

        QByteArray out;
        unsigned int count=0;
        for (QList< QList<QString> >::iterator dev = fileSource.hex.begin(); dev != fileSource.hex.end(); ++dev)
        {
            QList<QString> data(*dev);
            for (QList<QString>::iterator code = data.begin(); code != data.end(); ++code)
            {
                QString str(*code);
                bool ok;
                int n = str.toInt(&ok, 16);
                out[count] = char(n);
                count++;
            };
        };
        QByteArray GCL_default;
        QByteArray temp;
        int a = 172;
        QFile GCLfile(":default.gcl");           // Read the default KATANA GCL file .
        if (GCLfile.open(QIODevice::ReadOnly))
        { GCL_default = GCLfile.readAll(); };
        temp = out.mid(11, 128);
        GCL_default.replace(a, 128, temp);         //address "00"
        temp = out.mid(152, 128);
        GCL_default.replace(a+128, 128, temp);     //address "01"
        temp = out.mid(293, 128);
        GCL_default.replace(a+256, 128, temp);     //address "02"
        temp = out.mid(434, 128);
        GCL_default.replace(a+384, 128, temp);     //address "03"
        temp = out.mid(575, 128);
        GCL_default.replace(a+512, 128, temp);     //address "04"
        temp = out.mid(716, 128);
        GCL_default.replace(a+640, 128, temp);      //address "05"
        temp = out.mid(857, 128);
        GCL_default.replace(a+768, 128, temp);     //address "06"
        temp = out.mid(998, 128);
        GCL_default.replace(a+896, 128, temp);     //address "07"
        temp = out.mid(1139, 128);
        GCL_default.replace(a+1024, 128, temp);    //address "08"
        temp = out.mid(1280, 128);
        GCL_default.replace(a+1152, 128, temp);    //address "09"
        temp = out.mid(1421, 128);
        GCL_default.replace(a+1280, 128, temp);    //address "0A"
        temp = out.mid(1562, 128);
        GCL_default.replace(a+1408, 128, temp);    //address "0B"
        temp = out.mid(1703, 128);
        GCL_default.replace(a+1536, 128, temp);    //address "0C"
        temp = out.mid(1844, 128);
        GCL_default.replace(a+1664, 128, temp);    //address "0D"
        temp = out.mid(1985, 128);
        GCL_default.replace(a+1792, 128, temp);    //address "0E"
        temp = out.mid(2126, 128);
        GCL_default.replace(a+1920, 128, temp);    //address "0F"
        temp = out.mid(2267, 60);
        GCL_default.replace(a+2048, 60, temp);    //address "10"

        file.write(GCL_default);
    };
}

void sysxWriter::writeTSL(QString fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        hexToTSL();
        file.write(this->TSL_default);
    };
}

void sysxWriter::hexToTSL()
{
    SysxIO *sysxIO = SysxIO::Instance();
    this->fileSource = sysxIO->getFileSource();  //copy the current QString patch data.

    QByteArray temp;
    unsigned int count=0;
    for (QList< QList<QString> >::iterator dev = this->fileSource.hex.begin(); dev != this->fileSource.hex.end(); ++dev)
    {   // convert QString data to char hex (QByteArray).
        QList<QString> data(*dev);
        for (QList<QString>::iterator code = data.begin(); code != data.end(); ++code)
        {
            QString str(*code);
            bool ok;
            int n = str.toInt(&ok, 16);
            temp[count] = char(n);
            count++;
        };
    };

    QFile TSLfile(":default.tsl");           // Read the default KATANA TSL file .
    if (TSLfile.open(QIODevice::ReadOnly))
    { this->TSL_default = TSLfile.readAll(); };
    AppendTSL(temp.mid(11, 1), "patch_name1");  //copy patch name
    AppendTSL(temp.mid(12, 1), "patch_name2");  //copy patch name
    AppendTSL(temp.mid(13, 1), "patch_name3");  //copy patch name
    AppendTSL(temp.mid(14, 1), "patch_name4");  //copy patch name
    AppendTSL(temp.mid(15, 1), "patch_name5");  //copy patch name
    AppendTSL(temp.mid(16, 1), "patch_name6");  //copy patch name
    AppendTSL(temp.mid(17, 1), "patch_name7");  //copy patch name
    AppendTSL(temp.mid(18, 1), "patch_name8");  //copy patch name
    AppendTSL(temp.mid(19, 1), "patch_name9");  //copy patch name
    AppendTSL(temp.mid(20, 1), "patch_name10"); //copy patch name
    AppendTSL(temp.mid(21, 1), "patch_name11"); //copy patch name
    AppendTSL(temp.mid(22, 1), "patch_name12"); //copy patch name
    AppendTSL(temp.mid(23, 1), "patch_name13"); //copy patch name
    AppendTSL(temp.mid(24, 1), "patch_name14"); //copy patch name
    AppendTSL(temp.mid(25, 1), "patch_name15"); //copy patch name
    AppendTSL(temp.mid(26, 1), "patch_name16"); //copy patch name
    AppendTSL(temp.mid(27, 1), "output_select");   //copy output select


    AppendTSL(temp.mid(43, 1), "comp_on_off");     //copy comp
    AppendTSL(temp.mid(44, 1), "comp_type");       //copy comp
    AppendTSL(temp.mid(45, 1), "comp_sustain");    //copy comp
    AppendTSL(temp.mid(46, 1), "comp_attack");     //copy comp
    AppendTSL(temp.mid(47, 1), "comp_tone");       //copy comp
    AppendTSL(temp.mid(48, 1), "comp_level");      //copy comp
    AppendTSL(temp.mid(59, 1), "od_ds_on_off");              //copy dist
    AppendTSL(temp.mid(60, 1), "od_ds_type");                //copy dist
    AppendTSL(temp.mid(61, 1), "od_ds_drive");               //copy dist
    AppendTSL(temp.mid(62, 1), "od_ds_bottom");              //copy dist
    AppendTSL(temp.mid(63, 1), "od_ds_tone");                //copy dist
    AppendTSL(temp.mid(64, 1), "od_ds_solo_sw");             //copy dist
    AppendTSL(temp.mid(65, 1), "od_ds_solo_level");          //copy dist
    AppendTSL(temp.mid(66, 1), "od_ds_effect_level");        //copy dist
    AppendTSL(temp.mid(67, 1), "od_ds_direct_mix");          //copy dist
    AppendTSL(temp.mid(68, 1), "od_ds_custom_type");         //copy dist
    AppendTSL(temp.mid(69, 1), "od_ds_custom_bottom");       //copy dist
    AppendTSL(temp.mid(70, 1), "od_ds_custom_top");          //copy dist
    AppendTSL(temp.mid(71, 1), "od_ds_custom_low");          //copy dist
    AppendTSL(temp.mid(72, 1), "od_ds_custom_high");         //copy dist
    AppendTSL(temp.mid(73, 1), "od_ds_custom_character");    //copy dist
    AppendTSL(temp.mid(91, 1), "preamp_a_on_off");                 //copy pre A
    AppendTSL(temp.mid(92, 1), "preamp_a_type");                   //copy pre A
    AppendTSL(temp.mid(93, 1), "preamp_a_gain");                   //copy pre A
    AppendTSL(temp.mid(94, 1), "preamp_a_t_comp");                 //copy pre A
    AppendTSL(temp.mid(95, 1), "preamp_a_bass");                   //copy pre A
    AppendTSL(temp.mid(96, 1), "preamp_a_middle");                 //copy pre A
    AppendTSL(temp.mid(97, 1), "preamp_a_treble");                 //copy pre A
    AppendTSL(temp.mid(98, 1), "preamp_a_presence");               //copy pre A
    AppendTSL(temp.mid(99, 1), "preamp_a_level");                  //copy pre A
    AppendTSL(temp.mid(100, 1), "preamp_a_bright");                //copy pre A
    AppendTSL(temp.mid(101, 1), "preamp_a_gain_sw");               //copy pre A
    AppendTSL(temp.mid(102, 1), "preamp_a_solo_sw");               //copy pre A
    AppendTSL(temp.mid(103, 1), "preamp_a_solo_level");            //copy pre A
    AppendTSL(temp.mid(104, 1), "preamp_a_sp_type");               //copy pre A
    AppendTSL(temp.mid(105, 1), "preamp_a_mic_type");              //copy pre A
    AppendTSL(temp.mid(106, 1), "preamp_a_mic_dis");               //copy pre A
    AppendTSL(temp.mid(107, 1), "preamp_a_mic_pos");               //copy pre A
    AppendTSL(temp.mid(108, 1), "preamp_a_mic_level");             //copy pre A
    AppendTSL(temp.mid(109, 1), "preamp_a_direct_mix");            //copy pre A
    AppendTSL(temp.mid(110, 1), "preamp_a_custom_type");           //copy pre A
    AppendTSL(temp.mid(111, 1), "preamp_a_custom_bottom");         //copy pre A
    AppendTSL(temp.mid(112, 1), "preamp_a_custom_edge");           //copy pre A
    AppendTSL(temp.mid(115, 1), "preamp_a_custom_preamp_low");     //copy pre A
    AppendTSL(temp.mid(116, 1), "preamp_a_custom_preamp_high");    //copy pre A
    AppendTSL(temp.mid(117, 1), "preamp_a_custom_char");           //copy pre A
    AppendTSL(temp.mid(118, 1), "preamp_a_custom_sp_size");        //copy pre A
    AppendTSL(temp.mid(119, 1), "preamp_a_custom_sp_color_low");   //copy pre A
    AppendTSL(temp.mid(120, 1), "preamp_a_custom_sp_color_high");  //copy pre A
    AppendTSL(temp.mid(121, 1), "preamp_a_custom_sp_num");         //copy pre A
    AppendTSL(temp.mid(122, 1), "preamp_a_custom_sp_cabinet");     //copy pre A
    AppendTSL(temp.mid(152, 1), "preamp_b_on_off");              //copy pre B
    AppendTSL(temp.mid(153, 1), "preamp_b_type");                //copy pre B
    AppendTSL(temp.mid(154, 1), "preamp_b_gain");                //copy pre B
    AppendTSL(temp.mid(155, 1), "preamp_b_t_comp");              //copy pre B
    AppendTSL(temp.mid(156, 1), "preamp_b_bass");                //copy pre B
    AppendTSL(temp.mid(157, 1), "preamp_b_middle");              //copy pre B
    AppendTSL(temp.mid(158, 1), "preamp_b_treble");              //copy pre B
    AppendTSL(temp.mid(159, 1), "preamp_b_presence");            //copy pre B
    AppendTSL(temp.mid(160, 1), "preamp_b_level");               //copy pre B
    AppendTSL(temp.mid(161, 1), "preamp_b_bright");              //copy pre B
    AppendTSL(temp.mid(162, 1), "preamp_b_gain_sw");             //copy pre B
    AppendTSL(temp.mid(163, 1), "preamp_b_solo_sw");             //copy pre B
    AppendTSL(temp.mid(164, 1), "preamp_b_solo_level");          //copy pre B
    AppendTSL(temp.mid(165, 1), "preamp_b_sp_type");             //copy pre B
    AppendTSL(temp.mid(166, 1), "preamp_b_mic_type");            //copy pre B
    AppendTSL(temp.mid(167, 1), "preamp_b_mic_dis");             //copy pre B
    AppendTSL(temp.mid(168, 1), "preamp_b_mic_pos");             //copy pre B
    AppendTSL(temp.mid(169, 1), "preamp_b_mic_level");           //copy pre B
    AppendTSL(temp.mid(170, 1), "preamp_b_direct_mix");          //copy pre B
    AppendTSL(temp.mid(171, 1), "preamp_b_custom_type");         //copy pre B
    AppendTSL(temp.mid(172, 1), "preamp_b_custom_bottom");       //copy pre B
    AppendTSL(temp.mid(173, 1), "preamp_b_custom_edge");         //copy pre B
    AppendTSL(temp.mid(176, 1), "preamp_b_custom_preamp_low");   //copy pre B
    AppendTSL(temp.mid(177, 1), "preamp_b_custom_preamp_high");  //copy pre B
    AppendTSL(temp.mid(178, 1), "preamp_b_custom_char");         //copy pre B
    AppendTSL(temp.mid(179, 1), "preamp_b_custom_sp_size");      //copy pre B
    AppendTSL(temp.mid(180, 1), "preamp_b_custom_sp_color_low"); //copy pre B
    AppendTSL(temp.mid(181, 1), "preamp_b_custom_sp_color_high");//copy pre B
    AppendTSL(temp.mid(182, 1), "preamp_b_custom_sp_num");       //copy pre B
    AppendTSL(temp.mid(183, 1), "preamp_b_custom_sp_cabinet");   //copy pre B
    AppendTSL(temp.mid(200, 1), "eq_on_off");                 //copy EQ
    AppendTSL(temp.mid(201, 1), "eq_low_cut");                //copy EQ
    AppendTSL(temp.mid(202, 1), "eq_low_gain");               //copy EQ
    AppendTSL(temp.mid(203, 1), "eq_low_mid_freq");           //copy EQ
    AppendTSL(temp.mid(204, 1), "eq_low_mid_q");              //copy EQ
    AppendTSL(temp.mid(205, 1), "eq_low_mid_gain");           //copy EQ
    AppendTSL(temp.mid(206, 1), "eq_high_mid_freq");          //copy EQ
    AppendTSL(temp.mid(207, 1), "eq_high_mid_q");             //copy EQ
    AppendTSL(temp.mid(208, 1), "eq_high_mid_gain");          //copy EQ
    AppendTSL(temp.mid(209, 1), "eq_high_gain");              //copy EQ
    AppendTSL(temp.mid(210, 1), "eq_high_cut");               //copy EQ
    AppendTSL(temp.mid(211, 1), "eq_level");                  //copy EQ
    AppendTSL(temp.mid(216, 1), "fx1_on_off");                  //copy FX1
    AppendTSL(temp.mid(217, 1), "fx1_fx_type");                  //copy FX1
    AppendTSL(temp.mid(218, 1), "fx1_sub_od_ds_type");                  //copy FX1
    AppendTSL(temp.mid(219, 1), "fx1_sub_od_ds_drive");                  //copy FX1
    AppendTSL(temp.mid(220, 1), "fx1_sub_od_ds_bottom");                  //copy FX1
    AppendTSL(temp.mid(221, 1), "fx1_sub_od_ds_tone");                  //copy FX1
    AppendTSL(temp.mid(222, 1), "fx1_sub_od_ds_solo_sw");                  //copy FX1
    AppendTSL(temp.mid(223, 1), "fx1_sub_od_ds_solo_level");                  //copy FX1
    AppendTSL(temp.mid(224, 1), "fx1_sub_od_ds_effect_level");                  //copy FX1
    AppendTSL(temp.mid(225, 1), "fx1_sub_od_ds_direct_mix");                  //copy FX1
    AppendTSL(temp.mid(228, 1), "fx1_t_wah_mode");                  //copy FX1
    AppendTSL(temp.mid(229, 1), "fx1_t_wah_polar");                  //copy FX1
    AppendTSL(temp.mid(230, 1), "fx1_t_wah_sens");                  //copy FX1
    AppendTSL(temp.mid(231, 1), "fx1_t_wah_freq");                  //copy FX1
    AppendTSL(temp.mid(232, 1), "fx1_t_wah_peak");                  //copy FX1
    AppendTSL(temp.mid(233, 1), "fx1_t_wah_direct_mix");                  //copy FX1
    AppendTSL(temp.mid(234, 1), "fx1_t_wah_effect_level");                  //copy FX1
    AppendTSL(temp.mid(236, 1), "fx1_auto_wah_mode");                  //copy FX1
    AppendTSL(temp.mid(237, 1), "fx1_auto_wah_freq");                  //copy FX1
    AppendTSL(temp.mid(238, 1), "fx1_auto_wah_peak");                  //copy FX1
    AppendTSL(temp.mid(239, 1), "fx1_auto_wah_rate");                  //copy FX1
    AppendTSL(temp.mid(240, 1), "fx1_auto_wah_depth");                  //copy FX1
    AppendTSL(temp.mid(241, 1), "fx1_auto_wah_direct_mix");                  //copy FX1
    AppendTSL(temp.mid(242, 1), "fx1_auto_wah_effect_level");                  //copy FX1
    AppendTSL(temp.mid(245, 1), "fx1_sub_wah_type");                  //copy FX1
    AppendTSL(temp.mid(246, 1), "fx1_sub_wah_pedal_pos");                  //copy FX1
    AppendTSL(temp.mid(247, 1), "fx1_sub_wah_pedal_min");                  //copy FX1
    AppendTSL(temp.mid(248, 1), "fx1_sub_wah_pedal_max");                  //copy FX1
    AppendTSL(temp.mid(249, 1), "fx1_sub_wah_effect_level");                  //copy FX1
    AppendTSL(temp.mid(250, 1), "fx1_sub_wah_direct_mix");                  //copy FX1
    AppendTSL(temp.mid(251, 1), "fx1_adv_comp_type");                  //copy FX1
    AppendTSL(temp.mid(252, 1), "fx1_adv_comp_sustain");                  //copy FX1
    AppendTSL(temp.mid(253, 1), "fx1_adv_comp_attack");                  //copy FX1
    AppendTSL(temp.mid(254, 1), "fx1_adv_comp_tone");                  //copy FX1
    AppendTSL(temp.mid(255, 1), "fx1_adv_comp_level");                  //copy FX1
    AppendTSL(temp.mid(257, 1), "fx1_limiter_type");                  //copy FX1
    AppendTSL(temp.mid(258, 1), "fx1_limiter_attack");                  //copy FX1
    AppendTSL(temp.mid(259, 1), "fx1_limiter_thresh");                  //copy FX1
    AppendTSL(temp.mid(260, 1), "fx1_limiter_ratio");                  //copy FX1
    AppendTSL(temp.mid(261, 1), "fx1_limiter_release");                  //copy FX1
    AppendTSL(temp.mid(262, 1), "fx1_limiter_level");                  //copy FX1
    AppendTSL(temp.mid(264, 1), "fx1_graphic_eq_31hz");                  //copy FX1
    AppendTSL(temp.mid(265, 1), "fx1_graphic_eq_62hz");                  //copy FX1
    AppendTSL(temp.mid(266, 1), "fx1_graphic_eq_125hz");                  //copy FX1
    AppendTSL(temp.mid(267, 1), "fx1_graphic_eq_250hz");                  //copy FX1
    AppendTSL(temp.mid(268, 1), "fx1_graphic_eq_500hz");                  //copy FX1
    AppendTSL(temp.mid(269, 1), "fx1_graphic_eq_1khz");                  //copy FX1
    AppendTSL(temp.mid(270, 1), "fx1_graphic_eq_2khz");                  //copy FX1
    AppendTSL(temp.mid(271, 1), "fx1_graphic_eq_4khz");                  //copy FX1
    AppendTSL(temp.mid(272, 1), "fx1_graphic_eq_8khz");                  //copy FX1
    AppendTSL(temp.mid(273, 1), "fx1_graphic_eq_16khz");                  //copy FX1
    AppendTSL(temp.mid(274, 1), "fx1_graphic_eq_level");                  //copy FX1
    AppendTSL(temp.mid(276, 1), "fx1_parametric_eq_low_cut");                  //copy FX1
    AppendTSL(temp.mid(277, 1), "fx1_parametric_eq_low_gain");                  //copy FX1
    AppendTSL(temp.mid(278, 1), "fx1_parametric_eq_low_mid_freq");                  //copy FX1
    AppendTSL(temp.mid(279, 1), "fx1_parametric_eq_low_mid_q");                  //copy FX1
    AppendTSL(temp.mid(293, 1), "fx1_parametric_eq_low_mid_gain");                  //copy FX1
    AppendTSL(temp.mid(294, 1), "fx1_parametric_eq_high_mid_freq");                  //copy FX1
    AppendTSL(temp.mid(295, 1), "fx1_parametric_eq_high_mid_q");                  //copy FX1
    AppendTSL(temp.mid(296, 1), "fx1_parametric_eq_high_mid_gain");                  //copy FX1
    AppendTSL(temp.mid(297, 1), "fx1_parametric_eq_high_gain");                  //copy FX1
    AppendTSL(temp.mid(298, 1), "fx1_parametric_eq_high_cut");                  //copy FX1
    AppendTSL(temp.mid(299, 1), "fx1_parametric_eq_level");                  //copy FX1
    AppendTSL(temp.mid(301, 1), "fx1_tone_modify_type");                  //copy FX1
    AppendTSL(temp.mid(302, 1), "fx1_tone_modify_reso");                  //copy FX1
    AppendTSL(temp.mid(303, 1), "fx1_tone_modify_low");                  //copy FX1
    AppendTSL(temp.mid(304, 1), "fx1_tone_modify_high");                  //copy FX1
    AppendTSL(temp.mid(305, 1), "fx1_tone_modify_level");                  //copy FX1
    AppendTSL(temp.mid(307, 1), "fx1_guitar_sim_type");                  //copy FX1
    AppendTSL(temp.mid(308, 1), "fx1_guitar_sim_low");                  //copy FX1
    AppendTSL(temp.mid(309, 1), "fx1_guitar_sim_high");                  //copy FX1
    AppendTSL(temp.mid(310, 1), "fx1_guitar_sim_level");                  //copy FX1
    AppendTSL(temp.mid(311, 1), "fx1_guitar_sim_body");                  //copy FX1
    AppendTSL(temp.mid(313, 1), "fx1_slow_gear_sens");                  //copy FX1
    AppendTSL(temp.mid(314, 1), "fx1_slow_gear_rise_time");                  //copy FX1
    AppendTSL(temp.mid(315, 1), "fx1_slow_gear_level");                  //copy FX1
    AppendTSL(temp.mid(317, 1), "fx1_defretter_tone");                  //copy FX1
    AppendTSL(temp.mid(318, 1), "fx1_defretter_sens");                  //copy FX1
    AppendTSL(temp.mid(319, 1), "fx1_defretter_attack");                  //copy FX1
    AppendTSL(temp.mid(320, 1), "fx1_defretter_depth");                  //copy FX1
    AppendTSL(temp.mid(321, 1), "fx1_defretter_reso");                  //copy FX1
    AppendTSL(temp.mid(322, 1), "fx1_defretter_effect_level");                  //copy FX1
    AppendTSL(temp.mid(323, 1), "fx1_defretter_direct_mix");                  //copy FX1
    AppendTSL(temp.mid(325, 1), "fx1_wave_synth_wave");                  //copy FX1
    AppendTSL(temp.mid(326, 1), "fx1_wave_synth_cutoff");                  //copy FX1
    AppendTSL(temp.mid(327, 1), "fx1_wave_synth_reso");                  //copy FX1
    AppendTSL(temp.mid(328, 1), "fx1_wave_synth_filter_sens");                  //copy FX1
    AppendTSL(temp.mid(329, 1), "fx1_wave_synth_filter_decay");                  //copy FX1
    AppendTSL(temp.mid(330, 1), "fx1_wave_synth_filter_depth");                  //copy FX1
    AppendTSL(temp.mid(331, 1), "fx1_wave_synth_synth_level");                  //copy FX1
    AppendTSL(temp.mid(332, 1), "fx1_wave_synth_direct_mix");                  //copy FX1
    AppendTSL(temp.mid(334, 1), "fx1_sitar_sim_tone");                  //copy FX1
    AppendTSL(temp.mid(335, 1), "fx1_sitar_sim_sens");                  //copy FX1
    AppendTSL(temp.mid(336, 1), "fx1_sitar_sim_depth");                  //copy FX1
    AppendTSL(temp.mid(337, 1), "fx1_sitar_sim_reso");                  //copy FX1
    AppendTSL(temp.mid(338, 1), "fx1_sitar_sim_buzz");                  //copy FX1
    AppendTSL(temp.mid(339, 1), "fx1_sitar_sim_effect_level");                  //copy FX1
    AppendTSL(temp.mid(340, 1), "fx1_sitar_sim_direct_mix");                  //copy FX1
    AppendTSL(temp.mid(342, 1), "fx1_octave_range");                  //copy FX1
    AppendTSL(temp.mid(343, 1), "fx1_octave_level");                  //copy FX1
    AppendTSL(temp.mid(344, 1), "fx1_octave_direct_mix");                  //copy FX1
    AppendTSL(temp.mid(346, 1), "fx1_pitch_shifter_voice");                  //copy FX1
    AppendTSL(temp.mid(347, 1), "fx1_pitch_shifter_ps1mode");                  //copy FX1
    AppendTSL(temp.mid(348, 1), "fx1_pitch_shifter_ps1pitch");                  //copy FX1
    AppendTSL(temp.mid(349, 1), "fx1_pitch_shifter_ps1fine");                  //copy FX1
    AppendTSL(temp.mid(350, 2), "fx1_pitch_shifter_ps1pre_dly");                  //copy FX1
    AppendTSL(temp.mid(350, 1), "fx1_pitch_shifter_ps1pre_dly_h");                  //copy FX1
    AppendTSL(temp.mid(351, 1), "fx1_pitch_shifter_ps1pre_dly_l");                  //copy FX1
    AppendTSL(temp.mid(352, 1), "fx1_pitch_shifter_ps1level");                  //copy FX1
    AppendTSL(temp.mid(353, 1), "fx1_pitch_shifter_ps2mode");                  //copy FX1
    AppendTSL(temp.mid(354, 1), "fx1_pitch_shifter_ps2pitch");                  //copy FX1
    AppendTSL(temp.mid(355, 1), "fx1_pitch_shifter_ps2fine");                  //copy FX1
    AppendTSL(temp.mid(356, 2), "fx1_pitch_shifter_ps2pre_dly");                  //copy FX1
    AppendTSL(temp.mid(356, 1), "fx1_pitch_shifter_ps2pre_dly_h");                  //copy FX1
    AppendTSL(temp.mid(357, 1), "fx1_pitch_shifter_ps2pre_dly_l");                  //copy FX1
    AppendTSL(temp.mid(358, 1), "fx1_pitch_shifter_ps2level");                  //copy FX1
    AppendTSL(temp.mid(359, 1), "fx1_pitch_shifter_ps1f_back");                  //copy FX1
    AppendTSL(temp.mid(360, 1), "fx1_pitch_shifter_direct_mix");                  //copy FX1
    AppendTSL(temp.mid(362, 1), "fx1_harmonist_voice");                  //copy FX1
    AppendTSL(temp.mid(363, 1), "fx1_harmonist_hr1harm");                  //copy FX1
    AppendTSL(temp.mid(364, 2), "fx1_harmonist_hr1pre_dly");                  //copy FX1
    AppendTSL(temp.mid(364, 1), "fx1_harmonist_hr1pre_dly_h");                  //copy FX1
    AppendTSL(temp.mid(365, 1), "fx1_harmonist_hr1pre_dly_l");                  //copy FX1
    AppendTSL(temp.mid(366, 1), "fx1_harmonist_hr1level");                  //copy FX1
    AppendTSL(temp.mid(367, 1), "fx1_harmonist_hr2harm");                  //copy FX1
    AppendTSL(temp.mid(368, 2), "fx1_harmonist_hr2pre_dly");                  //copy FX1
    AppendTSL(temp.mid(368, 1), "fx1_harmonist_hr2pre_dly_h");                  //copy FX1
    AppendTSL(temp.mid(369, 1), "fx1_harmonist_hr2pre_dly_l");                  //copy FX1
    AppendTSL(temp.mid(370, 1), "fx1_harmonist_hr2level");                  //copy FX1
    AppendTSL(temp.mid(372, 1), "fx1_harmonist_direct_mix");                  //copy FX1
    AppendTSL(temp.mid(373, 1), "fx1_harmonist_hr1c");                  //copy FX1
    AppendTSL(temp.mid(375, 1), "fx1_harmonist_hr1d");                  //copy FX1
    AppendTSL(temp.mid(274, 1), "fx1_harmonist_hr1db");                  //copy FX1
    AppendTSL(temp.mid(377, 1), "fx1_harmonist_hr1e");                  //copy FX1
    AppendTSL(temp.mid(376, 1), "fx1_harmonist_hr1eb");                  //copy FX1
    AppendTSL(temp.mid(378, 1), "fx1_harmonist_hr1f");                  //copy FX1
    AppendTSL(temp.mid(379, 1), "fx1_harmonist_hr1f_s");                  //copy FX1
    AppendTSL(temp.mid(371, 1), "fx1_harmonist_hr1f_back");                  //copy FX1
    AppendTSL(temp.mid(380, 1), "fx1_harmonist_hr1g");                  //copy FX1
    AppendTSL(temp.mid(382, 1), "fx1_harmonist_hr1a");                  //copy FX1
    AppendTSL(temp.mid(381, 1), "fx1_harmonist_hr1ab");                  //copy FX1
    AppendTSL(temp.mid(384, 1), "fx1_harmonist_hr1b");                  //copy FX1
    AppendTSL(temp.mid(383, 1), "fx1_harmonist_hr1bb");                  //copy FX1
    AppendTSL(temp.mid(385, 1), "fx1_harmonist_hr2c");                  //copy FX1
    AppendTSL(temp.mid(387, 1), "fx1_harmonist_hr2d");                  //copy FX1
    AppendTSL(temp.mid(386, 1), "fx1_harmonist_hr2db");                  //copy FX1
    AppendTSL(temp.mid(389, 1), "fx1_harmonist_hr2e");                  //copy FX1
    AppendTSL(temp.mid(388, 1), "fx1_harmonist_hr2eb");                  //copy FX1
    AppendTSL(temp.mid(390, 1), "fx1_harmonist_hr2f");                  //copy FX1
    AppendTSL(temp.mid(391, 1), "fx1_harmonist_hr2f_s");                  //copy FX1
    AppendTSL(temp.mid(392, 1), "fx1_harmonist_hr2g");                  //copy FX1
    AppendTSL(temp.mid(394, 1), "fx1_harmonist_hr2a");                  //copy FX1
    AppendTSL(temp.mid(393, 1), "fx1_harmonist_hr2ab");                  //copy FX1
    AppendTSL(temp.mid(396, 1), "fx1_harmonist_hr2b");                  //copy FX1
    AppendTSL(temp.mid(395, 1), "fx1_harmonist_hr2bb");                  //copy FX1
    AppendTSL(temp.mid(398, 1), "fx1_sound_hold_hold");                  //copy FX1
    AppendTSL(temp.mid(399, 1), "fx1_sound_hold_rise_time");                  //copy FX1
    AppendTSL(temp.mid(400, 1), "fx1_sound_hold_effect_level");                  //copy FX1
    AppendTSL(temp.mid(402, 1), "fx1_ac_processor_type");                  //copy FX1
    AppendTSL(temp.mid(403, 1), "fx1_ac_processor_bass");                  //copy FX1
    AppendTSL(temp.mid(404, 1), "fx1_ac_processor_middle");                  //copy FX1
    AppendTSL(temp.mid(405, 1), "fx1_ac_processor_middle_freq");                  //copy FX1
    AppendTSL(temp.mid(406, 1), "fx1_ac_processor_treble");                  //copy FX1
    AppendTSL(temp.mid(407, 1), "fx1_ac_processor_presence");                  //copy FX1
    AppendTSL(temp.mid(408, 1), "fx1_ac_processor_level");                  //copy FX1
    AppendTSL(temp.mid(410, 1), "fx1_phaser_type");                  //copy FX1
    AppendTSL(temp.mid(411, 1), "fx1_phaser_rate");                  //copy FX1
    AppendTSL(temp.mid(412, 1), "fx1_phaser_depth");                  //copy FX1
    AppendTSL(temp.mid(413, 1), "fx1_phaser_manual");                  //copy FX1
    AppendTSL(temp.mid(414, 1), "fx1_phaser_reso");                  //copy FX1
    AppendTSL(temp.mid(415, 1), "fx1_phaser_step_rate");                  //copy FX1
    AppendTSL(temp.mid(416, 1), "fx1_phaser_effect_level");                  //copy FX1
    AppendTSL(temp.mid(417, 1), "fx1_phaser_direct_mix");                  //copy FX1
    AppendTSL(temp.mid(419, 1), "fx1_flanger_rate");                  //copy FX1
    AppendTSL(temp.mid(420, 1), "fx1_flanger_depth");                  //copy FX1
    AppendTSL(temp.mid(434, 1), "fx1_flanger_manual");                  //copy FX1
    AppendTSL(temp.mid(435, 1), "fx1_flanger_reso");                  //copy FX1
    AppendTSL(temp.mid(436, 1), "fx1_flanger_separation");                  //copy FX1
    AppendTSL(temp.mid(437, 1), "fx1_flanger_low_cut");                  //copy FX1
    AppendTSL(temp.mid(438, 1), "fx1_flanger_effect_level");                  //copy FX1
    AppendTSL(temp.mid(439, 1), "fx1_flanger_direct_mix");                  //copy FX1
    AppendTSL(temp.mid(441, 1), "fx1_tremolo_wave_shape");                  //copy FX1
    AppendTSL(temp.mid(442, 1), "fx1_tremolo_rate");                  //copy FX1
    AppendTSL(temp.mid(443, 1), "fx1_tremolo_depth");                  //copy FX1
    AppendTSL(temp.mid(444, 1), "fx1_tremolo_level");                  //copy FX1
    AppendTSL(temp.mid(446, 1), "fx1_rotary_speed_select");                  //copy FX1
    AppendTSL(temp.mid(447, 1), "fx1_rotary_rate_slow");                  //copy FX1
    AppendTSL(temp.mid(448, 1), "fx1_rotary_rate_fast");                  //copy FX1
    AppendTSL(temp.mid(449, 1), "fx1_rotary_rise_time");                  //copy FX1
    AppendTSL(temp.mid(450, 1), "fx1_rotary_fall_time");                  //copy FX1
    AppendTSL(temp.mid(451, 1), "fx1_rotary_depth");                  //copy FX1
    AppendTSL(temp.mid(452, 1), "fx1_rotary_level");                  //copy FX1
    AppendTSL(temp.mid(454, 1), "fx1_uni_v_rate");                  //copy FX1
    AppendTSL(temp.mid(455, 1), "fx1_uni_v_depth");                  //copy FX1
    AppendTSL(temp.mid(456, 1), "fx1_uni_v_level");                  //copy FX1
    AppendTSL(temp.mid(458, 1), "fx1_pan_type");                  //copy FX1
    AppendTSL(temp.mid(459, 1), "fx1_pan_pos");                  //copy FX1
    AppendTSL(temp.mid(460, 1), "fx1_pan_wave_shape");                  //copy FX1
    AppendTSL(temp.mid(461, 1), "fx1_pan_rate");                  //copy FX1
    AppendTSL(temp.mid(462, 1), "fx1_pan_depth");                  //copy FX1
    AppendTSL(temp.mid(463, 1), "fx1_pan_level");                  //copy FX1
    AppendTSL(temp.mid(465, 1), "fx1_slicer_pattern");                  //copy FX1
    AppendTSL(temp.mid(466, 1), "fx1_slicer_rate");                  //copy FX1
    AppendTSL(temp.mid(467, 1), "fx1_slicer_trigger_sens");                  //copy FX1
    AppendTSL(temp.mid(468, 1), "fx1_slicer_effect_level");                  //copy FX1
    AppendTSL(temp.mid(469, 1), "fx1_slicer_direct_mix");                  //copy FX1
    AppendTSL(temp.mid(471, 1), "fx1_vibrato_rate");                  //copy FX1
    AppendTSL(temp.mid(472, 1), "fx1_vibrato_depth");                  //copy FX1
    AppendTSL(temp.mid(473, 1), "fx1_vibrato_trigger");                  //copy FX1
    AppendTSL(temp.mid(474, 1), "fx1_vibrato_rise_time");                  //copy FX1
    AppendTSL(temp.mid(475, 1), "fx1_vibrato_level");                  //copy FX1
    AppendTSL(temp.mid(477, 1), "fx1_ring_mod_mode");                  //copy FX1
    AppendTSL(temp.mid(478, 1), "fx1_ring_mod_freq");                  //copy FX1
    AppendTSL(temp.mid(479, 1), "fx1_ring_mod_effect_level");                  //copy FX1
    AppendTSL(temp.mid(480, 1), "fx1_ring_mod_direct_mix");                  //copy FX1
    AppendTSL(temp.mid(482, 1), "fx1_humanizer_mode");                  //copy FX1
    AppendTSL(temp.mid(483, 1), "fx1_humanizer_vowel1");                  //copy FX1
    AppendTSL(temp.mid(484, 1), "fx1_humanizer_vowel2");                  //copy FX1
    AppendTSL(temp.mid(485, 1), "fx1_humanizer_sens");                  //copy FX1
    AppendTSL(temp.mid(486, 1), "fx1_humanizer_rate");                  //copy FX1
    AppendTSL(temp.mid(487, 1), "fx1_humanizer_depth");                  //copy FX1
    AppendTSL(temp.mid(488, 1), "fx1_humanizer_manual");                  //copy FX1
    AppendTSL(temp.mid(489, 1), "fx1_humanizer_level");                  //copy FX1
    AppendTSL(temp.mid(491, 1), "fx1_2x2_chorus_xover_freq");                  //copy FX1
    AppendTSL(temp.mid(492, 1), "fx1_2x2_chorus_low_rate");                  //copy FX1
    AppendTSL(temp.mid(493, 1), "fx1_2x2_chorus_low_depth");                  //copy FX1
    AppendTSL(temp.mid(494, 1), "fx1_2x2_chorus_low_pre_delay");                  //copy FX1
    AppendTSL(temp.mid(495, 1), "fx1_2x2_chorus_low_level");                  //copy FX1
    AppendTSL(temp.mid(496, 1), "fx1_2x2_chorus_high_rate");                  //copy FX1
    AppendTSL(temp.mid(497, 1), "fx1_2x2_chorus_high_depth");                  //copy FX1
    AppendTSL(temp.mid(498, 1), "fx1_2x2_chorus_high_pre_delay");                  //copy FX1
    AppendTSL(temp.mid(499, 1), "fx1_2x2_chorus_high_level");                  //copy FX1
    AppendTSL(temp.mid(500, 1), "fx1_2x2_chorus_direct_level");                  //copy FX1
    AppendTSL(temp.mid(501, 1), "fx1_sub_delay_type");                  //copy FX1
    AppendTSL(temp.mid(502, 2), "fx1_sub_delay_time");                  //copy FX1
    AppendTSL(temp.mid(502, 1), "fx1_sub_delay_time_h");                  //copy FX1
    AppendTSL(temp.mid(503, 1), "fx1_sub_delay_time_l");                  //copy FX1
    AppendTSL(temp.mid(504, 1), "fx1_sub_delay_f_back");                  //copy FX1
    AppendTSL(temp.mid(505, 1), "fx1_sub_delay_high_cut");                  //copy FX1
    AppendTSL(temp.mid(506, 1), "fx1_sub_delay_effect_level");                  //copy FX1
    AppendTSL(temp.mid(507, 1), "fx1_sub_delay_direct_mix");                  //copy FX1
    AppendTSL(temp.mid(508, 1), "fx1_sub_delay_tap_time");                  //copy FX1
    AppendTSL(temp.mid(510, 1), "fx2_on_off");                  //copy FX2
    AppendTSL(temp.mid(511, 1), "fx2_fx_type");                  //copy FX2
    AppendTSL(temp.mid(512, 1), "fx2_sub_od_ds_type");                  //copy FX2
    AppendTSL(temp.mid(513, 1), "fx2_sub_od_ds_drive");                  //copy FX2
    AppendTSL(temp.mid(514, 1), "fx2_sub_od_ds_bottom");                  //copy FX2
    AppendTSL(temp.mid(515, 1), "fx2_sub_od_ds_tone");                  //copy FX2
    AppendTSL(temp.mid(516, 1), "fx2_sub_od_ds_solo_sw");                  //copy FX2
    AppendTSL(temp.mid(517, 1), "fx2_sub_od_ds_solo_level");                  //copy FX2
    AppendTSL(temp.mid(518, 1), "fx2_sub_od_ds_effect_level");                  //copy FX2
    AppendTSL(temp.mid(519, 1), "fx2_sub_od_ds_direct_mix");                  //copy FX2
    AppendTSL(temp.mid(522, 1), "fx2_t_wah_mode");                  //copy FX2
    AppendTSL(temp.mid(523, 1), "fx2_t_wah_polar");                  //copy FX2
    AppendTSL(temp.mid(524, 1), "fx2_t_wah_sens");                  //copy FX2
    AppendTSL(temp.mid(525, 1), "fx2_t_wah_freq");                  //copy FX2
    AppendTSL(temp.mid(526, 1), "fx2_t_wah_peak");                  //copy FX2
    AppendTSL(temp.mid(527, 1), "fx2_t_wah_direct_mix");                  //copy FX2
    AppendTSL(temp.mid(528, 1), "fx2_t_wah_effect_level");                  //copy FX2
    AppendTSL(temp.mid(530, 1), "fx2_auto_wah_mode");                  //copy FX2
    AppendTSL(temp.mid(531, 1), "fx2_auto_wah_freq");                  //copy FX2
    AppendTSL(temp.mid(532, 1), "fx2_auto_wah_peak");                  //copy FX2
    AppendTSL(temp.mid(533, 1), "fx2_auto_wah_rate");                  //copy FX2
    AppendTSL(temp.mid(534, 1), "fx2_auto_wah_depth");                  //copy FX2
    AppendTSL(temp.mid(535, 1), "fx2_auto_wah_direct_mix");                  //copy FX2
    AppendTSL(temp.mid(536, 1), "fx2_auto_wah_effect_level");                  //copy FX2
    AppendTSL(temp.mid(538, 1), "fx2_sub_wah_type");                  //copy FX2
    AppendTSL(temp.mid(539, 1), "fx2_sub_wah_pedal_pos");                  //copy FX2
    AppendTSL(temp.mid(540, 1), "fx2_sub_wah_pedal_min");                  //copy FX2
    AppendTSL(temp.mid(541, 1), "fx2_sub_wah_pedal_max");                  //copy FX2
    AppendTSL(temp.mid(542, 1), "fx2_sub_wah_effect_level");                  //copy FX2
    AppendTSL(temp.mid(543, 1), "fx2_sub_wah_direct_mix");                  //copy FX2
    AppendTSL(temp.mid(545, 1), "fx2_adv_comp_type");                  //copy FX2
    AppendTSL(temp.mid(546, 1), "fx2_adv_comp_sustain");                  //copy FX2
    AppendTSL(temp.mid(547, 1), "fx2_adv_comp_attack");                  //copy FX2
    AppendTSL(temp.mid(548, 1), "fx2_adv_comp_tone");                  //copy FX2
    AppendTSL(temp.mid(549, 1), "fx2_adv_comp_level");                  //copy FX2
    AppendTSL(temp.mid(551, 1), "fx2_limiter_type");                  //copy FX2
    AppendTSL(temp.mid(552, 1), "fx2_limiter_attack");                  //copy FX2
    AppendTSL(temp.mid(553, 1), "fx2_limiter_thresh");                  //copy FX2
    AppendTSL(temp.mid(554, 1), "fx2_limiter_ratio");                  //copy FX2
    AppendTSL(temp.mid(555, 1), "fx2_limiter_release");                  //copy FX2
    AppendTSL(temp.mid(556, 1), "fx2_limiter_level");                  //copy FX2
    AppendTSL(temp.mid(558, 1), "fx2_graphic_eq_31hz");                  //copy FX2
    AppendTSL(temp.mid(559, 1), "fx2_graphic_eq_62hz");                  //copy FX2
    AppendTSL(temp.mid(560, 1), "fx2_graphic_eq_125hz");                  //copy FX2
    AppendTSL(temp.mid(561, 1), "fx2_graphic_eq_250hz");                  //copy FX2
    AppendTSL(temp.mid(575, 1), "fx2_graphic_eq_500hz");                  //copy FX2
    AppendTSL(temp.mid(576, 1), "fx2_graphic_eq_1khz");                  //copy FX2
    AppendTSL(temp.mid(577, 1), "fx2_graphic_eq_2khz");                  //copy FX2
    AppendTSL(temp.mid(578, 1), "fx2_graphic_eq_4khz");                  //copy FX2
    AppendTSL(temp.mid(579, 1), "fx2_graphic_eq_8khz");                  //copy FX2
    AppendTSL(temp.mid(580, 1), "fx2_graphic_eq_16khz");                  //copy FX2
    AppendTSL(temp.mid(581, 1), "fx2_graphic_eq_level");                  //copy FX2
    AppendTSL(temp.mid(583, 1), "fx2_parametric_eq_low_cut");                  //copy FX2
    AppendTSL(temp.mid(584, 1), "fx2_parametric_eq_low_gain");                  //copy FX2
    AppendTSL(temp.mid(585, 1), "fx2_parametric_eq_low_mid_freq");                  //copy FX2
    AppendTSL(temp.mid(586, 1), "fx2_parametric_eq_low_mid_q");                  //copy FX2
    AppendTSL(temp.mid(587, 1), "fx2_parametric_eq_low_mid_gain");                  //copy FX2
    AppendTSL(temp.mid(588, 1), "fx2_parametric_eq_high_mid_freq");                  //copy FX2
    AppendTSL(temp.mid(589, 1), "fx2_parametric_eq_high_mid_q");                  //copy FX2
    AppendTSL(temp.mid(590, 1), "fx2_parametric_eq_high_mid_gain");                  //copy FX2
    AppendTSL(temp.mid(591, 1), "fx2_parametric_eq_high_gain");                  //copy FX2
    AppendTSL(temp.mid(592, 1), "fx2_parametric_eq_high_cut");                  //copy FX2
    AppendTSL(temp.mid(593, 1), "fx2_parametric_eq_level");                  //copy FX2
    AppendTSL(temp.mid(595, 1), "fx2_tone_modify_type");                  //copy FX2
    AppendTSL(temp.mid(596, 1), "fx2_tone_modify_reso");                  //copy FX2
    AppendTSL(temp.mid(597, 1), "fx2_tone_modify_low");                  //copy FX2
    AppendTSL(temp.mid(598, 1), "fx2_tone_modify_high");                  //copy FX2
    AppendTSL(temp.mid(599, 1), "fx2_tone_modify_level");                  //copy FX2
    AppendTSL(temp.mid(601, 1), "fx2_guitar_sim_type");                  //copy FX2
    AppendTSL(temp.mid(602, 1), "fx2_guitar_sim_low");                  //copy FX2
    AppendTSL(temp.mid(603, 1), "fx2_guitar_sim_high");                  //copy FX2
    AppendTSL(temp.mid(604, 1), "fx2_guitar_sim_level");                  //copy FX2
    AppendTSL(temp.mid(605, 1), "fx2_guitar_sim_body");                  //copy FX2
    AppendTSL(temp.mid(607, 1), "fx2_slow_gear_sens");                  //copy FX2
    AppendTSL(temp.mid(608, 1), "fx2_slow_gear_rise_time");                  //copy FX2
    AppendTSL(temp.mid(609, 1), "fx2_slow_gear_level");                  //copy FX2
    AppendTSL(temp.mid(611, 1), "fx2_defretter_tone");                  //copy FX2
    AppendTSL(temp.mid(612, 1), "fx2_defretter_sens");                  //copy FX2
    AppendTSL(temp.mid(613, 1), "fx2_defretter_attack");                  //copy FX2
    AppendTSL(temp.mid(614, 1), "fx2_defretter_depth");                  //copy FX2
    AppendTSL(temp.mid(615, 1), "fx2_defretter_reso");                  //copy FX2
    AppendTSL(temp.mid(616, 1), "fx2_defretter_effect_level");                  //copy FX2
    AppendTSL(temp.mid(617, 1), "fx2_defretter_direct_mix");                  //copy FX2
    AppendTSL(temp.mid(619, 1), "fx2_wave_synth_wave");                  //copy FX2
    AppendTSL(temp.mid(620, 1), "fx2_wave_synth_cutoff");                  //copy FX2
    AppendTSL(temp.mid(621, 1), "fx2_wave_synth_reso");                  //copy FX2
    AppendTSL(temp.mid(622, 1), "fx2_wave_synth_filter_sens");                  //copy FX2
    AppendTSL(temp.mid(623, 1), "fx2_wave_synth_filter_decay");                  //copy FX2
    AppendTSL(temp.mid(624, 1), "fx2_wave_synth_filter_depth");                  //copy FX2
    AppendTSL(temp.mid(625, 1), "fx2_wave_synth_synth_level");                  //copy FX2
    AppendTSL(temp.mid(626, 1), "fx2_wave_synth_direct_mix");                  //copy FX2
    AppendTSL(temp.mid(628, 1), "fx2_sitar_sim_tone");                  //copy FX2
    AppendTSL(temp.mid(629, 1), "fx2_sitar_sim_sens");                  //copy FX2
    AppendTSL(temp.mid(630, 1), "fx2_sitar_sim_depth");                  //copy FX2
    AppendTSL(temp.mid(631, 1), "fx2_sitar_sim_reso");                  //copy FX2
    AppendTSL(temp.mid(632, 1), "fx2_sitar_sim_buzz");                  //copy FX2
    AppendTSL(temp.mid(633, 1), "fx2_sitar_sim_effect_level");                  //copy FX2
    AppendTSL(temp.mid(634, 1), "fx2_sitar_sim_direct_mix");                  //copy FX2
    AppendTSL(temp.mid(636, 1), "fx2_octave_range");                  //copy FX2
    AppendTSL(temp.mid(637, 1), "fx2_octave_level");                  //copy FX2
    AppendTSL(temp.mid(638, 1), "fx2_octave_direct_mix");                  //copy FX2
    AppendTSL(temp.mid(640, 1), "fx2_pitch_shifter_voice");                  //copy FX2
    AppendTSL(temp.mid(641, 1), "fx2_pitch_shifter_ps1mode");                  //copy FX2
    AppendTSL(temp.mid(642, 1), "fx2_pitch_shifter_ps1pitch");                  //copy FX2
    AppendTSL(temp.mid(643, 1), "fx2_pitch_shifter_ps1fine");                  //copy FX2
    AppendTSL(temp.mid(644, 2), "fx2_pitch_shifter_ps1pre_dly");                  //copy FX2
    AppendTSL(temp.mid(644, 1), "fx2_pitch_shifter_ps1pre_dly_h");                  //copy FX2
    AppendTSL(temp.mid(645, 1), "fx2_pitch_shifter_ps1pre_dly_l");                  //copy FX2
    AppendTSL(temp.mid(646, 1), "fx2_pitch_shifter_ps1level");                  //copy FX2
    AppendTSL(temp.mid(647, 1), "fx2_pitch_shifter_ps2mode");                  //copy FX2
    AppendTSL(temp.mid(648, 1), "fx2_pitch_shifter_ps2pitch");                  //copy FX2
    AppendTSL(temp.mid(649, 1), "fx2_pitch_shifter_ps2fine");                  //copy FX2
    AppendTSL(temp.mid(650, 2), "fx2_pitch_shifter_ps2pre_dly");                  //copy FX2
    AppendTSL(temp.mid(650, 1), "fx2_pitch_shifter_ps2pre_dly_h");                  //copy FX2
    AppendTSL(temp.mid(650, 1), "fx2_pitch_shifter_ps2pre_dly_l");                  //copy FX2
    AppendTSL(temp.mid(652, 1), "fx2_pitch_shifter_ps2level");                  //copy FX2
    AppendTSL(temp.mid(653, 1), "fx2_pitch_shifter_ps1f_back");                  //copy FX2
    AppendTSL(temp.mid(654, 1), "fx2_pitch_shifter_direct_mix");                  //copy FX2
    AppendTSL(temp.mid(656, 1), "fx2_harmonist_voice");                  //copy FX2
    AppendTSL(temp.mid(657, 1), "fx2_harmonist_hr1harm");                  //copy FX2
    AppendTSL(temp.mid(658, 2), "fx2_harmonist_hr1pre_dly");                  //copy FX2
    AppendTSL(temp.mid(658, 1), "fx2_harmonist_hr1pre_dly_h");                  //copy FX2
    AppendTSL(temp.mid(659, 1), "fx2_harmonist_hr1pre_dly_l");                  //copy FX2
    AppendTSL(temp.mid(660, 1), "fx2_harmonist_hr1level");                  //copy FX2
    AppendTSL(temp.mid(661, 1), "fx2_harmonist_hr2harm");                  //copy FX2
    AppendTSL(temp.mid(662, 2), "fx2_harmonist_hr2pre_dly");                  //copy FX2
    AppendTSL(temp.mid(662, 1), "fx2_harmonist_hr2pre_dly_h");                  //copy FX2
    AppendTSL(temp.mid(662, 1), "fx2_harmonist_hr2pre_dly_l");                  //copy FX2
    AppendTSL(temp.mid(664, 1), "fx2_harmonist_hr2level");                  //copy FX2
    AppendTSL(temp.mid(665, 1), "fx2_harmonist_hr1f_back");                  //copy FX2
    AppendTSL(temp.mid(666, 1), "fx2_harmonist_direct_mix");                  //copy FX2
    AppendTSL(temp.mid(667, 1), "fx2_harmonist_hr1c");                  //copy FX2
    AppendTSL(temp.mid(668, 1), "fx2_harmonist_hr1db");                  //copy FX2
    AppendTSL(temp.mid(669, 1), "fx2_harmonist_hr1d");                  //copy FX2
    AppendTSL(temp.mid(670, 1), "fx2_harmonist_hr1eb");                  //copy FX2
    AppendTSL(temp.mid(671, 1), "fx2_harmonist_hr1e");                  //copy FX2
    AppendTSL(temp.mid(672, 1), "fx2_harmonist_hr1f");                  //copy FX2
    AppendTSL(temp.mid(673, 1), "fx2_harmonist_hr1f_s");                  //copy FX2
    AppendTSL(temp.mid(674, 1), "fx2_harmonist_hr1g");                  //copy FX2
    AppendTSL(temp.mid(675, 1), "fx2_harmonist_hr1ab");                  //copy FX2
    AppendTSL(temp.mid(676, 1), "fx2_harmonist_hr1a");                  //copy FX2
    AppendTSL(temp.mid(677, 1), "fx2_harmonist_hr1bb");                  //copy FX2
    AppendTSL(temp.mid(678, 1), "fx2_harmonist_hr1b");                  //copy FX2
    AppendTSL(temp.mid(679, 1), "fx2_harmonist_hr2c");                  //copy FX2
    AppendTSL(temp.mid(680, 1), "fx2_harmonist_hr2db");                  //copy FX2
    AppendTSL(temp.mid(681, 1), "fx2_harmonist_hr2d");                  //copy FX2
    AppendTSL(temp.mid(682, 1), "fx2_harmonist_hr2eb");                  //copy FX2
    AppendTSL(temp.mid(683, 1), "fx2_harmonist_hr2e");                  //copy FX2
    AppendTSL(temp.mid(684, 1), "fx2_harmonist_hr2f");                  //copy FX2
    AppendTSL(temp.mid(685, 1), "fx2_harmonist_hr2f_s");                  //copy FX2
    AppendTSL(temp.mid(686, 1), "fx2_harmonist_hr2g");                  //copy FX2
    AppendTSL(temp.mid(687, 1), "fx2_harmonist_hr2ab");                  //copy FX2
    AppendTSL(temp.mid(688, 1), "fx2_harmonist_hr2a");                  //copy FX2
    AppendTSL(temp.mid(689, 1), "fx2_harmonist_hr2bb");                  //copy FX2
    AppendTSL(temp.mid(690, 1), "fx2_harmonist_hr2b");                  //copy FX2
    AppendTSL(temp.mid(692, 1), "fx2_sound_hold_hold");                  //copy FX2
    AppendTSL(temp.mid(693, 1), "fx2_sound_hold_rise_time");                  //copy FX2
    AppendTSL(temp.mid(694, 1), "fx2_sound_hold_effect_level");                  //copy FX2
    AppendTSL(temp.mid(696, 1), "fx2_ac_processor_type");                  //copy FX2
    AppendTSL(temp.mid(697, 1), "fx2_ac_processor_bass");                  //copy FX2
    AppendTSL(temp.mid(698, 1), "fx2_ac_processor_middle");                  //copy FX2
    AppendTSL(temp.mid(699, 1), "fx2_ac_processor_middle_freq");                  //copy FX2
    AppendTSL(temp.mid(700, 1), "fx2_ac_processor_treble");                  //copy FX2
    AppendTSL(temp.mid(701, 1), "fx2_ac_processor_presence");                  //copy FX2
    AppendTSL(temp.mid(702, 1), "fx2_ac_processor_level");                  //copy FX2
    AppendTSL(temp.mid(716, 1), "fx2_phaser_type");                  //copy FX2
    AppendTSL(temp.mid(717, 1), "fx2_phaser_rate");                  //copy FX2
    AppendTSL(temp.mid(718, 1), "fx2_phaser_depth");                  //copy FX2
    AppendTSL(temp.mid(719, 1), "fx2_phaser_manual");                  //copy FX2
    AppendTSL(temp.mid(720, 1), "fx2_phaser_reso");                  //copy FX2
    AppendTSL(temp.mid(721, 1), "fx2_phaser_step_rate");                  //copy FX2
    AppendTSL(temp.mid(722, 1), "fx2_phaser_effect_level");                  //copy FX2
    AppendTSL(temp.mid(723, 1), "fx2_phaser_direct_mix");                  //copy FX2
    AppendTSL(temp.mid(726, 1), "fx2_flanger_rate");                  //copy FX2
    AppendTSL(temp.mid(727, 1), "fx2_flanger_depth");                  //copy FX2
    AppendTSL(temp.mid(728, 1), "fx2_flanger_manual");                  //copy FX2
    AppendTSL(temp.mid(729, 1), "fx2_flanger_reso");                  //copy FX2
    AppendTSL(temp.mid(730, 1), "fx2_flanger_separation");                  //copy FX2
    AppendTSL(temp.mid(731, 1), "fx2_flanger_low_cut");                  //copy FX2
    AppendTSL(temp.mid(732, 1), "fx2_flanger_effect_level");                  //copy FX2
    AppendTSL(temp.mid(733, 1), "fx2_flanger_direct_mix");                  //copy FX2
    AppendTSL(temp.mid(735, 1), "fx2_tremolo_wave_shape");                  //copy FX2
    AppendTSL(temp.mid(736, 1), "fx2_tremolo_rate");                  //copy FX2
    AppendTSL(temp.mid(737, 1), "fx2_tremolo_depth");                  //copy FX2
    AppendTSL(temp.mid(738, 1), "fx2_tremolo_level");                  //copy FX2
    AppendTSL(temp.mid(740, 1), "fx2_rotary_speed_select");                  //copy FX2
    AppendTSL(temp.mid(741, 1), "fx2_rotary_rate_slow");                  //copy FX2
    AppendTSL(temp.mid(742, 1), "fx2_rotary_rate_fast");                  //copy FX2
    AppendTSL(temp.mid(743, 1), "fx2_rotary_rise_time");                  //copy FX2
    AppendTSL(temp.mid(744, 1), "fx2_rotary_fall_time");                  //copy FX2
    AppendTSL(temp.mid(745, 1), "fx2_rotary_depth");                  //copy FX2
    AppendTSL(temp.mid(746, 1), "fx2_rotary_level");                  //copy FX2
    AppendTSL(temp.mid(748, 1), "fx2_uni_v_rate");                  //copy FX2
    AppendTSL(temp.mid(749, 1), "fx2_uni_v_depth");                  //copy FX2
    AppendTSL(temp.mid(750, 1), "fx2_uni_v_level");                  //copy FX2
    AppendTSL(temp.mid(752, 1), "fx2_pan_type");                  //copy FX2
    AppendTSL(temp.mid(753, 1), "fx2_pan_pos");                  //copy FX2
    AppendTSL(temp.mid(754, 1), "fx2_pan_wave_shape");                  //copy FX2
    AppendTSL(temp.mid(755, 1), "fx2_pan_rate");                  //copy FX2
    AppendTSL(temp.mid(756, 1), "fx2_pan_depth");                  //copy FX2
    AppendTSL(temp.mid(757, 1), "fx2_pan_level");                  //copy FX2
    AppendTSL(temp.mid(759, 1), "fx2_slicer_pattern");                  //copy FX2
    AppendTSL(temp.mid(760, 1), "fx2_slicer_rate");                  //copy FX2
    AppendTSL(temp.mid(761, 1), "fx2_slicer_trigger_sens");                  //copy FX2
    AppendTSL(temp.mid(762, 1), "fx2_slicer_effect_level");                  //copy FX2
    AppendTSL(temp.mid(763, 1), "fx2_slicer_direct_mix");                  //copy FX2
    AppendTSL(temp.mid(765, 1), "fx2_vibrato_rate");                  //copy FX2
    AppendTSL(temp.mid(766, 1), "fx2_vibrato_depth");                  //copy FX2
    AppendTSL(temp.mid(767, 1), "fx2_vibrato_trigger");                  //copy FX2
    AppendTSL(temp.mid(768, 1), "fx2_vibrato_rise_time");                  //copy FX2
    AppendTSL(temp.mid(769, 1), "fx2_vibrato_level");                  //copy FX2
    AppendTSL(temp.mid(771, 1), "fx2_ring_mod_mode");                  //copy FX2
    AppendTSL(temp.mid(772, 1), "fx2_ring_mod_freq");                  //copy FX2
    AppendTSL(temp.mid(773, 1), "fx2_ring_mod_effect_level");                  //copy FX2
    AppendTSL(temp.mid(774, 1), "fx2_ring_mod_direct_mix");                  //copy FX2
    AppendTSL(temp.mid(776, 1), "fx2_humanizer_mode");                  //copy FX2
    AppendTSL(temp.mid(777, 1), "fx2_humanizer_vowel1");                  //copy FX2
    AppendTSL(temp.mid(778, 1), "fx2_humanizer_vowel2");                  //copy FX2
    AppendTSL(temp.mid(779, 1), "fx2_humanizer_sens");                  //copy FX2
    AppendTSL(temp.mid(780, 1), "fx2_humanizer_rate");                  //copy FX2
    AppendTSL(temp.mid(781, 1), "fx2_humanizer_depth");                  //copy FX2
    AppendTSL(temp.mid(782, 1), "fx2_humanizer_manual");                  //copy FX2
    AppendTSL(temp.mid(783, 1), "fx2_humanizer_level");                  //copy FX2
    AppendTSL(temp.mid(785, 1), "fx2_2x2_chorus_xover_freq");                  //copy FX2
    AppendTSL(temp.mid(786, 1), "fx2_2x2_chorus_low_rate");                  //copy FX2
    AppendTSL(temp.mid(787, 1), "fx2_2x2_chorus_low_depth");                  //copy FX2
    AppendTSL(temp.mid(788, 1), "fx2_2x2_chorus_low_pre_delay");                  //copy FX2
    AppendTSL(temp.mid(789, 1), "fx2_2x2_chorus_low_level");                  //copy FX2
    AppendTSL(temp.mid(790, 1), "fx2_2x2_chorus_high_rate");                  //copy FX2
    AppendTSL(temp.mid(791, 1), "fx2_2x2_chorus_high_depth");                  //copy FX2
    AppendTSL(temp.mid(792, 1), "fx2_2x2_chorus_high_pre_delay");                  //copy FX2
    AppendTSL(temp.mid(793, 1), "fx2_2x2_chorus_high_level");                  //copy FX2
    AppendTSL(temp.mid(794, 1), "fx2_2x2_chorus_direct_level");                  //copy FX2
    AppendTSL(temp.mid(795, 1), "fx2_sub_delay_type");                  //copy FX2
    AppendTSL(temp.mid(796, 2), "fx2_sub_delay_time");                  //copy FX2
    AppendTSL(temp.mid(796, 1), "fx2_sub_delay_time_h");                  //copy FX2
    AppendTSL(temp.mid(797, 1), "fx2_sub_delay_time_l");                  //copy FX2
    AppendTSL(temp.mid(798, 1), "fx2_sub_delay_f_back");                  //copy FX2
    AppendTSL(temp.mid(799, 1), "fx2_sub_delay_high_cut");                  //copy FX2
    AppendTSL(temp.mid(800, 1), "fx2_sub_delay_effect_level");                  //copy FX2
    AppendTSL(temp.mid(801, 1), "fx2_sub_delay_direct_mix");                  //copy FX2
    AppendTSL(temp.mid(802, 1), "fx2_sub_delay_tap_time");                  //copy FX2
    AppendTSL(temp.mid(812, 1), "delay_on_off");                  //copy DD
    AppendTSL(temp.mid(813, 1), "delay_type");                  //copy DD
    AppendTSL(temp.mid(814, 2), "delay_delay_time");                  //copy DD
    AppendTSL(temp.mid(814, 1), "delay_delay_time_h");                  //copy DD
    AppendTSL(temp.mid(815, 1), "delay_delay_time_l");                  //copy DD
    AppendTSL(temp.mid(816, 1), "delay_f_back");                  //copy DD
    AppendTSL(temp.mid(817, 1), "delay_high_cut");                  //copy DD
    AppendTSL(temp.mid(818, 1), "delay_effect_level");                  //copy DD
    AppendTSL(temp.mid(819, 1), "delay_direct_mix");                  //copy DD
    AppendTSL(temp.mid(820, 1), "delay_tap_time");                  //copy DD
    AppendTSL(temp.mid(821, 2), "delay_d1_time");                  //copy DD
    AppendTSL(temp.mid(821, 1), "delay_d1_time_h");                  //copy DD
    AppendTSL(temp.mid(822, 1), "delay_d1_time_l");                  //copy DD
    AppendTSL(temp.mid(823, 1), "delay_d1_f_back");                  //copy DD
    AppendTSL(temp.mid(824, 1), "delay_d1_hi_cut");                  //copy DD
    AppendTSL(temp.mid(825, 1), "delay_d1_level");                  //copy DD
    AppendTSL(temp.mid(826, 2), "delay_d2_time");                  //copy DD
    AppendTSL(temp.mid(826, 1), "delay_d2_time_h");                  //copy DD
    AppendTSL(temp.mid(827, 1), "delay_d2_time_l");                  //copy DD
    AppendTSL(temp.mid(828, 1), "delay_d2_f_back");                  //copy DD
    AppendTSL(temp.mid(829, 1), "delay_d2_hi_cut");                  //copy DD
    AppendTSL(temp.mid(830, 1), "delay_d2_level");                  //copy DD
    AppendTSL(temp.mid(831, 1), "delay_mod_rate");                  //copy DD
    AppendTSL(temp.mid(832, 1), "delay_mod_depth");                  //copy DD
    AppendTSL(temp.mid(857, 1), "chorus_on_off");                  //copy CE
    AppendTSL(temp.mid(858, 1), "chorus_mode");                  //copy CE
    AppendTSL(temp.mid(859, 1), "chorus_rate");                  //copy CE
    AppendTSL(temp.mid(860, 1), "chorus_depth");                  //copy CE
    AppendTSL(temp.mid(861, 1), "chorus_pre_delay");                  //copy CE
    AppendTSL(temp.mid(862, 1), "chorus_low_cut");                  //copy CE
    AppendTSL(temp.mid(863, 1), "chorus_high_cut");                  //copy CE
    AppendTSL(temp.mid(864, 1), "chorus_effect_level");                  //copy CE
    AppendTSL(temp.mid(865, 1), "chorus_direct_level");                  //copy CE
    AppendTSL(temp.mid(873, 1), "reverb_on_off");                  //copy RV
    AppendTSL(temp.mid(874, 1), "reverb_type");                  //copy RV
    AppendTSL(temp.mid(875, 1), "reverb_time");                  //copy RV
    AppendTSL(temp.mid(876, 2), "reverb_pre_delay");                  //copy RV
    AppendTSL(temp.mid(876, 1), "reverb_pre_delay_h");                  //copy RV
    AppendTSL(temp.mid(877, 1), "reverb_pre_delay_l");                  //copy RV
    AppendTSL(temp.mid(878, 1), "reverb_low_cut");                  //copy RV
    AppendTSL(temp.mid(879, 1), "reverb_high_cut");                  //copy RV
    AppendTSL(temp.mid(880, 1), "reverb_density");                  //copy RV
    AppendTSL(temp.mid(881, 1), "reverb_effect_level");                  //copy RV
    AppendTSL(temp.mid(882, 1), "reverb_direct_mix");                  //copy RV
    AppendTSL(temp.mid(883, 1), "reverb_spring_sens");                  //copy RV
    AppendTSL(temp.mid(889, 1), "pedal_fx_on_off");                  //copy PedalFX
    AppendTSL(temp.mid(891, 1), "pedal_fx_pedal_bend_pitch");                  //copy PedalFX
    AppendTSL(temp.mid(892, 1), "pedal_fx_pedal_bend_position");                  //copy PedalFX
    AppendTSL(temp.mid(893, 1), "pedal_fx_pedal_bend_effect_level");                  //copy PedalFX
    AppendTSL(temp.mid(894, 1), "pedal_fx_pedal_bend_direct_mix");                  //copy PedalFX
    AppendTSL(temp.mid(895, 1), "pedal_fx_wah_type");                  //copy PedalFX
    AppendTSL(temp.mid(896, 1), "pedal_fx_wah_position");                  //copy PedalFX
    AppendTSL(temp.mid(897, 1), "pedal_fx_wah_pedal_min");                  //copy PedalFX
    AppendTSL(temp.mid(898, 1), "pedal_fx_wah_pedal_max");                  //copy PedalFX
    AppendTSL(temp.mid(899, 1), "pedal_fx_wah_effect_level");                  //copy PedalFX
    AppendTSL(temp.mid(900, 1), "pedal_fx_wah_direct_mix");                  //copy PedalFX
    AppendTSL(temp.mid(911, 1), "foot_volume_volume_curve");                  //copy FV
    AppendTSL(temp.mid(909, 1), "foot_volume_volume_min");                  //copy FV
    AppendTSL(temp.mid(910, 1), "foot_volume_volume_max");                  //copy FV
    AppendTSL(temp.mid(908, 1), "foot_volume_level");                  //copy FV
    AppendTSL(temp.mid(921, 1), "divider_mode");                  //copy Divide/Mix
    AppendTSL(temp.mid(922, 1), "divider_ch_select");                  //copy Divide/Mix
    AppendTSL(temp.mid(923, 1), "divider_ch_a_dynamic");                  //copy Divide/Mix
    AppendTSL(temp.mid(924, 1), "divider_ch_a_dynamic_sens");                  //copy Divide/Mix
    AppendTSL(temp.mid(925, 1), "divider_ch_a_filter");                  //copy Divide/Mix
    AppendTSL(temp.mid(926, 1), "divider_ch_a_cutoff_freq");                  //copy Divide/Mix
    AppendTSL(temp.mid(927, 1), "divider_ch_b_dynamic");                  //copy Divide/Mix
    AppendTSL(temp.mid(928, 1), "divider_ch_b_dynamic_sens");                  //copy Divide/Mix
    AppendTSL(temp.mid(929, 1), "divider_ch_b_filter");                  //copy Divide/Mix
    AppendTSL(temp.mid(930, 1), "divider_ch_b_cutoff_freq");                  //copy Divide/Mix
    AppendTSL(temp.mid(937, 1), "mixer_mode");                  //copy Divide/Mix
    AppendTSL(temp.mid(938, 1), "mixer_ch_a_b_balance");                  //copy Divide/Mix
    AppendTSL(temp.mid(939, 1), "mixer_spread");                  //copy Divide/Mix
    AppendTSL(temp.mid(942, 1), "send_return_on_off");                  //copy S/R
    AppendTSL(temp.mid(943, 1), "send_return_mode");                  //copy S/R
    AppendTSL(temp.mid(944, 1), "send_return_send_level");                  //copy S/R
    AppendTSL(temp.mid(945, 1), "send_return_return_level");                  //copy S/R
    AppendTSL(temp.mid(946, 1), "send_return_adjust");                  //copy S/R
    AppendTSL(temp.mid(953, 1), "amp_control");                  //copy Amp ctrl
    AppendTSL(temp.mid(956, 1), "ns1_on_off");                  //copy NS
    AppendTSL(temp.mid(957, 1), "ns1_threshold");                  //copy NS
    AppendTSL(temp.mid(958, 1), "ns1_release");                  //copy NS
    AppendTSL(temp.mid(959, 1), "ns1_detect");                  //copy NS
    AppendTSL(temp.mid(961, 1), "ns2_on_off");                  //copy NS
    AppendTSL(temp.mid(962, 1), "ns2_threshold");                  //copy NS
    AppendTSL(temp.mid(963, 1), "ns2_release");                  //copy NS
    AppendTSL(temp.mid(964, 1), "ns2_detect");                  //copy NS
    AppendTSL(temp.mid(969, 1), "accel_fx_type");                  //copy Accel
    AppendTSL(temp.mid(970, 1), "accel_fx_s_bend_pitch");                  //copy Accel
    AppendTSL(temp.mid(971, 1), "accel_fx_s_bend_rise_time");                  //copy Accel
    AppendTSL(temp.mid(972, 1), "accel_fx_s_bend_fall_time");                  //copy Accel
    AppendTSL(temp.mid(973, 1), "accel_fx_laser_beam_rate");                  //copy Accel
    AppendTSL(temp.mid(974, 1), "accel_fx_laser_beam_depth");                  //copy Accel
    AppendTSL(temp.mid(975, 1), "accel_fx_laser_beam_rise_time");                  //copy Accel
    AppendTSL(temp.mid(976, 1), "accel_fx_laser_beam_fall_time");                  //copy Accel
    AppendTSL(temp.mid(977, 1), "accel_fx_ring_mod_freq");                  //copy Accel
    AppendTSL(temp.mid(978, 1), "accel_fx_ring_mod_rise_time");                  //copy Accel
    AppendTSL(temp.mid(979, 1), "accel_fx_ring_mod_fall_time");                  //copy Accel
    AppendTSL(temp.mid(980, 1), "accel_fx_ring_mod_ring_level");                  //copy Accel
    AppendTSL(temp.mid(981, 1), "accel_fx_ring_mod_octave_level");                  //copy Accel
    AppendTSL(temp.mid(982, 1), "accel_fx_ring_mod_direct_mix");                  //copy Accel
    AppendTSL(temp.mid(983, 1), "accel_fx_twist_level");                  //copy Accel
    AppendTSL(temp.mid(984, 1), "accel_fx_twist_rise_time");                  //copy Accel
    AppendTSL(temp.mid(998, 1), "accel_fx_twist_fall_time");                  //copy Accel
    AppendTSL(temp.mid(999, 1), "accel_fx_warp_level");                  //copy Accel
    AppendTSL(temp.mid(1000, 1), "accel_fx_warp_rise_time");                  //copy Accel
    AppendTSL(temp.mid(1001, 1), "accel_fx_warp_fall_time");                  //copy Accel
    AppendTSL(temp.mid(1002, 1), "accel_fx_feedbacker_mode");                  //copy Accel
    AppendTSL(temp.mid(1003, 1), "accel_fx_feedbacker_depth");                  //copy Accel
    AppendTSL(temp.mid(1004, 1), "accel_fx_feedbacker_rise_time");                  //copy Accel
    AppendTSL(temp.mid(1005, 1), "accel_fx_feedbacker_octave_rise_time");                  //copy Accel
    AppendTSL(temp.mid(1006, 1), "accel_fx_feedbacker_f_back_level");                  //copy Accel
    AppendTSL(temp.mid(1007, 1), "accel_fx_feedbacker_octave_f_back_level");                  //copy Accel
    AppendTSL(temp.mid(1008, 1), "accel_fx_feedbacker_vib_rate");                  //copy Accel
    AppendTSL(temp.mid(1009, 1), "accel_fx_feedbacker_vib_depth");                  //copy Accel
    AppendTSL(temp.mid(1013, 1), "patch_category");               //copy Master
    AppendTSL(temp.mid(1014, 1), "patch_level");                  //copy Master
    AppendTSL(temp.mid(1015, 1), "master_eq_low_gain");           //copy Master
    AppendTSL(temp.mid(1016, 1), "master_eq_mid_freq");           //copy Master
    AppendTSL(temp.mid(1017, 1), "master_eq_mid_q");              //copy Master
    AppendTSL(temp.mid(1018, 1), "master_eq_mid_gain");           //copy Master
    AppendTSL(temp.mid(1019, 1), "master_eq_high_gain");          //copy Master
    AppendTSL(temp.mid(1020, 2), "master_bpm");                 //copy Master
    AppendTSL(temp.mid(1020, 1), "master_bpm_h");                 //copy Master
    AppendTSL(temp.mid(1021, 1), "master_bpm_l");                 //copy Master
    AppendTSL(temp.mid(1022, 1), "master_key");                   //copy Master
    AppendTSL(temp.mid(1023, 1), "master_beat");                  //copy Master
    AppendTSL(temp.mid(1024, 1), "pitch_detection");              //copy Master
    AppendTSL(temp.mid(1030, 1), "position1");
    AppendTSL(temp.mid(1031, 1), "position2");
    AppendTSL(temp.mid(1032, 1), "position3");
    AppendTSL(temp.mid(1033, 1), "position4");
    AppendTSL(temp.mid(1034, 1), "position5");
    AppendTSL(temp.mid(1035, 1), "position6");
    AppendTSL(temp.mid(1036, 1), "position7");
    AppendTSL(temp.mid(1037, 1), "position8");
    AppendTSL(temp.mid(1038, 1), "position9");
    AppendTSL(temp.mid(1039, 1), "position10");
    AppendTSL(temp.mid(1040, 1), "position11");
    AppendTSL(temp.mid(1041, 1), "position12");
    AppendTSL(temp.mid(1042, 1), "position13");
    AppendTSL(temp.mid(1043, 1), "position14");
    AppendTSL(temp.mid(1044, 1), "position15");
    AppendTSL(temp.mid(1045, 1), "position16");
    AppendTSL(temp.mid(1046, 1), "position17");
    AppendTSL(temp.mid(1047, 1), "position18");
    AppendTSL(temp.mid(1048, 1), "position19");
    AppendTSL(temp.mid(1049, 1), "position20");
    AppendTSL(temp.mid(1030, 1), "fx_chain_position1");                   //copy Chain position
    AppendTSL(temp.mid(1031, 1), "fx_chain_position2");                   //copy Chain position
    AppendTSL(temp.mid(1032, 1), "fx_chain_position3");                   //copy Chain position
    AppendTSL(temp.mid(1033, 1), "fx_chain_position4");                   //copy Chain position
    AppendTSL(temp.mid(1034, 1), "fx_chain_position5");                   //copy Chain position
    AppendTSL(temp.mid(1035, 1), "fx_chain_position6");                   //copy Chain position
    AppendTSL(temp.mid(1036, 1), "fx_chain_position7");                   //copy Chain position
    AppendTSL(temp.mid(1037, 1), "fx_chain_position8");                   //copy Chain position
    AppendTSL(temp.mid(1038, 1), "fx_chain_position9");                   //copy Chain position
    AppendTSL(temp.mid(1039, 1), "fx_chain_position10");                  //copy Chain position
    AppendTSL(temp.mid(1040, 1), "fx_chain_position11");                  //copy Chain position
    AppendTSL(temp.mid(1041, 1), "fx_chain_position12");                  //copy Chain position
    AppendTSL(temp.mid(1042, 1), "fx_chain_position13");                  //copy Chain position
    AppendTSL(temp.mid(1043, 1), "fx_chain_position14");                  //copy Chain position
    AppendTSL(temp.mid(1044, 1), "fx_chain_position15");                  //copy Chain position
    AppendTSL(temp.mid(1045, 1), "fx_chain_position16");                  //copy Chain position
    AppendTSL(temp.mid(1046, 1), "fx_chain_position17");                  //copy Chain position
    AppendTSL(temp.mid(1047, 1), "fx_chain_position18");                  //copy Chain position
    AppendTSL(temp.mid(1048, 1), "fx_chain_position19");                  //copy Chain position
    AppendTSL(temp.mid(1049, 1), "fx_chain_position20");                  //copy Chain position
    AppendTSL(temp.mid(1062, 1), "manual_mode_bank_down");                      //copy Manual Mode
    AppendTSL(temp.mid(1063, 1), "manual_mode_bank_up");                        //copy Manual Mode
    AppendTSL(temp.mid(1064, 1), "manual_mode_number_pedal1");                  //copy Manual Mode
    AppendTSL(temp.mid(1065, 1), "manual_mode_number_pedal2");                  //copy Manual Mode
    AppendTSL(temp.mid(1066, 1), "manual_mode_number_pedal3");                  //copy Manual Mode
    AppendTSL(temp.mid(1067, 1), "manual_mode_number_pedal4");                  //copy Manual Mode
    AppendTSL(temp.mid(1068, 1), "manual_mode_phrase_loop");                    //copy Manual Mode
    AppendTSL(temp.mid(1069, 1), "manual_mode_accel_ctrl");                     //copy Manual Mode
    AppendTSL(temp.mid(1078, 1), "ctl_exp_accel_ctl_func");                 //copy CTL/EXP
    AppendTSL(temp.mid(1079, 1), "ctl_exp_accel_ctl_min");                  //copy CTL/EXP
    AppendTSL(temp.mid(1080, 1), "ctl_exp_accel_ctl_max");                  //copy CTL/EXP
    AppendTSL(temp.mid(1081, 1), "ctl_exp_accel_ctl_src_mode");             //copy CTL/EXP
    AppendTSL(temp.mid(1094, 1), "ctl_exp_exp_sw_func");                    //copy CTL/EXP
    AppendTSL(temp.mid(1095, 1), "ctl_exp_exp_sw_min");                     //copy CTL/EXP
    AppendTSL(temp.mid(1096, 1), "ctl_exp_exp_sw_max");                     //copy CTL/EXP
    AppendTSL(temp.mid(1097, 1), "ctl_exp_exp_sw_src_mode");                //copy CTL/EXP
    AppendTSL(temp.mid(1110, 1), "ctl_exp_sub_ctl1_func");                  //copy CTL/EXP
    AppendTSL(temp.mid(1111, 1), "ctl_exp_sub_ctl1_min");                   //copy CTL/EXP
    AppendTSL(temp.mid(1112, 1), "ctl_exp_sub_ctl1_max");                   //copy CTL/EXP
    AppendTSL(temp.mid(1113, 1), "ctl_exp_sub_ctl1_src_mode");              //copy CTL/EXP
    AppendTSL(temp.mid(1139, 1), "ctl_exp_sub_ctl2_func");                  //copy CTL/EXP
    AppendTSL(temp.mid(1140, 1), "ctl_exp_sub_ctl2_min");                   //copy CTL/EXP
    AppendTSL(temp.mid(1141, 1), "ctl_exp_sub_ctl2_max");                   //copy CTL/EXP
    AppendTSL(temp.mid(1142, 1), "ctl_exp_sub_ctl2_src_mode");              //copy CTL/EXP
    AppendTSL(temp.mid(1155, 1), "ctl_exp_exp_func");                       //copy CTL/EXP
    AppendTSL(temp.mid(1156, 1), "ctl_exp_sub_exp_patch_level_min");        //copy CTL/EXP
    AppendTSL(temp.mid(1157, 1), "ctl_exp_sub_exp_patch_level_max");        //copy CTL/EXP
    AppendTSL(temp.mid(1171, 1), "ctl_exp_sub_exp_func");                   //copy CTL/EXP
    AppendTSL(temp.mid(1172, 1), "ctl_exp_exp_patch_level_min");            //copy CTL/EXP
    AppendTSL(temp.mid(1173, 1), "ctl_exp_exp_patch_level_max");            //copy CTL/EXP
    AppendTSL(temp.mid(1187, 1), "assign1_on_off");                  //copy Assigns
    AppendTSL(temp.mid(1188, 2), "assign1_target");                  //copy Assigns
    AppendTSL(temp.mid(1188, 1), "assign1_target_h");                //copy Assigns
    AppendTSL(temp.mid(1189, 1), "assign1_target_l");                //copy Assigns
    AppendTSL(temp.mid(1190, 2), "assign1_target_min");              //copy Assigns
    AppendTSL(temp.mid(1190, 1), "assign1_target_min_h");            //copy Assigns
    AppendTSL(temp.mid(1191, 1), "assign1_target_min_l");            //copy Assigns
    AppendTSL(temp.mid(1192, 2), "assign1_target_max");              //copy Assigns
    AppendTSL(temp.mid(1192, 1), "assign1_target_max_h");            //copy Assigns
    AppendTSL(temp.mid(1193, 1), "assign1_target_max_l");            //copy Assigns
    AppendTSL(temp.mid(1194, 1), "assign1_source");                  //copy Assigns
    AppendTSL(temp.mid(1195, 1), "assign1_source_mode");             //copy Assigns
    AppendTSL(temp.mid(1196, 1), "assign1_act_range_lo");            //copy Assigns
    AppendTSL(temp.mid(1197, 1), "assign1_act_range_hi");            //copy Assigns
    AppendTSL(temp.mid(1198, 1), "assign1_int_pdl_trigger");         //copy Assigns
    AppendTSL(temp.mid(1199, 1), "assign1_int_pdl_time");            //copy Assigns
    AppendTSL(temp.mid(1200, 1), "assign1_int_pdl_curve");           //copy Assigns
    AppendTSL(temp.mid(1201, 1), "assign1_wave_rate");               //copy Assigns
    AppendTSL(temp.mid(1202, 1), "assign1_waveform");                //copy Assigns
    AppendTSL(temp.mid(1219, 1), "assign2_on_off");                  //copy Assigns
    AppendTSL(temp.mid(1220, 2), "assign2_target");                  //copy Assigns
    AppendTSL(temp.mid(1220, 1), "assign2_target_h");                //copy Assigns
    AppendTSL(temp.mid(1221, 1), "assign2_target_l");                //copy Assigns
    AppendTSL(temp.mid(1222, 2), "assign2_target_min");              //copy Assigns
    AppendTSL(temp.mid(1222, 1), "assign2_target_min_h");            //copy Assigns
    AppendTSL(temp.mid(1223, 1), "assign2_target_min_l");            //copy Assigns
    AppendTSL(temp.mid(1224, 2), "assign2_target_max");              //copy Assigns
    AppendTSL(temp.mid(1224, 1), "assign2_target_max_h");            //copy Assigns
    AppendTSL(temp.mid(1225, 1), "assign2_target_max_l");            //copy Assigns
    AppendTSL(temp.mid(1226, 1), "assign2_source");                  //copy Assigns
    AppendTSL(temp.mid(1227, 1), "assign2_source_mode");             //copy Assigns
    AppendTSL(temp.mid(1228, 1), "assign2_act_range_lo");            //copy Assigns
    AppendTSL(temp.mid(1229, 1), "assign2_act_range_hi");            //copy Assigns
    AppendTSL(temp.mid(1230, 1), "assign2_int_pdl_trigger");         //copy Assigns
    AppendTSL(temp.mid(1231, 1), "assign2_int_pdl_time");            //copy Assigns
    AppendTSL(temp.mid(1232, 1), "assign2_int_pdl_curve");           //copy Assigns
    AppendTSL(temp.mid(1233, 1), "assign2_wave_rate");               //copy Assigns
    AppendTSL(temp.mid(1234, 1), "assign2_waveform");                //copy Assigns
    AppendTSL(temp.mid(1251, 1), "assign3_on_off");                  //copy Assigns
    AppendTSL(temp.mid(1252, 2), "assign3_target");                  //copy Assigns
    AppendTSL(temp.mid(1252, 1), "assign3_target_h");                //copy Assigns
    AppendTSL(temp.mid(1253, 1), "assign3_target_l");                //copy Assigns
    AppendTSL(temp.mid(1254, 2), "assign3_target_min");              //copy Assigns
    AppendTSL(temp.mid(1254, 1), "assign3_target_min_h");            //copy Assigns
    AppendTSL(temp.mid(1255, 1), "assign3_target_min_l");            //copy Assigns
    AppendTSL(temp.mid(1256, 2), "assign3_target_max");              //copy Assigns
    AppendTSL(temp.mid(1256, 1), "assign3_target_max_h");            //copy Assigns
    AppendTSL(temp.mid(1257, 1), "assign3_target_max_l");            //copy Assigns
    AppendTSL(temp.mid(1258, 1), "assign3_source");                  //copy Assigns
    AppendTSL(temp.mid(1259, 1), "assign3_source_mode");             //copy Assigns
    AppendTSL(temp.mid(1260, 1), "assign3_act_range_lo");            //copy Assigns
    AppendTSL(temp.mid(1261, 1), "assign3_act_range_hi");            //copy Assigns
    AppendTSL(temp.mid(1262, 1), "assign3_int_pdl_trigger");         //copy Assigns
    AppendTSL(temp.mid(1263, 1), "assign3_int_pdl_time");            //copy Assigns
    AppendTSL(temp.mid(1264, 1), "assign3_int_pdl_curve");           //copy Assigns
    AppendTSL(temp.mid(1265, 1), "assign3_wave_rate");               //copy Assigns
    AppendTSL(temp.mid(1266, 1), "assign3_waveform");                //copy Assigns
    AppendTSL(temp.mid(1296, 1), "assign4_on_off");                  //copy Assigns
    AppendTSL(temp.mid(1297, 2), "assign4_target");                  //copy Assigns
    AppendTSL(temp.mid(1297, 1), "assign4_target_h");                //copy Assigns
    AppendTSL(temp.mid(1298, 1), "assign4_target_l");                //copy Assigns
    AppendTSL(temp.mid(1299, 2), "assign4_target_min");              //copy Assigns
    AppendTSL(temp.mid(1299, 1), "assign4_target_min_h");            //copy Assigns
    AppendTSL(temp.mid(1300, 1), "assign4_target_min_l");            //copy Assigns
    AppendTSL(temp.mid(1301, 2), "assign4_target_max");              //copy Assigns
    AppendTSL(temp.mid(1301, 1), "assign4_target_max_h");            //copy Assigns
    AppendTSL(temp.mid(1302, 1), "assign4_target_max_l");            //copy Assigns
    AppendTSL(temp.mid(1303, 1), "assign4_source");                  //copy Assigns
    AppendTSL(temp.mid(1304, 1), "assign4_source_mode");             //copy Assigns
    AppendTSL(temp.mid(1305, 1), "assign4_act_range_lo");            //copy Assigns
    AppendTSL(temp.mid(1306, 1), "assign4_act_range_hi");            //copy Assigns
    AppendTSL(temp.mid(1307, 1), "assign4_int_pdl_trigger");         //copy Assigns
    AppendTSL(temp.mid(1308, 1), "assign4_int_pdl_time");            //copy Assigns
    AppendTSL(temp.mid(1309, 1), "assign4_int_pdl_curve");           //copy Assigns
    AppendTSL(temp.mid(1310, 1), "assign4_wave_rate");               //copy Assigns
    AppendTSL(temp.mid(1311, 1), "assign4_waveform");                //copy Assigns
    AppendTSL(temp.mid(1328, 1), "assign5_on_off");                  //copy Assigns
    AppendTSL(temp.mid(1329, 2), "assign5_target");                  //copy Assigns
    AppendTSL(temp.mid(1329, 1), "assign5_target_h");                //copy Assigns
    AppendTSL(temp.mid(1330, 1), "assign5_target_l");                //copy Assigns
    AppendTSL(temp.mid(1331, 1), "assign5_target_min_h");            //copy Assigns
    AppendTSL(temp.mid(1332, 1), "assign5_target_min_l");            //copy Assigns
    AppendTSL(temp.mid(1333, 2), "assign5_target_max");              //copy Assigns
    AppendTSL(temp.mid(1333, 1), "assign5_target_max_h");            //copy Assigns
    AppendTSL(temp.mid(1334, 1), "assign5_target_max_l");            //copy Assigns
    AppendTSL(temp.mid(1335, 1), "assign5_source");                  //copy Assigns
    AppendTSL(temp.mid(1336, 1), "assign5_source_mode");             //copy Assigns
    AppendTSL(temp.mid(1337, 1), "assign5_act_range_lo");            //copy Assigns
    AppendTSL(temp.mid(1338, 1), "assign5_act_range_hi");            //copy Assigns
    AppendTSL(temp.mid(1339, 1), "assign5_int_pdl_trigger");         //copy Assigns
    AppendTSL(temp.mid(1340, 1), "assign5_int_pdl_time");            //copy Assigns
    AppendTSL(temp.mid(1341, 1), "assign5_int_pdl_curve");           //copy Assigns
    AppendTSL(temp.mid(1342, 1), "assign5_wave_rate");               //copy Assigns
    AppendTSL(temp.mid(1343, 1), "assign5_waveform");                //copy Assigns
    AppendTSL(temp.mid(1360, 1), "assign6_on_off");                  //copy Assigns
    AppendTSL(temp.mid(1361, 2), "assign6_target");                  //copy Assigns
    AppendTSL(temp.mid(1361, 1), "assign6_target_h");                //copy Assigns
    AppendTSL(temp.mid(1362, 1), "assign6_target_l");                //copy Assigns
    AppendTSL(temp.mid(1363, 2), "assign6_target_min");              //copy Assigns
    AppendTSL(temp.mid(1363, 1), "assign6_target_min_h");            //copy Assigns
    AppendTSL(temp.mid(1364, 1), "assign6_target_min_l");            //copy Assigns
    AppendTSL(temp.mid(1365, 2), "assign6_target_max");              //copy Assigns
    AppendTSL(temp.mid(1365, 1), "assign6_target_max_h");            //copy Assigns
    AppendTSL(temp.mid(1366, 1), "assign6_target_max_l");            //copy Assigns
    AppendTSL(temp.mid(1367, 1), "assign6_source");                  //copy Assigns
    AppendTSL(temp.mid(1368, 1), "assign6_source_mode");             //copy Assigns
    AppendTSL(temp.mid(1369, 1), "assign6_act_range_lo");            //copy Assigns
    AppendTSL(temp.mid(1370, 1), "assign6_act_range_hi");            //copy Assigns
    AppendTSL(temp.mid(1371, 1), "assign6_int_pdl_trigger");         //copy Assigns
    AppendTSL(temp.mid(1372, 1), "assign6_int_pdl_time");            //copy Assigns
    AppendTSL(temp.mid(1373, 1), "assign6_int_pdl_curve");           //copy Assigns
    AppendTSL(temp.mid(1374, 1), "assign6_wave_rate");               //copy Assigns
    AppendTSL(temp.mid(1375, 1), "assign6_waveform");                //copy Assigns
    AppendTSL(temp.mid(1392, 1), "assign7_on_off");                  //copy Assigns
    AppendTSL(temp.mid(1393, 2), "assign7_target");                  //copy Assigns
    AppendTSL(temp.mid(1393, 1), "assign7_target_h");                //copy Assigns
    AppendTSL(temp.mid(1394, 1), "assign7_target_l");                //copy Assigns
    AppendTSL(temp.mid(1395, 2), "assign7_target_min");              //copy Assigns
    AppendTSL(temp.mid(1395, 1), "assign7_target_min_h");            //copy Assigns
    AppendTSL(temp.mid(1396, 1), "assign7_target_min_l");            //copy Assigns
    AppendTSL(temp.mid(1397, 2), "assign7_target_max");              //copy Assigns
    AppendTSL(temp.mid(1397, 1), "assign7_target_max_h");            //copy Assigns
    AppendTSL(temp.mid(1398, 1), "assign7_target_max_l");            //copy Assigns
    AppendTSL(temp.mid(1399, 1), "assign7_source");                  //copy Assigns
    AppendTSL(temp.mid(1400, 1), "assign7_source_mode");             //copy Assigns
    AppendTSL(temp.mid(1401, 1), "assign7_act_range_lo");            //copy Assigns
    AppendTSL(temp.mid(1402, 1), "assign7_act_range_hi");            //copy Assigns
    AppendTSL(temp.mid(1403, 1), "assign7_int_pdl_trigger");         //copy Assigns
    AppendTSL(temp.mid(1404, 1), "assign7_int_pdl_time");            //copy Assigns
    AppendTSL(temp.mid(1405, 1), "assign7_int_pdl_curve");           //copy Assigns
    AppendTSL(temp.mid(1406, 1), "assign7_wave_rate");               //copy Assigns
    AppendTSL(temp.mid(1407, 1), "assign7_waveform");                //copy Assigns
    AppendTSL(temp.mid(1437, 1), "assign8_on_off");                  //copy Assigns
    AppendTSL(temp.mid(1438, 2), "assign8_target");                  //copy Assigns
    AppendTSL(temp.mid(1438, 1), "assign8_target_h");                //copy Assigns
    AppendTSL(temp.mid(1439, 1), "assign8_target_l");                //copy Assigns
    AppendTSL(temp.mid(1440, 2), "assign8_target_min");              //copy Assigns
    AppendTSL(temp.mid(1440, 1), "assign8_target_min_h");            //copy Assigns
    AppendTSL(temp.mid(1441, 1), "assign8_target_min_l");            //copy Assigns
    AppendTSL(temp.mid(1442, 2), "assign8_target_max");              //copy Assigns
    AppendTSL(temp.mid(1442, 1), "assign8_target_max_h");            //copy Assigns
    AppendTSL(temp.mid(1443, 1), "assign8_target_max_l");            //copy Assigns
    AppendTSL(temp.mid(1444, 1), "assign8_source");                  //copy Assigns
    AppendTSL(temp.mid(1445, 1), "assign8_source_mode");             //copy Assigns
    AppendTSL(temp.mid(1446, 1), "assign8_act_range_lo");            //copy Assigns
    AppendTSL(temp.mid(1447, 1), "assign8_act_range_hi");            //copy Assigns
    AppendTSL(temp.mid(1448, 1), "assign8_int_pdl_trigger");         //copy Assigns
    AppendTSL(temp.mid(1449, 1), "assign8_int_pdl_time");            //copy Assigns
    AppendTSL(temp.mid(1450, 1), "assign8_int_pdl_curve");           //copy Assigns
    AppendTSL(temp.mid(1451, 1), "assign8_wave_rate");               //copy Assigns
    AppendTSL(temp.mid(1452, 1), "assign8_waveform");                //copy Assigns
    AppendTSL(temp.mid(1469, 1), "assign_common_input_sens");        //copy Assign common Input sense

    AppendTSL(temp.mid(2283, 1), "fx1_acsim_high");              //copy FX1
    AppendTSL(temp.mid(2284, 1), "fx1_acsim_body");              //copy FX1
    AppendTSL(temp.mid(2285, 1), "fx1_acsim_low");               //copy FX1
    AppendTSL(temp.mid(2287, 1), "fx1_acsim_level");             //copy FX1
    AppendTSL(temp.mid(2289, 1), "fx1_rotary2_balance");         //copy FX1
    AppendTSL(temp.mid(2290, 1), "fx1_rotary2_speed_sel");       //copy FX1
    AppendTSL(temp.mid(2291, 1), "fx1_rotary2_rate_slow");       //copy FX1
    AppendTSL(temp.mid(2292, 1), "fx1_rotary2_rate_fast");       //copy FX1
    AppendTSL(temp.mid(2293, 1), "fx1_rotary2_risetime");        //copy FX1
    AppendTSL(temp.mid(2294, 1), "fx1_rotary2_falltime");        //copy FX1
    AppendTSL(temp.mid(2295, 1), "fx1_rotary2_depth");           //copy FX1
    AppendTSL(temp.mid(2296, 1), "fx1_rotary2_level");           //copy FX1
    AppendTSL(temp.mid(2297, 1), "fx1_rotary2_direct_mix");      //copy FX1
    AppendTSL(temp.mid(2298, 1), "fx2_acsim_high");                 //copy FX2
    AppendTSL(temp.mid(2299, 1), "fx2_acsim_body");                 //copy FX2
    AppendTSL(temp.mid(2300, 1), "fx2_acsim_low");                  //copy FX2
    AppendTSL(temp.mid(2302, 1), "fx2_acsim_level");                //copy FX2
    AppendTSL(temp.mid(2304, 1), "fx2_rotary2_balance");            //copy FX2
    AppendTSL(temp.mid(2305, 1), "fx2_rotary2_speed_sel");          //copy FX2
    AppendTSL(temp.mid(2306, 1), "fx2_rotary2_rate_slow");          //copy FX2
    AppendTSL(temp.mid(2307, 1), "fx2_rotary2_rate_fast");          //copy FX2
    AppendTSL(temp.mid(2308, 1), "fx2_rotary2_risetime");           //copy FX2
    AppendTSL(temp.mid(2309, 1), "fx2_rotary2_falltime");           //copy FX2
    AppendTSL(temp.mid(2310, 1), "fx2_rotary2_depth");              //copy FX2
    AppendTSL(temp.mid(2311, 1), "fx2_rotary2_level");              //copy FX2
    AppendTSL(temp.mid(2312, 1), "fx2_rotary2_direct_mix");         //copy FX2
    AppendTSL(temp.mid(2314, 1), "prm_fx2_teraecho_mode");          //copy FX2
    AppendTSL(temp.mid(2315, 1), "prm_fx2_teraecho_time");          //copy FX2
    AppendTSL(temp.mid(2316, 1), "prm_fx2_teraecho_feedback");      //copy FX2
    AppendTSL(temp.mid(2317, 1), "prm_fx2_teraecho_tone");          //copy FX2
    AppendTSL(temp.mid(2318, 1), "prm_fx2_teraecho_effect_level");  //copy FX2
    AppendTSL(temp.mid(2319, 1), "prm_fx2_teraecho_direct_mix");    //copy FX2
    AppendTSL(temp.mid(2320, 1), "prm_fx2_teraecho_hold");          //copy FX2
    AppendTSL(temp.mid(2322, 1), "prm_fx2_overtone_detune");        //copy FX2
    AppendTSL(temp.mid(2323, 1), "prm_fx2_overtone_tone");          //copy FX2
    AppendTSL(temp.mid(2324, 1), "prm_fx2_overtone_upper_level");   //copy FX2
    AppendTSL(temp.mid(2325, 1), "prm_fx2_overtone_lower_level");   //copy FX2
    AppendTSL(temp.mid(2326, 1), "prm_fx2_overtone_direct_level");  //copy FX2

    AppendTSL(temp.mid(2328, 1), "fx1_phaser90e_script");    //copy
    AppendTSL(temp.mid(2329, 1), "fx1_phaser90e_speed");     //copy
    AppendTSL(temp.mid(2330, 1), "fx1_flanger117e_manual");  //copy
    AppendTSL(temp.mid(2331, 1), "fx1_flanger117e_width");   //copy
    AppendTSL(temp.mid(2332, 1), "fx1_flanger117e_speed");   //copy
    AppendTSL(temp.mid(2333, 1), "fx1_flanger117e_regen");   //copy
    AppendTSL(temp.mid(2334, 1), "fx2_phaser90e_script");    //copy
    AppendTSL(temp.mid(2335, 1), "fx2_phaser90e_speed");     //copy
    AppendTSL(temp.mid(2336, 1), "fx2_flanger117e_manual");  //copy
    AppendTSL(temp.mid(2337, 1), "fx2_flanger117e_width");   //copy
    AppendTSL(temp.mid(2338, 1), "fx2_flanger117e_speed");   //copy
    AppendTSL(temp.mid(2339, 1), "fx2_flanger117e_regen");   //copy
    AppendTSL(temp.mid(2340, 1), "delay_vtg_lpf");           //copy
    AppendTSL(temp.mid(2341, 1), "delay_vtg_filter");        //copy
    AppendTSL(temp.mid(2342, 1), "delay_vtg_feedback_phase");//copy
    AppendTSL(temp.mid(2343, 1), "delay_vtg_effect_phase");  //copy
    AppendTSL(temp.mid(2344, 1), "delay_vtg_mod_sw");        //copy
    AppendTSL(temp.mid(2345, 1), "delay2_on_off");           //copy
    AppendTSL(temp.mid(2346, 1), "delay2_type");             //copy
    //AppendTSL(temp.mid(2347, 2), "delay2_delay_time");       //copy
    AppendTSL(temp.mid(2347, 1), "delay2_delay_time_h");     //copy
    AppendTSL(temp.mid(2348, 1), "delay2_delay_time_l");     //copy
    AppendTSL(temp.mid(2349, 1), "delay2_f_back");           //copy
    AppendTSL(temp.mid(2350, 1), "delay2_high_cut");         //copy
    AppendTSL(temp.mid(2351, 1), "delay2_effect_level");     //copy
    AppendTSL(temp.mid(2352, 1), "delay2_direct_mix");       //copy
    AppendTSL(temp.mid(2353, 1), "delay2_tap_time");         //copy
    //AppendTSL(temp.mid(2354, 2), "delay2_d1_time");          //copy
    AppendTSL(temp.mid(2354, 1), "delay2_d1_time_h");        //copy
    AppendTSL(temp.mid(2355, 1), "delay2_d1_time_l");        //copy
    AppendTSL(temp.mid(2356, 1), "delay2_d1_f_back");        //copy
    AppendTSL(temp.mid(2357, 1), "delay2_d1_hi_cut");        //copy
    AppendTSL(temp.mid(2358, 1), "delay2_d1_level");         //copy
    //AppendTSL(temp.mid(2359, 2), "delay2_d2_time");          //copy
    AppendTSL(temp.mid(2359, 1), "delay2_d2_time_h");        //copy
    AppendTSL(temp.mid(2360, 1), "delay2_d2_time_l");        //copy
    AppendTSL(temp.mid(2361, 1), "delay2_d2_f_back");        //copy
    AppendTSL(temp.mid(2362, 1), "delay2_d2_hi_cut");        //copy
    AppendTSL(temp.mid(2363, 1), "delay2_d2_level");         //copy
    AppendTSL(temp.mid(2364, 1), "delay2_mod_rate");         //copy
    AppendTSL(temp.mid(2365, 1), "delay2_mod_depth");        //copy
    AppendTSL(temp.mid(2366, 1), "delay2_vtg_lpf");          //copy
    AppendTSL(temp.mid(2367, 1), "delay2_vtg_filter");       //copy
    AppendTSL(temp.mid(2368, 1), "delay2_vtg_feedback_phase");//copy
    AppendTSL(temp.mid(2369, 1), "delay2_vtg_effect_phase"); //copy
    AppendTSL(temp.mid(2370, 1), "delay2_vtg_mod_sw");       //copy

    AppendTSL(temp.mid(2371, 1), "fx1_wah95e_pedal_pos");
    AppendTSL(temp.mid(2372, 1), "fx1_wah95e_pedal_min");
    AppendTSL(temp.mid(2373, 1), "fx1_wah95e_pedal_max");
    AppendTSL(temp.mid(2374, 1), "fx1_wah95e_effect_level");
    AppendTSL(temp.mid(2375, 1), "fx1_wah95e_direct_mix");
    AppendTSL(temp.mid(2376, 1), "fx1_dc30_selector");
    AppendTSL(temp.mid(2377, 1), "fx1_dc30_input_volume");
    AppendTSL(temp.mid(2378, 1), "fx1_dc30_chorus_intensity");
    AppendTSL(temp.mid(2379, 1), "fx1_dc30_echo_repeat_rate_h");
    AppendTSL(temp.mid(23780, 1), "fx1_dc30_echo_repeat_rate_l");
    AppendTSL(temp.mid(2381, 1), "fx1_dc30_echo_intensity");
    AppendTSL(temp.mid(2382, 1), "fx1_dc30_echo_volume");
    AppendTSL(temp.mid(2383, 1), "fx1_dc30_tone");
    AppendTSL(temp.mid(2384, 1), "fx1_dc30_output");
    AppendTSL(temp.mid(2385, 1), "fx2_wah95e_pedal_pos");
    AppendTSL(temp.mid(2386, 1), "fx2_wah95e_pedal_min");
    AppendTSL(temp.mid(2387, 1), "fx2_wah95e_pedal_max");
    AppendTSL(temp.mid(2388, 1), "fx2_wah95e_effect_level");
    AppendTSL(temp.mid(2389, 1), "fx2_wah95e_direct_mix");
    AppendTSL(temp.mid(2390, 1), "fx2_dc30_selector");
    AppendTSL(temp.mid(2391, 1), "fx2_dc30_input_volume");
    AppendTSL(temp.mid(2392, 1), "fx2_dc30_chorus_intensity");
    AppendTSL(temp.mid(2393, 1), "fx2_dc30_echo_repeat_rate_h");
    AppendTSL(temp.mid(2394, 1), "fx2_dc30_echo_repeat_rate_l");
    //int addr11 = 2408;
    AppendTSL(temp.mid(2408, 1), "fx2_dc30_echo_intensity");
    AppendTSL(temp.mid(2409, 1), "fx2_dc30_echo_volume");
    AppendTSL(temp.mid(2410, 1), "fx2_dc30_tone");
    AppendTSL(temp.mid(2411, 1), "fx2_dc30_output");
    AppendTSL(temp.mid(2412, 1), "eq_type");
    AppendTSL(temp.mid(2413, 1), "eq_geq_31hz");
    AppendTSL(temp.mid(2414, 1), "eq_geq_62hz");
    AppendTSL(temp.mid(2415, 1), "eq_geq_125hz");
    AppendTSL(temp.mid(2416, 1), "eq_geq_250hz");
    AppendTSL(temp.mid(2417, 1), "eq_geq_500hz");
    AppendTSL(temp.mid(2418, 1), "eq_geq_1khz");
    AppendTSL(temp.mid(2419, 1), "eq_geq_2khz");
    AppendTSL(temp.mid(2420, 1), "eq_geq_4khz");
    AppendTSL(temp.mid(2421, 1), "eq_geq_8khz");
    AppendTSL(temp.mid(2422, 1), "eq_geq_16khz");    
    AppendTSL(temp.mid(2423, 1), "eq_geq_level");

    AppendTSL(temp.mid(2424, 1), "pedal_fx_position");
    AppendTSL(temp.mid(2425, 1), "pedal_fx_type");
    AppendTSL(temp.mid(2426, 1), "pedal_fx_evh95_position");
    AppendTSL(temp.mid(2427, 1), "pedal_fx_evh95_pedal_min");
    AppendTSL(temp.mid(2428, 1), "pedal_fx_evh95_pedal_max");
    AppendTSL(temp.mid(2429, 1), "pedal_fx_evh95_effect_level");
    AppendTSL(temp.mid(2430, 1), "pedal_fx_evh95_direct_mix");
    AppendTSL(temp.mid(2431, 1), "fx1_heavy_oct_1oct_level");
    AppendTSL(temp.mid(2432, 1), "fx1_heavy_oct_2oct_level");
    AppendTSL(temp.mid(2433, 1), "fx1_heavy_oct_direct_mix");
    AppendTSL(temp.mid(2434, 1), "fx2_heavy_oct_1oct_level");
    AppendTSL(temp.mid(2435, 1), "fx2_heavy_oct_2oct_level");
    AppendTSL(temp.mid(2436, 1), "fx2_heavy_oct_direct_mix");


    int addr12 = 2549;
    AppendTSL(temp.mid(addr12, 1), "chain_ptn");                  //copy KATANA Chain
    AppendTSL(temp.mid(addr12+1, 1), "fxbox_asgn_fx1a_g");          //copy
    AppendTSL(temp.mid(addr12+2, 1), "fxbox_asgn_fx1a_r");          //copy
    AppendTSL(temp.mid(addr12+3, 1), "fxbox_asgn_fx1a_y");          //copy
    AppendTSL(temp.mid(addr12+4, 1), "fxbox_asgn_fx1b_g");          //copy
    AppendTSL(temp.mid(addr12+5, 1), "fxbox_asgn_fx1b_r");          //copy
    AppendTSL(temp.mid(addr12+6, 1), "fxbox_asgn_fx1b_y");          //copy
    AppendTSL(temp.mid(addr12+7, 1), "fxbox_asgn_fx2a_g");          //copy
    AppendTSL(temp.mid(addr12+8, 1), "fxbox_asgn_fx2a_r");          //copy
    AppendTSL(temp.mid(addr12+9, 1), "fxbox_asgn_fx2a_y");          //copy
    AppendTSL(temp.mid(addr12+10, 1), "fxbox_asgn_fx2b_g");          //copy
    AppendTSL(temp.mid(addr12+11, 1), "fxbox_asgn_fx2b_r");          //copy
    AppendTSL(temp.mid(addr12+12, 1), "fxbox_asgn_fx2b_y");          //copy
    AppendTSL(temp.mid(addr12+13, 1), "fxbox_asgn_fx3_g");           //copy
    AppendTSL(temp.mid(addr12+14, 1), "fxbox_asgn_fx3_r");           //copy
    AppendTSL(temp.mid(addr12+15, 1), "fxbox_asgn_fx3_y");           //copy
    AppendTSL(temp.mid(addr12+16, 1), "fxbox_sel_fx1b");             //copy
    AppendTSL(temp.mid(addr12+17, 1), "fxbox_sel_fx2a");             //copy
    AppendTSL(temp.mid(addr12+18, 1), "fxbox_sel_fx3");              //copy
    AppendTSL(temp.mid(addr12+19, 1), "fx_active_ab_fx1");           //copy
    AppendTSL(temp.mid(addr12+20, 1), "fx_active_ab_fx2");           //copy
    AppendTSL(temp.mid(addr12+21, 1), "fxbox_sel_fx1a");             //copy
    AppendTSL(temp.mid(addr12+22, 1), "fxbox_sel_fx2b");             //copy
    AppendTSL(temp.mid(addr12+23, 1), "send_return_position");       //copy
    AppendTSL(temp.mid(addr12+24, 1), "eq_position");                //copy
    AppendTSL(temp.mid(addr12+25, 1), "fxbox_asgn_fx3b_g");          //copy
    AppendTSL(temp.mid(addr12+26, 1), "fxbox_asgn_fx3b_r");          //copy
    AppendTSL(temp.mid(addr12+27, 1), "fxbox_asgn_fx3b_y");          //copy
    AppendTSL(temp.mid(addr12+28, 1), "fxbox_layer_fx3_g");          //copy
    AppendTSL(temp.mid(addr12+29, 1), "fxbox_layer_fx3_r");          //copy
    AppendTSL(temp.mid(addr12+30, 1), "fxbox_layer_fx3_y");          //copy
    AppendTSL(temp.mid(addr12+31, 1), "pedal_function_exp_pedal");
    AppendTSL(temp.mid(addr12+32, 1), "pedal_function_gafc_exp1");
    AppendTSL(temp.mid(addr12+33, 1), "pedal_function_gafc_exp2");

    AppendTSL(temp.mid(addr12+48, 1), "knob_assign_booster");
    AppendTSL(temp.mid(addr12+49, 1), "knob_assign_delay");
    AppendTSL(temp.mid(addr12+50, 1), "knob_assign_reverb");
    AppendTSL(temp.mid(addr12+51, 1), "knob_assign_chorus");
    AppendTSL(temp.mid(addr12+52, 1), "knob_assign_flanger");
    AppendTSL(temp.mid(addr12+53, 1), "knob_assign_phaser");
    AppendTSL(temp.mid(addr12+54, 1), "knob_assign_uni_v");
    AppendTSL(temp.mid(addr12+55, 1), "knob_assign_tremolo");
    AppendTSL(temp.mid(addr12+56, 1), "knob_assign_vibrato");
    AppendTSL(temp.mid(addr12+57, 1), "knob_assign_rotary");
    AppendTSL(temp.mid(addr12+58, 1), "knob_assign_ring_mod");
    AppendTSL(temp.mid(addr12+59, 1), "knob_assign_slow_gear");
    AppendTSL(temp.mid(addr12+60, 1), "knob_assign_slicer");
    AppendTSL(temp.mid(addr12+61, 1), "knob_assign_comp");
    AppendTSL(temp.mid(addr12+62, 1), "knob_assign_limiter");
    AppendTSL(temp.mid(addr12+63, 1), "knob_assign_t_wah");
    AppendTSL(temp.mid(addr12+64, 1), "knob_assign_auto_wah");
    AppendTSL(temp.mid(addr12+65, 1), "knob_assign_pedal_wah");
    AppendTSL(temp.mid(addr12+66, 1), "knob_assign_geq");
    AppendTSL(temp.mid(addr12+67, 1), "knob_assign_peq");
    AppendTSL(temp.mid(addr12+68, 1), "knob_assign_guitar_sim");
    AppendTSL(temp.mid(addr12+69, 1), "knob_assign_ac_guitar_sim");
    AppendTSL(temp.mid(addr12+70, 1), "knob_assign_ac_processor");
    AppendTSL(temp.mid(addr12+71, 1), "knob_assign_wave_synth");
    AppendTSL(temp.mid(addr12+72, 1), "knob_assign_octave");
    AppendTSL(temp.mid(addr12+73, 1), "knob_assign_pitch_shifter");
    AppendTSL(temp.mid(addr12+74, 1), "knob_assign_harmonist");
    AppendTSL(temp.mid(addr12+75, 1), "knob_assign_humanizer");
    AppendTSL(temp.mid(addr12+76, 1), "knob_assign_evh_phaser");
    AppendTSL(temp.mid(addr12+77, 1), "knob_assign_evh_flanger");
    AppendTSL(temp.mid(addr12+78, 1), "knob_assign_wah95e");
    AppendTSL(temp.mid(addr12+79, 1), "knob_assign_dc30");
    AppendTSL(temp.mid(addr12+80, 1), "knob_assign_heavy_oct");
    int addr13 = 2690;
    AppendTSL(temp.mid(addr13+0, 1), "exp_pedal_assign_booster");
    AppendTSL(temp.mid(addr13+1, 1), "exp_pedal_assign_delay");
    AppendTSL(temp.mid(addr13+2, 1), "exp_pedal_assign_reverb");
    AppendTSL(temp.mid(addr13+3, 1), "exp_pedal_assign_chorus");
    AppendTSL(temp.mid(addr13+4, 1), "exp_pedal_assign_flanger");
    AppendTSL(temp.mid(addr13+5, 1), "exp_pedal_assign_phaser");
    AppendTSL(temp.mid(addr13+6, 1), "exp_pedal_assign_uni_v");
    AppendTSL(temp.mid(addr13+7, 1), "exp_pedal_assign_tremolo");
    AppendTSL(temp.mid(addr13+8, 1), "exp_pedal_assign_vibrato");
    AppendTSL(temp.mid(addr13+9, 1), "exp_pedal_assign_rotary");
    AppendTSL(temp.mid(addr13+10, 1), "exp_pedal_assign_ring_mod");
    AppendTSL(temp.mid(addr13+11, 1), "exp_pedal_assign_slow_gear");
    AppendTSL(temp.mid(addr13+12, 1), "exp_pedal_assign_slicer");
    AppendTSL(temp.mid(addr13+13, 1), "exp_pedal_assign_comp");
    AppendTSL(temp.mid(addr13+14, 1), "exp_pedal_assign_limiter");
    AppendTSL(temp.mid(addr13+15, 1), "exp_pedal_assign_t_wah");
    AppendTSL(temp.mid(addr13+16, 1), "exp_pedal_assign_auto_wah");
    AppendTSL(temp.mid(addr13+17, 1), "exp_pedal_assign_pedal_wah");
    AppendTSL(temp.mid(addr13+18, 1), "exp_pedal_assign_geq");
    AppendTSL(temp.mid(addr13+19, 1), "exp_pedal_assign_peq");
    AppendTSL(temp.mid(addr13+20, 1), "exp_pedal_assign_guitar_sim");
    AppendTSL(temp.mid(addr13+21, 1), "exp_pedal_assign_ac_guitar_sim");
    AppendTSL(temp.mid(addr13+22, 1), "exp_pedal_assign_ac_processor");
    AppendTSL(temp.mid(addr13+23, 1), "exp_pedal_assign_wave_synth");
    AppendTSL(temp.mid(addr13+24, 1), "exp_pedal_assign_octave");
    AppendTSL(temp.mid(addr13+25, 1), "exp_pedal_assign_pitch_shifter");
    AppendTSL(temp.mid(addr13+26, 1), "exp_pedal_assign_harmonist");
    AppendTSL(temp.mid(addr13+27, 1), "exp_pedal_assign_humanizer");
    AppendTSL(temp.mid(addr13+28, 1), "exp_pedal_assign_evh_phaser");
    AppendTSL(temp.mid(addr13+29, 1), "exp_pedal_assign_evh_flanger");
    AppendTSL(temp.mid(addr13+30, 1), "exp_pedal_assign_wah95e");
    AppendTSL(temp.mid(addr13+31, 1), "exp_pedal_assign_dc30");
    AppendTSL(temp.mid(addr13+32, 1), "exp_pedal_assign_booster_min");
    AppendTSL(temp.mid(addr13+33, 1), "exp_pedal_assign_booster_max");
    AppendTSL(temp.mid(addr13+34, 1), "exp_pedal_assign_delay_min_h");
    AppendTSL(temp.mid(addr13+35, 1), "exp_pedal_assign_delay_min_l");
    AppendTSL(temp.mid(addr13+36, 1), "exp_pedal_assign_delay_max_h");
    AppendTSL(temp.mid(addr13+37, 1), "exp_pedal_assign_delay_max_l");
    AppendTSL(temp.mid(addr13+38, 1), "exp_pedal_assign_reverb_min_h");
    AppendTSL(temp.mid(addr13+39, 1), "exp_pedal_assign_reverb_min_l");
    AppendTSL(temp.mid(addr13+40, 1), "exp_pedal_assign_reverb_max_h");
    AppendTSL(temp.mid(addr13+41, 1), "exp_pedal_assign_reverb_max_l");
    AppendTSL(temp.mid(addr13+42, 1), "exp_pedal_assign_chorus_min");
    AppendTSL(temp.mid(addr13+43, 1), "exp_pedal_assign_chorus_max");
    AppendTSL(temp.mid(addr13+44, 1), "exp_pedal_assign_flanger_min");
    AppendTSL(temp.mid(addr13+45, 1), "exp_pedal_assign_flanger_max");
    AppendTSL(temp.mid(addr13+46, 1), "exp_pedal_assign_phaser_min");
    AppendTSL(temp.mid(addr13+47, 1), "exp_pedal_assign_phaser_max");
    AppendTSL(temp.mid(addr13+48, 1), "exp_pedal_assign_uni_v_min");
    AppendTSL(temp.mid(addr13+49, 1), "exp_pedal_assign_uni_v_max");
    AppendTSL(temp.mid(addr13+50, 1), "exp_pedal_assign_tremolo_min");
    AppendTSL(temp.mid(addr13+51, 1), "exp_pedal_assign_tremolo_max");
    AppendTSL(temp.mid(addr13+52, 1), "exp_pedal_assign_vibrato_min");
    AppendTSL(temp.mid(addr13+53, 1), "exp_pedal_assign_vibrato_max");
    AppendTSL(temp.mid(addr13+54, 1), "exp_pedal_assign_rotary_min");
    AppendTSL(temp.mid(addr13+55, 1), "exp_pedal_assign_rotary_max");
    AppendTSL(temp.mid(addr13+56, 1), "exp_pedal_assign_ring_mod_min");
    AppendTSL(temp.mid(addr13+57, 1), "exp_pedal_assign_ring_mod_max");
    AppendTSL(temp.mid(addr13+58, 1), "exp_pedal_assign_slow_gear_min");
    AppendTSL(temp.mid(addr13+59, 1), "exp_pedal_assign_slow_gear_max");
    AppendTSL(temp.mid(addr13+60, 1), "exp_pedal_assign_slicer_min");
    AppendTSL(temp.mid(addr13+61, 1), "exp_pedal_assign_slicer_max");
    AppendTSL(temp.mid(addr13+62, 1), "exp_pedal_assign_comp_min");
    AppendTSL(temp.mid(addr13+63, 1), "exp_pedal_assign_comp_max");
    AppendTSL(temp.mid(addr13+64, 1), "exp_pedal_assign_limiter_min");
    AppendTSL(temp.mid(addr13+65, 1), "exp_pedal_assign_limiter_max");
    AppendTSL(temp.mid(addr13+66, 1), "exp_pedal_assign_t_wah_min");
    AppendTSL(temp.mid(addr13+67, 1), "exp_pedal_assign_t_wah_max");
    AppendTSL(temp.mid(addr13+68, 1), "exp_pedal_assign_auto_wah_min");
    AppendTSL(temp.mid(addr13+69, 1), "exp_pedal_assign_auto_wah_max");
    AppendTSL(temp.mid(addr13+70, 1), "exp_pedal_assign_pedal_wah_min");
    AppendTSL(temp.mid(addr13+71, 1), "exp_pedal_assign_pedal_wah_max");
    AppendTSL(temp.mid(addr13+72, 1), "exp_pedal_assign_geq_min");
    AppendTSL(temp.mid(addr13+73, 1), "exp_pedal_assign_geq_max");
    AppendTSL(temp.mid(addr13+74, 1), "exp_pedal_assign_peq_min");
    AppendTSL(temp.mid(addr13+75, 1), "exp_pedal_assign_peq_max");
    AppendTSL(temp.mid(addr13+76, 1), "exp_pedal_assign_guitar_sim_min");
    AppendTSL(temp.mid(addr13+77, 1), "exp_pedal_assign_guitar_sim_max");
    AppendTSL(temp.mid(addr13+78, 1), "exp_pedal_assign_ac_guitar_sim_min");
    AppendTSL(temp.mid(addr13+79, 1), "exp_pedal_assign_ac_guitar_sim_max");
    AppendTSL(temp.mid(addr13+80, 1), "exp_pedal_assign_ac_processor_min");
    AppendTSL(temp.mid(addr13+81, 1), "exp_pedal_assign_ac_processor_max");
    AppendTSL(temp.mid(addr13+82, 1), "exp_pedal_assign_wave_synth_min");
    AppendTSL(temp.mid(addr13+83, 1), "exp_pedal_assign_wave_synth_max");
    AppendTSL(temp.mid(addr13+84, 1), "exp_pedal_assign_octave_min");
    AppendTSL(temp.mid(addr13+85, 1), "exp_pedal_assign_octave_max");
    AppendTSL(temp.mid(addr13+86, 1), "exp_pedal_assign_pitch_shifter_min_h");
    AppendTSL(temp.mid(addr13+87, 1), "exp_pedal_assign_pitch_shifter_min_l");
    AppendTSL(temp.mid(addr13+88, 1), "exp_pedal_assign_pitch_shifter_max_h");
    AppendTSL(temp.mid(addr13+89, 1), "exp_pedal_assign_pitch_shifter_max_l");
    AppendTSL(temp.mid(addr13+90, 1), "exp_pedal_assign_harmonist_min_h");
    AppendTSL(temp.mid(addr13+91, 1), "exp_pedal_assign_harmonist_min_l");
    AppendTSL(temp.mid(addr13+92, 1), "exp_pedal_assign_harmonist_max_h");
    AppendTSL(temp.mid(addr13+93, 1), "exp_pedal_assign_harmonist_max_l");
    AppendTSL(temp.mid(addr13+94, 1), "exp_pedal_assign_humanizer_min");
    AppendTSL(temp.mid(addr13+95, 1), "exp_pedal_assign_humanizer_max");
    AppendTSL(temp.mid(addr13+96, 1), "exp_pedal_assign_evh_phaser_min");
    AppendTSL(temp.mid(addr13+97, 1), "exp_pedal_assign_evh_phaser_max");
    AppendTSL(temp.mid(addr13+98, 1), "exp_pedal_assign_evh_flanger_min");
    AppendTSL(temp.mid(addr13+99, 1), "exp_pedal_assign_evh_flanger_max");
    AppendTSL(temp.mid(addr13+100, 1), "exp_pedal_assign_wah95e_min");
    AppendTSL(temp.mid(addr13+101, 1), "exp_pedal_assign_wah95e_max");
    AppendTSL(temp.mid(addr13+102, 1), "exp_pedal_assign_dc30_min_h");
    AppendTSL(temp.mid(addr13+103, 1), "exp_pedal_assign_dc30_min_l");
    AppendTSL(temp.mid(addr13+104, 1), "exp_pedal_assign_dc30_max_h");
    AppendTSL(temp.mid(addr13+105, 1), "exp_pedal_assign_dc30_max_l");
    AppendTSL(temp.mid(addr13+106, 1), "exp_pedal_assign_heavy_oct");
    AppendTSL(temp.mid(addr13+107, 1), "exp_pedal_assign_heavy_oct_min");
    AppendTSL(temp.mid(addr13+108, 1), "exp_pedal_assign_heavy_oct_max");

    int addr14 = 2831;
    AppendTSL(temp.mid(addr14+0, 1), "gafc_exp1_assign_booster");
    AppendTSL(temp.mid(addr14+1, 1), "gafc_exp1_assign_delay");
    AppendTSL(temp.mid(addr14+2, 1), "gafc_exp1_assign_reverb");
    AppendTSL(temp.mid(addr14+3, 1), "gafc_exp1_assign_chorus");
    AppendTSL(temp.mid(addr14+4, 1), "gafc_exp1_assign_flanger");
    AppendTSL(temp.mid(addr14+5, 1), "gafc_exp1_assign_phaser");
    AppendTSL(temp.mid(addr14+6, 1), "gafc_exp1_assign_uni_v");
    AppendTSL(temp.mid(addr14+7, 1), "gafc_exp1_assign_tremolo");
    AppendTSL(temp.mid(addr14+8, 1), "gafc_exp1_assign_vibrato");
    AppendTSL(temp.mid(addr14+9, 1), "gafc_exp1_assign_rotary");
    AppendTSL(temp.mid(addr14+10, 1), "gafc_exp1_assign_ring_mod");
    AppendTSL(temp.mid(addr14+11, 1), "gafc_exp1_assign_slow_gear");
    AppendTSL(temp.mid(addr14+12, 1), "gafc_exp1_assign_slicer");
    AppendTSL(temp.mid(addr14+13, 1), "gafc_exp1_assign_comp");
    AppendTSL(temp.mid(addr14+14, 1), "gafc_exp1_assign_limiter");
    AppendTSL(temp.mid(addr14+15, 1), "gafc_exp1_assign_t_wah");
    AppendTSL(temp.mid(addr14+16, 1), "gafc_exp1_assign_auto_wah");
    AppendTSL(temp.mid(addr14+17, 1), "gafc_exp1_assign_pedal_wah");
    AppendTSL(temp.mid(addr14+18, 1), "gafc_exp1_assign_geq");
    AppendTSL(temp.mid(addr14+19, 1), "gafc_exp1_assign_peq");
    AppendTSL(temp.mid(addr14+20, 1), "gafc_exp1_assign_guitar_sim");
    AppendTSL(temp.mid(addr14+21, 1), "gafc_exp1_assign_ac_guitar_sim");
    AppendTSL(temp.mid(addr14+22, 1), "gafc_exp1_assign_ac_processor");
    AppendTSL(temp.mid(addr14+23, 1), "gafc_exp1_assign_wave_synth");
    AppendTSL(temp.mid(addr14+24, 1), "gafc_exp1_assign_octave");
    AppendTSL(temp.mid(addr14+25, 1), "gafc_exp1_assign_pitch_shifter");
    AppendTSL(temp.mid(addr14+26, 1), "gafc_exp1_assign_harmonist");
    AppendTSL(temp.mid(addr14+27, 1), "gafc_exp1_assign_humanizer");
    AppendTSL(temp.mid(addr14+28, 1), "gafc_exp1_assign_evh_phaser");
    AppendTSL(temp.mid(addr14+29, 1), "gafc_exp1_assign_evh_flanger");
    AppendTSL(temp.mid(addr14+30, 1), "gafc_exp1_assign_wah95e");
    AppendTSL(temp.mid(addr14+31, 1), "gafc_exp1_assign_dc30");
    AppendTSL(temp.mid(addr14+32, 1), "gafc_exp1_assign_booster_min");
    AppendTSL(temp.mid(addr14+33, 1), "gafc_exp1_assign_booster_max");
    AppendTSL(temp.mid(addr14+34, 1), "gafc_exp1_assign_delay_min_h");
    AppendTSL(temp.mid(addr14+35, 1), "gafc_exp1_assign_delay_min_l");
    AppendTSL(temp.mid(addr14+36, 1), "gafc_exp1_assign_delay_max_h");
    AppendTSL(temp.mid(addr14+37, 1), "gafc_exp1_assign_delay_max_l");
    AppendTSL(temp.mid(addr14+38, 1), "gafc_exp1_assign_reverb_min_h");
    AppendTSL(temp.mid(addr14+39, 1), "gafc_exp1_assign_reverb_min_l");
    AppendTSL(temp.mid(addr14+40, 1), "gafc_exp1_assign_reverb_max_h");
    AppendTSL(temp.mid(addr14+41, 1), "gafc_exp1_assign_reverb_max_l");
    AppendTSL(temp.mid(addr14+42, 1), "gafc_exp1_assign_chorus_min");
    AppendTSL(temp.mid(addr14+43, 1), "gafc_exp1_assign_chorus_max");
    AppendTSL(temp.mid(addr14+44, 1), "gafc_exp1_assign_flanger_min");
    AppendTSL(temp.mid(addr14+45, 1), "gafc_exp1_assign_flanger_max");
    AppendTSL(temp.mid(addr14+46, 1), "gafc_exp1_assign_phaser_min");
    AppendTSL(temp.mid(addr14+47, 1), "gafc_exp1_assign_phaser_max");
    AppendTSL(temp.mid(addr14+48, 1), "gafc_exp1_assign_uni_v_min");
    AppendTSL(temp.mid(addr14+49, 1), "gafc_exp1_assign_uni_v_max");
    AppendTSL(temp.mid(addr14+50, 1), "gafc_exp1_assign_tremolo_min");
    AppendTSL(temp.mid(addr14+51, 1), "gafc_exp1_assign_tremolo_max");
    AppendTSL(temp.mid(addr14+52, 1), "gafc_exp1_assign_vibrato_min");
    AppendTSL(temp.mid(addr14+53, 1), "gafc_exp1_assign_vibrato_max");
    AppendTSL(temp.mid(addr14+54, 1), "gafc_exp1_assign_rotary_min");
    AppendTSL(temp.mid(addr14+55, 1), "gafc_exp1_assign_rotary_max");
    AppendTSL(temp.mid(addr14+56, 1), "gafc_exp1_assign_ring_mod_min");
    AppendTSL(temp.mid(addr14+57, 1), "gafc_exp1_assign_ring_mod_max");
    AppendTSL(temp.mid(addr14+58, 1), "gafc_exp1_assign_slow_gear_min");
    AppendTSL(temp.mid(addr14+59, 1), "gafc_exp1_assign_slow_gear_max");
    AppendTSL(temp.mid(addr14+60, 1), "gafc_exp1_assign_slicer_min");
    AppendTSL(temp.mid(addr14+61, 1), "gafc_exp1_assign_slicer_max");
    AppendTSL(temp.mid(addr14+62, 1), "gafc_exp1_assign_comp_min");
    AppendTSL(temp.mid(addr14+63, 1), "gafc_exp1_assign_comp_max");
    AppendTSL(temp.mid(addr14+64, 1), "gafc_exp1_assign_limiter_min");
    AppendTSL(temp.mid(addr14+65, 1), "gafc_exp1_assign_limiter_max");
    AppendTSL(temp.mid(addr14+66, 1), "gafc_exp1_assign_t_wah_min");
    AppendTSL(temp.mid(addr14+67, 1), "gafc_exp1_assign_t_wah_max");
    AppendTSL(temp.mid(addr14+68, 1), "gafc_exp1_assign_auto_wah_min");
    AppendTSL(temp.mid(addr14+69, 1), "gafc_exp1_assign_auto_wah_max");
    AppendTSL(temp.mid(addr14+70, 1), "gafc_exp1_assign_pedal_wah_min");
    AppendTSL(temp.mid(addr14+71, 1), "gafc_exp1_assign_pedal_wah_max");
    AppendTSL(temp.mid(addr14+72, 1), "gafc_exp1_assign_geq_min");
    AppendTSL(temp.mid(addr14+73, 1), "gafc_exp1_assign_geq_max");
    AppendTSL(temp.mid(addr14+74, 1), "gafc_exp1_assign_peq_min");
    AppendTSL(temp.mid(addr14+75, 1), "gafc_exp1_assign_peq_max");
    AppendTSL(temp.mid(addr14+76, 1), "gafc_exp1_assign_guitar_sim_min");
    AppendTSL(temp.mid(addr14+77, 1), "gafc_exp1_assign_guitar_sim_max");
    AppendTSL(temp.mid(addr14+78, 1), "gafc_exp1_assign_ac_guitar_sim_min");
    AppendTSL(temp.mid(addr14+79, 1), "gafc_exp1_assign_ac_guitar_sim_max");
    AppendTSL(temp.mid(addr14+80, 1), "gafc_exp1_assign_ac_processor_min");
    AppendTSL(temp.mid(addr14+81, 1), "gafc_exp1_assign_ac_processor_max");
    AppendTSL(temp.mid(addr14+82, 1), "gafc_exp1_assign_wave_synth_min");
    AppendTSL(temp.mid(addr14+83, 1), "gafc_exp1_assign_wave_synth_max");
    AppendTSL(temp.mid(addr14+84, 1), "gafc_exp1_assign_octave_min");
    AppendTSL(temp.mid(addr14+85, 1), "gafc_exp1_assign_octave_max");
    AppendTSL(temp.mid(addr14+86, 1), "gafc_exp1_assign_pitch_shifter_min_h");
    AppendTSL(temp.mid(addr14+87, 1), "gafc_exp1_assign_pitch_shifter_min_l");
    AppendTSL(temp.mid(addr14+88, 1), "gafc_exp1_assign_pitch_shifter_max_h");
    AppendTSL(temp.mid(addr14+89, 1), "gafc_exp1_assign_pitch_shifter_max_l");
    AppendTSL(temp.mid(addr14+90, 1), "gafc_exp1_assign_harmonist_min_h");
    AppendTSL(temp.mid(addr14+91, 1), "gafc_exp1_assign_harmonist_min_l");
    AppendTSL(temp.mid(addr14+92, 1), "gafc_exp1_assign_harmonist_max_h");
    AppendTSL(temp.mid(addr14+93, 1), "gafc_exp1_assign_harmonist_max_l");
    AppendTSL(temp.mid(addr14+94, 1), "gafc_exp1_assign_humanizer_min");
    AppendTSL(temp.mid(addr14+95, 1), "gafc_exp1_assign_humanizer_max");
    AppendTSL(temp.mid(addr14+96, 1), "gafc_exp1_assign_evh_phaser_min");
    AppendTSL(temp.mid(addr14+97, 1), "gafc_exp1_assign_evh_phaser_max");
    AppendTSL(temp.mid(addr14+98, 1), "gafc_exp1_assign_evh_flanger_min");
    AppendTSL(temp.mid(addr14+99, 1), "gafc_exp1_assign_evh_flanger_max");
    AppendTSL(temp.mid(addr14+100, 1), "gafc_exp1_assign_wah95e_min");
    AppendTSL(temp.mid(addr14+101, 1), "gafc_exp1_assign_wah95e_max");
    AppendTSL(temp.mid(addr14+102, 1), "gafc_exp1_assign_dc30_min_h");
    AppendTSL(temp.mid(addr14+103, 1), "gafc_exp1_assign_dc30_min_l");
    AppendTSL(temp.mid(addr14+104, 1), "gafc_exp1_assign_dc30_max_h");
    AppendTSL(temp.mid(addr14+105, 1), "gafc_exp1_assign_dc30_max_l");
    AppendTSL(temp.mid(addr14+106, 1), "gafc_exp1_assign_heavy_oct");
    AppendTSL(temp.mid(addr14+107, 1), "gafc_exp1_assign_heavy_oct_min");
    AppendTSL(temp.mid(addr14+108, 1), "gafc_exp1_assign_heavy_oct_max");


    int addr15 = 2972;
    AppendTSL(temp.mid(addr15+0, 1), "gafc_exp2_assign_booster");
    AppendTSL(temp.mid(addr15+1, 1), "gafc_exp2_assign_delay");
    AppendTSL(temp.mid(addr15+2, 1), "gafc_exp2_assign_reverb");
    AppendTSL(temp.mid(addr15+3, 1), "gafc_exp2_assign_chorus");
    AppendTSL(temp.mid(addr15+4, 1), "gafc_exp2_assign_flanger");
    AppendTSL(temp.mid(addr15+5, 1), "gafc_exp2_assign_phaser");
    AppendTSL(temp.mid(addr15+6, 1), "gafc_exp2_assign_uni_v");
    AppendTSL(temp.mid(addr15+7, 1), "gafc_exp2_assign_tremolo");
    AppendTSL(temp.mid(addr15+8, 1), "gafc_exp2_assign_vibrato");
    AppendTSL(temp.mid(addr15+9, 1), "gafc_exp2_assign_rotary");
    AppendTSL(temp.mid(addr15+10, 1), "gafc_exp2_assign_ring_mod");
    AppendTSL(temp.mid(addr15+11, 1), "gafc_exp2_assign_slow_gear");
    AppendTSL(temp.mid(addr15+12, 1), "gafc_exp2_assign_slicer");
    AppendTSL(temp.mid(addr15+13, 1), "gafc_exp2_assign_comp");
    AppendTSL(temp.mid(addr15+14, 1), "gafc_exp2_assign_limiter");
    AppendTSL(temp.mid(addr15+15, 1), "gafc_exp2_assign_t_wah");
    AppendTSL(temp.mid(addr15+16, 1), "gafc_exp2_assign_auto_wah");
    AppendTSL(temp.mid(addr15+17, 1), "gafc_exp2_assign_pedal_wah");
    AppendTSL(temp.mid(addr15+18, 1), "gafc_exp2_assign_geq");
    AppendTSL(temp.mid(addr15+19, 1), "gafc_exp2_assign_peq");
    AppendTSL(temp.mid(addr15+20, 1), "gafc_exp2_assign_guitar_sim");
    AppendTSL(temp.mid(addr15+21, 1), "gafc_exp2_assign_ac_guitar_sim");
    AppendTSL(temp.mid(addr15+22, 1), "gafc_exp2_assign_ac_processor");
    AppendTSL(temp.mid(addr15+23, 1), "gafc_exp2_assign_wave_synth");
    AppendTSL(temp.mid(addr15+24, 1), "gafc_exp2_assign_octave");
    AppendTSL(temp.mid(addr15+25, 1), "gafc_exp2_assign_pitch_shifter");
    AppendTSL(temp.mid(addr15+26, 1), "gafc_exp2_assign_harmonist");
    AppendTSL(temp.mid(addr15+27, 1), "gafc_exp2_assign_humanizer");
    AppendTSL(temp.mid(addr15+28, 1), "gafc_exp2_assign_evh_phaser");
    AppendTSL(temp.mid(addr15+29, 1), "gafc_exp2_assign_evh_flanger");
    AppendTSL(temp.mid(addr15+30, 1), "gafc_exp2_assign_wah95e");
    AppendTSL(temp.mid(addr15+31, 1), "gafc_exp2_assign_dc30");
    AppendTSL(temp.mid(addr15+32, 1), "gafc_exp2_assign_booster_min");
    AppendTSL(temp.mid(addr15+33, 1), "gafc_exp2_assign_booster_max");
    AppendTSL(temp.mid(addr15+34, 1), "gafc_exp2_assign_delay_min_h");
    AppendTSL(temp.mid(addr15+35, 1), "gafc_exp2_assign_delay_min_l");
    AppendTSL(temp.mid(addr15+36, 1), "gafc_exp2_assign_delay_max_h");
    AppendTSL(temp.mid(addr15+37, 1), "gafc_exp2_assign_delay_max_l");
    AppendTSL(temp.mid(addr15+38, 1), "gafc_exp2_assign_reverb_min_h");
    AppendTSL(temp.mid(addr15+39, 1), "gafc_exp2_assign_reverb_min_l");
    AppendTSL(temp.mid(addr15+40, 1), "gafc_exp2_assign_reverb_max_h");
    AppendTSL(temp.mid(addr15+41, 1), "gafc_exp2_assign_reverb_max_l");
    AppendTSL(temp.mid(addr15+42, 1), "gafc_exp2_assign_chorus_min");
    AppendTSL(temp.mid(addr15+43, 1), "gafc_exp2_assign_chorus_max");
    AppendTSL(temp.mid(addr15+44, 1), "gafc_exp2_assign_flanger_min");
    AppendTSL(temp.mid(addr15+45, 1), "gafc_exp2_assign_flanger_max");
    AppendTSL(temp.mid(addr15+46, 1), "gafc_exp2_assign_phaser_min");
    AppendTSL(temp.mid(addr15+47, 1), "gafc_exp2_assign_phaser_max");
    AppendTSL(temp.mid(addr15+48, 1), "gafc_exp2_assign_uni_v_min");
    AppendTSL(temp.mid(addr15+49, 1), "gafc_exp2_assign_uni_v_max");
    AppendTSL(temp.mid(addr15+50, 1), "gafc_exp2_assign_tremolo_min");
    AppendTSL(temp.mid(addr15+51, 1), "gafc_exp2_assign_tremolo_max");
    AppendTSL(temp.mid(addr15+52, 1), "gafc_exp2_assign_vibrato_min");
    AppendTSL(temp.mid(addr15+53, 1), "gafc_exp2_assign_vibrato_max");
    AppendTSL(temp.mid(addr15+54, 1), "gafc_exp2_assign_rotary_min");
    AppendTSL(temp.mid(addr15+55, 1), "gafc_exp2_assign_rotary_max");
    AppendTSL(temp.mid(addr15+56, 1), "gafc_exp2_assign_ring_mod_min");
    AppendTSL(temp.mid(addr15+57, 1), "gafc_exp2_assign_ring_mod_max");
    AppendTSL(temp.mid(addr15+58, 1), "gafc_exp2_assign_slow_gear_min");
    AppendTSL(temp.mid(addr15+59, 1), "gafc_exp2_assign_slow_gear_max");
    AppendTSL(temp.mid(addr15+60, 1), "gafc_exp2_assign_slicer_min");
    AppendTSL(temp.mid(addr15+61, 1), "gafc_exp2_assign_slicer_max");
    AppendTSL(temp.mid(addr15+62, 1), "gafc_exp2_assign_comp_min");
    AppendTSL(temp.mid(addr15+63, 1), "gafc_exp2_assign_comp_max");
    AppendTSL(temp.mid(addr15+64, 1), "gafc_exp2_assign_limiter_min");
    AppendTSL(temp.mid(addr15+65, 1), "gafc_exp2_assign_limiter_max");
    AppendTSL(temp.mid(addr15+66, 1), "gafc_exp2_assign_t_wah_min");
    AppendTSL(temp.mid(addr15+67, 1), "gafc_exp2_assign_t_wah_max");
    AppendTSL(temp.mid(addr15+68, 1), "gafc_exp2_assign_auto_wah_min");
    AppendTSL(temp.mid(addr15+69, 1), "gafc_exp2_assign_auto_wah_max");
    AppendTSL(temp.mid(addr15+70, 1), "gafc_exp2_assign_pedal_wah_min");
    AppendTSL(temp.mid(addr15+71, 1), "gafc_exp2_assign_pedal_wah_max");
    AppendTSL(temp.mid(addr15+72, 1), "gafc_exp2_assign_geq_min");
    AppendTSL(temp.mid(addr15+73, 1), "gafc_exp2_assign_geq_max");
    AppendTSL(temp.mid(addr15+74, 1), "gafc_exp2_assign_peq_min");
    AppendTSL(temp.mid(addr15+75, 1), "gafc_exp2_assign_peq_max");
    AppendTSL(temp.mid(addr15+76, 1), "gafc_exp2_assign_guitar_sim_min");
    AppendTSL(temp.mid(addr15+77, 1), "gafc_exp2_assign_guitar_sim_max");
    AppendTSL(temp.mid(addr15+78, 1), "gafc_exp2_assign_ac_guitar_sim_min");
    AppendTSL(temp.mid(addr15+79, 1), "gafc_exp2_assign_ac_guitar_sim_max");
    AppendTSL(temp.mid(addr15+80, 1), "gafc_exp2_assign_ac_processor_min");
    AppendTSL(temp.mid(addr15+81, 1), "gafc_exp2_assign_ac_processor_max");
    AppendTSL(temp.mid(addr15+82, 1), "gafc_exp2_assign_wave_synth_min");
    AppendTSL(temp.mid(addr15+83, 1), "gafc_exp2_assign_wave_synth_max");
    AppendTSL(temp.mid(addr15+84, 1), "gafc_exp2_assign_octave_min");
    AppendTSL(temp.mid(addr15+85, 1), "gafc_exp2_assign_octave_max");
    AppendTSL(temp.mid(addr15+86, 1), "gafc_exp2_assign_pitch_shifter_min_h");
    AppendTSL(temp.mid(addr15+87, 1), "gafc_exp2_assign_pitch_shifter_min_l");
    AppendTSL(temp.mid(addr15+88, 1), "gafc_exp2_assign_pitch_shifter_max_h");
    AppendTSL(temp.mid(addr15+89, 1), "gafc_exp2_assign_pitch_shifter_max_l");
    AppendTSL(temp.mid(addr15+90, 1), "gafc_exp2_assign_harmonist_min_h");
    AppendTSL(temp.mid(addr15+91, 1), "gafc_exp2_assign_harmonist_min_l");
    AppendTSL(temp.mid(addr15+92, 1), "gafc_exp2_assign_harmonist_max_h");
    AppendTSL(temp.mid(addr15+93, 1), "gafc_exp2_assign_harmonist_max_l");
    AppendTSL(temp.mid(addr15+94, 1), "gafc_exp2_assign_humanizer_min");
    AppendTSL(temp.mid(addr15+95, 1), "gafc_exp2_assign_humanizer_max");
    AppendTSL(temp.mid(addr15+96, 1), "gafc_exp2_assign_evh_phaser_min");
    AppendTSL(temp.mid(addr15+97, 1), "gafc_exp2_assign_evh_phaser_max");
    AppendTSL(temp.mid(addr15+98, 1), "gafc_exp2_assign_evh_flanger_min");
    AppendTSL(temp.mid(addr15+99, 1), "gafc_exp2_assign_evh_flanger_max");
    AppendTSL(temp.mid(addr15+100, 1), "gafc_exp2_assign_wah95e_min");
    AppendTSL(temp.mid(addr15+101, 1), "gafc_exp2_assign_wah95e_max");
    AppendTSL(temp.mid(addr15+102, 1), "gafc_exp2_assign_dc30_min_h");
    AppendTSL(temp.mid(addr15+103, 1), "gafc_exp2_assign_dc30_min_l");
    AppendTSL(temp.mid(addr15+104, 1), "gafc_exp2_assign_dc30_max_h");
    AppendTSL(temp.mid(addr15+105, 1), "gafc_exp2_assign_dc30_max_l");
    AppendTSL(temp.mid(addr15+106, 1), "gafc_exp2_assign_heavy_oct");
    AppendTSL(temp.mid(addr15+107, 1), "gafc_exp2_assign_heavy_oct_min");
    AppendTSL(temp.mid(addr15+108, 1), "gafc_exp2_assign_heavy_oct_max");


    TextTSL(temp.mid(11, 16).trimmed(), "patchname");  //copy patch name
    TextTSL(temp.mid(11, 16).trimmed(), "name");  //copy patch name

    QByteArray name("positionList");
    int start_index = this->TSL_default.indexOf(name)+(name.size()+3); //find pointer to start of Json value.
    QByteArray list;
    QByteArray posList;
    list.append(temp.mid(1030, 20));  // copy 20 bytes of chain data
    for(int x=0; x<list.size(); ++x)
    {
        bool ok;
        char a = list.at(x);
        QString val = QString::number(a, 16).toUpper();
        int value = val.toInt(&ok, 16);
        QByteArray v;
        v.setNum(value);
        posList.append(v);
        if(x<list.size()-1) {posList.append(","); };
    };
    this->TSL_default.replace(start_index, 49, posList); //replace positionList
    QByteArray note(temp.mid(1658, 32).trimmed());
    if(note.isEmpty()){note.append("by FxFloorBoard");};
    TextTSL(note, "note");  //copy text notes.

    MidiTable *midiTable = MidiTable::Instance();
    QString hex = QString::number(sysxIO->getSourceValue("Structure", "07", "00", "0F"), 16).toUpper();
    Midi items = midiTable->getMidiMap("Structure", "07", "00", "0F", "0"+hex);
    QByteArray txt;
    for(int x=0; x<items.desc.size(); x++ )
    { txt.append(items.desc.at(x));   };
    TextTSL(txt.toUpper(), "category");
    QString host(QSysInfo::machineHostName());
    txt.clear();
    for(int x=0; x<host.size(); x++ )
    { txt.append(host.at(x));   };
    TextTSL(txt, "id");

}

void sysxWriter::AppendTSL(QByteArray hex, const char* Json_name)
{
    bool ok;
    char a = hex.at(0);
    QString val = QString::number(a, 16).toUpper();
    if(hex.size()>1){ val = QString::number(a*128, 16).toUpper(); };
    char c = hex.at(1);
    QString val2 = QString::number(c, 16).toUpper();
    int value;
    if(hex.size()>1){ value = ((val2).toInt(&ok, 16)+(val).toInt(&ok, 16)); }
    else{ value = val.toInt(&ok, 16); };
    QByteArray name(Json_name);
    int start_index = this->TSL_default.indexOf(char(34)+name+char(34))+(name.size()+3); //find pointer to start of Json value.
    QByteArray b(":");
    unsigned int incr = 1;
LOOP:
    incr--;
    if((this->TSL_default.mid((start_index-1), 1).contains(b)) && (this->TSL_default.mid((start_index-name.size()-3), 1 ).contains(char(34))))
    {
        int end_index = this->TSL_default.indexOf(",", start_index)-start_index; //find pointer to end of value to get the size of the value.
        if(this->TSL_default.at(start_index+end_index-1)==char(125)) { end_index=end_index-1; }; // if find a } then back 2 steps
        QByteArray v;
        v.setNum(value);
        this->TSL_default.replace(start_index, end_index, v);
        incr=0;
    }else if (incr>0) {start_index = this->TSL_default.indexOf(char(34)+name+char(34), start_index)+(name.size()+3); goto LOOP; };
}

void sysxWriter::TextTSL(QByteArray hex, const char* Json_name)
{
    QByteArray name(Json_name);   // name of function to be searched for
    int start_index = this->TSL_default.indexOf(char(34)+name+char(34))+name.size()+4;   //find pointer to start of Json value after :".
    QByteArray b(":");
    int incr = 1;
    if(name == "name"){ incr = 2; }; // name listed twice.
    QByteArray null("null");
LOOP2:
    incr--;
    if(this->TSL_default.mid(start_index-2, 1) != b)  // check if correct name is found - a ": should be after the name.
    {
        start_index = this->TSL_default.indexOf(char(34)+name+char(34), start_index+name.size())+2;   // if not correct, move to the next name instance
    };
    if(this->TSL_default.mid(start_index-2, 1) == b)    // if name": is a match - find end of string field ",
    {

        int end_index = this->TSL_default.indexOf(",", start_index)-start_index-1; //find pointer to end of value to get the size of the value.
        if(name=="name"){end_index = this->TSL_default.indexOf(char(125), start_index)-start_index-1;}; // "name": only uses } after and not ,

        if(this->TSL_default.at(start_index+end_index-1)==char(125)) { end_index=end_index-1; }; // if find a } then back 2 steps.
        if(this->TSL_default.at(start_index+end_index-1)==char(93)) { end_index=end_index-2; }; // if find a ] then back 3 steps.

        if(this->TSL_default.mid(start_index-1, 4).contains(null) && hex.simplified().isEmpty()) //if no text and file contains null - do nothing.
        {

        }
        else if(hex.simplified().isEmpty()) // if no text and file probably has existing quoted text.
        {
            this->TSL_default.replace(start_index-1, end_index+1, "null");   //replace the old string with null.
        }
        else if(this->TSL_default.mid(start_index-1, 4).contains(null) && !hex.isEmpty())
        {
            this->TSL_default.replace(start_index-1, end_index+2, (char(34)+hex+char(34)));   //replace 'null' with the new string
        }
        else
        {
             this->TSL_default.replace(start_index, end_index, hex);   //replace the old string with the new string
        };
    };
    start_index = this->TSL_default.lastIndexOf(char(34)+name+char(34))+name.size()+4;
    if (incr>0) {goto LOOP2; };
}

void sysxWriter::TextTSL_mk2(QByteArray hex, const char* Json_name)
{
    QByteArray name(Json_name);   // name of function to be searched for
    int start_index = this->TSL_default.indexOf(char(34)+name+char(34))+name.size()+4;   //find pointer to start of Json value after :".
    QByteArray b(":");
    int incr = 1;
    QByteArray null("null");
LOOP3:
    incr--;
    if(this->TSL_default.mid(start_index-2, 1) != b)  // check if correct name is found - a ": should be after the name.
    {
        start_index = this->TSL_default.indexOf(char(34)+name+char(34), start_index+name.size())+2;   // if not correct, move to the next name instance
    };
    if(this->TSL_default.mid(start_index-2, 1) == b)    // if name": is a match - find end of string field ",
    {

        int end_index = this->TSL_default.indexOf(",", start_index)-start_index-1; //find pointer to end of value to get the size of the value.

        if(this->TSL_default.at(start_index+end_index-1)==char(125)) { end_index=end_index-1; }; // if find a } then back 2 steps.
        if(this->TSL_default.at(start_index+end_index-1)==char(93)) { end_index=end_index-2; }; // if find a ] then back 3 steps.

        if(this->TSL_default.mid(start_index-1, 4).contains(null) && hex.simplified().isEmpty()) //if no text and file contains null - do nothing.
        {

        }
        else if(hex.simplified().isEmpty()) // if no text and file probably has existing quoted text.
        {
            this->TSL_default.replace(start_index-1, end_index+1, "null");   //replace the old string with null.
        }
        else if(this->TSL_default.mid(start_index-1, 4).contains(null) && !hex.isEmpty())
        {
            this->TSL_default.replace(start_index-1, end_index+2, (char(34)+hex+char(34)));   //replace 'null' with the new string
        }
        else
        {
             this->TSL_default.replace(start_index, end_index, hex);   //replace the old string with the new string
        };
    };
    start_index = this->TSL_default.lastIndexOf(char(34)+name+char(34))+name.size()+4;
    if (incr>0) {goto LOOP3; };
}

SysxData sysxWriter::getFileSource()
{
    return fileSource;
}

SysxData sysxWriter::getSystemSource()
{
    return systemSource;
}

QString sysxWriter::getFileName()
{
    return fileName;
}

void sysxWriter::convertFromGT10()
{
    QByteArray GT10_default;
    QFile file(":gt10_default.syx");           // Read the default GT-10 sysx file .
    if (file.open(QIODevice::ReadOnly))
    {	GT10_default = file.readAll(); };

    if(headerType == "GT10_SYX"){         // if GT-10 syx patch file format

        QByteArray temp;
        data.remove(1618, 50);
        temp = data.mid(1605, 13);
        data.remove(1605, 13);
        data.insert(1620, temp);
        data.remove(1761, 49);

        QByteArray standard_data = data;
        QFile file(":default.syx");   // Read the default GT-10 sysx file so we don't start empty handed.
        if (file.open(QIODevice::ReadOnly))
        {	data = file.readAll(); };

        temp = data.mid(1763, 282);           // copy patch description from default.syx
        standard_data.append(temp);
        data = standard_data;

        translate10to100();
    }
    else if (headerType == "GT10_SMF")                      // SMF ******************************************************************
    {                                        // file contains a .mid type SMF patch file header from Boss Librarian
        QByteArray GT10_default;
        QFile file(":gt10_default.syx");              // Read the default GT-10 sysx file so we don't start empty handed.
        if (file.open(QIODevice::ReadOnly))
        {	GT10_default = file.readAll(); };
        QByteArray temp;                         // TRANSLATION of GT-10 SMF PATCHES, data read from smf patch *************

        index = 1;
        int patchCount = (data.size()-32)/1806;
        if (patchCount>1)
        {
            QString msgText;
            QString patchText;
            QString patchNumber;
            char r;
            this->patchList.clear();
            this->patchList.append(QObject::tr("Select Patch"));
            int a = 43;                          // locate patch text start position from the start of the file
            for (int h=0;h<patchCount;h++)
            {
                for (int b=0;b<16;b++)
                {
                    r = char(data[a+b]);
                    patchText.append(r);
                };
                patchNumber = QString::number(h+1, 10).toUpper();
                msgText.append(patchNumber + " : ");
                msgText.append(patchText + "   ");
                this->patchList.append(msgText);
                patchText.clear();
                msgText.clear();
                a=a+1806;                      // offset is set in front of marker
            };

            QString type = "smf";
            fileDialog *dialog = new fileDialog(fileName, patchList, data, GT10_default, type);
            dialog->exec();
            patchIndex(this->index);
        };

        int a=0;
        if (patchCount>1)
        {
            int q=index-1;      // find start of required patch
            a = q*1806;
        };
        //temp = data.mid(a+43, 128);            // copy SMF 128 bytes
        GT10_default.replace(11, 16, data.mid(a+43, 16));             // replace gt10 address "00"
        temp = data.mid(a+171, 114);           // copy SMF part1
        temp.append(data.mid(a+301,14));       // copy SMF part2
        GT10_default.replace(152, 128, temp);            // replace gt10 address "01"
        temp = data.mid(a+315, 128);           // copy SMF part1
        GT10_default.replace(293, 128, temp);            // replace gt10 address "02"
        temp = data.mid(a+443, 100);           // copy SMF part1
        temp.append(data.mid(a+559,28));       // copy SMF part2
        GT10_default.replace(434, 128, temp);            // replace gt10 address "03"
        temp = data.mid(a+587, 128);           // copy SMF part1
        GT10_default.replace(575, 128, temp);            // replace gt10 address "04"
        temp = data.mid(a+715, 86);            // copy SMF part1
        GT10_default.replace(716, 86, temp);             // replace gt10 address "05"
        temp = data.mid(a+859, 128);           // copy SMF part1
        GT10_default.replace(815,128, temp);             // replace gt10 address "06"
        temp = data.mid(a+987, 72);            // copy SMF part1
        temp.append(data.mid(a+1075,56));      // copy SMF part2
        GT10_default.replace(956, 128, temp);            // replace gt10 address "07"
        temp = data.mid(a+1131, 128);          // copy SMF part1
        GT10_default.replace(1097,128, temp);            // replace gt10 address "08"
        temp = data.mid(a+1259, 58);           // copy SMF part1
        temp.append(data.mid(a+1333,42));      // copy SMF part2
        GT10_default.replace(1238, 100, temp);           // replace gt10 address "09"
        temp = data.mid(a+1403, 128);          // copy SMF part1
        GT10_default.replace(1351,128, temp);            // replace gt10 address "0A"
        temp = data.mid(a+1531, 44);           // copy SMF part1
        temp.append(data.mid(a+1591,84));      // copy SMF part2
        GT10_default.replace(1492, 128, temp);           // replace gt10 address "0B"
        temp = data.mid(a+1675, 128);          // copy SMF part1
        GT10_default.replace(1633,128, temp);            // replace gt10 address "0C"
        if (index>0)
        {
            data = GT10_default;
            translate10to100();
        }
    }
    else if (headerType == "GT10_GXG")         // if the read file is a Boss Librarian type. ***************************************
    {
        index=1;
        char msb = char(data[34]);             // find patch count msb bit in GXG file at byte 34
        char lsb = char(data[35]);             // find patch count lsb bit in GXG file at byte 35
        bool ok;
        int patchCount;
        patchCount = (256*QString::number(msb, 16).toUpper().toInt(&ok, 16)) + (QString::number(lsb, 16).toUpper().toInt(&ok, 16));
        QByteArray marker;
        if (patchCount>1)
        {
            QString msgText;
            marker = data.mid(170, 2);      //copy marker key to find "06A5" which marks the start of each patch block
            QString patchText;
            QString patchNumber;
            this->patchList.clear();
            this->patchList.append(QObject::tr("Select Patch"));
            int a = data.indexOf(marker, 0); // locate patch start position from the start of the file
            a=a+2;                             // offset is set in front of marker
            for (int h=0;h<patchCount;h++)
            {
                for (int b=0;b<16;b++)
                {
                    char r = char(data[a+b]);
                    patchText.append(r);
                };
                patchNumber = QString::number(h+1, 10).toUpper();
                msgText.append(patchNumber + " : ");
                msgText.append(patchText + "   ");
                this->patchList.append(msgText);
                patchText.clear();
                msgText.clear();
                a = data.indexOf(marker, a); // locate patch start position from the start of the file
                a=a+2;                      // offset is set in front of marker
            };

            QString type = "gxg";
            fileDialog *dialog = new fileDialog(fileName, patchList, data, GT10_default, type);
            dialog->exec();
            patchIndex(this->index);
        };

        marker = data.mid(170, 2);                 //copy marker key to find "06A5" which marks the start of each patch block
        int a = data.indexOf(marker, 0);  // locate patch start position from the start of the file
        a=a+2;                                     // offset is set in front of marker
        if (patchCount>1)
        {
            int q=index-1;
            for (int h=0;h<q;h++)
            {
                a = data.indexOf(marker, a);          // locate patch start position from the start of the file
                a=a+2;
            };                                     // offset is set in front of marker
        };
        QByteArray temp;
        temp = data.mid(a, 128);
        GT10_default.replace(11, 128, temp);       //address "00" +
        temp = data.mid(a+128, 128);
        GT10_default.replace(152, 128, temp);      //address "01" +
        temp = data.mid(a+256, 128);
        GT10_default.replace(293, 128, temp);      //address "02" +
        temp = data.mid(a+384, 128);
        GT10_default.replace(434, 128, temp);      //address "03" +
        temp = data.mid(a+512, 128);
        GT10_default.replace(575, 128, temp);      //address "04" +
        temp = data.mid(a+640, 86);
        GT10_default.replace(716, 86, temp);       //address "05" +
        temp = data.mid(a+768, 128);
        GT10_default.replace(815, 128, temp);      //address "06" +
        temp = data.mid(a+896, 128);
        GT10_default.replace(956, 128, temp);      //address "07" +
        temp = data.mid(a+1024, 128);
        GT10_default.replace(1097, 128, temp);     //address "08" +
        temp = data.mid(a+1152, 100);
        GT10_default.replace(1238, 100, temp);     //address "09" +
        temp = data.mid(a+1280, 128);
        GT10_default.replace(1351, 128, temp);     //address "0A" +
        temp = data.mid(a+1408, 128);
        GT10_default.replace(1492, 128, temp);     //address "0B" +
        temp = data.mid(a+1536, 128);
        GT10_default.replace(1633, 128, temp);     //address "0C" +
        data = GT10_default;

        if (index>0)
        {
            translate10to100();
        };
    }
}

void sysxWriter::translate10to100()
{
    MidiTable *midiTable = MidiTable::Instance();
    QFile file(":default.syx");           // Read the default KATANA sysx file .
    if (file.open(QIODevice::ReadOnly))
    {GT100_default = file.readAll(); };

    GT100_default.replace(11, 16, data.mid(11, 16));   //copy name
    GT100_default.replace(27, 1, data.mid(28, 1));     //copy output select
    GT100_default.replace(43, 4, data.mid(75, 4));     //copy comp
    GT100_default.replace(47, 2, data.mid(81, 2));     //copy comp
    GT100_default.replace(59, 1, data.mid(123, 1));    //copy dist
    GT100_default.replace(60, 1, midiTable->getArrayValue("Tables", "00", "00", "00", data.mid(124, 1))); // convert type
    GT100_default.replace(61, 3, data.mid(125, 3));    //copy dist
    GT100_default.replace(64, 2, data.mid(130, 2));    //copy dist
    GT100_default.replace(66, 2, data.mid(128, 2));    //copy dist
    GT100_default.replace(68, 1, data.mid(132, 1));    //copy dist
    GT100_default.replace(69, 1, midiTable->getArrayValue("Tables", "00", "00", "01", data.mid(133, 1))); //convert custom dist
    GT100_default.replace(70, 1, midiTable->getArrayValue("Tables", "00", "00", "01", data.mid(134, 1))); //convert custom dist
    GT100_default.replace(71, 1, midiTable->getArrayValue("Tables", "00", "00", "01", data.mid(135, 1))); //convert custom dist
    GT100_default.replace(72, 1, midiTable->getArrayValue("Tables", "00", "00", "01", data.mid(136, 1))); //convert custom dist
    GT100_default.replace(91, 1, data.mid(152, 1));  //copy Preamp A sw
    GT100_default.replace(92, 1, midiTable->getArrayValue("Tables", "00", "00", "02", data.mid(168, 1))); //convert pre type
    GT100_default.replace(93, 1, data.mid(169, 1));    //copy pre A gain
    GT100_default.replace(95, 16, data.mid(170, 16));  //copy pre A
    GT100_default.replace(111, 1, midiTable->getArrayValue("Tables", "00", "00", "01", data.mid(186, 1))); //convert custom pre
    GT100_default.replace(112, 1, midiTable->getArrayValue("Tables", "00", "00", "01", data.mid(187, 1))); //convert custom pre
    GT100_default.replace(115, 1, midiTable->getArrayValue("Tables", "00", "00", "01", data.mid(190, 1))); //convert custom pre
    GT100_default.replace(116, 1, midiTable->getArrayValue("Tables", "00", "00", "01", data.mid(191, 1))); //convert custom pre
    GT100_default.replace(118, 5, data.mid(192, 5));  //copy custom spkr A
    GT100_default.replace(152, 1, data.mid(152, 1));  //copy Preamp B sw
    GT100_default.replace(153, 1, midiTable->getArrayValue("Tables", "00", "00", "02", data.mid(200, 1))); //convert pre type
    GT100_default.replace(154, 1, data.mid(201, 1));    //copy pre B gain
    GT100_default.replace(156, 16, data.mid(202, 16));  //copy pre B
    GT100_default.replace(172, 1, midiTable->getArrayValue("Tables", "00", "00", "01", data.mid(218, 1))); //convert custom pre
    GT100_default.replace(173, 1, midiTable->getArrayValue("Tables", "00", "00", "01", data.mid(219, 1))); //convert custom pre
    GT100_default.replace(176, 1, midiTable->getArrayValue("Tables", "00", "00", "01", data.mid(222, 1))); //convert custom pre
    GT100_default.replace(177, 1, midiTable->getArrayValue("Tables", "00", "00", "01", data.mid(223, 1))); //convert custom pre
    GT100_default.replace(179, 5, data.mid(224, 5));  //copy custom spkr B
    GT100_default.replace(200, 12, data.mid(264, 12));  //copy EQ
    GT100_default.replace(201, 1, midiTable->getArrayValue("Tables", "00", "00", "03", data.mid(265, 1))); //convert lo cut
    GT100_default.replace(210, 1, midiTable->getArrayValue("Tables", "00", "00", "04", data.mid(274, 1))); //convert hi cut
    GT100_default.replace(216, 1, data.mid(293, 1));  //copy FX1 sw
    GT100_default.replace(217, 1, midiTable->getArrayValue("Tables", "00", "00", "05", data.mid(294, 1))); //convert FX type
    GT100_default.replace(251, 5, data.mid(295, 5));  //copy ACS
    GT100_default.replace(257, 6, data.mid(300, 6));  //copy ALM
    GT100_default.replace(228, 7, data.mid(306, 7));  //copy TW
    GT100_default.replace(236, 7, data.mid(313, 7));  //copy AW
    GT100_default.replace(441, 3, data.mid(320, 3));  //copy TR
    GT100_default.replace(410, 8, data.mid(323, 8));  //copy PH
    GT100_default.replace(419, 2, data.mid(331, 2));  //copy FL
    GT100_default.replace(434, 6, data.mid(333, 6));  //copy FL
    GT100_default.replace(458, 5, data.mid(339, 5));  //copy PAN
    GT100_default.replace(471, 4, data.mid(344, 4));  //copy VB
    GT100_default.replace(454, 3, data.mid(348, 3));  //copy UV
    GT100_default.replace(477, 4, data.mid(351, 4));  //copy RM
    GT100_default.replace(313, 2, data.mid(355, 2));  //copy SG
    GT100_default.replace(482, 8, data.mid(370, 8));  //copy HU
    GT100_default.replace(465, 3, data.mid(378, 3));  //copy SL
    GT100_default.replace(276, 1, midiTable->getArrayValue("Tables", "00", "00", "03", data.mid(381, 1))); //convert lo cut
    GT100_default.replace(277, 3, data.mid(382, 3));  //copy SEQ
    GT100_default.replace(293, 5, data.mid(385, 5));  //copy SEQ
    GT100_default.replace(298, 1, midiTable->getArrayValue("Tables", "00", "00", "04", data.mid(390, 1))); //convert hi cut
    GT100_default.replace(299, 1, data.mid(391, 1));  //copy SEQ
    GT100_default.replace(362, 29, data.mid(392, 29));//copy HR
    GT100_default.replace(391, 6, data.mid(434, 6));  //copy HR
    GT100_default.replace(346, 15, data.mid(440, 15));//copy PS
    GT100_default.replace(342, 3, data.mid(455, 3));  //copy OC
    GT100_default.replace(446, 6, data.mid(458, 6));  //copy RT
    GT100_default.replace(491, 9, data.mid(464, 9));  //copy 2CE
    GT100_default.replace(504, 4, data.mid(475, 4));  //copy SDD
    GT100_default.replace(502, 2, data.mid(473, 2));  //copy SDD
    GT100_default.replace(505, 1, midiTable->getArrayValue("Tables", "00", "00", "04", data.mid(476, 1))); //convert hi cut
    GT100_default.replace(317, 7, data.mid(479, 7));  //copy DF
    GT100_default.replace(334, 7, data.mid(486, 7));  //copy SITAR
    GT100_default.replace(325, 8, data.mid(493, 8));  //copy WSY
    GT100_default.replace(398, 3, data.mid(744, 3));  //copy SH
    GT100_default.replace(301, 5, data.mid(747, 5));  //copy TM
    GT100_default.replace(307, 5, data.mid(752, 5));  //copy GS
    GT100_default.replace(402, 7, data.mid(757, 7));  //copy AC
    GT100_default.replace(244, 6, data.mid(764, 6));  //copy SWAH
    GT100_default.replace(264, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(771, 1))); //convert GEQ
    GT100_default.replace(265, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(772, 1))); //convert GEQ
    GT100_default.replace(266, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(773, 1))); //convert GEQ
    GT100_default.replace(267, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(774, 1))); //convert GEQ
    GT100_default.replace(268, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(775, 1))); //convert GEQ
    GT100_default.replace(269, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(776, 1))); //convert GEQ
    GT100_default.replace(270, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(777, 1))); //convert GEQ
    GT100_default.replace(271, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(778, 1))); //convert GEQ
    GT100_default.replace(272, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(779, 1))); //convert GEQ
    GT100_default.replace(273, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(780, 1))); //convert GEQ
    GT100_default.replace(274, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(770, 1))); //convert GEQ
    GT100_default.replace(510, 1, data.mid(815, 1));  //copy FX2 sw
    GT100_default.replace(511, 1, midiTable->getArrayValue("Tables", "00", "00", "05", data.mid(816, 1))); //convert FX type
    GT100_default.replace(545, 5, data.mid(817, 5));  //copy ACS
    GT100_default.replace(551, 6, data.mid(822, 6));  //copy ALM
    GT100_default.replace(522, 7, data.mid(828, 7));  //copy TW
    GT100_default.replace(530, 7, data.mid(835, 7));  //copy AW
    GT100_default.replace(735, 3, data.mid(842, 3));  //copy TR
    GT100_default.replace(716, 8, data.mid(845, 8));  //copy PH
    GT100_default.replace(726, 8, data.mid(853, 8));  //copy FL
    GT100_default.replace(752, 5, data.mid(861, 5));  //copy PAN
    GT100_default.replace(765, 4, data.mid(866, 4));  //copy VB
    GT100_default.replace(748, 3, data.mid(870, 3));  //copy UV
    GT100_default.replace(771, 4, data.mid(873, 4));  //copy RM
    GT100_default.replace(607, 2, data.mid(877, 2));  //copy SG
    GT100_default.replace(776, 8, data.mid(892, 8));  //copy HU
    GT100_default.replace(759, 3, data.mid(900, 3));  //copy SL
    GT100_default.replace(583, 1, midiTable->getArrayValue("Tables", "00", "00", "03", data.mid(903, 1))); //convert lo cut
    GT100_default.replace(584, 8, data.mid(904, 8));  //copy SEQ
    GT100_default.replace(592, 1, midiTable->getArrayValue("Tables", "00", "00", "04", data.mid(912, 1))); //convert hi cut
    GT100_default.replace(593, 1, data.mid(913, 1));  //copy SEQ
    GT100_default.replace(656, 29, data.mid(914, 29));//copy HR
    GT100_default.replace(685, 6, data.mid(956, 6));  //copy HR
    GT100_default.replace(640, 15, data.mid(962, 15));//copy PS
    GT100_default.replace(636, 3, data.mid(977, 3));  //copy OC
    GT100_default.replace(740, 6, data.mid(980, 6));  //copy RT
    GT100_default.replace(785, 9, data.mid(986, 9));  //copy 2CE
    GT100_default.replace(796, 6, data.mid(995, 6));  //copy SDD
    GT100_default.replace(799, 1, midiTable->getArrayValue("Tables", "00", "00", "04", data.mid(998, 1))); //convert hi cut
    GT100_default.replace(611, 7, data.mid(1001, 7));  //copy DF
    GT100_default.replace(628, 7, data.mid(1008, 7));  //copy SITAR
    GT100_default.replace(619, 8, data.mid(1015, 8));  //copy WSY
    GT100_default.replace(692, 3, data.mid(1266, 3));  //copy SH
    GT100_default.replace(595, 5, data.mid(1269, 5));  //copy TM
    GT100_default.replace(601, 5, data.mid(1274, 5));  //copy GS
    GT100_default.replace(696, 7, data.mid(1279, 7));  //copy AC
    GT100_default.replace(538, 6, data.mid(1286, 6));  //copy SWAH
    GT100_default.replace(558, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1293, 1))); //convert GEQ
    GT100_default.replace(559, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1294, 1))); //convert GEQ
    GT100_default.replace(560, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1295, 1))); //convert GEQ
    GT100_default.replace(561, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1296, 1))); //convert GEQ
    GT100_default.replace(575, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1297, 1))); //convert GEQ
    GT100_default.replace(576, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1298, 1))); //convert GEQ
    GT100_default.replace(577, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1299, 1))); //convert GEQ
    GT100_default.replace(578, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1300, 1))); //convert GEQ
    GT100_default.replace(579, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1301, 1))); //convert GEQ
    GT100_default.replace(580, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1302, 1))); //convert GEQ
    GT100_default.replace(581, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1292, 1))); //convert GEQ
    GT100_default.replace(812, 1, data.mid(1351, 1));  //copy DD sw
    GT100_default.replace(813, 1, midiTable->getArrayValue("Tables", "00", "00", "07", data.mid(1352, 1))); //convert DD type
    int d = ArrayToInt(data.mid(1353, 2));    //copy DD time
    if(d>2000 && d<3400){d=d/2; };
    if(d>3400){d=d-1400; };
    GT100_default.replace(814, 2, IntToArray(d));

    GT100_default.replace(820, 1, data.mid(1355, 1));  //copy DD tap
    GT100_default.replace(816, 1, data.mid(1356, 1));  //copy DD fb
    GT100_default.replace(817, 1, midiTable->getArrayValue("Tables", "00", "00", "04", data.mid(1357, 1))); //convert hi cut
    d = ArrayToInt(data.mid(1358, 2));   //copy DD
    if(d>1000 && d<1700){d=d/2; };
    if(d>1700 && d<1707){d=d-700; };
    if(d>1706){d=1007; };
    GT100_default.replace(821, 2, IntToArray(d));
    GT100_default.replace(823, 3, data.mid(1360, 3));  //copy dual DD 1
    d = ArrayToInt(data.mid(1363, 2));
    if(d>1000 && d<1700){d=d/2; };
    if(d>1700 && d<1707){d=d-700; };
    if(d>1706){d=1007; };
    GT100_default.replace(826, 2, IntToArray(d));
    GT100_default.replace(828, 3, data.mid(1365, 3));  //copy dual DD 2
    GT100_default.replace(824, 1, midiTable->getArrayValue("Tables", "00", "00", "04", data.mid(1361, 1))); //convert D1 hi cut
    GT100_default.replace(829, 1, midiTable->getArrayValue("Tables", "00", "00", "04", data.mid(1366, 1))); //convert D2 hi cut
    GT100_default.replace(831, 2, data.mid(1372, 2));  //copy DD mod
    GT100_default.replace(818, 2, data.mid(1374, 2));  //copy DD level
    GT100_default.replace(857, 5, data.mid(1383, 5));  //copy CE
    GT100_default.replace(862, 1, midiTable->getArrayValue("Tables", "00", "00", "03", data.mid(1388, 1))); //convert lo cut
    GT100_default.replace(863, 1, midiTable->getArrayValue("Tables", "00", "00", "04", data.mid(1389, 1))); //convert hi cut
    GT100_default.replace(864, 1, data.mid(1390, 1));  //copy CE
    GT100_default.replace(873, 3, data.mid(1399, 3));  //copy RV
    GT100_default.replace(878, 1, midiTable->getArrayValue("Tables", "00", "00", "03", data.mid(1403, 1))); //convert lo cut
    GT100_default.replace(879, 1, midiTable->getArrayValue("Tables", "00", "00", "04", data.mid(1404, 1))); //convert hi cut
    GT100_default.replace(880, 4, data.mid(1405, 4));  //copy RV
    GT100_default.replace(876, 2, data.mid(1409, 2));  //copy RV pre delay
    GT100_default.replace(889, 1, data.mid(1415, 1));  //copy PFX sw
    GT100_default.replace(1155, 1, data.mid(1420, 1));  //copy PFX mode
    GT100_default.replace(1094, 1, midiTable->getArrayValue("Tables", "00", "00", "08", data.mid(1421, 1))); //convert EXP sw func
    GT100_default.replace(1078, 1, midiTable->getArrayValue("Tables", "00", "00", "09", data.mid(1422, 1))); //convert ctl 1 func
    GT100_default.replace(1110, 1, midiTable->getArrayValue("Tables", "00", "00", "09", data.mid(1423, 1))); //convert ctl 2 func
    GT100_default.replace(895, 6, data.mid(1424, 6));  //copy PFX WAH
    GT100_default.replace(891, 4, data.mid(1436, 4));  //copy PFX PB
    GT100_default.replace(908, 1, data.mid(1441, 1));  //copy PFX FV
    GT100_default.replace(906, 2, data.mid(1442, 2));  //copy PFX FV
    GT100_default.replace(905, 1, data.mid(1444, 1));  //copy PFX FV
    GT100_default.replace(1014, 1, data.mid(1447, 1));  //copy MST
    GT100_default.replace(1015, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1448, 1))); //convert MEQ
    GT100_default.replace(1018, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1449, 1))); //convert MEQ
    GT100_default.replace(1017, 1, data.mid(1450, 1));  //copy MST
    GT100_default.replace(1016, 1, data.mid(1451, 1));  //copy MST
    GT100_default.replace(1019, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1452, 1))); //convert MEQ
    GT100_default.replace(1020, 3, data.mid(1453, 3));  //copy MST bpm
    GT100_default.replace(953, 1, data.mid(1456, 1));  //copy AMP sw
    GT100_default.replace(956, 4, data.mid(1464, 4));  //copy NS1
    GT100_default.replace(961, 4, data.mid(1468, 4));  //copy NS2
    GT100_default.replace(942, 4, data.mid(1472, 4));  //copy SR loop

    QByteArray chain(data.mid(1492, 18)); //copy gt10 chain
    QString chn;
    QString list;
    for(int x=0; x<18; ++x)
    {
        char r = chain.at(x);
        QString val = QString::number(r, 16).toUpper();
        if(val.size()<2){val.prepend("0"); };
        chn.append(val);
        list.append(chn.mid(x*2, 1)); // make list of 10's units of chain items, channel b are prepended with 4
    };
    int pos = list.indexOf("4"); //find position of channel b start

    chain.replace(0, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(0, 1))); //convert chain
    chain.replace(1, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(1, 1))); //convert chain
    chain.replace(2, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(2, 1))); //convert chain
    chain.replace(3, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(3, 1))); //convert chain
    chain.replace(4, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(4, 1))); //convert chain
    chain.replace(5, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(5, 1))); //convert chain
    chain.replace(6, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(6, 1))); //convert chain
    chain.replace(7, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(7, 1))); //convert chain
    chain.replace(8, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(8, 1))); //convert chain
    chain.replace(9, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(9, 1))); //convert chain
    chain.replace(10, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(10, 1))); //convert chain
    chain.replace(11, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(11, 1))); //convert chain
    chain.replace(12, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(12, 1))); //convert chain
    chain.replace(13, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(13, 1))); //convert chain
    chain.replace(14, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(14, 1))); //convert chain
    chain.replace(15, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(15, 1))); //convert chain
    chain.replace(16, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(16, 1))); //convert chain
    chain.replace(17, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(17, 1))); //convert chain
    chain.insert(17, char(10));
    chain.insert(pos, char(18));
    GT100_default.replace(1030, 20, chain);

    GT100_default.replace(1187, 1, data.mid(1524, 1));  //copy Assign 1
    GT100_default.replace(1188, 2, midiTable->getArrayValue("Tables", "00", "00", "0B", data.mid(1525, 2))); //convert target
    GT100_default.replace(1190, 4, data.mid(1527, 4));  //copy min/max
    GT100_default.replace(1194, 1, midiTable->getArrayValue("Tables", "00", "00", "0C", data.mid(1531, 1))); //convert source
    GT100_default.replace(1195, 3, data.mid(1532, 3));  //copy Assign
    GT100_default.replace(1198, 1, midiTable->getArrayValue("Tables", "00", "00", "0D", data.mid(1535, 1))); //convert int pedal
    GT100_default.replace(1199, 4, data.mid(1536, 4));  //copy Assign
    GT100_default.replace(1219, 1, data.mid(1540, 1));  //copy Assign 2
    GT100_default.replace(1220, 2, midiTable->getArrayValue("Tables", "00", "00", "0B", data.mid(1541, 2))); //convert target
    GT100_default.replace(1222, 4, data.mid(1543, 4));  //copy min/max
    GT100_default.replace(1226, 1, midiTable->getArrayValue("Tables", "00", "00", "0C", data.mid(1547, 1))); //convert source
    GT100_default.replace(1227, 3, data.mid(1548, 3));  //copy Assign
    GT100_default.replace(1230, 1, midiTable->getArrayValue("Tables", "00", "00", "0D", data.mid(1551, 1))); //convert int pedal
    GT100_default.replace(1231, 4, data.mid(1552, 4));  //copy Assign
    GT100_default.replace(1251, 1, data.mid(1556, 1));  //copy Assign 3
    GT100_default.replace(1252, 2, midiTable->getArrayValue("Tables", "00", "00", "0B", data.mid(1557, 2))); //convert target
    GT100_default.replace(1254, 4, data.mid(1559, 4));  //copy min/max
    GT100_default.replace(1258, 1, midiTable->getArrayValue("Tables", "00", "00", "0C", data.mid(1563, 1))); //convert source
    GT100_default.replace(1259, 3, data.mid(1564, 3));  //copy Assign
    GT100_default.replace(1262, 1, midiTable->getArrayValue("Tables", "00", "00", "0D", data.mid(1567, 1))); //convert int pedal
    GT100_default.replace(1263, 4, data.mid(1568, 4));  //copy Assign
    GT100_default.replace(1296, 1, data.mid(1572, 1));  //copy Assign 4
    GT100_default.replace(1297, 2, midiTable->getArrayValue("Tables", "00", "00", "0B", data.mid(1573, 2))); //convert target
    GT100_default.replace(1299, 4, data.mid(1575, 4));  //copy min/max
    GT100_default.replace(1303, 1, midiTable->getArrayValue("Tables", "00", "00", "0C", data.mid(1579, 1))); //convert source
    GT100_default.replace(1304, 3, data.mid(1580, 3));  //copy Assign
    GT100_default.replace(1307, 1, midiTable->getArrayValue("Tables", "00", "00", "0D", data.mid(1583, 1))); //convert int pedal
    GT100_default.replace(1308, 4, data.mid(1584, 4));  //copy Assign
    GT100_default.replace(1328, 1, data.mid(1588, 1));  //copy Assign 5
    GT100_default.replace(1329, 2, midiTable->getArrayValue("Tables", "00", "00", "0B", data.mid(1589, 2))); //convert target
    GT100_default.replace(1331, 4, data.mid(1591, 4));  //copy min/max
    GT100_default.replace(1335, 1, midiTable->getArrayValue("Tables", "00", "00", "0C", data.mid(1595, 1))); //convert source
    GT100_default.replace(1336, 3, data.mid(1596, 3));  //copy Assign
    GT100_default.replace(1339, 1, midiTable->getArrayValue("Tables", "00", "00", "0D", data.mid(1599, 1))); //convert int pedal
    GT100_default.replace(1340, 4, data.mid(1600, 4));  //copy Assign
    GT100_default.replace(1360, 1, data.mid(1604, 1));  //copy Assign 6
    GT100_default.replace(1361, 2, midiTable->getArrayValue("Tables", "00", "00", "0B", data.mid(1605, 2))); //convert target
    GT100_default.replace(1363, 4, data.mid(1607, 4));  //copy min/max
    GT100_default.replace(1367, 1, midiTable->getArrayValue("Tables", "00", "00", "0C", data.mid(1611, 1))); //convert source
    GT100_default.replace(1368, 3, data.mid(1612, 3));  //copy Assign
    GT100_default.replace(1371, 1, midiTable->getArrayValue("Tables", "00", "00", "0D", data.mid(1615, 1))); //convert int pedal
    GT100_default.replace(1372, 4, data.mid(1616, 4));  //copy Assign
    GT100_default.replace(1392, 1, data.mid(1633, 1));  //copy Assign 7
    GT100_default.replace(1393, 2, midiTable->getArrayValue("Tables", "00", "00", "0B", data.mid(1634, 2))); //convert target
    GT100_default.replace(1395, 4, data.mid(1636, 4));  //copy min/max
    GT100_default.replace(1399, 1, midiTable->getArrayValue("Tables", "00", "00", "0C", data.mid(1640, 1))); //convert source
    GT100_default.replace(1400, 3, data.mid(1641, 3));  //copy Assign
    GT100_default.replace(1403, 1, midiTable->getArrayValue("Tables", "00", "00", "0D", data.mid(1644, 1))); //convert int pedal
    GT100_default.replace(1404, 4, data.mid(1645, 4));  //copy Assign
    GT100_default.replace(1437, 1, data.mid(1649, 1));  //copy Assign 8
    GT100_default.replace(1438, 2, midiTable->getArrayValue("Tables", "00", "00", "0B", data.mid(1650, 2))); //convert target
    GT100_default.replace(1440, 4, data.mid(1652, 4));  //copy min/max
    GT100_default.replace(1444, 1, midiTable->getArrayValue("Tables", "00", "00", "0C", data.mid(1656, 1))); //convert source
    GT100_default.replace(1445, 3, data.mid(1657, 3));  //copy Assign
    GT100_default.replace(1448, 1, midiTable->getArrayValue("Tables", "00", "00", "0D", data.mid(1660, 1))); //convert int pedal
    GT100_default.replace(1449, 4, data.mid(1661, 4));  //copy Assign
    GT100_default.replace(1469, 1, data.mid(1665, 1));  //copy input sensitivity
    GT100_default.replace(922, 1, data.mid(154, 1));  //copy Channel A/B select
    GT100_default.replace(939, 1, data.mid(155, 1));  //copy Channel Delay
    GT100_default.replace(924, 1, data.mid(156, 1));  //copy Channel A dynamic
    GT100_default.replace(928, 1, data.mid(156, 1));  //copy Channel B dynamic
    GT100_default.replace(921, 1, midiTable->getArrayValue("Tables", "00", "00", "0E", data.mid(153, 1))); //convert CH mode
    GT100_default.replace(923, 1, midiTable->getArrayValue("Tables", "00", "00", "0F", data.mid(153, 1))); //convert dynamic A
    GT100_default.replace(927, 1, midiTable->getArrayValue("Tables", "00", "00", "10", data.mid(153, 1))); //convert dynamic B

    SysxIO *sysxIO = SysxIO::Instance();
    sysxIO->setFileSource("Structure", GT100_default);
    sysxIO->setFileName(this->fileName);
    this->fileSource = sysxIO->getFileSource();
}

void sysxWriter::convertFromGT8()
{
    if (data.size() == 1010 || data.size() == 1241){      // if the file size = gt-8 patch

        QByteArray gt8_data = data;
        QFile file(":gt10_default.syx");   // Read the default GT-10 sysx file so we don't start empty handed.
        if (file.open(QIODevice::ReadOnly))
        {	data = file.readAll(); };
        char r;
        QByteArray temp;                         // TRANSLATION of GT-8 PATCHES, data read from gt8 patch **************
        QByteArray Qhex;                            // and used to replace gt10 patch data*********************************
        QFile hexfile(":HexLookupTable.hex");   // use a QByteArray of hex numbers from a lookup table.
        if (hexfile.open(QIODevice::ReadOnly))
        {	Qhex = hexfile.readAll(); };
        temp = gt8_data.mid(692, 16);           // copy gt8 name
        data.replace(11, 16, temp);              // replace gt10 name
        temp = gt8_data.mid(154, 1);             // copy gt8 compressor part1
        temp.append(gt8_data.mid(156,7));        // copy gt8 compressor part2
        data.replace(75, 8, temp);               // replace gt10 compressor
        temp = gt8_data.mid(212, 1);             // copy gt8 dist/od part1
        data.replace(123, 1, temp);              // replace gt10 distortion part1
        r = gt8_data.at(214);
        temp = Qhex.mid((180+r), 1);             // convert DISTORTION types from HexLookupTable address 180->
        temp.append(gt8_data.mid(215,5));        // copy gt8 dist/od part2
        data.replace(124, 6, temp);              // replace gt10 distortion


        temp = gt8_data.mid(233, 1);             // copy gt8 preAmp control part1
        temp.append(gt8_data.mid(235,2));        // copy gt8 preAmp control part2
        temp.append(gt8_data.mid(238,1));        // copy gt8 preAmp control part3
        temp.append(gt8_data.mid(237,1));        // copy gt8 preAmp control part4
        data.replace(152, 5, temp);              // replace gt10 preAmp control
        r = gt8_data.at(239);
        temp = Qhex.mid((224+r), 1);             // convert PRE_AMP types from HexLookupTable 224->
        temp.append(gt8_data.mid(240, 16));      // copy gt8 preAmp channel A
        data.replace(168, 17, temp);             // replace gt10 preAmp channel A
        r = gt8_data.at(256);
        temp = Qhex.mid((224+r), 1);             // convert PRE_AMP types from HexLookupTable 224->
        temp.append(gt8_data.mid(257, 16));      // copy gt8 preAmp channel B
        data.replace(200, 17, temp);             // replace gt10 preAmp channel B
        temp = gt8_data.mid(286, 1);             // copy gt8 EQ part1
        temp.append(gt8_data.mid(288,11));       // copy gt8 EQ part2
        data.replace(264, 12, temp);             // replace gt10 EQ


        temp = gt8_data.mid(545, 1);            // copy gt8 delay part1
        temp.append(gt8_data.mid(547,16));      // copy gt8 delay part2
        temp.append(gt8_data.mid(565,4));       // copy gt8 delay part3
        temp.append(gt8_data.mid(563,2));       // copy gt8 delay part4
        temp.append(gt8_data.mid(569,2));       // copy gt8 delay part5
        if (temp.mid(2,1)=="11"){temp.replace(2, 1, "00"); }; // gt10 don't have hold function in delay
        data.replace(1351, 25, temp);           // replace gt10 delay
        temp = gt8_data.mid(584, 1);            // copy gt8 chorus part1
        temp.append(gt8_data.mid(586,7));       // copy gt8 chorus part2
        data.replace(1383, 8, temp);            // replace gt10 chorus
        temp = gt8_data.mid(606, 1);            // copy gt8 reverb part1
        temp.append(gt8_data.mid(608,3));       // copy gt8 reverb part2
        temp.append(gt8_data.mid(611,5));       // copy gt8 reverb part3
        data.replace(1399, 9, temp);            // replace gt10 reverb
        temp = (gt8_data.mid(610,1));           // copy gt8 reverb part4
        data.replace(1410, 1, temp);            // replace gt10 reverb


        temp.clear();                            // Convert GT8 Chain items to GT10 format
        r = gt8_data.at(665);
        temp.append(Qhex.mid(r+164, 1));
        r = gt8_data.at(666);
        temp.append(Qhex.mid(r+164, 1));
        r = gt8_data.at(667);
        temp.append(Qhex.mid(r+164, 1));
        r = gt8_data.at(668);
        temp.append(Qhex.mid(r+164, 1));
        r = gt8_data.at(669);
        temp.append(Qhex.mid(r+164, 1));
        r = gt8_data.at(670);
        temp.append(Qhex.mid(r+164, 1));
        r = gt8_data.at(671);
        temp.append(Qhex.mid(r+164, 1));
        r = gt8_data.at(672);
        temp.append(Qhex.mid(r+164, 1));
        r = gt8_data.at(673);
        temp.append(Qhex.mid(r+164, 1));
        r = gt8_data.at(674);
        temp.append(Qhex.mid(r+164, 1));
        r = gt8_data.at(675);
        temp.append(Qhex.mid(r+164, 1));
        r = gt8_data.at(676);
        temp.append(Qhex.mid(r+164, 1));
        r = gt8_data.at(677);
        temp.append(Qhex.mid(r+164, 1));
        r = gt8_data.at(678);
        temp.append(Qhex.mid(r+164, 1));

        int a = temp.indexOf(Qhex.mid(4,1)); // locate gt10_preamp 1 **** find "04"
        temp.insert(a-1, Qhex.mid(16, 1));       //insert gt10_split before preamp 1 "10"
        temp.insert(a+1, Qhex.mid(17, 1));       // insert gt10_merge after preamp 1  "11"
        temp.insert(a+1, Qhex.mid(77, 1));     // insert NS_2 "4D"  77
        temp.insert(a+1, Qhex.mid(67, 1));       // insert channel B "43" 67
        data.replace(1492, 18, temp);            // replace gt10 chain


        temp = gt8_data.mid(194, 1);             // copy gt8 Loop part1
        temp.append(gt8_data.mid(196,3));        // copy gt8 Loop part2
        data.replace(1472, 4, temp);             // replace gt10 Loop (Send/Return))

        temp = gt8_data.mid(629, 4);             // copy gt8 NS
        data.replace(1464, 4, temp);             // replace gt10 NS_1
        data.replace(1468, 4, temp);             // replace gt10 NS_2

        temp = gt8_data.mid(176, 1);             // copy gt8 wah on/off
        data.replace(1415, 1, temp);             // replace gt10 pedal fx on/off
        r = gt8_data.at(737);
        temp = Qhex.mid((r+1), 1);               // copy gt8 ExSw Func
        data.replace(1421, 1, temp);             // replace gt10 ExSw 1 Func + 1
        r = gt8_data.at(722);
        temp = Qhex.mid((r+1), 1);               // copy gt8 CTL Func
        data.replace(1422, 1, temp);             // replace gt10 CTL 1 Func + 1
        temp = gt8_data.mid(178, 2);             // copy gt8 wah
        data.replace(1424, 2, temp);             // replace gt10 wah
        temp = gt8_data.mid(180, 1);             // copy gt8 wah level
        data.replace(1428, 1, temp);             // replace gt10 wah
        // todo replace direct level with effect inverted (optional).

        temp = gt8_data.mid(469, 5);             // copy gt8 pedal bend (fx2)
        data.replace(1435, 5, temp);             // replace gt10 pedal bend

        temp = gt8_data.mid(636, 1);             // copy gt8 FV level
        data.replace(1441, 1, temp);             // replace gt10 FV level
        temp = gt8_data.mid(637, 1);             // copy gt8 FV vol curve
        data.replace(1444, 1, temp);             // replace gt10 FV vol curve
        temp = gt8_data.mid(752, 2);             // copy gt8 FV vol min/max
        data.replace(1442, 2, temp);             // replace gt10 FV vol min/max

        temp = gt8_data.mid(651, 1);             // copy gt8 Amp control
        data.replace(1456, 1, temp);             // replace gt10 Amp control

        temp = gt8_data.mid(633, 1);             // copy gt8 Master patch level
        data.replace(1447, 1, temp);             // replace gt10 Master patch level
        temp = gt8_data.mid(634, 2);             // copy gt8 Master BPM
        data.replace(1454, 2, temp);             // replace gt10 Master BPM

        temp = gt8_data.mid(11, 1);              // copy gt8 FX1 on/off
        data.replace(293, 1, temp);              // replace gt10 FX1 on/off
        r = gt8_data.at(13);
        temp = Qhex.mid((128+r), 1);       // convert FX1 Type: selection items from lookup table Qhex (HexLookupTable.hex file) from 128 to 144
        temp.append(gt8_data.mid(14, 25));       // copy gt8 FX1 part2
        data.replace(294, 26, temp);             // replace gt10 FX1 part2
        temp = gt8_data.mid(49, 19);              // copy gt8 FX1 part3
        data.replace(320, 19, temp);             // replace gt10 FX1 part3
        temp = gt8_data.mid(68, 16);              // copy gt8 FX1 part4
        data.replace(341, 16, temp);             // replace gt10 FX1 part4
        temp = gt8_data.mid(98, 24);              // copy gt8 FX1 part5
        data.replace(357, 24, temp);             // replace gt10 FX1 part5
        temp = gt8_data.mid(130, 11);              // copy gt8 FX1 part6
        data.replace(381, 11, temp);              // replace gt10 FX1 part6
        temp = gt8_data.mid(84, 14);              // copy gt8 FX1 part7
        data.replace(479, 14, temp);             // replace gt10 FX1 part7
        temp = gt8_data.mid(122, 8);              // copy gt8 FX1 part8
        data.replace(493, 8, temp);              // replace gt10 FX1 part8
        temp = gt8_data.mid(39, 10);              // copy gt8 FX1 part9
        data.replace(747, 10, temp);               // replace gt10 FX1 part9


        temp = gt8_data.mid(312, 1);              // copy gt8 FX2 on/off
        data.replace(815, 1, temp);              // replace gt10 FX2 on/off
        r = gt8_data.at(314);
        temp = Qhex.mid((128+r), 1);       // convert FX2 Type: selection items from lookup table Qhex (HexLookupTable.hex file) from 128 to 144
        temp.append(gt8_data.mid(315, 25));       // copy gt8 FX2 part2
        data.replace(816, 26, temp);             // replace gt10 FX2 part2
        temp = gt8_data.mid(350, 19);              // copy gt8 FX2 part3
        data.replace(842, 19, temp);             // replace gt10 FX2 part3
        temp = gt8_data.mid(369, 16);              // copy gt8 FX2 part4
        data.replace(863, 16, temp);             // replace gt10 FX2 part4
        temp = gt8_data.mid(399, 24);              // copy gt8 FX2 part5
        data.replace(879, 24, temp);             // replace gt10 FX2 part5
        temp = gt8_data.mid(431, 11);              // copy gt8 FX2 part6
        data.replace(903, 11, temp);              // replace gt10 FX2 part6
        temp = gt8_data.mid(385, 14);              // copy gt8 FX2 part7
        data.replace(1001, 14, temp);             // replace gt10 FX2 part7
        temp = gt8_data.mid(423, 8);              // copy gt8 FX2 part8
        data.replace(1015, 8, temp);              // replace gt10 FX2 part8
        temp = gt8_data.mid(340, 10);              // copy gt8 FX2 part9
        data.replace(1269, 10, temp);               // replace gt10 FX2 part9
        temp = gt8_data.mid(442, 4);              // copy gt8 FX2 part10  HR
        temp.append(gt8_data.mid(447,5));
        temp.append(gt8_data.mid(446,1));
        temp.append(gt8_data.mid(453,1));
        data.replace(914, 11, temp);               // replace gt10 FX2 part10  HR
        temp = gt8_data.mid(454, 6);              // copy gt8 FX2 part11  PS
        temp.append(gt8_data.mid(461,7));
        temp.append(gt8_data.mid(460,1));
        temp.append(gt8_data.mid(468,1));
        data.replace(962, 15, temp);               // replace gt10 FX2 part11  PS
        temp = gt8_data.mid(474, 18);              // copy gt8 FX2 part12  OC, RT, 2CE
        data.replace(977, 18, temp);               // replace gt10 FX2 part12  OC, RT, 2CE
        temp = gt8_data.mid(492, 4);              // copy gt8 FX2 part13  AR
        temp.append(gt8_data.mid(497,4));
        data.replace(1040, 8, temp);               // replace gt10 FX2 part13  AR
        temp = gt8_data.mid(502, 1);              // copy gt8 FX2 part14  SYN
        temp.append(gt8_data.mid(501,1));
        temp.append(gt8_data.mid(503,15));
        data.replace(1023, 17, temp);               // replace gt10 FX2 part14 SYN
        temp = gt8_data.mid(518, 7);              // copy gt8 FX2 part15  AC
        data.replace(1279, 7, temp);               // replace gt10 FX2 part15 AC
        temp = gt8_data.mid(525, 3);              // copy gt8 FX2 part16  SH
        data.replace(1266, 3, temp);               // replace gt10 FX2 part16 SH
        temp = gt8_data.mid(528, 3);              // copy gt8 FX2 part17  SDD
        data.replace(995, 3, temp);               // replace gt10 FX2 part17 SDD
        temp = gt8_data.mid(531, 1);              // copy gt8 FX2 part18  SDD
        data.replace(999, 1, temp);               // replace gt10 FX2 part18 SDD

        MidiTable *midiTable = MidiTable::Instance();
        data.replace(1524, 1, gt8_data.mid(767, 1));  //copy Assign 1
        data.replace(1525, 2, midiTable->getArrayValue("Tables", "00", "00", "12", gt8_data.mid(769, 2))); //convert target
        data.replace(1527, 4, gt8_data.mid(771, 4));  //copy min/max
        data.replace(1531, 1, midiTable->getArrayValue("Tables", "00", "00", "13", gt8_data.mid(775, 1))); //convert source
        data.replace(1532, 3, gt8_data.mid(776, 3));  //copy Assign
        data.replace(1535, 1, midiTable->getArrayValue("Tables", "00", "00", "14", gt8_data.mid(779, 1))); //convert int pedal
        data.replace(1536, 4, gt8_data.mid(780, 4));  //copy Assign
        data.replace(1540, 1, gt8_data.mid(797, 1));  //copy Assign 2
        data.replace(1541, 2, midiTable->getArrayValue("Tables", "00", "00", "12", gt8_data.mid(799, 2))); //convert target
        data.replace(1543, 4, gt8_data.mid(801, 4));  //copy min/max
        data.replace(1547, 1, midiTable->getArrayValue("Tables", "00", "00", "13", gt8_data.mid(805, 1))); //convert source
        data.replace(1548, 3, gt8_data.mid(806, 3));  //copy Assign
        data.replace(1551, 1, midiTable->getArrayValue("Tables", "00", "00", "14", gt8_data.mid(809, 1))); //convert int pedal
        data.replace(1552, 4, gt8_data.mid(810, 4));  //copy Assign
        data.replace(1556, 1, gt8_data.mid(827, 1));  //copy Assign 3
        data.replace(1557, 2, midiTable->getArrayValue("Tables", "00", "00", "12", gt8_data.mid(829, 2))); //convert target
        data.replace(1559, 4, gt8_data.mid(831, 4));  //copy min/max
        data.replace(1563, 1, midiTable->getArrayValue("Tables", "00", "00", "13", gt8_data.mid(835, 1))); //convert source
        data.replace(1564, 3, gt8_data.mid(836, 3));  //copy Assign
        data.replace(1567, 1, midiTable->getArrayValue("Tables", "00", "00", "14", gt8_data.mid(839, 1))); //convert int pedal
        data.replace(1568, 4, gt8_data.mid(840, 4));  //copy Assign
        data.replace(1572, 1, gt8_data.mid(857, 1));  //copy Assign 4
        data.replace(1573, 2, midiTable->getArrayValue("Tables", "00", "00", "12", gt8_data.mid(859, 2))); //convert target
        data.replace(1575, 4, gt8_data.mid(861, 4));  //copy min/max
        data.replace(1579, 1, midiTable->getArrayValue("Tables", "00", "00", "13", gt8_data.mid(865, 1))); //convert source
        data.replace(1580, 3, gt8_data.mid(866, 3));  //copy Assign
        data.replace(1583, 1, midiTable->getArrayValue("Tables", "00", "00", "14", gt8_data.mid(869, 1))); //convert int pedal
        data.replace(1584, 4, gt8_data.mid(870, 4));  //copy Assign
        data.replace(1588, 1, gt8_data.mid(887, 1));  //copy Assign 5
        data.replace(1589, 2, midiTable->getArrayValue("Tables", "00", "00", "12", gt8_data.mid(889, 2))); //convert target
        data.replace(1591, 4, gt8_data.mid(891, 4));  //copy min/max
        data.replace(1595, 1, midiTable->getArrayValue("Tables", "00", "00", "13", gt8_data.mid(895, 1))); //convert source
        data.replace(1596, 3, gt8_data.mid(896, 3));  //copy Assign
        data.replace(1599, 1, midiTable->getArrayValue("Tables", "00", "00", "14", gt8_data.mid(899, 1))); //convert int pedal
        data.replace(1600, 4, gt8_data.mid(900, 4));  //copy Assign
        data.replace(1604, 1, gt8_data.mid(917, 1));  //copy Assign 6
        data.replace(1605, 2, midiTable->getArrayValue("Tables", "00", "00", "12", gt8_data.mid(919, 2))); //convert target
        data.replace(1607, 4, gt8_data.mid(921, 4));  //copy min/max
        data.replace(1611, 1, midiTable->getArrayValue("Tables", "00", "00", "13", gt8_data.mid(925, 1))); //convert source
        data.replace(1612, 3, gt8_data.mid(926, 3));  //copy Assign
        data.replace(1615, 1, midiTable->getArrayValue("Tables", "00", "00", "14", gt8_data.mid(929, 1))); //convert int pedal
        data.replace(1616, 4, gt8_data.mid(930, 4));  //copy Assign
        data.replace(1633, 1, gt8_data.mid(947, 1));  //copy Assign 7
        data.replace(1634, 2, midiTable->getArrayValue("Tables", "00", "00", "12", gt8_data.mid(949, 2))); //convert target
        data.replace(1636, 4, gt8_data.mid(951, 4));  //copy min/max
        data.replace(1640, 1, midiTable->getArrayValue("Tables", "00", "00", "13", gt8_data.mid(955, 1))); //convert source
        data.replace(1641, 3, gt8_data.mid(956, 3));  //copy Assign
        data.replace(1644, 1, midiTable->getArrayValue("Tables", "00", "00", "14", gt8_data.mid(959, 1))); //convert int pedal
        data.replace(1645, 4, gt8_data.mid(960, 4));  //copy Assign
        data.replace(1649, 1, gt8_data.mid(977, 1));  //copy Assign 8
        data.replace(1650, 2, midiTable->getArrayValue("Tables", "00", "00", "12", gt8_data.mid(979, 2))); //convert target
        data.replace(1652, 4, gt8_data.mid(981, 4));  //copy min/max
        data.replace(1656, 1, midiTable->getArrayValue("Tables", "00", "00", "13", gt8_data.mid(985, 1))); //convert source
        data.replace(1657, 3, gt8_data.mid(986, 3));  //copy Assign
        data.replace(1660, 1, midiTable->getArrayValue("Tables", "00", "00", "14", gt8_data.mid(989, 1))); //convert int pedal
        data.replace(1661, 4, gt8_data.mid(990, 4));  //copy Assign
        data.replace(1665, 1, gt8_data.mid(1007, 1));  //copy input sensitivity */

        translate10to100();

    };
}

void sysxWriter::convertFromGT10B()
{
    QByteArray GT10B_default;
    QFile file(":gt10b_default.syx");           // Read the default GT-10B sysx file .
    if (file.open(QIODevice::ReadOnly))
    {	GT10B_default = file.readAll(); };

    if(headerType == "GT10B_SYX"){         // if GT-10B syx patch file format
        translate10Bto100();
    }
    else if (headerType == "GT10B_SMF")                      // SMF ******************************************************************
    {                                        // file contains a .mid type SMF patch file header from Boss Librarian
        QByteArray smf_data = data;
        QFile file(":gt10b_default.syx");              // Read the default GT-10B sysx file so we don't start empty handed.
        if (file.open(QIODevice::ReadOnly))
        {	data = file.readAll(); };
        QByteArray temp;                         // TRANSLATION of GT-10B SMF PATCHES, data read from smf patch **************
        //bool isGT10B = false;
        //if ( smf_data.at(37) != data.at(5) ){ isGT10B = true; };    // check if a valid GT-10B file.
        index = 1;
        int patchCount = (smf_data.size()-32)/1806;
        if (patchCount>1)
        {
            QString msgText;
            QString patchText;
            QString patchNumber;
            char r;
            this->patchList.clear();
            this->patchList.append(QObject::tr("Select Patch"));
            int a = 43;                         // locate patch text start position from the start of the file
            for (int h=0;h<patchCount;h++)
            {
                for (int b=0;b<16;b++)
                {
                    r = char(smf_data[a+b]);
                    patchText.append(r);
                };
                patchNumber = QString::number(h+1, 10).toUpper();
                msgText.append(patchNumber + " : ");
                msgText.append(patchText + "   ");
                this->patchList.append(msgText);
                patchText.clear();
                msgText.clear();
                a=a+1806;                      // offset is set in front of marker
            };

            QString type = "smf";
            fileDialog *dialog = new fileDialog(fileName, patchList, data, GT10B_default, type);
            dialog->exec();
            patchIndex(this->index);
        };

        int a=0;                             // offset is set to first patch
        if (patchCount>1)
        {
            int q=index-1;
            a = q*1806;
        };

        temp = smf_data.mid(a+43, 128);            // copy SMF 128 bytes
        data.replace(11, 128, temp);             // replace gt10 address "00"...
        temp = smf_data.mid(a+171, 114);           // copy SMF part1...
        temp.append(smf_data.mid(a+301,14));       // copy SMF part2...
        data.replace(152, 128, temp);            // replace gt10 address "01"...
        temp = smf_data.mid(a+315, 128);           // copy SMF part1...
        data.replace(293, 128, temp);            // replace gt10 address "02"...
        temp = smf_data.mid(a+443, 100);           // copy SMF part1...
        data.replace(434, 100, temp);            // replace gt10 address "03"...
        temp = smf_data.mid(a+715, 86);            // copy SMF part1...
        temp.append(smf_data.mid(a+817,42));       // copy SMF part2...
        data.replace(547, 128, temp);             // replace gt10 address "05"...
        temp = smf_data.mid(a+859, 128);           // copy SMF part1...
        data.replace(688,128, temp);             // replace gt10 address "06"...
        temp = smf_data.mid(a+987, 72);            // copy SMF part1...
        temp.append(smf_data.mid(a+1075,28));      // copy SMF part2...
        data.replace(829, 100, temp);            // replace gt10 address "07"...
        temp = smf_data.mid(a+1259, 58);           // copy SMF part1...
        temp.append(smf_data.mid(a+1333,42));      // copy SMF part2...
        data.replace(942, 100, temp);           // replace gt10 address "09"...
        temp = smf_data.mid(a+1403, 128);          // copy SMF part1...
        data.replace(1083,128, temp);            // replace gt10 address "0A"...
        // if(isGT10B != true) {
        temp = smf_data.mid(a+1531, 44);           // copy SMF part1...
        temp.append(smf_data.mid(a+1591,84));      // copy SMF part2...
        data.replace(1224, 128, temp);           // replace gt10 address "0B"...
        temp = smf_data.mid(a+1675, 128);          // copy SMF part1...
        data.replace(1365,128, temp);            // replace gt10 address "0C"...
        //} else {
        //temp = smf_data.mid(a+1531, 18);           // copy SMF part1...
        //data.replace(1224, 18, temp);           // replace gt10 address "0B"...chain only.
        //};

        if (index>0)
        {
            translate10Bto100();
        };
    }
    else if (headerType == "GT10B_GXB")      // if the read file is a Boss Librarian type. ***************************************
    {
        index=1;
        char msb = char(data[34]);     // find patch count msb bit in GXB file at byte 34
        char lsb = char(data[35]);     // find patch count lsb bit in GXB file at byte 35
        bool ok;
        int patchCount;
        patchCount = (256*QString::number(msb, 16).toUpper().toInt(&ok, 16)) + (QString::number(lsb, 16).toUpper().toInt(&ok, 16));
        QByteArray marker;
        if (patchCount>1)
        {
            QString msgText;
            marker = data.mid(170, 2);      //copy marker key to find "06A5" which marks the start of each patch block
            QString patchText;
            QString patchNumber;
            this->patchList.clear();
            this->patchList.append(QObject::tr("Select Patch"));
            int a = data.indexOf(marker, 0); // locate patch start position from the start of the file
            a=a+2;                             // offset is set in front of marker
            for (int h=0;h<patchCount;h++)
            {
                for (int b=0;b<16;b++)
                {
                    char r = char(data[a+b]);
                    patchText.append(r);
                };
                patchNumber = QString::number(h+1, 10).toUpper();
                msgText.append(patchNumber + " : ");
                msgText.append(patchText + "   ");
                this->patchList.append(msgText);
                patchText.clear();
                msgText.clear();
                a = data.indexOf(marker, a); // locate patch start position from the start of the file
                a=a+2;                      // offset is set in front of marker
            };

            QString type = "gxb";
            fileDialog *dialog = new fileDialog(fileName, patchList, data, GT10B_default, type);
            dialog->exec();
            patchIndex(this->index);
        };

        marker = data.mid(170, 2);      //copy marker key to find "06A5" which marks the start of each patch block
        int a = data.indexOf(marker, 0); // locate patch start position from the start of the file
        a=a+2;                             // offset is set in front of marker
        if (patchCount>1)
        {
            int q=index-1;
            for (int h=0;h<q;h++)
            {
                a = data.indexOf(marker, a); // locate patch start position from the start of the file
                a=a+2;
            };                             // offset is set in front of marker
        };
        QByteArray temp;
        temp = data.mid(a, 128);
        GT10B_default.replace(11, 128, temp);      //address "00" +
        temp = data.mid(a+128, 128);
        GT10B_default.replace(152, 128, temp);     //address "01" +
        temp = data.mid(a+256, 128);
        GT10B_default.replace(293, 128, temp);     //address "02" +
        temp = data.mid(a+384, 100);
        GT10B_default.replace(434, 100, temp);     //address "03" +      no "04"
        temp = data.mid(a+640, 128);
        GT10B_default.replace(547, 128, temp);     //address "05" +
        temp = data.mid(a+768, 128);
        GT10B_default.replace(688, 128, temp);     //address "06" +
        temp = data.mid(a+896, 100);
        GT10B_default.replace(829, 100, temp);     //address "07" +      no "08"
        temp = data.mid(a+1152, 128);
        GT10B_default.replace(942, 128, temp);     //address "09" +
        temp = data.mid(a+1280, 128);
        GT10B_default.replace(1083, 128, temp);    //address "0A" +
        temp = data.mid(a+1408, 128);
        GT10B_default.replace(1224, 128, temp);    //address "0B" +
        temp = data.mid(a+1536, 128);
        GT10B_default.replace(1365, 128, temp);    //address "0C" +

        data = GT10B_default;
        if (index>0)
        {
            translate10Bto100();
        };
    };
}

void sysxWriter::translate10Bto100()
{
    MidiTable *midiTable = MidiTable::Instance();
    QFile file(":default.syx");           // Read the default KATANA sysx file .
    if (file.open(QIODevice::ReadOnly))
    {GT100_default = file.readAll(); };

    GT100_default.replace(11, 16, data.mid(11, 16));   //copy name
    GT100_default.replace(27, 1, midiTable->getArrayValue("Tables", "00", "00", "16", data.mid(28, 1))); //convert output select
    GT100_default.replace(43, 1, data.mid(91, 1));     //copy comp
    GT100_default.replace(44, 1, midiTable->getArrayValue("Tables", "00", "00", "17", data.mid(92, 1))); //convert comp type
    GT100_default.replace(47, 2, data.mid(93, 2));     //copy comp
    GT100_default.replace(45, 2, data.mid(95, 2));     //copy comp
    GT100_default.replace(59, 1, data.mid(123, 1));    //copy dist
    GT100_default.replace(60, 1, midiTable->getArrayValue("Tables", "00", "00", "18", data.mid(124, 1))); // convert dist type
    GT100_default.replace(61, 3, data.mid(125, 3));    //copy dist
    GT100_default.replace(64, 2, data.mid(130, 2));    //copy dist
    GT100_default.replace(66, 2, data.mid(128, 2));    //copy dist
    GT100_default.replace(68, 1, data.mid(132, 1));    //copy dist

    GT100_default.replace(91, 1, data.mid(232, 1));  //copy Preamp A sw
    GT100_default.replace(92, 1, midiTable->getArrayValue("Tables", "00", "00", "19", data.mid(233, 1))); //convert pre type
    GT100_default.replace(93, 1, data.mid(234, 1));    //copy pre A gain
    GT100_default.replace(95, 3, data.mid(235, 3));  //copy pre A tone
    GT100_default.replace(98, 1, data.mid(251, 1));  //copy pre A presense
    GT100_default.replace(99, 1, data.mid(238, 1));  //copy pre A level
    GT100_default.replace(100, 1, data.mid(239, 1));  //copy pre A bright
    GT100_default.replace(104, 1, midiTable->getArrayValue("Tables", "00", "00", "1A", data.mid(247, 1))); //convert spkr type
    GT100_default.replace(105, 1, data.mid(257, 1));  //copy spkr mic type
    GT100_default.replace(106, 1, data.mid(256, 1));  //copy spkr mic dist
    GT100_default.replace(107, 3, data.mid(248, 3));  //copy spkr mic pos & level

    //GT100_default.replace(152, 1, data.mid(232, 1));  //copy Preamp B sw
    GT100_default.replace(153, 1, midiTable->getArrayValue("Tables", "00", "00", "19", data.mid(233, 1))); //convert pre type
    GT100_default.replace(154, 1, data.mid(234, 1));    //copy pre A gain
    GT100_default.replace(156, 3, data.mid(235, 3));  //copy pre A tone
    GT100_default.replace(159, 1, data.mid(251, 1));  //copy pre A presense
    GT100_default.replace(160, 1, data.mid(238, 1));  //copy pre A level
    GT100_default.replace(161, 1, data.mid(239, 1));  //copy pre A bright
    GT100_default.replace(165, 1, midiTable->getArrayValue("Tables", "00", "00", "1A", data.mid(247, 1))); //convert spkr type
    GT100_default.replace(166, 1, data.mid(257, 1));  //copy spkr mic type
    GT100_default.replace(167, 1, data.mid(256, 1));  //copy spkr mic dist
    GT100_default.replace(168, 3, data.mid(248, 3));  //copy spkr mic pos & level

    GT100_default.replace(200, 12, data.mid(264, 12));  //copy EQ
    GT100_default.replace(201, 1, midiTable->getArrayValue("Tables", "00", "00", "03", data.mid(265, 1))); //convert lo cut
    GT100_default.replace(210, 1, midiTable->getArrayValue("Tables", "00", "00", "04", data.mid(274, 1))); //convert hi cut
    GT100_default.replace(216, 1, data.mid(293, 1));  //copy FX1 sw
    GT100_default.replace(217, 1, midiTable->getArrayValue("Tables", "00", "00", "1B", data.mid(294, 1))); //convert FX type
    GT100_default.replace(251, 5, data.mid(295, 5));  //copy ACS
    GT100_default.replace(257, 6, data.mid(300, 6));  //copy ALM
    GT100_default.replace(228, 7, data.mid(306, 7));  //copy TW
    GT100_default.replace(236, 7, data.mid(313, 7));  //copy AW
    GT100_default.replace(441, 3, data.mid(320, 3));  //copy TR
    GT100_default.replace(410, 8, data.mid(323, 8));  //copy PH
    GT100_default.replace(419, 2, data.mid(331, 2));  //copy FL
    GT100_default.replace(434, 6, data.mid(333, 6));  //copy FL
    GT100_default.replace(458, 5, data.mid(339, 5));  //copy PAN
    GT100_default.replace(471, 4, data.mid(344, 4));  //copy VB
    GT100_default.replace(454, 3, data.mid(348, 3));  //copy UV
    GT100_default.replace(477, 4, data.mid(351, 4));  //copy RM
    GT100_default.replace(313, 2, data.mid(355, 2));  //copy SG
    GT100_default.replace(482, 8, data.mid(370, 8));  //copy HU
    GT100_default.replace(465, 3, data.mid(378, 3));  //copy SL
    GT100_default.replace(276, 1, midiTable->getArrayValue("Tables", "00", "00", "03", data.mid(381, 1))); //convert lo cut
    GT100_default.replace(277, 3, data.mid(382, 3));  //copy SEQ
    GT100_default.replace(293, 5, data.mid(385, 5));  //copy SEQ
    GT100_default.replace(298, 1, midiTable->getArrayValue("Tables", "00", "00", "04", data.mid(390, 1))); //convert hi cut
    GT100_default.replace(299, 1, data.mid(391, 1));  //copy SEQ
    GT100_default.replace(362, 29, data.mid(392, 29));//copy HR
    GT100_default.replace(391, 6, data.mid(434, 6));  //copy HR
    GT100_default.replace(346, 15, data.mid(440, 15));//copy PS
    GT100_default.replace(342, 3, data.mid(455, 3));  //copy OC
    GT100_default.replace(446, 6, data.mid(458, 6));  //copy RT
    GT100_default.replace(491, 9, data.mid(464, 9));  //copy 2CE
    GT100_default.replace(504, 4, data.mid(475, 4));  //copy SDD
    GT100_default.replace(502, 2, data.mid(473, 2));  //copy SDD
    GT100_default.replace(505, 1, midiTable->getArrayValue("Tables", "00", "00", "04", data.mid(476, 1))); //convert hi cut
    GT100_default.replace(317, 7, data.mid(479, 7));  //copy DF
    //GT100_default.replace(334, 7, data.mid(486, 7));  //copy SITAR
    GT100_default.replace(325, 8, data.mid(622, 8));  //copy WSY
    GT100_default.replace(398, 3, data.mid(575, 3));  //copy SH
    GT100_default.replace(301, 5, data.mid(578, 5));  //copy TM
    GT100_default.replace(307, 5, data.mid(617, 5));  //copy GS
    GT100_default.replace(408, 1, data.mid(612, 1));  //copy AC-enhancer level/sense
    GT100_default.replace(406, 1, data.mid(616, 1));  //copy AC-enhancer high
    GT100_default.replace(403, 1, data.mid(614, 1));  //copy AC-enhancer low
    GT100_default.replace(244, 6, data.mid(595, 6));  //copy SWAH
    GT100_default.replace(264, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(602, 1))); //convert GEQ
    GT100_default.replace(265, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(603, 1))); //convert GEQ
    GT100_default.replace(266, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(604, 1))); //convert GEQ
    GT100_default.replace(267, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(605, 1))); //convert GEQ
    GT100_default.replace(268, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(606, 1))); //convert GEQ
    GT100_default.replace(269, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(607, 1))); //convert GEQ
    GT100_default.replace(270, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(608, 1))); //convert GEQ
    GT100_default.replace(271, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(609, 1))); //convert GEQ
    GT100_default.replace(272, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(610, 1))); //convert GEQ
    GT100_default.replace(273, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(611, 1))); //convert GEQ
    GT100_default.replace(274, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(601, 1))); //convert GEQ

    GT100_default.replace(510, 1, data.mid(688, 1));  //copy FX2 sw
    GT100_default.replace(511, 1, midiTable->getArrayValue("Tables", "00", "00", "1B", data.mid(689, 1))); //convert FX type
    GT100_default.replace(545, 5, data.mid(690, 5));  //copy ACS
    GT100_default.replace(551, 6, data.mid(695, 6));  //copy ALM
    GT100_default.replace(522, 7, data.mid(701, 7));  //copy TW
    GT100_default.replace(530, 7, data.mid(708, 7));  //copy AW
    GT100_default.replace(735, 3, data.mid(715, 3));  //copy TR
    GT100_default.replace(717, 8, data.mid(718, 8));  //copy PH
    GT100_default.replace(726, 8, data.mid(726, 8));  //copy FL
    GT100_default.replace(752, 5, data.mid(734, 5));  //copy PAN
    GT100_default.replace(765, 4, data.mid(739, 4));  //copy VB
    GT100_default.replace(748, 3, data.mid(743, 3));  //copy UV
    GT100_default.replace(771, 4, data.mid(746, 4));  //copy RM
    GT100_default.replace(607, 2, data.mid(750, 2));  //copy SG
    GT100_default.replace(776, 8, data.mid(765, 8));  //copy HU
    GT100_default.replace(759, 3, data.mid(773, 3));  //copy SL
    GT100_default.replace(583, 1, midiTable->getArrayValue("Tables", "00", "00", "03", data.mid(776, 1))); //convert lo cut
    GT100_default.replace(584, 8, data.mid(777, 8));  //copy SEQ
    GT100_default.replace(592, 1, midiTable->getArrayValue("Tables", "00", "00", "04", data.mid(785, 1))); //convert hi cut
    GT100_default.replace(593, 1, data.mid(786, 1));  //copy SEQ
    GT100_default.replace(656, 29, data.mid(787, 29));//copy HR
    GT100_default.replace(685, 6, data.mid(829, 6));  //copy HR
    GT100_default.replace(640, 15, data.mid(835, 15));//copy PS
    GT100_default.replace(636, 3, data.mid(850, 3));  //copy OC
    GT100_default.replace(740, 6, data.mid(853, 6));  //copy RT
    GT100_default.replace(785, 9, data.mid(859, 9));  //copy 2CE
    GT100_default.replace(796, 6, data.mid(868, 6));  //copy SDD
    GT100_default.replace(799, 1, midiTable->getArrayValue("Tables", "00", "00", "04", data.mid(871, 1))); //convert hi cut
    GT100_default.replace(611, 7, data.mid(874, 7));  //copy DF
    //GT100_default.replace(628, 7, data.mid(1008, 7));  //copy SITAR
    GT100_default.replace(619, 8, data.mid(1017, 8));  //copy WSY
    GT100_default.replace(692, 3, data.mid(970, 3));  //copy SH
    GT100_default.replace(595, 5, data.mid(973, 5));  //copy TM
    GT100_default.replace(601, 5, data.mid(978, 5));  //copy GS
    GT100_default.replace(702, 1, data.mid(894, 1));  //copy AC-enhancer level/sense
    GT100_default.replace(700, 1, data.mid(897, 1));  //copy AC-enhancer high
    GT100_default.replace(697, 1, data.mid(899, 1));  //copy AC-enhancer low
    GT100_default.replace(538, 6, data.mid(990, 6));  //copy SWAH
    GT100_default.replace(558, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(997, 1))); //convert GEQ
    GT100_default.replace(559, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(998, 1))); //convert GEQ
    GT100_default.replace(560, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(999, 1))); //convert GEQ
    GT100_default.replace(561, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1000, 1))); //convert GEQ
    GT100_default.replace(575, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1001, 1))); //convert GEQ
    GT100_default.replace(576, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1002, 1))); //convert GEQ
    GT100_default.replace(577, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1003, 1))); //convert GEQ
    GT100_default.replace(578, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1004, 1))); //convert GEQ
    GT100_default.replace(579, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1005, 1))); //convert GEQ
    GT100_default.replace(580, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1006, 1))); //convert GEQ
    GT100_default.replace(581, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(996, 1))); //convert GEQ
    GT100_default.replace(812, 1, data.mid(1083, 1));  //copy DD sw
    GT100_default.replace(813, 1, midiTable->getArrayValue("Tables", "00", "00", "07", data.mid(1084, 1))); //convert DD type
    GT100_default.replace(814, 2, data.mid(1085, 2));  //copy DD time
    GT100_default.replace(820, 1, data.mid(1087, 1));  //copy DD tap
    GT100_default.replace(816, 1, data.mid(1088, 1));  //copy DD fb
    GT100_default.replace(817, 1, midiTable->getArrayValue("Tables", "00", "00", "04", data.mid(1089, 1))); //convert hi cut
    GT100_default.replace(821, 10, data.mid(1090, 10));  //copy DD
    GT100_default.replace(824, 1, midiTable->getArrayValue("Tables", "00", "00", "03", data.mid(1093, 1))); //convert lo cut
    GT100_default.replace(829, 1, midiTable->getArrayValue("Tables", "00", "00", "04", data.mid(1098, 1))); //convert hi cut
    GT100_default.replace(831, 2, data.mid(1104, 2));  //copy DD mod
    GT100_default.replace(818, 2, data.mid(1106, 2));  //copy DD level
    GT100_default.replace(857, 5, data.mid(1115, 5));  //copy CE
    GT100_default.replace(862, 1, midiTable->getArrayValue("Tables", "00", "00", "03", data.mid(1120, 1))); //convert lo cut
    GT100_default.replace(863, 1, midiTable->getArrayValue("Tables", "00", "00", "04", data.mid(1121, 1))); //convert hi cut
    GT100_default.replace(864, 1, data.mid(1122, 1));  //copy CE
    GT100_default.replace(873, 3, data.mid(1131, 3));  //copy RV
    GT100_default.replace(878, 1, midiTable->getArrayValue("Tables", "00", "00", "03", data.mid(1135, 1))); //convert lo cut
    GT100_default.replace(879, 1, midiTable->getArrayValue("Tables", "00", "00", "04", data.mid(1136, 1))); //convert hi cut
    GT100_default.replace(880, 4, data.mid(1137, 4));  //copy RV
    GT100_default.replace(876, 2, data.mid(1141, 2));  //copy RV pre delay
    GT100_default.replace(889, 1, data.mid(1147, 1));  //copy PFX sw
    GT100_default.replace(1155, 1, data.mid(1152, 1));  //copy PFX mode
    GT100_default.replace(1094, 1, midiTable->getArrayValue("Tables", "00", "00", "1D", data.mid(1153, 1))); //convert EXP sw func
    GT100_default.replace(1078, 1, midiTable->getArrayValue("Tables", "00", "00", "1D", data.mid(1154, 1))); //convert ctl 1 func
    GT100_default.replace(1110, 1, midiTable->getArrayValue("Tables", "00", "00", "1D", data.mid(1155, 1))); //convert ctl 2 func
    GT100_default.replace(895, 6, data.mid(1156, 6));  //copy PFX WAH
    GT100_default.replace(891, 4, data.mid(1168, 4));  //copy PFX PB
    GT100_default.replace(908, 1, data.mid(1173, 1));  //copy PFX FV
    GT100_default.replace(906, 2, data.mid(1174, 2));  //copy PFX FV
    GT100_default.replace(905, 1, data.mid(1176, 1));  //copy PFX FV
    GT100_default.replace(1014, 1, data.mid(1179, 1));  //copy MST
    GT100_default.replace(1015, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1180, 1))); //convert MEQ
    GT100_default.replace(1018, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1181, 1))); //convert MEQ
    GT100_default.replace(1017, 1, data.mid(1182, 1));  //copy MST
    GT100_default.replace(1016, 1, data.mid(1183, 1));  //copy MST
    GT100_default.replace(1019, 1, midiTable->getArrayValue("Tables", "00", "00", "06", data.mid(1184, 1))); //convert MEQ
    GT100_default.replace(1020, 3, data.mid(1185, 3));  //copy MST bpm
    GT100_default.replace(953, 1, data.mid(1188, 1));  //copy AMP sw
    GT100_default.replace(956, 4, data.mid(1196, 4));  //copy NS1
    GT100_default.replace(961, 4, data.mid(1200, 4));  //copy NS2
    GT100_default.replace(942, 4, data.mid(1204, 4));  //copy SR loop

    QByteArray chain(data.mid(1224, 18)); //copy gt10b chain
    QString chn;
    QString list;
    for(int x=0; x<18; ++x)
    {
        char r = chain.at(x);
        QString val = QString::number(r, 16).toUpper();
        if(val.size()<2){val.prepend("0"); };
        chn.append(val);
        list.append(chn.mid(x*2, 1)); // make list of 10's units of chain items, channel b are prepended with 4
    };
    int pos = list.indexOf("4"); //find position of channel b start

    chain.replace(0, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(0, 1))); //convert chain
    chain.replace(1, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(1, 1))); //convert chain
    chain.replace(2, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(2, 1))); //convert chain
    chain.replace(3, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(3, 1))); //convert chain
    chain.replace(4, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(4, 1))); //convert chain
    chain.replace(5, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(5, 1))); //convert chain
    chain.replace(6, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(6, 1))); //convert chain
    chain.replace(7, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(7, 1))); //convert chain
    chain.replace(8, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(8, 1))); //convert chain
    chain.replace(9, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(9, 1))); //convert chain
    chain.replace(10, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(10, 1))); //convert chain
    chain.replace(11, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(11, 1))); //convert chain
    chain.replace(12, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(12, 1))); //convert chain
    chain.replace(13, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(13, 1))); //convert chain
    chain.replace(14, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(14, 1))); //convert chain
    chain.replace(15, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(15, 1))); //convert chain
    chain.replace(16, 1, midiTable->getArrayValue("Tables", "00", "00", "0A", chain.mid(16, 1))); //convert chain
    chain.remove(17, 1);
    chain.insert(16, char(10));  // insert Accel
    chain.insert(pos, char(3));   // insert pre B
    chain.insert(pos, char(18));  // insert chain mid link
    GT100_default.replace(1030, 20, chain);

    GT100_default.replace(1187, 1, data.mid(1256, 1));  //copy Assign 1
    GT100_default.replace(1188, 2, midiTable->getArrayValue("Tables", "00", "00", "1C", data.mid(1257, 2))); //convert target
    GT100_default.replace(1190, 4, data.mid(1259, 4));  //copy min/max
    GT100_default.replace(1194, 1, midiTable->getArrayValue("Tables", "00", "00", "0C", data.mid(1263, 1))); //convert source
    GT100_default.replace(1195, 3, data.mid(1264, 3));  //copy Assign
    GT100_default.replace(1198, 1, midiTable->getArrayValue("Tables", "00", "00", "0D", data.mid(1267, 1))); //convert int pedal
    GT100_default.replace(1199, 4, data.mid(1268, 4));  //copy Assign
    GT100_default.replace(1219, 1, data.mid(1272, 1));  //copy Assign 2
    GT100_default.replace(1220, 2, midiTable->getArrayValue("Tables", "00", "00", "1C", data.mid(1273, 2))); //convert target
    GT100_default.replace(1222, 4, data.mid(1275, 4));  //copy min/max
    GT100_default.replace(1226, 1, midiTable->getArrayValue("Tables", "00", "00", "0C", data.mid(1279, 1))); //convert source
    GT100_default.replace(1227, 3, data.mid(1280, 3));  //copy Assign
    GT100_default.replace(1230, 1, midiTable->getArrayValue("Tables", "00", "00", "0D", data.mid(1283, 1))); //convert int pedal
    GT100_default.replace(1231, 4, data.mid(1284, 4));  //copy Assign
    GT100_default.replace(1251, 1, data.mid(1288, 1));  //copy Assign 3
    GT100_default.replace(1252, 2, midiTable->getArrayValue("Tables", "00", "00", "1C", data.mid(1289, 2))); //convert target
    GT100_default.replace(1254, 4, data.mid(1291, 4));  //copy min/max
    GT100_default.replace(1258, 1, midiTable->getArrayValue("Tables", "00", "00", "0C", data.mid(1295, 1))); //convert source
    GT100_default.replace(1259, 3, data.mid(1296, 3));  //copy Assign
    GT100_default.replace(1262, 1, midiTable->getArrayValue("Tables", "00", "00", "0D", data.mid(1299, 1))); //convert int pedal
    GT100_default.replace(1263, 4, data.mid(1300, 4));  //copy Assign
    GT100_default.replace(1296, 1, data.mid(1304, 1));  //copy Assign 4
    GT100_default.replace(1297, 2, midiTable->getArrayValue("Tables", "00", "00", "1C", data.mid(1305, 2))); //convert target
    GT100_default.replace(1299, 4, data.mid(1307, 4));  //copy min/max
    GT100_default.replace(1303, 1, midiTable->getArrayValue("Tables", "00", "00", "0C", data.mid(1311, 1))); //convert source
    GT100_default.replace(1304, 3, data.mid(1312, 3));  //copy Assign
    GT100_default.replace(1307, 1, midiTable->getArrayValue("Tables", "00", "00", "0D", data.mid(1315, 1))); //convert int pedal
    GT100_default.replace(1308, 4, data.mid(1316, 4));  //copy Assign
    GT100_default.replace(1328, 1, data.mid(1320, 1));  //copy Assign 5
    GT100_default.replace(1329, 2, midiTable->getArrayValue("Tables", "00", "00", "1C", data.mid(1321, 2))); //convert target
    GT100_default.replace(1331, 4, data.mid(1323, 4));  //copy min/max
    GT100_default.replace(1335, 1, midiTable->getArrayValue("Tables", "00", "00", "0C", data.mid(1327, 1))); //convert source
    GT100_default.replace(1336, 3, data.mid(1328, 3));  //copy Assign
    GT100_default.replace(1339, 1, midiTable->getArrayValue("Tables", "00", "00", "0D", data.mid(1331, 1))); //convert int pedal
    GT100_default.replace(1340, 4, data.mid(1332, 4));  //copy Assign
    GT100_default.replace(1360, 1, data.mid(1336, 1));  //copy Assign 6
    GT100_default.replace(1361, 2, midiTable->getArrayValue("Tables", "00", "00", "1C", data.mid(1337, 2))); //convert target
    GT100_default.replace(1363, 4, data.mid(1339, 4));  //copy min/max
    GT100_default.replace(1367, 1, midiTable->getArrayValue("Tables", "00", "00", "0C", data.mid(1343, 1))); //convert source
    GT100_default.replace(1368, 3, data.mid(1344, 3));  //copy Assign
    GT100_default.replace(1371, 1, midiTable->getArrayValue("Tables", "00", "00", "0D", data.mid(1347, 1))); //convert int pedal
    GT100_default.replace(1372, 4, data.mid(1348, 4));  //copy Assign
    GT100_default.replace(1392, 1, data.mid(1365, 1));  //copy Assign 7
    GT100_default.replace(1393, 2, midiTable->getArrayValue("Tables", "00", "00", "1C", data.mid(1366, 2))); //convert target
    GT100_default.replace(1395, 4, data.mid(1368, 4));  //copy min/max
    GT100_default.replace(1399, 1, midiTable->getArrayValue("Tables", "00", "00", "0C", data.mid(1372, 1))); //convert source
    GT100_default.replace(1400, 3, data.mid(1373, 3));  //copy Assign
    GT100_default.replace(1403, 1, midiTable->getArrayValue("Tables", "00", "00", "0D", data.mid(1376, 1))); //convert int pedal
    GT100_default.replace(1404, 4, data.mid(1377, 4));  //copy Assign
    GT100_default.replace(1437, 1, data.mid(1381, 1));  //copy Assign 8
    GT100_default.replace(1438, 2, midiTable->getArrayValue("Tables", "00", "00", "1C", data.mid(1382, 2))); //convert target
    GT100_default.replace(1440, 4, data.mid(1384, 4));  //copy min/max
    GT100_default.replace(1444, 1, midiTable->getArrayValue("Tables", "00", "00", "0C", data.mid(1388, 1))); //convert source
    GT100_default.replace(1445, 3, data.mid(1389, 3));  //copy Assign
    GT100_default.replace(1448, 1, midiTable->getArrayValue("Tables", "00", "00", "0D", data.mid(1392, 1))); //convert int pedal
    GT100_default.replace(1449, 4, data.mid(1393, 4));  //copy Assign
    GT100_default.replace(1469, 1, data.mid(1397, 1));  //copy input sensitivity

    SysxIO *sysxIO = SysxIO::Instance();
    sysxIO->setFileSource("Structure", GT100_default);
    sysxIO->setFileName(this->fileName);
    this->fileSource = sysxIO->getFileSource();
}

void sysxWriter::convertFromGT6B()
{
    MidiTable *midiTable = MidiTable::Instance();
    QByteArray gt6b_data = data;
    QFile file(":gt10b_default.syx");   // Read the default GT-10B sysx file so we don't start empty handed.
    if (file.open(QIODevice::ReadOnly))
    {	data = file.readAll(); };
    char r;
    QByteArray temp;                         // TRANSLATION of GT-6B PATCHES, data read from gt6b patch **************
    QByteArray Qhex;                         // and used to replace gt10b patch data*********************************
    QFile hexfile(":HexLookupTable.hex");    // use a QByteArray of hex numbers from a lookup table.
    if (hexfile.open(QIODevice::ReadOnly))
    {	Qhex = hexfile.readAll(); };
    temp = gt6b_data.mid(403, 14);           // copy gt6b name
    temp.append(char(32));
    temp.append(char(32));                   // add 2 blank spaces as gt6b has only 14 characters
    data.replace(11, 16, temp);              // replace gt10b name
    temp = gt6b_data.mid(10,1);              // copy off/on
    data.replace(91, 1, temp);               // replace gt10b compressor sw
    data.replace(92, 1, midiTable->getArrayValue("Tables", "00", "00", "1E", gt6b_data.mid(12, 1))); //convert comp type
    temp = gt6b_data.mid(19,2);              // copy gt6b compressor tone & level
    temp.append(gt6b_data.mid(15,1));        // copy gt6b compressor sustain
    temp.append(gt6b_data.mid(13,2));        // copy gt6b compressor attack & threshold
    temp.append(gt6b_data.mid(16,1));        // copy gt6b compressor ratio (big 1 only)
    temp.append(gt6b_data.mid(13,1));        // copy gt6b compressor attack again
    temp.append(gt6b_data.mid(18,1));        // copy gt6b compressor release
    data.replace(93, 8, temp);               // replace gt10b compressor...
    temp = gt6b_data.mid(100, 1);            // copy gt6b dist/od on/off
    data.replace(123, 1, temp);              // replace gt10 distortion
    data.replace(124, 1, midiTable->getArrayValue("Tables", "00", "00", "1F", gt6b_data.mid(102, 1))); //convert dist type
    temp = gt6b_data.mid(103,5);             // copy gt6b dist/od part2
    data.replace(125, 5, temp);              // replace gt10b distortion

    temp = gt6b_data.mid(120, 1);            // copy gt6b preAmp off/on
    data.replace(232, 1, temp);              // replace gt10b preamp off/on
    data.replace(233, 1, midiTable->getArrayValue("Tables", "00", "00", "20", gt6b_data.mid(122, 1))); //convert pre type
    temp = gt6b_data.mid(128,1);             // copy gt6b preAmp gain
    temp.append(gt6b_data.mid(133,1));        // copy gt6b preAmp bass
    temp.append(gt6b_data.mid(131,1));        // copy gt6b preAmp mid
    temp.append(gt6b_data.mid(129,1));        // copy gt6b preAmp treble
    temp.append(gt6b_data.mid(135,1));        // copy gt6b preAmp level
    temp.append(gt6b_data.mid(123,1));        // copy gt6b preAmp bright
    temp.append(gt6b_data.mid(132,1));        // copy gt6b preAmp mid freq
    temp.append(gt6b_data.mid(134,1));        // copy gt6b preAmp ultra low
    temp.append(gt6b_data.mid(130,1));        // copy gt6b preAmp ultra hi
    temp.append(gt6b_data.mid(125,1));        // copy gt6b preAmp response
    temp.append(gt6b_data.mid(124,1));        // copy gt6b preAmp deep
    temp.append(gt6b_data.mid(127,1));        // copy gt6b preAmp shape
    temp.append(gt6b_data.mid(126,1));        // copy gt6b preAmp enhancer
    data.replace(234, 13, temp);              // replace gt10b preamp
    data.replace(247, 1, midiTable->getArrayValue("Tables", "00", "00", "21", gt6b_data.mid(138, 1))); //convert
    temp = gt6b_data.mid(136, 1);            // read gt6b spkr off/on
    if (temp == Qhex.mid((151), 1)){         // if spkr is off
        temp = Qhex.mid((151), 1);               // read 00 from HexLookupTable 151->
        data.replace(247, 1, temp);   };         // replace gt10b spkr type to OFF
    temp = gt6b_data.mid(139,3);             // copy gt6b spkr settings
    data.replace(248, 3, temp);              // replace gt10b spkr settings

    temp = gt6b_data.mid(155, 1);             // copy gt6b EQ off/on
    data.replace(264, 1, temp);               // replace gt10b EQ
    data.replace(268, 1, gt6b_data.mid(157, 1));   //copy gt6b EQ lo Q
    data.replace(267, 1, midiTable->getArrayValue("Tables", "00", "00", "2A", gt6b_data.mid(158, 1))); //convert lo freq
    data.replace(269, 1, gt6b_data.mid(159, 1));   //copy gt6b EQ lo gain
    data.replace(271, 1, gt6b_data.mid(160, 1));   //copy gt6b EQ mid Q
    data.replace(270, 1, midiTable->getArrayValue("Tables", "00", "00", "2B", gt6b_data.mid(161, 1))); //convert mid freq
    data.replace(272, 1, gt6b_data.mid(162, 1));   //copy gt6b EQ mid gain
    data.replace(274, 1, midiTable->getArrayValue("Tables", "00", "00", "2C", gt6b_data.mid(164, 1))); //convert hi freq
    data.replace(273, 1, gt6b_data.mid(165, 1));   //copy gt6b EQ hi gain
    data.replace(275, 1, gt6b_data.mid(166, 1));   //copy gt6b EQ level

    temp = gt6b_data.mid(322, 1);            // copy gt6b delay off/on
    temp.append(gt6b_data.mid(333,1));       // copy gt6b delay type
    data.replace(1083, 2, temp);             // replace gt10b delay
    r = gt6b_data.at(334);                   // copy gt6b delay time
    temp = Qhex.mid((r/6), 1);               // convert delay time to delay MSB
    data.replace(1085, 1, temp);             // replace gt10b delay
    r = gt6b_data.at(335);                   // copy gt6b delay time
    temp = Qhex.mid((r*5), 1);               // convert delay time to delay LSB
    data.replace(1086, 1, temp);             // replace gt10b delay
    temp = gt6b_data.mid(336, 3);            // copy gt6b delay stuff
    data.replace(1087, 3, temp);             // replace gt10b delay
    temp = gt6b_data.mid(339, 1);            // copy gt6b delay level
    data.replace(1106, 1, temp);             // replace gt10b delay
    temp = Qhex.mid(100, 1);                 // copy 0x64
    data.replace(1107, 1, temp);             // replace gt10b direct to %100

    temp = gt6b_data.mid(322, 1);            // copy gt6b reverb off/on
    temp.append(gt6b_data.mid(325,2));       // copy gt6b reverb type & time
    data.replace(1131, 3, temp);             // replace gt10b reverb type & time
    temp = gt6b_data.mid(328,4);             // copy gt6b reverb stuff
    temp.append(Qhex.mid(100, 1));           // copy 0x64 to set direct to %100
    data.replace(1135, 5, temp);             // replace gt10b reverb
    temp = (gt6b_data.mid(327,1));           // copy gt6b reverb pre-delay
    data.replace(1142, 1, temp);             // replace gt10b reverb pre-delay LSB

    if ( gt6b_data.mid(322, 1) == Qhex.mid(1, 1) ){ // if the rev/dly/sos effect pedal is "ON"
        r = gt6b_data.at(324);               // copy gt6b delay/rev/sos select
        temp = Qhex.mid((160+r), 1);         // set delay off/on state from hexlookuptable->160
        data.replace(1083, 1, temp);         // replace gt10b delay off/on
        temp = Qhex.mid((164+r), 1);         // set reverb off/on state from hexlookuptable->164
        data.replace(1131, 1, temp);         // replace gt10b reverb off/on
    };

    temp = gt6b_data.mid(302, 1);            // copy gt6b chorus off/on
    temp.append(gt6b_data.mid(304,5));       // copy gt6b chorus stuff
    data.replace(1115, 6, temp);             // replace gt10b chorus
    temp = gt6b_data.mid(309,1);             // copy gt6b chorus level
    data.replace(1122, 1, temp);             // replace gt10b chorus

    temp = gt6b_data.mid(379, 12);
    QString chn;
    QString list;
    for(int x=0; x<12; ++x)
    {
        char r = temp.at(x);
        QString val = QString::number(r, 16).toUpper();
        if(val.size()<2){val.prepend("0"); };
        chn.append(val);
        list.append(chn.mid((x*2)+1, 1)); // make list of 01's units of chain items
    };
    int pos = list.indexOf("4")+1; //find position of preamp

    temp.replace(0, 1, midiTable->getArrayValue("Tables", "00", "00", "22", temp.mid(0, 1))); //convert chain
    temp.replace(1, 1, midiTable->getArrayValue("Tables", "00", "00", "22", temp.mid(1, 1))); //convert chain
    temp.replace(2, 1, midiTable->getArrayValue("Tables", "00", "00", "22", temp.mid(2, 1))); //convert chain
    temp.replace(3, 1, midiTable->getArrayValue("Tables", "00", "00", "22", temp.mid(3, 1))); //convert chain
    temp.replace(4, 1, midiTable->getArrayValue("Tables", "00", "00", "22", temp.mid(4, 1))); //convert chain
    temp.replace(5, 1, midiTable->getArrayValue("Tables", "00", "00", "22", temp.mid(5, 1))); //convert chain
    temp.replace(6, 1, midiTable->getArrayValue("Tables", "00", "00", "22", temp.mid(6, 1))); //convert chain
    temp.replace(7, 1, midiTable->getArrayValue("Tables", "00", "00", "22", temp.mid(7, 1))); //convert chain
    temp.replace(8, 1, midiTable->getArrayValue("Tables", "00", "00", "22", temp.mid(8, 1))); //convert chain
    temp.replace(9, 1, midiTable->getArrayValue("Tables", "00", "00", "22", temp.mid(9, 1))); //convert chain
    temp.replace(10, 1, midiTable->getArrayValue("Tables", "00", "00", "22", temp.mid(10, 1))); //convert chain
    temp.replace(11, 1, midiTable->getArrayValue("Tables", "00", "00", "22", temp.mid(11, 1))); //convert chain

    temp.insert(pos, Qhex.mid(17, 1));       // insert gt10b_merge after preamp 1  "11"
    temp.insert(pos, Qhex.mid(78, 1));       // add LP "4E" 78
    temp.insert(pos, Qhex.mid(77, 1));       // insert NS_2 "4D"  77
    temp.append(Qhex.mid(15 ,1));            // add DGT
    temp.insert(pos-1, Qhex.mid(16, 1));     // insert gt10b_split before preamp 1 "10"
    temp.append(Qhex.mid(67, 1));            // insert preamp B "43" last for gt-10b
    data.replace(1224, 18, temp);            // replace gt10b chain

    temp = gt6b_data.mid(359, 3);            // copy gt6b NS
    data.replace(1196, 3, temp);             // replace gt10b NS_1
    data.replace(1200, 3, temp);             // replace gt10b NS_2

    temp = gt6b_data.mid(67, 1);             // copy gt6b wah on/off
    data.replace(1147, 1, temp);             // replace gt10b pedal fx on/off
    temp = gt6b_data.mid(70, 2);             // copy gt6b wah
    data.replace(1156, 2, temp);             // replace gt10b wah
    temp = gt6b_data.mid(72, 1);             // copy gt6b wah level
    data.replace(1160, 1, temp);             // replace gt10b wah

    temp = gt6b_data.mid(223, 5);            // copy gt6b pedal bend (fx2)
    data.replace(1167, 5, temp);             // replace gt10b pedal bend

    temp = gt6b_data.mid(430, 2);            // copy gt6b FV min/max
    data.replace(1174, 2, temp);             // replace gt10b FV min/max
    temp = gt6b_data.mid(366, 1);            // copy gt6b FV vol
    data.replace(1173, 1, temp);             // replace gt10b FV vol

    data.replace(1153, 1, midiTable->getArrayValue("Tables", "00", "00", "29", gt6b_data.mid(446, 2))); //convert ExSw Func
    data.replace(1154, 1, midiTable->getArrayValue("Tables", "00", "00", "29", gt6b_data.mid(467, 2))); //convert CTL Func

    temp = gt6b_data.mid(366, 1);            // copy gt6b Master patch level
    data.replace(1179, 1, temp);             // replace gt10b Master patch level
    temp = gt6b_data.mid(363, 2);            // copy gt6b Master BPM
    data.replace(1185, 2, temp);             // replace gt10b Master BPM

    temp = gt6b_data.mid(33, 1);            // copy gt8 FX1 on/off
    data.replace(293, 1, temp);             // replace gt10b FX1 on/off
    data.replace(294, 1, midiTable->getArrayValue("Tables", "00", "00", "25", gt6b_data.mid(35, 1))); //convert FX1 Type
    temp = gt6b_data.mid(36, 2);            // copy gt6b FX1 octave
    data.replace(456, 2, temp);             // replace gt10b FX1 octave
    temp = gt6b_data.mid(40,3);             // copy gt6b FX1 enh
    data.replace(612, 3, temp);             // replace gt10b FX1 enh
    temp = gt6b_data.mid(45, 2);            // copy gt6b FX1 sg
    data.replace(355, 2, temp);             // replace gt10b FX1 sg
    temp = gt6b_data.mid(47, 3);            // copy gt6b FX1 df
    data.replace(480, 3, temp);             // replace gt10b FX1 df
    temp = gt6b_data.mid(50, 1);            // copy gt6b FX1 df
    data.replace(484, 1, temp);             // replace gt10b FX1 df
    temp = gt6b_data.mid(51, 4);            // copy gt6b FX1 rm
    data.replace(351, 4, temp);             // replace gt10b FX1 rm

    temp = gt6b_data.mid(179, 1);           // copy gt6b FX2 on/off
    data.replace(688, 1, temp);             // replace gt10b FX2 on/off
    data.replace(689, 1, midiTable->getArrayValue("Tables", "00", "00", "26", gt6b_data.mid(181, 1))); //convert FX2 Type
    temp = gt6b_data.mid(182, 5);           // copy gt6b FX2 PH
    temp.append(gt6b_data.mid(188,3));      // copy gt6b FX2 PH
    data.replace(718, 8, temp);             // replace gt10b FX2 PH
    temp = gt6b_data.mid(190, 5);           // copy gt6b FX2 FL
    data.replace(726, 5, temp);             // replace gt10b FX2 FL
    temp = gt6b_data.mid(195,1);            // copy gt6b FX2 FL
    data.replace(732, 1, temp);             // replace gt10b FX2 FL
    temp = gt6b_data.mid(196, 4);          // copy gt6b FX2 HR
    data.replace(787, 4, temp);            // replace gt10b FX2 HR
    data.replace(788, 1, midiTable->getArrayValue("Tables", "00", "00", "27", gt6b_data.mid(197, 1))); //convert FX2
    temp = gt6b_data.mid(200, 1);          // copy gt6b FX2 HR
    data.replace(796, 1, temp);            // replace gt10b FX2 HR
    temp = gt6b_data.mid(201, 5);          // copy gt6b FX2 HR
    data.replace(791, 5, temp);            // replace gt10b FX2 HR
    data.replace(792, 1, midiTable->getArrayValue("Tables", "00", "00", "27", gt6b_data.mid(202, 1))); //convert FX2
    temp = gt6b_data.mid(207, 1);          // copy gt6b FX2 HR
    data.replace(797, 1, temp);            // replace gt10b FX2 HR
    temp = gt6b_data.mid(208, 6);          // copy gt6b FX2 PS
    data.replace(835, 6, temp);            // replace gt10b FX2 PS
    temp = gt6b_data.mid(214, 1);          // copy gt6b FX2 PS
    data.replace(848, 1, temp);            // replace gt10b FX2 PS
    temp = gt6b_data.mid(215, 7);          // copy gt6b FX2 PS
    data.replace(841, 7, temp);            // replace gt10b FX2 PS
    temp = gt6b_data.mid(222, 1);          // copy gt6b FX2 PS
    data.replace(849, 1, temp);            // replace gt10b FX2 PS
    temp = gt6b_data.mid(228, 9);          // copy gt6b FX2 2CE
    data.replace(859, 9, temp);            // replace gt10b FX2 2CE
    data.replace(773, 1, midiTable->getArrayValue("Tables", "00", "00", "28", gt6b_data.mid(237, 1))); //convert FX2 ASL phrase
    temp = gt6b_data.mid(239, 2);          // copy gt6b FX2 ASL rate/sense
    data.replace(774, 2, temp);            // replace gt10b FX2 SL tempo
    temp = gt6b_data.mid(246, 3);          // copy gt6b FX2 SDD
    data.replace(868, 3, temp);            // replace gt10b FX2 SDD
    temp = gt6b_data.mid(249, 1);          // copy gt6b FX2 SDD
    data.replace(872, 1, temp);            // replace gt10b FX2 SDD
    temp = gt6b_data.mid(250, 4);          // copy gt6b FX2 VB
    data.replace(739, 4, temp);            // replace gt10b FX2 VB
    temp = gt6b_data.mid(254, 3);          // copy gt6b FX2 HU
    data.replace(765, 3, temp);            // replace gt10b FX2 HU
    temp = gt6b_data.mid(258, 5);          // copy gt6b FX2 HU
    data.replace(768, 5, temp);            // replace gt10b FX2 HU
    temp = gt6b_data.mid(264, 3);          // copy gt6b FX2 PAN
    data.replace(736, 3, temp);            // replace gt10b FX2 PAN
    temp = gt6b_data.mid(267, 1);          // copy gt6b FX2 SYN wave
    data.replace(888, 1, temp);            // replace gt10b FX2 SYN
    temp = gt6b_data.mid(276, 1);          // copy gt6b FX2 SYN cutoff
    data.replace(889, 1, temp);            // replace gt10b FX2 SYN
    temp = gt6b_data.mid(275, 1);          // copy gt6b FX2 SYN reso
    data.replace(890, 1, temp);            // replace gt10b FX2 SYN
    temp = gt6b_data.mid(272, 1);          // copy gt6b FX2 SYN sens
    data.replace(891, 1, temp);            // replace gt10b FX2 SYN
    temp = gt6b_data.mid(278, 1);          // copy gt6b FX2 SYN decay
    data.replace(892, 1, temp);            // replace gt10b FX2 SYN
    temp = gt6b_data.mid(277, 1);          // copy gt6b FX2 SYN depth
    data.replace(893, 1, temp);            // replace gt10b FX2 SYN
    temp = gt6b_data.mid(279, 2);          // copy gt6b FX2 SYN levels
    data.replace(894, 2, temp);            // replace gt10b FX2 SYN

    data.replace(1256, 1, gt6b_data.mid(502, 1));  //copy Assign 1
    data.replace(1257, 2, midiTable->getArrayValue("Tables", "00", "00", "23", gt6b_data.mid(504, 2))); //convert target
    data.replace(1259, 4, gt6b_data.mid(506, 4));  //copy min/max
    data.replace(1263, 1, midiTable->getArrayValue("Tables", "00", "00", "24", gt6b_data.mid(510, 1))); //convert source
    data.replace(1264, 3, gt6b_data.mid(511, 3));  //copy Assign

    data.replace(1272, 1, gt6b_data.mid(526, 1));  //copy Assign 2
    data.replace(1273, 2, midiTable->getArrayValue("Tables", "00", "00", "23", gt6b_data.mid(528, 2))); //convert target
    data.replace(1275, 4, gt6b_data.mid(530, 4));  //copy min/max
    data.replace(1279, 1, midiTable->getArrayValue("Tables", "00", "00", "24", gt6b_data.mid(534, 1))); //convert source
    data.replace(1280, 3, gt6b_data.mid(535, 3));  //copy Assign

    data.replace(1288, 1, gt6b_data.mid(550, 1));  //copy Assign 3
    data.replace(1289, 2, midiTable->getArrayValue("Tables", "00", "00", "23", gt6b_data.mid(552, 2))); //convert target
    data.replace(1291, 4, gt6b_data.mid(554, 4));  //copy min/max
    data.replace(1295, 1, midiTable->getArrayValue("Tables", "00", "00", "24", gt6b_data.mid(558, 1))); //convert source
    data.replace(1296, 3, gt6b_data.mid(559, 3));  //copy Assign

    data.replace(1304, 1, gt6b_data.mid(574, 1));  //copy Assign 4
    data.replace(1305, 2, midiTable->getArrayValue("Tables", "00", "00", "23", gt6b_data.mid(576, 2))); //convert target
    data.replace(1307, 4, gt6b_data.mid(578, 4));  //copy min/max
    data.replace(1311, 1, midiTable->getArrayValue("Tables", "00", "00", "24", gt6b_data.mid(582, 1))); //convert source
    data.replace(1312, 3, gt6b_data.mid(583, 3));  //copy Assign

    data.replace(1320, 1, gt6b_data.mid(598, 1));  //copy Assign 5
    data.replace(1321, 2, midiTable->getArrayValue("Tables", "00", "00", "23", gt6b_data.mid(600, 2))); //convert target
    data.replace(1323, 4, gt6b_data.mid(602, 4));  //copy min/max
    data.replace(1327, 1, midiTable->getArrayValue("Tables", "00", "00", "24", gt6b_data.mid(606, 1))); //convert source
    data.replace(1328, 3, gt6b_data.mid(607, 3));  //copy Assign

    data.replace(1336, 1, gt6b_data.mid(622, 1));  //copy Assign 6
    data.replace(1337, 2, midiTable->getArrayValue("Tables", "00", "00", "23", gt6b_data.mid(624, 2))); //convert target
    data.replace(1339, 4, gt6b_data.mid(626, 4));  //copy min/max
    data.replace(1343, 1, midiTable->getArrayValue("Tables", "00", "00", "24", gt6b_data.mid(630, 1))); //convert source
    data.replace(1344, 3, gt6b_data.mid(631, 3));  //copy Assign

    data.replace(1365, 1, gt6b_data.mid(646, 1));  //copy Assign 7
    data.replace(1366, 2, midiTable->getArrayValue("Tables", "00", "00", "23", gt6b_data.mid(648, 2))); //convert target
    data.replace(1368, 4, gt6b_data.mid(650, 4));  //copy min/max
    data.replace(1372, 1, midiTable->getArrayValue("Tables", "00", "00", "24", gt6b_data.mid(654, 1))); //convert source
    data.replace(1373, 3, gt6b_data.mid(655, 3));  //copy Assign

    data.replace(1381, 1, gt6b_data.mid(670, 1));  //copy Assign 8
    data.replace(1382, 2, midiTable->getArrayValue("Tables", "00", "00", "23", gt6b_data.mid(672, 2))); //convert target
    data.replace(1384, 4, gt6b_data.mid(674, 4));  //copy min/max
    data.replace(1388, 1, midiTable->getArrayValue("Tables", "00", "00", "24", gt6b_data.mid(678, 1))); //convert source
    data.replace(1389, 3, gt6b_data.mid(679, 3));  //copy Assign

    translate10Bto100();
}

int sysxWriter::ArrayToInt(QByteArray Array)
{
    bool ok;
    char a = Array.at(0);
    QString val = QString::number(a, 16).toUpper();
    int msb = val.toInt(&ok, 16);

    a = Array.at(1);
    val = QString::number(a, 16).toUpper();
    int lsb = val.toInt(&ok, 16);
    int value = (msb*128) + lsb;

    return value;
}

QByteArray sysxWriter::IntToArray(int value)
{
    QByteArray Array;
    int msb = value/128;
    int lsb = value-(msb*128);
    Array.append(char(msb));
    Array.append(char(lsb));
    return Array;                                  // return two byte QByteArray
}

QByteArray sysxWriter::GetJsonArray(QByteArray text, int pos )
{
    QByteArray null("ul");                                                      // when read later on without "" null = ul
    QByteArray str((char(34)+text+char(34)));
    int start_index = data.indexOf(str, pos)+(text.size()+4);                    //find pointer to start of Json value.
    int end_index = data.indexOf(",", start_index)-start_index-1;                  //find pointer to end of value to get the size of the value.
    QByteArray x = data.mid(start_index , end_index );                           //copy the Json value and return as a QByteArray.
    if(data.mid(start_index, 4).contains(null)) {x.clear(); };
    //if(x.at(0)==(char)34) {x.remove(0, 1); };
    //if(x.contains((char)34)) {x.truncate(x.indexOf((char)34)); };
    return x;
}

QByteArray sysxWriter::GetJsonValue(QByteArray text, int pos )
{
    QByteArray str((char(34)+text+char(34)));
    int start_index = data.indexOf(str, pos)+(text.size()+3); //find pointer to start of Json value.
    int end_index = data.indexOf(",", start_index)-start_index;                  //find pointer to end of value to get the size of the value.
    QByteArray x = data.mid(start_index , end_index );                 //copy the Json value and return as a QByteArray.
    //if(x.at(0)==(char)34) {x.remove(0, 1); };
    //if(x.contains((char)34)) {x.truncate(x.indexOf((char)34)); };
    return x;
}

QByteArray sysxWriter::GetJsonHex(QByteArray text, int pos )
{
    QByteArray str((char(34)+text+char(34)));
    int start_index = data.indexOf(str, pos)+(text.size()+3); //find pointer to start of Json value.
    bool ok;
    int end_index = data.indexOf(",", start_index)-start_index;                  //find pointer to end of value to get the size of the value.
    QByteArray x = data.mid(start_index , end_index );
    //if(x.at(0)==(char)34) {x.remove(0, 1); };
    // if(x.contains((char)34)) {x.truncate(x.indexOf((char)34)); };
    int value =  x.toInt(&ok, 10);
    QByteArray Array;
    Array.append(char(value));
    return Array;                                    // return one byte QByteArray
}

QByteArray sysxWriter::GetJsonString(QByteArray text, int pos )
{
    QByteArray str((char(34)+text+char(34)));
    int start_index = data.indexOf(str, pos)+(text.size()+4); //find pointer to start of Json value.
    int end_index = data.indexOf("]", start_index)-start_index;                  //find pointer to end of value to get the size of the value.
    QByteArray x = data.mid(start_index , end_index );                 //copy the Json value and return as a QByteArray.
     bool ok;
     QByteArray z;
     for(int f=0; f<x.size(); f++){
            QByteArray y;
            x.remove(x.indexOf(char(34)), 1);
            x.remove(x.indexOf(char(34)), 1);
            x.remove(x.indexOf(char(44)), 1);
            y.append(x.at(f)).append(x.at(f+1));
            z.append(char(y.toInt(&ok, 16)));
            f=f+1;
    };
    return z;
}

void sysxWriter::GT100_default_replace(int pos, int size, QByteArray value)
{

    if(!value.isEmpty() && !value.isNull())
    {
        GT100_default.replace(pos, size, value);
    };
}

void sysxWriter::convertFromTSL(int patch)
{
    QFile file(":default.syx");           // Read the default KATANA sysx file .
    if (file.open(QIODevice::ReadOnly))
    {GT100_default = file.readAll(); };

    if(patch>0){
        SysxIO *sysxIO = SysxIO::Instance();
        this->data = sysxIO->data;
        if(this->data.isEmpty())
        {
            QFile file(":default.tsl");           // Read the default Katana sysx file .
            if (file.open(QIODevice::ReadOnly))
            {this->data = file.readAll(); };
        };
    };
    QByteArray temp;
    int pnum;
       if(data.contains("paramSet"))
       {
           mk2 = true;
           pnum = data.count("paramSet"); // mk2 uses paramSet instead of params
       }
       else
       {
           mk2 = false;
           pnum = data.count("params"); // count the number of times 'params' appears to get patch count
       };
     int pindex = data.size()/(pnum+1);
    QString device=GetJsonArray("device", 1);

    int patchCount = pnum;
    if (patchCount>1)
    {
        QString msgText;
        QString patchText;
        QString patchNumber;
        this->patchList.clear();
        this->patchList.append(QObject::tr("Select Patch"));
        int a = 400; // locate patch text start position from the start of the file
        if(mk2){a=127;}; //start of patchset data
        for (int h=0;h<patchCount;h++)
        {
            if(mk2){patchText.append(GetJsonString("UserPatch%PatchName", a));}
            else{patchText.append(GetJsonValue("patchname", a).trimmed().remove(0, 1) ); patchText.chop(1); };

            patchNumber = QString::number(h+1, 10).toUpper();
            msgText.append(patchNumber + " : ");
            msgText.append(patchText + "   ");
            this->patchList.append(msgText);
            patchText.clear();
            msgText.clear();
            a=a+pindex;                      // offset is set in front of marker
        };

        if(patch<1){
            fileDialog *dialog = new fileDialog(fileName, patchList, data, GT100_default, device);
            dialog->exec();
            patchIndex(this->index);
        }else{
            SysxIO *sysxIO = SysxIO::Instance();
            if(patch>patchCount){ patch=patchCount; };
            sysxIO->patchListValue = patch;
            this->index=sysxIO->patchListValue;
        };
    };

    int a=0;
    if (patchCount>1)
    {
        int q=index-1;      // find start of required patch
        a = q*pindex;
    };
    if(device.contains("KATANA MkII"))
        {
            GT100_default_replace(11, 16, GetJsonString("UserPatch%PatchName", a));
            QByteArray z(GetJsonString("UserPatch%Patch_0", a));  //convert JSON string to sysx data
            GT100_default_replace(60, 15, z.mid(0, 15));          // replace Booster
            GT100_default_replace(91, 32, z.mid(16, 32));         // replace Preamp
            GT100_default_replace(200, 1, z.mid(48, 1));          // replace EQ sw
            GT100_default_replace(2412, 1, z.mid(49, 1));         // replace EQ sel
            GT100_default_replace(201, 12, z.mid(50, 12));        // replace EQ para
            GT100_default_replace(2413, 11, z.mid(61, 11));       // replace EQ graphic
            z.clear();

            z.append(GetJsonString("UserPatch%Fx(1)", a));
            GT100_default_replace(216, 2, z.mid(0, 2));          // replace mod sw & type
            GT100_default_replace(228, 7, z.mid(2, 7));          // replace mod twah
            GT100_default_replace(236, 7, z.mid(9, 7));          // replace mod awah
            GT100_default_replace(244, 6, z.mid(16, 6));         // replace mod swah
            GT100_default_replace(251, 5, z.mid(22, 5));         // replace mod acs
            GT100_default_replace(257, 6, z.mid(27, 6));         // replace mod limiter
            GT100_default_replace(264, 11, z.mid(33, 11));       // replace mod geq
            GT100_default_replace(276, 4, z.mid(44, 4));         // replace mod seq
            GT100_default_replace(293, 7, z.mid(48, 7));         // replace mod seq
            GT100_default_replace(307, 5, z.mid(55, 5));         // replace mod gs
            GT100_default_replace(313, 3, z.mid(60, 3));         // replace mod sg
            GT100_default_replace(325, 8, z.mid(63, 8));         // replace mod wsy
            GT100_default_replace(342, 3, z.mid(71, 3));         // replace mod oct
            GT100_default_replace(346, 15, z.mid(74, 15));       // replace mod ps
            GT100_default_replace(362, 35, z.mid(89, 35));       // replace mod hr
            GT100_default_replace(402, 7, z.mid(124, 7));        // replace mod ac
            GT100_default_replace(410, 8, z.mid(131, 8));        // replace mod ph
            GT100_default_replace(419, 2, z.mid(139, 2));        // replace mod fl
            GT100_default_replace(434, 6, z.mid(141, 6));        // replace mod fl
            GT100_default_replace(441, 4, z.mid(147, 4));        // replace mod tr
            GT100_default_replace(446, 7, z.mid(151, 7));        // replace mod rt1
            GT100_default_replace(454, 3, z.mid(158, 3));        // replace mod uv
            GT100_default_replace(465, 5, z.mid(161, 5));        // replace mod sl
            GT100_default_replace(471, 5, z.mid(166, 5));        // replace mod vb
            GT100_default_replace(477, 4, z.mid(171, 4));        // replace mod rm
            GT100_default_replace(482, 8, z.mid(175, 8));        // replace mod hu
            GT100_default_replace(491, 10, z.mid(183, 10));      // replace mod 2ce
            GT100_default_replace(2283, 1, z.mid(196, 1));       // replace mod acgs
            GT100_default_replace(2284, 2, z.mid(194, 2));       // replace mod acgs
            GT100_default_replace(2286, 1, z.mid(193, 1));       // replace mod acgs
            GT100_default_replace(2287, 1, z.mid(197, 1));       // replace mod acgs
            GT100_default_replace(2328, 2, z.mid(198, 2));       // replace mod ph90
            GT100_default_replace(2230, 4, z.mid(200, 4));       // replace mod fl117
            GT100_default_replace(2371, 5, z.mid(204, 5));       // replace mod wah95
            GT100_default_replace(2376, 9, z.mid(209, 9));       // replace mod dc30
            GT100_default_replace(2432, 3, z.mid(218, 3));       // replace mod H Oct
            z.clear();

            z.append(GetJsonString("UserPatch%Fx(2)", a));
            GT100_default_replace(510, 2, z.mid(0, 2));          // replace fx sw & type
            GT100_default_replace(522, 7, z.mid(2, 7));          // replace fx twah
            GT100_default_replace(530, 7, z.mid(9, 7));          // replace fx awah
            GT100_default_replace(538, 6, z.mid(16, 6));         // replace fx swah
            GT100_default_replace(545, 5, z.mid(22, 5));         // replace fx acs
            GT100_default_replace(551, 6, z.mid(27, 6));         // replace fx limiter
            GT100_default_replace(558, 11, z.mid(33, 11));       // replace fx geq
            GT100_default_replace(570, 4, z.mid(44, 4));         // replace fx seq
            GT100_default_replace(587, 7, z.mid(48, 7));         // replace fx seq
            GT100_default_replace(601, 5, z.mid(55, 5));         // replace fx gs
            GT100_default_replace(607, 3, z.mid(60, 3));         // replace fx sg
            GT100_default_replace(619, 8, z.mid(63, 8));         // replace fx wsy
            GT100_default_replace(636, 3, z.mid(71, 3));         // replace fx oc
            GT100_default_replace(640, 15, z.mid(74, 15));       // replace fx ps
            GT100_default_replace(656, 35, z.mid(89, 35));       // replace fx hr
            GT100_default_replace(696, 7, z.mid(124, 7));        // replace fx ac
            GT100_default_replace(704, 8, z.mid(131, 8));        // replace fx ph
            GT100_default_replace(713, 2, z.mid(139, 2));        // replace fx fl
            GT100_default_replace(728, 6, z.mid(141, 6));        // replace fx fl
            GT100_default_replace(735, 4, z.mid(147, 4));        // replace fx tr
            GT100_default_replace(740, 7, z.mid(151, 7));        // replace fx rt1
            GT100_default_replace(748, 3, z.mid(158, 3));        // replace fx uv
            GT100_default_replace(759, 5, z.mid(161, 5));        // replace fx sl
            GT100_default_replace(765, 5, z.mid(166, 5));        // replace fx vb
            GT100_default_replace(771, 4, z.mid(171, 4));        // replace fx rm
            GT100_default_replace(776, 8, z.mid(175, 8));        // replace fx hu
            GT100_default_replace(785, 10, z.mid(183, 10));      // replace fx 2ce
            GT100_default_replace(2298, 1, z.mid(196, 1));       // replace fx acgs
            GT100_default_replace(2299, 2, z.mid(194, 2));       // replace fx acgs
            GT100_default_replace(2301, 1, z.mid(193, 1));       // replace fx acgs
            GT100_default_replace(2302, 1, z.mid(197, 1));       // replace fx acgs
            GT100_default_replace(2334, 2, z.mid(198, 2));       // replace fx ph90
            GT100_default_replace(2336, 4, z.mid(200, 4));       // replace fx fl117
            GT100_default_replace(2385, 5, z.mid(204, 5));       // replace fx wah95
            GT100_default_replace(2390, 5, z.mid(209, 5));       // replace fx dc30
            GT100_default_replace(2408, 4, z.mid(214, 4));       // replace fx dc30
            GT100_default_replace(2435, 3, z.mid(218, 3));       // replace fx H Oct
            z.clear();

            z.append(GetJsonString("UserPatch%Delay(1)", a));
            GT100_default_replace(812, 21, z.mid(0, 21));          // replace delay 1
            GT100_default_replace(2339, 5, z.mid(21, 5));          // replace delay 1
            z.clear();
            z.append(GetJsonString("UserPatch%Delay(2)", a));
            GT100_default_replace(2345, 26, z.mid(0, 26));         // replace delay 2
            z.clear();
            z.append(GetJsonString("UserPatch%Patch_1", a));
            GT100_default_replace(873, 10, z.mid(0, 10));          // replace reverb
            GT100_default_replace(884, 1, z.mid(10, 1));           // replace reverb
            GT100_default_replace(889, 2, z.mid(16, 2));           // replace PedalFx sw
            GT100_default_replace(895, 6, z.mid(18, 6));           // replace PedalFx wah
            GT100_default_replace(892, 4, z.mid(24, 4));           // replace PedalFx pb
            GT100_default_replace(2426, 5, z.mid(28, 5));          // replace PedalFx wah95
            GT100_default_replace(907, 1, z.mid(33, 1));           // replace PedalFx fv
            GT100_default_replace(942, 4, z.mid(34, 4));           // replace SR loop
            GT100_default_replace(956, 3, z.mid(38, 3));           // replace NS
            GT100_default_replace(1014, 1, z.mid(48, 1));          // replace master level
            GT100_default_replace(1022, 1, z.mid(49, 1));          // replace master key
            z.clear();
            z.append(GetJsonString("UserPatch%Patch_2", a));
            GT100_default_replace(2549, 1, z.mid(0, 1));           // replace chain type
            GT100_default_replace(2573, 2, z.mid(1, 2));           // replace chain loop/eq
            GT100_default_replace(2550, 6, z.mid(4, 6));           // replace FX1A FX1B
            GT100_default_replace(2556, 3, z.mid(14, 3));          // replace FX2A Delay1
            GT100_default_replace(2559, 3, z.mid(10, 3));          // replace FX2B Fx
            GT100_default_replace(2562, 3, z.mid(16, 3));          // replace FX3 reverb
            GT100_default_replace(2565, 5, z.mid(25, 5));          // replace FXBOX select
            GT100_default_replace(2575, 3, z.mid(22, 3));          // replace FX3 delay2
            GT100_default_replace(2578, 3, z.mid(19, 3));          // replace FX3 layer
            GT100_default_replace(2581, 3, z.mid(30, 3));          // replace pedal function

           /*GT100_default_replace(944, 18, GetJsonString("UserPatch%Status", a));*/
            GT100_default_replace(2598, 33, GetJsonString("UserPatch%KnobAsgn", a));
            GT100_default_replace(2690, 33, GetJsonString("UserPatch%ExpPedalAsgn", a));
            GT100_default_replace(2722, 76, GetJsonString("UserPatch%ExpPedalAsgnMinMax", a));
            GT100_default_replace(2831, 33, GetJsonString("UserPatch%GafcExp1Asgn", a));
            GT100_default_replace(2863, 76, GetJsonString("UserPatch%GafcExp1AsgnMinMax", a));
            GT100_default_replace(2972, 33, GetJsonString("UserPatch%GafcExp2Asgn", a));
            GT100_default_replace(3004, 76, GetJsonString("UserPatch%GafcExp2AsgnMinMax", a));


            SysxIO *sysxIO = SysxIO::Instance();
            sysxIO->setFileSource("Structure", GT100_default);
            sysxIO->setFileName(this->fileName);
            this->fileSource = sysxIO->getFileSource();
        }
        else if(device.contains("GT"))
    {
        temp.append(char(GetJsonValue("patch_name1", a).toInt()) );  //copy patch name
        temp.append(char(GetJsonValue("patch_name2", a).toInt()) );  //copy patch name
        temp.append(char(GetJsonValue("patch_name3", a).toInt()) );  //copy patch name
        temp.append(char(GetJsonValue("patch_name4", a).toInt()) );  //copy patch name
        temp.append(char(GetJsonValue("patch_name5", a).toInt()) );  //copy patch name
        temp.append(char(GetJsonValue("patch_name6", a).toInt()) );  //copy patch name
        temp.append(char(GetJsonValue("patch_name7", a).toInt()) );  //copy patch name
        temp.append(char(GetJsonValue("patch_name8", a).toInt()) );  //copy patch name
        temp.append(char(GetJsonValue("patch_name9", a).toInt()) );  //copy patch name
        temp.append(char(GetJsonValue("patch_name10", a).toInt()) ); //copy patch name
        temp.append(char(GetJsonValue("patch_name11", a).toInt()) ); //copy patch name
        temp.append(char(GetJsonValue("patch_name12", a).toInt()) ); //copy patch name
        temp.append(char(GetJsonValue("patch_name13", a).toInt()) ); //copy patch name
        temp.append(char(GetJsonValue("patch_name14", a).toInt()) ); //copy patch name
        temp.append(char(GetJsonValue("patch_name15", a).toInt()) ); //copy patch name
        temp.append(char(GetJsonValue("patch_name16", a).toInt()) ); //copy patch name
        GT100_default_replace(11, 16, temp );                        //copy patch name
        GT100_default_replace(59, 1, GetJsonHex("od_ds_on_off", a));              //copy dist
        GT100_default_replace(60, 1, GetJsonHex("od_ds_type", a));                //copy dist
        GT100_default_replace(61, 1, GetJsonHex("od_ds_drive", a));               //copy dist
        GT100_default_replace(62, 1, GetJsonHex("od_ds_bottom", a));              //copy dist
        GT100_default_replace(63, 1, GetJsonHex("od_ds_tone", a));                //copy dist
        GT100_default_replace(64, 1, GetJsonHex("od_ds_solo_sw", a));             //copy dist
        GT100_default_replace(65, 1, GetJsonHex("od_ds_solo_level", a));          //copy dist
        GT100_default_replace(66, 1, GetJsonHex("od_ds_effect_level", a));        //copy dist
        GT100_default_replace(67, 1, GetJsonHex("od_ds_direct_mix", a));          //copy dist
        GT100_default_replace(68, 1, GetJsonHex("od_ds_custom_type", a));         //copy dist
        GT100_default_replace(69, 1, GetJsonHex("od_ds_custom_bottom", a));       //copy dist
        GT100_default_replace(70, 1, GetJsonHex("od_ds_custom_top", a));          //copy dist
        GT100_default_replace(71, 1, GetJsonHex("od_ds_custom_low", a));          //copy dist
        GT100_default_replace(72, 1, GetJsonHex("od_ds_custom_high", a));         //copy dist
        GT100_default_replace(73, 1, GetJsonHex("od_ds_custom_character", a));    //copy dist
        GT100_default_replace(91, 1, GetJsonHex("preamp_a_on_off", a));                 //copy pre A
        GT100_default_replace(92, 1, GetJsonHex("preamp_a_type", a));                   //copy pre A
        GT100_default_replace(93, 1, GetJsonHex("preamp_a_gain", a));                   //copy pre A
        GT100_default_replace(94, 1, GetJsonHex("preamp_a_t_comp", a));                 //copy pre A
        GT100_default_replace(95, 1, GetJsonHex("preamp_a_bass", a));                   //copy pre A
        GT100_default_replace(96, 1, GetJsonHex("preamp_a_middle", a));                 //copy pre A
        GT100_default_replace(97, 1, GetJsonHex("preamp_a_treble", a));                 //copy pre A
        GT100_default_replace(98, 1, GetJsonHex("preamp_a_presence", a));               //copy pre A
        GT100_default_replace(99, 1, GetJsonHex("preamp_a_level", a));                  //copy pre A
        GT100_default_replace(100, 1, GetJsonHex("preamp_a_bright", a));                //copy pre A
        GT100_default_replace(101, 1, GetJsonHex("preamp_a_gain_sw", a));               //copy pre A
        GT100_default_replace(110, 1, GetJsonHex("preamp_a_custom_type", a));           //copy pre A
        GT100_default_replace(111, 1, GetJsonHex("preamp_a_custom_bottom", a));         //copy pre A
        GT100_default_replace(112, 1, GetJsonHex("preamp_a_custom_edge", a));           //copy pre A
        GT100_default_replace(115, 1, GetJsonHex("preamp_a_custom_preamp_low", a));     //copy pre A
        GT100_default_replace(116, 1, GetJsonHex("preamp_a_custom_preamp_high", a));    //copy pre A
        GT100_default_replace(117, 1, GetJsonHex("preamp_a_custom_char", a));           //copy pre A
        GT100_default_replace(200, 1, GetJsonHex("eq_on_off", a));                 //copy EQ
        GT100_default_replace(201, 1, GetJsonHex("eq_low_cut", a));                //copy EQ
        GT100_default_replace(202, 1, GetJsonHex("eq_low_gain", a));               //copy EQ
        GT100_default_replace(203, 1, GetJsonHex("eq_low_mid_freq", a));           //copy EQ
        GT100_default_replace(204, 1, GetJsonHex("eq_low_mid_q", a));              //copy EQ
        GT100_default_replace(205, 1, GetJsonHex("eq_low_mid_gain", a));           //copy EQ
        GT100_default_replace(206, 1, GetJsonHex("eq_high_mid_freq", a));          //copy EQ
        GT100_default_replace(207, 1, GetJsonHex("eq_high_mid_q", a));             //copy EQ
        GT100_default_replace(208, 1, GetJsonHex("eq_high_mid_gain", a));          //copy EQ
        GT100_default_replace(209, 1, GetJsonHex("eq_high_gain", a));              //copy EQ
        GT100_default_replace(210, 1, GetJsonHex("eq_high_cut", a));               //copy EQ
        GT100_default_replace(211, 1, GetJsonHex("eq_level", a));                  //copy EQ
        GT100_default_replace(216, 1, GetJsonHex("fx1_on_off", a));                     //copy FX1
        GT100_default_replace(217, 1, GetJsonHex("fx1_fx_type", a));                    //copy FX1
        GT100_default_replace(218, 1, GetJsonHex("fx1_sub_od_ds_type", a));             //copy FX1
        GT100_default_replace(219, 1, GetJsonHex("fx1_sub_od_ds_drive", a));            //copy FX1
        GT100_default_replace(220, 1, GetJsonHex("fx1_sub_od_ds_bottom", a));           //copy FX1
        GT100_default_replace(221, 1, GetJsonHex("fx1_sub_od_ds_tone", a));             //copy FX1
        GT100_default_replace(222, 1, GetJsonHex("fx1_sub_od_ds_solo_sw", a));          //copy FX1
        GT100_default_replace(223, 1, GetJsonHex("fx1_sub_od_ds_solo_level", a));       //copy FX1
        GT100_default_replace(224, 1, GetJsonHex("fx1_sub_od_ds_effect_level", a));     //copy FX1
        GT100_default_replace(225, 1, GetJsonHex("fx1_sub_od_ds_direct_mix", a));       //copy FX1
        GT100_default_replace(228, 1, GetJsonHex("fx1_t_wah_mode", a));                 //copy FX1
        GT100_default_replace(229, 1, GetJsonHex("fx1_t_wah_polar", a));                //copy FX1
        GT100_default_replace(230, 1, GetJsonHex("fx1_t_wah_sens", a));                 //copy FX1
        GT100_default_replace(231, 1, GetJsonHex("fx1_t_wah_freq", a));                 //copy FX1
        GT100_default_replace(232, 1, GetJsonHex("fx1_t_wah_peak", a));                 //copy FX1
        GT100_default_replace(233, 1, GetJsonHex("fx1_t_wah_direct_mix", a));           //copy FX1
        GT100_default_replace(234, 1, GetJsonHex("fx1_t_wah_effect_level", a));         //copy FX1
        GT100_default_replace(236, 1, GetJsonHex("fx1_auto_wah_mode", a));              //copy FX1
        GT100_default_replace(237, 1, GetJsonHex("fx1_auto_wah_freq", a));              //copy FX1
        GT100_default_replace(238, 1, GetJsonHex("fx1_auto_wah_peak", a));              //copy FX1
        GT100_default_replace(239, 1, GetJsonHex("fx1_auto_wah_rate", a));              //copy FX1
        GT100_default_replace(240, 1, GetJsonHex("fx1_auto_wah_depth", a));             //copy FX1
        GT100_default_replace(241, 1, GetJsonHex("fx1_auto_wah_direct_mix", a));        //copy FX1
        GT100_default_replace(242, 1, GetJsonHex("fx1_auto_wah_effect_level", a));      //copy FX1
        GT100_default_replace(245, 1, GetJsonHex("fx1_sub_wah_type", a));               //copy FX1
        GT100_default_replace(246, 1, GetJsonHex("fx1_sub_wah_pedal_pos", a));          //copy FX1
        GT100_default_replace(247, 1, GetJsonHex("fx1_sub_wah_pedal_min", a));          //copy FX1
        GT100_default_replace(248, 1, GetJsonHex("fx1_sub_wah_pedal_max", a));          //copy FX1
        GT100_default_replace(249, 1, GetJsonHex("fx1_sub_wah_effect_level", a));       //copy FX1
        GT100_default_replace(250, 1, GetJsonHex("fx1_sub_wah_direct_mix", a));         //copy FX1
        GT100_default_replace(251, 1, GetJsonHex("fx1_adv_comp_type", a));              //copy FX1
        GT100_default_replace(252, 1, GetJsonHex("fx1_adv_comp_sustain", a));           //copy FX1
        GT100_default_replace(253, 1, GetJsonHex("fx1_adv_comp_attack", a));            //copy FX1
        GT100_default_replace(254, 1, GetJsonHex("fx1_adv_comp_tone", a));              //copy FX1
        GT100_default_replace(255, 1, GetJsonHex("fx1_adv_comp_level", a));             //copy FX1
        GT100_default_replace(257, 1, GetJsonHex("fx1_limiter_type", a));               //copy FX1
        GT100_default_replace(258, 1, GetJsonHex("fx1_limiter_attack", a));             //copy FX1
        GT100_default_replace(259, 1, GetJsonHex("fx1_limiter_thresh", a));             //copy FX1
        GT100_default_replace(260, 1, GetJsonHex("fx1_limiter_ratio", a));              //copy FX1
        GT100_default_replace(261, 1, GetJsonHex("fx1_limiter_release", a));            //copy FX1
        GT100_default_replace(262, 1, GetJsonHex("fx1_limiter_level", a));              //copy FX1
        GT100_default_replace(264, 1, GetJsonHex("fx1_graphic_eq_31hz", a));            //copy FX1
        GT100_default_replace(265, 1, GetJsonHex("fx1_graphic_eq_62hz", a));            //copy FX1
        GT100_default_replace(266, 1, GetJsonHex("fx1_graphic_eq_125hz", a));                  //copy FX1
        GT100_default_replace(267, 1, GetJsonHex("fx1_graphic_eq_250hz", a));                  //copy FX1
        GT100_default_replace(268, 1, GetJsonHex("fx1_graphic_eq_500hz", a));                  //copy FX1
        GT100_default_replace(269, 1, GetJsonHex("fx1_graphic_eq_1khz", a));                  //copy FX1
        GT100_default_replace(270, 1, GetJsonHex("fx1_graphic_eq_2khz", a));                  //copy FX1
        GT100_default_replace(271, 1, GetJsonHex("fx1_graphic_eq_4khz", a));                  //copy FX1
        GT100_default_replace(272, 1, GetJsonHex("fx1_graphic_eq_8khz", a));                  //copy FX1
        GT100_default_replace(273, 1, GetJsonHex("fx1_graphic_eq_16khz", a));                  //copy FX1
        GT100_default_replace(274, 1, GetJsonHex("fx1_graphic_eq_level", a));                  //copy FX1
        GT100_default_replace(276, 1, GetJsonHex("fx1_parametric_eq_low_cut", a));                  //copy FX1
        GT100_default_replace(277, 1, GetJsonHex("fx1_parametric_eq_low_gain", a));                  //copy FX1
        GT100_default_replace(278, 1, GetJsonHex("fx1_parametric_eq_low_mid_freq", a));                  //copy FX1
        GT100_default_replace(279, 1, GetJsonHex("fx1_parametric_eq_low_mid_q", a));                  //copy FX1
        GT100_default_replace(293, 1, GetJsonHex("fx1_parametric_eq_low_mid_gain", a));                  //copy FX1
        GT100_default_replace(294, 1, GetJsonHex("fx1_parametric_eq_high_mid_freq", a));                  //copy FX1
        GT100_default_replace(295, 1, GetJsonHex("fx1_parametric_eq_high_mid_q", a));                  //copy FX1
        GT100_default_replace(296, 1, GetJsonHex("fx1_parametric_eq_high_mid_gain", a));                  //copy FX1
        GT100_default_replace(297, 1, GetJsonHex("fx1_parametric_eq_high_gain", a));                  //copy FX1
        GT100_default_replace(298, 1, GetJsonHex("fx1_parametric_eq_high_cut", a));                  //copy FX1
        GT100_default_replace(299, 1, GetJsonHex("fx1_parametric_eq_level", a));                  //copy FX1
        GT100_default_replace(301, 1, GetJsonHex("fx1_tone_modify_type", a));                  //copy FX1
        GT100_default_replace(302, 1, GetJsonHex("fx1_tone_modify_reso", a));                  //copy FX1
        GT100_default_replace(303, 1, GetJsonHex("fx1_tone_modify_low", a));                  //copy FX1
        GT100_default_replace(304, 1, GetJsonHex("fx1_tone_modify_high", a));                  //copy FX1
        GT100_default_replace(305, 1, GetJsonHex("fx1_tone_modify_level", a));                  //copy FX1
        GT100_default_replace(307, 1, GetJsonHex("fx1_guitar_sim_type", a));                  //copy FX1
        GT100_default_replace(308, 1, GetJsonHex("fx1_guitar_sim_low", a));                  //copy FX1
        GT100_default_replace(309, 1, GetJsonHex("fx1_guitar_sim_high", a));                  //copy FX1
        GT100_default_replace(310, 1, GetJsonHex("fx1_guitar_sim_level", a));                  //copy FX1
        GT100_default_replace(311, 1, GetJsonHex("fx1_guitar_sim_body", a));                  //copy FX1
        GT100_default_replace(313, 1, GetJsonHex("fx1_slow_gear_sens", a));                  //copy FX1
        GT100_default_replace(314, 1, GetJsonHex("fx1_slow_gear_rise_time", a));                  //copy FX1
        GT100_default_replace(315, 1, GetJsonHex("fx1_slow_gear_level", a));                  //copy FX1
        GT100_default_replace(317, 1, GetJsonHex("fx1_defretter_tone", a));                  //copy FX1
        GT100_default_replace(318, 1, GetJsonHex("fx1_defretter_sens", a));                  //copy FX1
        GT100_default_replace(319, 1, GetJsonHex("fx1_defretter_attack", a));                  //copy FX1
        GT100_default_replace(320, 1, GetJsonHex("fx1_defretter_depth", a));                  //copy FX1
        GT100_default_replace(321, 1, GetJsonHex("fx1_defretter_reso", a));                  //copy FX1
        GT100_default_replace(322, 1, GetJsonHex("fx1_defretter_effect_level", a));                  //copy FX1
        GT100_default_replace(323, 1, GetJsonHex("fx1_defretter_direct_mix", a));                  //copy FX1
        GT100_default_replace(325, 1, GetJsonHex("fx1_wave_synth_wave", a));                  //copy FX1
        GT100_default_replace(326, 1, GetJsonHex("fx1_wave_synth_cutoff", a));                  //copy FX1
        GT100_default_replace(327, 1, GetJsonHex("fx1_wave_synth_reso", a));                  //copy FX1
        GT100_default_replace(328, 1, GetJsonHex("fx1_wave_synth_filter_sens", a));                  //copy FX1
        GT100_default_replace(329, 1, GetJsonHex("fx1_wave_synth_filter_decay", a));                  //copy FX1
        GT100_default_replace(330, 1, GetJsonHex("fx1_wave_synth_filter_depth", a));                  //copy FX1
        GT100_default_replace(331, 1, GetJsonHex("fx1_wave_synth_synth_level", a));                  //copy FX1
        GT100_default_replace(332, 1, GetJsonHex("fx1_wave_synth_direct_mix", a));                  //copy FX1
        GT100_default_replace(334, 1, GetJsonHex("fx1_sitar_sim_tone", a));                  //copy FX1
        GT100_default_replace(335, 1, GetJsonHex("fx1_sitar_sim_sens", a));                  //copy FX1
        GT100_default_replace(336, 1, GetJsonHex("fx1_sitar_sim_depth", a));                  //copy FX1
        GT100_default_replace(337, 1, GetJsonHex("fx1_sitar_sim_reso", a));                  //copy FX1
        GT100_default_replace(338, 1, GetJsonHex("fx1_sitar_sim_buzz", a));                  //copy FX1
        GT100_default_replace(339, 1, GetJsonHex("fx1_sitar_sim_effect_level", a));                  //copy FX1
        GT100_default_replace(340, 1, GetJsonHex("fx1_sitar_sim_direct_mix", a));                  //copy FX1
        GT100_default_replace(342, 1, GetJsonHex("fx1_octave_range", a));                  //copy FX1
        GT100_default_replace(343, 1, GetJsonHex("fx1_octave_level", a));                  //copy FX1
        GT100_default_replace(344, 1, GetJsonHex("fx1_octave_direct_mix", a));                  //copy FX1
        GT100_default_replace(346, 1, GetJsonHex("fx1_pitch_shifter_voice", a));                  //copy FX1
        GT100_default_replace(347, 1, GetJsonHex("fx1_pitch_shifter_ps1mode", a));                  //copy FX1
        GT100_default_replace(348, 1, GetJsonHex("fx1_pitch_shifter_ps1pitch", a));                  //copy FX1
        GT100_default_replace(349, 1, GetJsonHex("fx1_pitch_shifter_ps1fine", a));                  //copy FX1
        GT100_default_replace(350, 1, GetJsonHex("fx1_pitch_shifter_ps1pre_dly_h", a));                  //copy FX1
        GT100_default_replace(351, 1, GetJsonHex("fx1_pitch_shifter_ps1pre_dly_l", a));                  //copy FX1
        GT100_default_replace(352, 1, GetJsonHex("fx1_pitch_shifter_ps1level", a));                  //copy FX1
        GT100_default_replace(353, 1, GetJsonHex("fx1_pitch_shifter_ps2mode", a));                  //copy FX1
        GT100_default_replace(354, 1, GetJsonHex("fx1_pitch_shifter_ps2pitch", a));                  //copy FX1
        GT100_default_replace(355, 1, GetJsonHex("fx1_pitch_shifter_ps2fine", a));                  //copy FX1
        GT100_default_replace(356, 1, GetJsonHex("fx1_pitch_shifter_ps2pre_dly_h", a));                  //copy FX1
        GT100_default_replace(357, 1, GetJsonHex("fx1_pitch_shifter_ps2pre_dly_l", a));                  //copy FX1
        GT100_default_replace(358, 1, GetJsonHex("fx1_pitch_shifter_ps2level", a));                  //copy FX1
        GT100_default_replace(359, 1, GetJsonHex("fx1_pitch_shifter_ps1f_back", a));                  //copy FX1
        GT100_default_replace(360, 1, GetJsonHex("fx1_pitch_shifter_direct_mix", a));                  //copy FX1
        GT100_default_replace(362, 1, GetJsonHex("fx1_harmonist_voice", a));                  //copy FX1
        GT100_default_replace(363, 1, GetJsonHex("fx1_harmonist_hr1harm", a));                  //copy FX1
        GT100_default_replace(364, 1, GetJsonHex("fx1_harmonist_hr1pre_dly_h", a));                  //copy FX1
        GT100_default_replace(365, 1, GetJsonHex("fx1_harmonist_hr1pre_dly_l", a));                  //copy FX1
        GT100_default_replace(366, 1, GetJsonHex("fx1_harmonist_hr1level", a));                  //copy FX1
        GT100_default_replace(367, 1, GetJsonHex("fx1_harmonist_hr2harm", a));                  //copy FX1
        GT100_default_replace(368, 1, GetJsonHex("fx1_harmonist_hr2pre_dly_h", a));                  //copy FX1
        GT100_default_replace(369, 1, GetJsonHex("fx1_harmonist_hr2pre_dly_l", a));                  //copy FX1
        GT100_default_replace(370, 1, GetJsonHex("fx1_harmonist_hr2level", a));                  //copy FX1
        GT100_default_replace(371, 1, GetJsonHex("fx1_harmonist_hr1f_back", a));                  //copy FX1
        GT100_default_replace(372, 1, GetJsonHex("fx1_harmonist_direct_mix", a));                  //copy FX1
        GT100_default_replace(373, 1, GetJsonHex("fx1_harmonist_hr1c", a));                  //copy FX1
        GT100_default_replace(274, 1, GetJsonHex("fx1_harmonist_hr1db", a));                  //copy FX1
        GT100_default_replace(375, 1, GetJsonHex("fx1_harmonist_hr1d", a));                  //copy FX1
        GT100_default_replace(376, 1, GetJsonHex("fx1_harmonist_hr1eb", a));                  //copy FX1
        GT100_default_replace(377, 1, GetJsonHex("fx1_harmonist_hr1e", a));                  //copy FX1
        GT100_default_replace(378, 1, GetJsonHex("fx1_harmonist_hr1f", a));                  //copy FX1
        GT100_default_replace(379, 1, GetJsonHex("fx1_harmonist_hr1f_s", a));                  //copy FX1
        GT100_default_replace(380, 1, GetJsonHex("fx1_harmonist_hr1g", a));                  //copy FX1
        GT100_default_replace(381, 1, GetJsonHex("fx1_harmonist_hr1ab", a));                  //copy FX1
        GT100_default_replace(382, 1, GetJsonHex("fx1_harmonist_hr1a", a));                  //copy FX1
        GT100_default_replace(383, 1, GetJsonHex("fx1_harmonist_hr1bb", a));                  //copy FX1
        GT100_default_replace(384, 1, GetJsonHex("fx1_harmonist_hr1b", a));                  //copy FX1
        GT100_default_replace(385, 1, GetJsonHex("fx1_harmonist_hr2c", a));                  //copy FX1
        GT100_default_replace(386, 1, GetJsonHex("fx1_harmonist_hr2db", a));                  //copy FX1
        GT100_default_replace(387, 1, GetJsonHex("fx1_harmonist_hr2d", a));                  //copy FX1
        GT100_default_replace(388, 1, GetJsonHex("fx1_harmonist_hr2eb", a));                  //copy FX1
        GT100_default_replace(389, 1, GetJsonHex("fx1_harmonist_hr2e", a));                  //copy FX1
        GT100_default_replace(390, 1, GetJsonHex("fx1_harmonist_hr2f", a));                  //copy FX1
        GT100_default_replace(391, 1, GetJsonHex("fx1_harmonist_hr2f_s", a));                  //copy FX1
        GT100_default_replace(392, 1, GetJsonHex("fx1_harmonist_hr2g", a));                  //copy FX1
        GT100_default_replace(393, 1, GetJsonHex("fx1_harmonist_hr2ab", a));                  //copy FX1
        GT100_default_replace(394, 1, GetJsonHex("fx1_harmonist_hr2a", a));                  //copy FX1
        GT100_default_replace(395, 1, GetJsonHex("fx1_harmonist_hr2bb", a));                  //copy FX1
        GT100_default_replace(396, 1, GetJsonHex("fx1_harmonist_hr2b", a));                  //copy FX1
        GT100_default_replace(398, 1, GetJsonHex("fx1_sound_hold_hold", a));                  //copy FX1
        GT100_default_replace(399, 1, GetJsonHex("fx1_sound_hold_rise_time", a));                  //copy FX1
        GT100_default_replace(400, 1, GetJsonHex("fx1_sound_hold_effect_level", a));                  //copy FX1
        GT100_default_replace(402, 1, GetJsonHex("fx1_ac_processor_type", a));                  //copy FX1
        GT100_default_replace(403, 1, GetJsonHex("fx1_ac_processor_bass", a));                  //copy FX1
        GT100_default_replace(404, 1, GetJsonHex("fx1_ac_processor_middle", a));                  //copy FX1
        GT100_default_replace(405, 1, GetJsonHex("fx1_ac_processor_middle_freq", a));                  //copy FX1
        GT100_default_replace(406, 1, GetJsonHex("fx1_ac_processor_treble", a));                  //copy FX1
        GT100_default_replace(407, 1, GetJsonHex("fx1_ac_processor_presence", a));                  //copy FX1
        GT100_default_replace(408, 1, GetJsonHex("fx1_ac_processor_level", a));                  //copy FX1
        GT100_default_replace(410, 1, GetJsonHex("fx1_phaser_type", a));                  //copy FX1
        GT100_default_replace(411, 1, GetJsonHex("fx1_phaser_rate", a));                  //copy FX1
        GT100_default_replace(412, 1, GetJsonHex("fx1_phaser_depth", a));                  //copy FX1
        GT100_default_replace(413, 1, GetJsonHex("fx1_phaser_manual", a));                  //copy FX1
        GT100_default_replace(414, 1, GetJsonHex("fx1_phaser_reso", a));                  //copy FX1
        GT100_default_replace(415, 1, GetJsonHex("fx1_phaser_step_rate", a));                  //copy FX1
        GT100_default_replace(416, 1, GetJsonHex("fx1_phaser_effect_level", a));                  //copy FX1
        GT100_default_replace(417, 1, GetJsonHex("fx1_phaser_direct_mix", a));                  //copy FX1
        GT100_default_replace(419, 1, GetJsonHex("fx1_flanger_rate", a));                  //copy FX1
        GT100_default_replace(420, 1, GetJsonHex("fx1_flanger_depth", a));                  //copy FX1
        GT100_default_replace(434, 1, GetJsonHex("fx1_flanger_manual", a));                  //copy FX1
        GT100_default_replace(435, 1, GetJsonHex("fx1_flanger_reso", a));                  //copy FX1
        GT100_default_replace(436, 1, GetJsonHex("fx1_flanger_separation", a));                  //copy FX1
        GT100_default_replace(437, 1, GetJsonHex("fx1_flanger_low_cut", a));                  //copy FX1
        GT100_default_replace(438, 1, GetJsonHex("fx1_flanger_effect_level", a));                  //copy FX1
        GT100_default_replace(439, 1, GetJsonHex("fx1_flanger_direct_mix", a));                  //copy FX1
        GT100_default_replace(441, 1, GetJsonHex("fx1_tremolo_wave_shape", a));                  //copy FX1
        GT100_default_replace(442, 1, GetJsonHex("fx1_tremolo_rate", a));                  //copy FX1
        GT100_default_replace(443, 1, GetJsonHex("fx1_tremolo_depth", a));                  //copy FX1
        GT100_default_replace(444, 1, GetJsonHex("fx1_tremolo_level", a));                  //copy FX1
        GT100_default_replace(446, 1, GetJsonHex("fx1_rotary_speed_select", a));                  //copy FX1
        GT100_default_replace(447, 1, GetJsonHex("fx1_rotary_rate_slow", a));                  //copy FX1
        GT100_default_replace(448, 1, GetJsonHex("fx1_rotary_rate_fast", a));                  //copy FX1
        GT100_default_replace(449, 1, GetJsonHex("fx1_rotary_rise_time", a));                  //copy FX1
        GT100_default_replace(450, 1, GetJsonHex("fx1_rotary_fall_time", a));                  //copy FX1
        GT100_default_replace(451, 1, GetJsonHex("fx1_rotary_depth", a));                  //copy FX1
        GT100_default_replace(452, 1, GetJsonHex("fx1_rotary_level", a));                  //copy FX1
        GT100_default_replace(454, 1, GetJsonHex("fx1_uni_v_rate", a));                  //copy FX1
        GT100_default_replace(455, 1, GetJsonHex("fx1_uni_v_depth", a));                  //copy FX1
        GT100_default_replace(456, 1, GetJsonHex("fx1_uni_v_level", a));                  //copy FX1
        GT100_default_replace(458, 1, GetJsonHex("fx1_pan_type", a));                  //copy FX1
        GT100_default_replace(459, 1, GetJsonHex("fx1_pan_pos", a));                  //copy FX1
        GT100_default_replace(460, 1, GetJsonHex("fx1_pan_wave_shape", a));                  //copy FX1
        GT100_default_replace(461, 1, GetJsonHex("fx1_pan_rate", a));                  //copy FX1
        GT100_default_replace(462, 1, GetJsonHex("fx1_pan_depth", a));                  //copy FX1
        GT100_default_replace(463, 1, GetJsonHex("fx1_pan_level", a));                  //copy FX1
        GT100_default_replace(465, 1, GetJsonHex("fx1_slicer_pattern", a));                  //copy FX1
        GT100_default_replace(466, 1, GetJsonHex("fx1_slicer_rate", a));                  //copy FX1
        GT100_default_replace(467, 1, GetJsonHex("fx1_slicer_trigger_sens", a));                  //copy FX1
        GT100_default_replace(468, 1, GetJsonHex("fx1_slicer_effect_level", a));                  //copy FX1
        GT100_default_replace(469, 1, GetJsonHex("fx1_slicer_direct_mix", a));                  //copy FX1
        GT100_default_replace(471, 1, GetJsonHex("fx1_vibrato_rate", a));                  //copy FX1
        GT100_default_replace(472, 1, GetJsonHex("fx1_vibrato_depth", a));                  //copy FX1
        GT100_default_replace(473, 1, GetJsonHex("fx1_vibrato_trigger", a));                  //copy FX1
        GT100_default_replace(474, 1, GetJsonHex("fx1_vibrato_rise_time", a));                  //copy FX1
        GT100_default_replace(475, 1, GetJsonHex("fx1_vibrato_level", a));                  //copy FX1
        GT100_default_replace(477, 1, GetJsonHex("fx1_ring_mod_mode", a));                  //copy FX1
        GT100_default_replace(478, 1, GetJsonHex("fx1_ring_mod_freq", a));                  //copy FX1
        GT100_default_replace(479, 1, GetJsonHex("fx1_ring_mod_effect_level", a));                  //copy FX1
        GT100_default_replace(480, 1, GetJsonHex("fx1_ring_mod_direct_mix", a));                  //copy FX1
        GT100_default_replace(482, 1, GetJsonHex("fx1_humanizer_mode", a));                  //copy FX1
        GT100_default_replace(483, 1, GetJsonHex("fx1_humanizer_vowel1", a));                  //copy FX1
        GT100_default_replace(484, 1, GetJsonHex("fx1_humanizer_vowel2", a));                  //copy FX1
        GT100_default_replace(485, 1, GetJsonHex("fx1_humanizer_sens", a));                  //copy FX1
        GT100_default_replace(486, 1, GetJsonHex("fx1_humanizer_rate", a));                  //copy FX1
        GT100_default_replace(487, 1, GetJsonHex("fx1_humanizer_depth", a));                  //copy FX1
        GT100_default_replace(488, 1, GetJsonHex("fx1_humanizer_manual", a));                  //copy FX1
        GT100_default_replace(489, 1, GetJsonHex("fx1_humanizer_level", a));                  //copy FX1
        GT100_default_replace(491, 1, GetJsonHex("fx1_2x2_chorus_xover_freq", a));                  //copy FX1
        GT100_default_replace(492, 1, GetJsonHex("fx1_2x2_chorus_low_rate", a));                  //copy FX1
        GT100_default_replace(493, 1, GetJsonHex("fx1_2x2_chorus_low_depth", a));                  //copy FX1
        GT100_default_replace(494, 1, GetJsonHex("fx1_2x2_chorus_low_pre_delay", a));                  //copy FX1
        GT100_default_replace(495, 1, GetJsonHex("fx1_2x2_chorus_low_level", a));                  //copy FX1
        GT100_default_replace(496, 1, GetJsonHex("fx1_2x2_chorus_high_rate", a));                  //copy FX1
        GT100_default_replace(497, 1, GetJsonHex("fx1_2x2_chorus_high_depth", a));                  //copy FX1
        GT100_default_replace(498, 1, GetJsonHex("fx1_2x2_chorus_high_pre_delay", a));                  //copy FX1
        GT100_default_replace(499, 1, GetJsonHex("fx1_2x2_chorus_high_level", a));                  //copy FX1
        GT100_default_replace(500, 1, GetJsonHex("fx1_2x2_chorus_direct_level", a));                  //copy FX1
        GT100_default_replace(501, 1, GetJsonHex("fx1_sub_delay_type", a));                  //copy FX1
        GT100_default_replace(502, 1, GetJsonHex("fx1_sub_delay_time_h", a));                  //copy FX1
        GT100_default_replace(503, 1, GetJsonHex("fx1_sub_delay_time_l", a));                  //copy FX1
        GT100_default_replace(504, 1, GetJsonHex("fx1_sub_delay_f_back", a));                  //copy FX1
        GT100_default_replace(505, 1, GetJsonHex("fx1_sub_delay_high_cut", a));                  //copy FX1
        GT100_default_replace(506, 1, GetJsonHex("fx1_sub_delay_effect_level", a));                  //copy FX1
        GT100_default_replace(507, 1, GetJsonHex("fx1_sub_delay_direct_mix", a));                  //copy FX1
        GT100_default_replace(508, 1, GetJsonHex("fx1_sub_delay_tap_time", a));                  //copy FX1
        GT100_default_replace(510, 1, GetJsonHex("fx2_on_off", a));                  //copy FX2
        GT100_default_replace(511, 1, GetJsonHex("fx2_fx_type", a));                  //copy FX2
        GT100_default_replace(512, 1, GetJsonHex("fx2_sub_od_ds_type", a));                  //copy FX2
        GT100_default_replace(513, 1, GetJsonHex("fx2_sub_od_ds_drive", a));                  //copy FX2
        GT100_default_replace(514, 1, GetJsonHex("fx2_sub_od_ds_bottom", a));                  //copy FX2
        GT100_default_replace(515, 1, GetJsonHex("fx2_sub_od_ds_tone", a));                  //copy FX2
        GT100_default_replace(516, 1, GetJsonHex("fx2_sub_od_ds_solo_sw", a));                  //copy FX2
        GT100_default_replace(517, 1, GetJsonHex("fx2_sub_od_ds_solo_level", a));                  //copy FX2
        GT100_default_replace(518, 1, GetJsonHex("fx2_sub_od_ds_effect_level", a));                  //copy FX2
        GT100_default_replace(519, 1, GetJsonHex("fx2_sub_od_ds_direct_mix", a));                  //copy FX2
        GT100_default_replace(522, 1, GetJsonHex("fx2_t_wah_mode", a));                  //copy FX2
        GT100_default_replace(523, 1, GetJsonHex("fx2_t_wah_polar", a));                  //copy FX2
        GT100_default_replace(524, 1, GetJsonHex("fx2_t_wah_sens", a));                  //copy FX2
        GT100_default_replace(525, 1, GetJsonHex("fx2_t_wah_freq", a));                  //copy FX2
        GT100_default_replace(526, 1, GetJsonHex("fx2_t_wah_peak", a));                  //copy FX2
        GT100_default_replace(527, 1, GetJsonHex("fx2_t_wah_direct_mix", a));                  //copy FX2
        GT100_default_replace(528, 1, GetJsonHex("fx2_t_wah_effect_level", a));                  //copy FX2
        GT100_default_replace(530, 1, GetJsonHex("fx2_auto_wah_mode", a));                  //copy FX2
        GT100_default_replace(531, 1, GetJsonHex("fx2_auto_wah_freq", a));                  //copy FX2
        GT100_default_replace(532, 1, GetJsonHex("fx2_auto_wah_peak", a));                  //copy FX2
        GT100_default_replace(533, 1, GetJsonHex("fx2_auto_wah_rate", a));                  //copy FX2
        GT100_default_replace(534, 1, GetJsonHex("fx2_auto_wah_depth", a));                  //copy FX2
        GT100_default_replace(535, 1, GetJsonHex("fx2_auto_wah_direct_mix", a));                  //copy FX2
        GT100_default_replace(536, 1, GetJsonHex("fx2_auto_wah_effect_level", a));                  //copy FX2
        GT100_default_replace(538, 1, GetJsonHex("fx2_sub_wah_type", a));                  //copy FX2
        GT100_default_replace(539, 1, GetJsonHex("fx2_sub_wah_pedal_pos", a));                  //copy FX2
        GT100_default_replace(540, 1, GetJsonHex("fx2_sub_wah_pedal_min", a));                  //copy FX2
        GT100_default_replace(541, 1, GetJsonHex("fx2_sub_wah_pedal_max", a));                  //copy FX2
        GT100_default_replace(542, 1, GetJsonHex("fx2_sub_wah_effect_level", a));                  //copy FX2
        GT100_default_replace(543, 1, GetJsonHex("fx2_sub_wah_direct_mix", a));                  //copy FX2
        GT100_default_replace(545, 1, GetJsonHex("fx2_adv_comp_type", a));                  //copy FX2
        GT100_default_replace(546, 1, GetJsonHex("fx2_adv_comp_sustain", a));                  //copy FX2
        GT100_default_replace(547, 1, GetJsonHex("fx2_adv_comp_attack", a));                  //copy FX2
        GT100_default_replace(548, 1, GetJsonHex("fx2_adv_comp_tone", a));                  //copy FX2
        GT100_default_replace(549, 1, GetJsonHex("fx2_adv_comp_level", a));                  //copy FX2
        GT100_default_replace(551, 1, GetJsonHex("fx2_limiter_type", a));                  //copy FX2
        GT100_default_replace(552, 1, GetJsonHex("fx2_limiter_attack", a));                  //copy FX2
        GT100_default_replace(553, 1, GetJsonHex("fx2_limiter_thresh", a));                  //copy FX2
        GT100_default_replace(554, 1, GetJsonHex("fx2_limiter_ratio", a));                  //copy FX2
        GT100_default_replace(555, 1, GetJsonHex("fx2_limiter_release", a));                  //copy FX2
        GT100_default_replace(556, 1, GetJsonHex("fx2_limiter_level", a));                  //copy FX2
        GT100_default_replace(558, 1, GetJsonHex("fx2_graphic_eq_31hz", a));                  //copy FX2
        GT100_default_replace(559, 1, GetJsonHex("fx2_graphic_eq_62hz", a));                  //copy FX2
        GT100_default_replace(560, 1, GetJsonHex("fx2_graphic_eq_125hz", a));                  //copy FX2
        GT100_default_replace(561, 1, GetJsonHex("fx2_graphic_eq_250hz", a));                  //copy FX2
        GT100_default_replace(575, 1, GetJsonHex("fx2_graphic_eq_500hz", a));                  //copy FX2
        GT100_default_replace(576, 1, GetJsonHex("fx2_graphic_eq_1khz", a));                  //copy FX2
        GT100_default_replace(577, 1, GetJsonHex("fx2_graphic_eq_2khz", a));                  //copy FX2
        GT100_default_replace(578, 1, GetJsonHex("fx2_graphic_eq_4khz", a));                  //copy FX2
        GT100_default_replace(579, 1, GetJsonHex("fx2_graphic_eq_8khz", a));                  //copy FX2
        GT100_default_replace(580, 1, GetJsonHex("fx2_graphic_eq_16khz", a));                  //copy FX2
        GT100_default_replace(581, 1, GetJsonHex("fx2_graphic_eq_level", a));                  //copy FX2
        GT100_default_replace(583, 1, GetJsonHex("fx2_parametric_eq_low_cut", a));                  //copy FX2
        GT100_default_replace(584, 1, GetJsonHex("fx2_parametric_eq_low_gain", a));                  //copy FX2
        GT100_default_replace(585, 1, GetJsonHex("fx2_parametric_eq_low_mid_freq", a));                  //copy FX2
        GT100_default_replace(586, 1, GetJsonHex("fx2_parametric_eq_low_mid_q", a));                  //copy FX2
        GT100_default_replace(587, 1, GetJsonHex("fx2_parametric_eq_low_mid_gain", a));                  //copy FX2
        GT100_default_replace(588, 1, GetJsonHex("fx2_parametric_eq_high_mid_freq", a));                  //copy FX2
        GT100_default_replace(589, 1, GetJsonHex("fx2_parametric_eq_high_mid_q", a));                  //copy FX2
        GT100_default_replace(590, 1, GetJsonHex("fx2_parametric_eq_high_mid_gain", a));                  //copy FX2
        GT100_default_replace(591, 1, GetJsonHex("fx2_parametric_eq_high_gain", a));                  //copy FX2
        GT100_default_replace(592, 1, GetJsonHex("fx2_parametric_eq_high_cut", a));                  //copy FX2
        GT100_default_replace(593, 1, GetJsonHex("fx2_parametric_eq_level", a));                  //copy FX2
        GT100_default_replace(595, 1, GetJsonHex("fx2_tone_modify_type", a));                  //copy FX2
        GT100_default_replace(596, 1, GetJsonHex("fx2_tone_modify_reso", a));                  //copy FX2
        GT100_default_replace(597, 1, GetJsonHex("fx2_tone_modify_low", a));                  //copy FX2
        GT100_default_replace(598, 1, GetJsonHex("fx2_tone_modify_high", a));                  //copy FX2
        GT100_default_replace(599, 1, GetJsonHex("fx2_tone_modify_level", a));                  //copy FX2
        GT100_default_replace(601, 1, GetJsonHex("fx2_guitar_sim_type", a));                  //copy FX2
        GT100_default_replace(602, 1, GetJsonHex("fx2_guitar_sim_low", a));                  //copy FX2
        GT100_default_replace(603, 1, GetJsonHex("fx2_guitar_sim_high", a));                  //copy FX2
        GT100_default_replace(604, 1, GetJsonHex("fx2_guitar_sim_level", a));                  //copy FX2
        GT100_default_replace(605, 1, GetJsonHex("fx2_guitar_sim_body", a));                  //copy FX2
        GT100_default_replace(607, 1, GetJsonHex("fx2_slow_gear_sens", a));                  //copy FX2
        GT100_default_replace(608, 1, GetJsonHex("fx2_slow_gear_rise_time", a));                  //copy FX2
        GT100_default_replace(609, 1, GetJsonHex("fx2_slow_gear_level", a));                  //copy FX2
        GT100_default_replace(611, 1, GetJsonHex("fx2_defretter_tone", a));                  //copy FX2
        GT100_default_replace(612, 1, GetJsonHex("fx2_defretter_sens", a));                  //copy FX2
        GT100_default_replace(613, 1, GetJsonHex("fx2_defretter_attack", a));                  //copy FX2
        GT100_default_replace(614, 1, GetJsonHex("fx2_defretter_depth", a));                  //copy FX2
        GT100_default_replace(615, 1, GetJsonHex("fx2_defretter_reso", a));                  //copy FX2
        GT100_default_replace(616, 1, GetJsonHex("fx2_defretter_effect_level", a));                  //copy FX2
        GT100_default_replace(617, 1, GetJsonHex("fx2_defretter_direct_mix", a));                  //copy FX2
        GT100_default_replace(619, 1, GetJsonHex("fx2_wave_synth_wave", a));                  //copy FX2
        GT100_default_replace(620, 1, GetJsonHex("fx2_wave_synth_cutoff", a));                  //copy FX2
        GT100_default_replace(621, 1, GetJsonHex("fx2_wave_synth_reso", a));                  //copy FX2
        GT100_default_replace(622, 1, GetJsonHex("fx2_wave_synth_filter_sens", a));                  //copy FX2
        GT100_default_replace(623, 1, GetJsonHex("fx2_wave_synth_filter_decay", a));                  //copy FX2
        GT100_default_replace(624, 1, GetJsonHex("fx2_wave_synth_filter_depth", a));                  //copy FX2
        GT100_default_replace(625, 1, GetJsonHex("fx2_wave_synth_synth_level", a));                  //copy FX2
        GT100_default_replace(626, 1, GetJsonHex("fx2_wave_synth_direct_mix", a));                  //copy FX2
        GT100_default_replace(628, 1, GetJsonHex("fx2_sitar_sim_tone", a));                  //copy FX2
        GT100_default_replace(629, 1, GetJsonHex("fx2_sitar_sim_sens", a));                  //copy FX2
        GT100_default_replace(630, 1, GetJsonHex("fx2_sitar_sim_depth", a));                  //copy FX2
        GT100_default_replace(631, 1, GetJsonHex("fx2_sitar_sim_reso", a));                  //copy FX2
        GT100_default_replace(632, 1, GetJsonHex("fx2_sitar_sim_buzz", a));                  //copy FX2
        GT100_default_replace(633, 1, GetJsonHex("fx2_sitar_sim_effect_level", a));                  //copy FX2
        GT100_default_replace(634, 1, GetJsonHex("fx2_sitar_sim_direct_mix", a));                  //copy FX2
        GT100_default_replace(636, 1, GetJsonHex("fx2_octave_range", a));                  //copy FX2
        GT100_default_replace(637, 1, GetJsonHex("fx2_octave_level", a));                  //copy FX2
        GT100_default_replace(638, 1, GetJsonHex("fx2_octave_direct_mix", a));                  //copy FX2
        GT100_default_replace(640, 1, GetJsonHex("fx2_pitch_shifter_voice", a));                  //copy FX2
        GT100_default_replace(641, 1, GetJsonHex("fx2_pitch_shifter_ps1mode", a));                  //copy FX2
        GT100_default_replace(642, 1, GetJsonHex("fx2_pitch_shifter_ps1pitch", a));                  //copy FX2
        GT100_default_replace(643, 1, GetJsonHex("fx2_pitch_shifter_ps1fine", a));                  //copy FX2
        GT100_default_replace(644, 1, GetJsonHex("fx2_pitch_shifter_ps1pre_dly_h", a));                  //copy FX2
        GT100_default_replace(645, 1, GetJsonHex("fx2_pitch_shifter_ps1pre_dly_l", a));                  //copy FX2
        GT100_default_replace(646, 1, GetJsonHex("fx2_pitch_shifter_ps1level", a));                  //copy FX2
        GT100_default_replace(647, 1, GetJsonHex("fx2_pitch_shifter_ps2mode", a));                  //copy FX2
        GT100_default_replace(648, 1, GetJsonHex("fx2_pitch_shifter_ps2pitch", a));                  //copy FX2
        GT100_default_replace(649, 1, GetJsonHex("fx2_pitch_shifter_ps2fine", a));                  //copy FX2
        GT100_default_replace(650, 1, GetJsonHex("fx2_pitch_shifter_ps2pre_dly_h", a));                  //copy FX2
        GT100_default_replace(650, 1, GetJsonHex("fx2_pitch_shifter_ps2pre_dly_l", a));                  //copy FX2
        GT100_default_replace(652, 1, GetJsonHex("fx2_pitch_shifter_ps2level", a));                  //copy FX2
        GT100_default_replace(653, 1, GetJsonHex("fx2_pitch_shifter_ps1f_back", a));                  //copy FX2
        GT100_default_replace(654, 1, GetJsonHex("fx2_pitch_shifter_direct_mix", a));                  //copy FX2
        GT100_default_replace(656, 1, GetJsonHex("fx2_harmonist_voice", a));                  //copy FX2
        GT100_default_replace(657, 1, GetJsonHex("fx2_harmonist_hr1harm", a));                  //copy FX2
        GT100_default_replace(658, 1, GetJsonHex("fx2_harmonist_hr1pre_dly_h", a));                  //copy FX2
        GT100_default_replace(659, 1, GetJsonHex("fx2_harmonist_hr1pre_dly_l", a));                  //copy FX2
        GT100_default_replace(660, 1, GetJsonHex("fx2_harmonist_hr1level", a));                  //copy FX2
        GT100_default_replace(661, 1, GetJsonHex("fx2_harmonist_hr2harm", a));                  //copy FX2
        GT100_default_replace(662, 1, GetJsonHex("fx2_harmonist_hr2pre_dly_h", a));                  //copy FX2
        GT100_default_replace(662, 1, GetJsonHex("fx2_harmonist_hr2pre_dly_l", a));                  //copy FX2
        GT100_default_replace(664, 1, GetJsonHex("fx2_harmonist_hr2level", a));                  //copy FX2
        GT100_default_replace(665, 1, GetJsonHex("fx2_harmonist_hr1f_back", a));                  //copy FX2
        GT100_default_replace(666, 1, GetJsonHex("fx2_harmonist_direct_mix", a));                  //copy FX2
        GT100_default_replace(667, 1, GetJsonHex("fx2_harmonist_hr1c", a));                  //copy FX2
        GT100_default_replace(668, 1, GetJsonHex("fx2_harmonist_hr1db", a));                  //copy FX2
        GT100_default_replace(669, 1, GetJsonHex("fx2_harmonist_hr1d", a));                  //copy FX2
        GT100_default_replace(670, 1, GetJsonHex("fx2_harmonist_hr1eb", a));                  //copy FX2
        GT100_default_replace(671, 1, GetJsonHex("fx2_harmonist_hr1e", a));                  //copy FX2
        GT100_default_replace(672, 1, GetJsonHex("fx2_harmonist_hr1f", a));                  //copy FX2
        GT100_default_replace(673, 1, GetJsonHex("fx2_harmonist_hr1f_s", a));                  //copy FX2
        GT100_default_replace(674, 1, GetJsonHex("fx2_harmonist_hr1g", a));                  //copy FX2
        GT100_default_replace(675, 1, GetJsonHex("fx2_harmonist_hr1ab", a));                  //copy FX2
        GT100_default_replace(676, 1, GetJsonHex("fx2_harmonist_hr1a", a));                  //copy FX2
        GT100_default_replace(677, 1, GetJsonHex("fx2_harmonist_hr1bb", a));                  //copy FX2
        GT100_default_replace(678, 1, GetJsonHex("fx2_harmonist_hr1b", a));                  //copy FX2
        GT100_default_replace(679, 1, GetJsonHex("fx2_harmonist_hr2c", a));                  //copy FX2
        GT100_default_replace(680, 1, GetJsonHex("fx2_harmonist_hr2db", a));                  //copy FX2
        GT100_default_replace(681, 1, GetJsonHex("fx2_harmonist_hr2d", a));                  //copy FX2
        GT100_default_replace(682, 1, GetJsonHex("fx2_harmonist_hr2eb", a));                  //copy FX2
        GT100_default_replace(683, 1, GetJsonHex("fx2_harmonist_hr2e", a));                  //copy FX2
        GT100_default_replace(684, 1, GetJsonHex("fx2_harmonist_hr2f", a));                  //copy FX2
        GT100_default_replace(685, 1, GetJsonHex("fx2_harmonist_hr2f_s", a));                  //copy FX2
        GT100_default_replace(686, 1, GetJsonHex("fx2_harmonist_hr2g", a));                  //copy FX2
        GT100_default_replace(687, 1, GetJsonHex("fx2_harmonist_hr2ab", a));                  //copy FX2
        GT100_default_replace(688, 1, GetJsonHex("fx2_harmonist_hr2a", a));                  //copy FX2
        GT100_default_replace(689, 1, GetJsonHex("fx2_harmonist_hr2bb", a));                  //copy FX2
        GT100_default_replace(690, 1, GetJsonHex("fx2_harmonist_hr2b", a));                  //copy FX2
        GT100_default_replace(692, 1, GetJsonHex("fx2_sound_hold_hold", a));                  //copy FX2
        GT100_default_replace(693, 1, GetJsonHex("fx2_sound_hold_rise_time", a));                  //copy FX2
        GT100_default_replace(694, 1, GetJsonHex("fx2_sound_hold_effect_level", a));                  //copy FX2
        GT100_default_replace(696, 1, GetJsonHex("fx2_ac_processor_type", a));                  //copy FX2
        GT100_default_replace(697, 1, GetJsonHex("fx2_ac_processor_bass", a));                  //copy FX2
        GT100_default_replace(698, 1, GetJsonHex("fx2_ac_processor_middle", a));                  //copy FX2
        GT100_default_replace(699, 1, GetJsonHex("fx2_ac_processor_middle_freq", a));                  //copy FX2
        GT100_default_replace(700, 1, GetJsonHex("fx2_ac_processor_treble", a));                  //copy FX2
        GT100_default_replace(701, 1, GetJsonHex("fx2_ac_processor_presence", a));                  //copy FX2
        GT100_default_replace(702, 1, GetJsonHex("fx2_ac_processor_level", a));                  //copy FX2
        GT100_default_replace(716, 1, GetJsonHex("fx2_phaser_type", a));                  //copy FX2
        GT100_default_replace(717, 1, GetJsonHex("fx2_phaser_rate", a));                  //copy FX2
        GT100_default_replace(718, 1, GetJsonHex("fx2_phaser_depth", a));                  //copy FX2
        GT100_default_replace(719, 1, GetJsonHex("fx2_phaser_manual", a));                  //copy FX2
        GT100_default_replace(720, 1, GetJsonHex("fx2_phaser_reso", a));                  //copy FX2
        GT100_default_replace(721, 1, GetJsonHex("fx2_phaser_step_rate", a));                  //copy FX2
        GT100_default_replace(722, 1, GetJsonHex("fx2_phaser_effect_level", a));                  //copy FX2
        GT100_default_replace(723, 1, GetJsonHex("fx2_phaser_direct_mix", a));                  //copy FX2
        GT100_default_replace(726, 1, GetJsonHex("fx2_flanger_rate", a));                  //copy FX2
        GT100_default_replace(727, 1, GetJsonHex("fx2_flanger_depth", a));                  //copy FX2
        GT100_default_replace(728, 1, GetJsonHex("fx2_flanger_manual", a));                  //copy FX2
        GT100_default_replace(729, 1, GetJsonHex("fx2_flanger_reso", a));                  //copy FX2
        GT100_default_replace(730, 1, GetJsonHex("fx2_flanger_separation", a));                  //copy FX2
        GT100_default_replace(731, 1, GetJsonHex("fx2_flanger_low_cut", a));                  //copy FX2
        GT100_default_replace(732, 1, GetJsonHex("fx2_flanger_effect_level", a));                  //copy FX2
        GT100_default_replace(733, 1, GetJsonHex("fx2_flanger_direct_mix", a));                  //copy FX2
        GT100_default_replace(735, 1, GetJsonHex("fx2_tremolo_wave_shape", a));                  //copy FX2
        GT100_default_replace(736, 1, GetJsonHex("fx2_tremolo_rate", a));                  //copy FX2
        GT100_default_replace(737, 1, GetJsonHex("fx2_tremolo_depth", a));                  //copy FX2
        GT100_default_replace(738, 1, GetJsonHex("fx2_tremolo_level", a));                  //copy FX2
        GT100_default_replace(740, 1, GetJsonHex("fx2_rotary_speed_select", a));                  //copy FX2
        GT100_default_replace(741, 1, GetJsonHex("fx2_rotary_rate_slow", a));                  //copy FX2
        GT100_default_replace(742, 1, GetJsonHex("fx2_rotary_rate_fast", a));                  //copy FX2
        GT100_default_replace(743, 1, GetJsonHex("fx2_rotary_rise_time", a));                  //copy FX2
        GT100_default_replace(744, 1, GetJsonHex("fx2_rotary_fall_time", a));                  //copy FX2
        GT100_default_replace(745, 1, GetJsonHex("fx2_rotary_depth", a));                  //copy FX2
        GT100_default_replace(746, 1, GetJsonHex("fx2_rotary_level", a));                  //copy FX2
        GT100_default_replace(748, 1, GetJsonHex("fx2_uni_v_rate", a));                  //copy FX2
        GT100_default_replace(749, 1, GetJsonHex("fx2_uni_v_depth", a));                  //copy FX2
        GT100_default_replace(750, 1, GetJsonHex("fx2_uni_v_level", a));                  //copy FX2
        GT100_default_replace(752, 1, GetJsonHex("fx2_pan_type", a));                  //copy FX2
        GT100_default_replace(753, 1, GetJsonHex("fx2_pan_pos", a));                  //copy FX2
        GT100_default_replace(754, 1, GetJsonHex("fx2_pan_wave_shape", a));                  //copy FX2
        GT100_default_replace(755, 1, GetJsonHex("fx2_pan_rate", a));                  //copy FX2
        GT100_default_replace(756, 1, GetJsonHex("fx2_pan_depth", a));                  //copy FX2
        GT100_default_replace(757, 1, GetJsonHex("fx2_pan_level", a));                  //copy FX2
        GT100_default_replace(759, 1, GetJsonHex("fx2_slicer_pattern", a));                  //copy FX2
        GT100_default_replace(760, 1, GetJsonHex("fx2_slicer_rate", a));                  //copy FX2
        GT100_default_replace(761, 1, GetJsonHex("fx2_slicer_trigger_sens", a));                  //copy FX2
        GT100_default_replace(762, 1, GetJsonHex("fx2_slicer_effect_level", a));                  //copy FX2
        GT100_default_replace(763, 1, GetJsonHex("fx2_slicer_direct_mix", a));                  //copy FX2
        GT100_default_replace(765, 1, GetJsonHex("fx2_vibrato_rate", a));                  //copy FX2
        GT100_default_replace(766, 1, GetJsonHex("fx2_vibrato_depth", a));                  //copy FX2
        GT100_default_replace(767, 1, GetJsonHex("fx2_vibrato_trigger", a));                  //copy FX2
        GT100_default_replace(768, 1, GetJsonHex("fx2_vibrato_rise_time", a));                  //copy FX2
        GT100_default_replace(769, 1, GetJsonHex("fx2_vibrato_level", a));                  //copy FX2
        GT100_default_replace(771, 1, GetJsonHex("fx2_ring_mod_mode", a));                  //copy FX2
        GT100_default_replace(772, 1, GetJsonHex("fx2_ring_mod_freq", a));                  //copy FX2
        GT100_default_replace(773, 1, GetJsonHex("fx2_ring_mod_effect_level", a));                  //copy FX2
        GT100_default_replace(774, 1, GetJsonHex("fx2_ring_mod_direct_mix", a));                  //copy FX2
        GT100_default_replace(776, 1, GetJsonHex("fx2_humanizer_mode", a));                  //copy FX2
        GT100_default_replace(777, 1, GetJsonHex("fx2_humanizer_vowel1", a));                  //copy FX2
        GT100_default_replace(778, 1, GetJsonHex("fx2_humanizer_vowel2", a));                  //copy FX2
        GT100_default_replace(779, 1, GetJsonHex("fx2_humanizer_sens", a));                  //copy FX2
        GT100_default_replace(780, 1, GetJsonHex("fx2_humanizer_rate", a));                  //copy FX2
        GT100_default_replace(781, 1, GetJsonHex("fx2_humanizer_depth", a));                  //copy FX2
        GT100_default_replace(782, 1, GetJsonHex("fx2_humanizer_manual", a));                  //copy FX2
        GT100_default_replace(783, 1, GetJsonHex("fx2_humanizer_level", a));                  //copy FX2
        GT100_default_replace(785, 1, GetJsonHex("fx2_2x2_chorus_xover_freq", a));                  //copy FX2
        GT100_default_replace(786, 1, GetJsonHex("fx2_2x2_chorus_low_rate", a));                  //copy FX2
        GT100_default_replace(787, 1, GetJsonHex("fx2_2x2_chorus_low_depth", a));                  //copy FX2
        GT100_default_replace(788, 1, GetJsonHex("fx2_2x2_chorus_low_pre_delay", a));                  //copy FX2
        GT100_default_replace(789, 1, GetJsonHex("fx2_2x2_chorus_low_level", a));                  //copy FX2
        GT100_default_replace(790, 1, GetJsonHex("fx2_2x2_chorus_high_rate", a));                  //copy FX2
        GT100_default_replace(791, 1, GetJsonHex("fx2_2x2_chorus_high_depth", a));                  //copy FX2
        GT100_default_replace(792, 1, GetJsonHex("fx2_2x2_chorus_high_pre_delay", a));                  //copy FX2
        GT100_default_replace(793, 1, GetJsonHex("fx2_2x2_chorus_high_level", a));                  //copy FX2
        GT100_default_replace(794, 1, GetJsonHex("fx2_2x2_chorus_direct_level", a));                  //copy FX2
        GT100_default_replace(795, 1, GetJsonHex("fx2_sub_delay_type", a));                  //copy FX2
        GT100_default_replace(796, 1, GetJsonHex("fx2_sub_delay_time_h", a));                  //copy FX2
        GT100_default_replace(797, 1, GetJsonHex("fx2_sub_delay_time_l", a));                  //copy FX2
        GT100_default_replace(798, 1, GetJsonHex("fx2_sub_delay_f_back", a));                  //copy FX2
        GT100_default_replace(799, 1, GetJsonHex("fx2_sub_delay_high_cut", a));                  //copy FX2
        GT100_default_replace(800, 1, GetJsonHex("fx2_sub_delay_effect_level", a));                  //copy FX2
        GT100_default_replace(801, 1, GetJsonHex("fx2_sub_delay_direct_mix", a));                  //copy FX2
        GT100_default_replace(802, 1, GetJsonHex("fx2_sub_delay_tap_time", a));                  //copy FX2
        GT100_default_replace(812, 1, GetJsonHex("delay_on_off", a));                  //copy DD
        GT100_default_replace(813, 1, GetJsonHex("delay_type", a));                  //copy DD
        GT100_default_replace(814, 1, GetJsonHex("delay_delay_time_h", a));                  //copy DD
        GT100_default_replace(815, 1, GetJsonHex("delay_delay_time_l", a));                  //copy DD
        GT100_default_replace(816, 1, GetJsonHex("delay_f_back", a));                  //copy DD
        GT100_default_replace(817, 1, GetJsonHex("delay_high_cut", a));                  //copy DD
        GT100_default_replace(818, 1, GetJsonHex("delay_effect_level", a));                  //copy DD
        GT100_default_replace(819, 1, GetJsonHex("delay_direct_mix", a));                  //copy DD
        GT100_default_replace(820, 1, GetJsonHex("delay_tap_time", a));                  //copy DD
        GT100_default_replace(821, 1, GetJsonHex("delay_d1_time_h", a));                  //copy DD
        GT100_default_replace(822, 1, GetJsonHex("delay_d1_time_l", a));                  //copy DD
        GT100_default_replace(823, 1, GetJsonHex("delay_d1_f_back", a));                  //copy DD
        GT100_default_replace(824, 1, GetJsonHex("delay_d1_hi_cut", a));                  //copy DD
        GT100_default_replace(825, 1, GetJsonHex("delay_d1_level", a));                  //copy DD
        GT100_default_replace(826, 1, GetJsonHex("delay_d2_time_h", a));                  //copy DD
        GT100_default_replace(827, 1, GetJsonHex("delay_d2_time_l", a));                  //copy DD
        GT100_default_replace(828, 1, GetJsonHex("delay_d2_f_back", a));                  //copy DD
        GT100_default_replace(829, 1, GetJsonHex("delay_d2_hi_cut", a));                  //copy DD
        GT100_default_replace(830, 1, GetJsonHex("delay_d2_level", a));                  //copy DD
        GT100_default_replace(831, 1, GetJsonHex("delay_mod_rate", a));                  //copy DD
        GT100_default_replace(832, 1, GetJsonHex("delay_mod_depth", a));                  //copy DD
        GT100_default_replace(873, 1, GetJsonHex("reverb_on_off", a));                  //copy RV
        GT100_default_replace(874, 1, GetJsonHex("reverb_type", a));                  //copy RV
        GT100_default_replace(875, 1, GetJsonHex("reverb_time", a));                  //copy RV
        GT100_default_replace(876, 1, GetJsonHex("reverb_pre_delay_h", a));                  //copy RV
        GT100_default_replace(877, 1, GetJsonHex("reverb_pre_delay_l", a));                  //copy RV
        GT100_default_replace(878, 1, GetJsonHex("reverb_low_cut", a));                  //copy RV
        GT100_default_replace(879, 1, GetJsonHex("reverb_high_cut", a));                  //copy RV
        GT100_default_replace(880, 1, GetJsonHex("reverb_density", a));                  //copy RV
        GT100_default_replace(881, 1, GetJsonHex("reverb_effect_level", a));                  //copy RV
        GT100_default_replace(882, 1, GetJsonHex("reverb_direct_mix", a));                  //copy RV
        GT100_default_replace(883, 1, GetJsonHex("reverb_spring_sens", a));                  //copy RV
        GT100_default_replace(911, 1, GetJsonHex("foot_volume_volume_curve", a));                  //copy FV
        GT100_default_replace(909, 1, GetJsonHex("foot_volume_volume_min", a));                  //copy FV
        GT100_default_replace(910, 1, GetJsonHex("foot_volume_volume_max", a));                  //copy FV
        GT100_default_replace(908, 1, GetJsonHex("foot_volume_level", a));                  //copy FV
        GT100_default_replace(942, 1, GetJsonHex("send_return_on_off", a));                  //copy S/R
        GT100_default_replace(943, 1, GetJsonHex("send_return_mode", a));                  //copy S/R
        GT100_default_replace(944, 1, GetJsonHex("send_return_send_level", a));                  //copy S/R
        GT100_default_replace(945, 1, GetJsonHex("send_return_return_level", a));                  //copy S/R
        GT100_default_replace(946, 1, GetJsonHex("send_return_adjust", a));                  //copy S/R
        GT100_default_replace(956, 1, GetJsonHex("ns1_on_off", a));                  //copy NS
        GT100_default_replace(957, 1, GetJsonHex("ns1_threshold", a));                  //copy NS
        GT100_default_replace(958, 1, GetJsonHex("ns1_release", a));                  //copy NS
        GT100_default_replace(959, 1, GetJsonHex("ns1_detect", a));                  //copy NS
        GT100_default_replace(1013, 1, GetJsonHex("patch_category", a));               //copy Master
        GT100_default_replace(1014, 1, GetJsonHex("patch_level", a));
        GT100_default_replace(1015, 1, GetJsonHex("master_eq_low_gain", a));           //copy Master
        GT100_default_replace(1016, 1, GetJsonHex("master_eq_mid_freq", a));           //copy Master
        GT100_default_replace(1017, 1, GetJsonHex("master_eq_mid_q", a));              //copy Master
        GT100_default_replace(1018, 1, GetJsonHex("master_eq_mid_gain", a));           //copy Master
        GT100_default_replace(1019, 1, GetJsonHex("master_eq_high_gain", a));          //copy Master
        GT100_default_replace(1020, 1, GetJsonHex("master_bpm_h", a));                 //copy Master
        GT100_default_replace(1021, 1, GetJsonHex("master_bpm_l", a));                 //copy Master
        GT100_default_replace(1022, 1, GetJsonHex("master_key", a));                   //copy Master
        GT100_default_replace(1023, 1, GetJsonHex("master_beat", a));                  //copy Master
        GT100_default_replace(1024, 1, GetJsonHex("pitch_detection", a));              //copy Master
        GT100_default_replace(1030, 1, GetJsonHex("fx_chain_position1", a));                   //copy Chain position
        GT100_default_replace(1031, 1, GetJsonHex("fx_chain_position2", a));                   //copy Chain position
        GT100_default_replace(1032, 1, GetJsonHex("fx_chain_position3", a));                   //copy Chain position
        GT100_default_replace(1033, 1, GetJsonHex("fx_chain_position4", a));                   //copy Chain position
        GT100_default_replace(1034, 1, GetJsonHex("fx_chain_position5", a));                   //copy Chain position
        GT100_default_replace(1035, 1, GetJsonHex("fx_chain_position6", a));                   //copy Chain position
        GT100_default_replace(1036, 1, GetJsonHex("fx_chain_position7", a));                   //copy Chain position
        GT100_default_replace(1037, 1, GetJsonHex("fx_chain_position8", a));                   //copy Chain position
        GT100_default_replace(1038, 1, GetJsonHex("fx_chain_position9", a));                   //copy Chain position
        GT100_default_replace(1039, 1, GetJsonHex("fx_chain_position10", a));                  //copy Chain position
        GT100_default_replace(1040, 1, GetJsonHex("fx_chain_position11", a));                  //copy Chain position
        GT100_default_replace(1041, 1, GetJsonHex("fx_chain_position12", a));                  //copy Chain position
        GT100_default_replace(1042, 1, GetJsonHex("fx_chain_position13", a));                  //copy Chain position
        GT100_default_replace(1043, 1, GetJsonHex("fx_chain_position14", a));                  //copy Chain position
        GT100_default_replace(1044, 1, GetJsonHex("fx_chain_position15", a));                  //copy Chain position
        GT100_default_replace(1045, 1, GetJsonHex("fx_chain_position16", a));                  //copy Chain position
        GT100_default_replace(1046, 1, GetJsonHex("fx_chain_position17", a));                  //copy Chain position
        GT100_default_replace(1047, 1, GetJsonHex("fx_chain_position18", a));                  //copy Chain position
        GT100_default_replace(1048, 1, GetJsonHex("fx_chain_position19", a));                  //copy Chain position
        GT100_default_replace(1049, 1, GetJsonHex("fx_chain_position20", a));                  //copy Chain position
        GT100_default_replace(1078, 1, GetJsonHex("ctl_exp_accel_ctl_func", a));                 //copy CTL/EXP
        GT100_default_replace(1079, 1, GetJsonHex("ctl_exp_accel_ctl_min", a));                  //copy CTL/EXP
        GT100_default_replace(1080, 1, GetJsonHex("ctl_exp_accel_ctl_max", a));                  //copy CTL/EXP
        GT100_default_replace(1081, 1, GetJsonHex("ctl_exp_accel_ctl_src_mode", a));             //copy CTL/EXP
        GT100_default_replace(1094, 1, GetJsonHex("ctl_exp_exp_sw_func", a));                    //copy CTL/EXP
        GT100_default_replace(1095, 1, GetJsonHex("ctl_exp_exp_sw_min", a));                     //copy CTL/EXP
        GT100_default_replace(1096, 1, GetJsonHex("ctl_exp_exp_sw_max", a));                     //copy CTL/EXP
        GT100_default_replace(1097, 1, GetJsonHex("ctl_exp_exp_sw_src_mode", a));                //copy CTL/EXP
        GT100_default_replace(1110, 1, GetJsonHex("ctl_exp_sub_ctl1_func", a));                  //copy CTL/EXP
        GT100_default_replace(1111, 1, GetJsonHex("ctl_exp_sub_ctl1_min", a));                   //copy CTL/EXP
        GT100_default_replace(1112, 1, GetJsonHex("ctl_exp_sub_ctl1_max", a));                   //copy CTL/EXP
        GT100_default_replace(1113, 1, GetJsonHex("ctl_exp_sub_ctl1_src_mode", a));              //copy CTL/EXP
        GT100_default_replace(1139, 1, GetJsonHex("ctl_exp_sub_ctl2_func", a));                  //copy CTL/EXP
        GT100_default_replace(1140, 1, GetJsonHex("ctl_exp_sub_ctl2_min", a));                   //copy CTL/EXP
        GT100_default_replace(1141, 1, GetJsonHex("ctl_exp_sub_ctl2_max", a));                   //copy CTL/EXP
        GT100_default_replace(1142, 1, GetJsonHex("ctl_exp_sub_ctl2_src_mode", a));              //copy CTL/EXP
        GT100_default_replace(1155, 1, GetJsonHex("ctl_exp_exp_func", a));                       //copy CTL/EXP
        GT100_default_replace(1156, 1, GetJsonHex("ctl_exp_sub_exp_patch_level_min", a));        //copy CTL/EXP
        GT100_default_replace(1157, 1, GetJsonHex("ctl_exp_sub_exp_patch_level_max", a));        //copy CTL/EXP
        GT100_default_replace(1171, 1, GetJsonHex("ctl_exp_sub_exp_func", a));                   //copy CTL/EXP
        GT100_default_replace(1172, 1, GetJsonHex("ctl_exp_exp_patch_level_min", a));            //copy CTL/EXP
        GT100_default_replace(1173, 1, GetJsonHex("ctl_exp_exp_patch_level_max", a));            //copy CTL/EXP


        GT100_default_replace(1187, 1, GetJsonHex("assign1_on_off", a));                  //copy Assigns
        //GT100_default_replace(1188, 2, GetJsonHex("assign1_target", a));                  //copy Assigns
        GT100_default_replace(1188, 1, GetJsonHex("assign1_target_h", a));                //copy Assigns
        GT100_default_replace(1189, 1, GetJsonHex("assign1_target_l", a));                //copy Assigns
        //GT100_default_replace(1190, 2, GetJsonHex("assign1_target_min", a));              //copy Assigns
        GT100_default_replace(1190, 1, GetJsonHex("assign1_target_min_h", a));            //copy Assigns
        GT100_default_replace(1191, 1, GetJsonHex("assign1_target_min_l", a));            //copy Assigns
        //GT100_default_replace(1192, 2, GetJsonHex("assign1_target_max", a));              //copy Assigns
        GT100_default_replace(1192, 1, GetJsonHex("assign1_target_max_h", a));            //copy Assigns
        GT100_default_replace(1193, 1, GetJsonHex("assign1_target_max_l", a));            //copy Assigns
        GT100_default_replace(1194, 1, GetJsonHex("assign1_source", a));                  //copy Assigns
        GT100_default_replace(1195, 1, GetJsonHex("assign1_source_mode", a));             //copy Assigns
        GT100_default_replace(1196, 1, GetJsonHex("assign1_act_range_lo", a));            //copy Assigns
        GT100_default_replace(1197, 1, GetJsonHex("assign1_act_range_hi", a));            //copy Assigns
        GT100_default_replace(1198, 1, GetJsonHex("assign1_int_pdl_trigger", a));         //copy Assigns
        GT100_default_replace(1199, 1, GetJsonHex("assign1_int_pdl_time", a));            //copy Assigns
        GT100_default_replace(1200, 1, GetJsonHex("assign1_int_pdl_curve", a));           //copy Assigns
        GT100_default_replace(1201, 1, GetJsonHex("assign1_wave_rate", a));               //copy Assigns
        GT100_default_replace(1202, 1, GetJsonHex("assign1_waveform", a));                //copy Assigns
        GT100_default_replace(1219, 1, GetJsonHex("assign2_on_off", a));                  //copy Assigns
        //GT100_default_replace(1220, 2, GetJsonHex("assign2_target", a));                  //copy Assigns
        GT100_default_replace(1220, 1, GetJsonHex("assign2_target_h", a));                //copy Assigns
        GT100_default_replace(1221, 1, GetJsonHex("assign2_target_l", a));                //copy Assigns
        //GT100_default_replace(1222, 2, GetJsonHex("assign2_target_min", a));              //copy Assigns
        GT100_default_replace(1222, 1, GetJsonHex("assign2_target_min_h", a));            //copy Assigns
        GT100_default_replace(1223, 1, GetJsonHex("assign2_target_min_l", a));            //copy Assigns
        //GT100_default_replace(1224, 2, GetJsonHex("assign2_target_max", a));              //copy Assigns
        GT100_default_replace(1224, 1, GetJsonHex("assign2_target_max_h", a));            //copy Assigns
        GT100_default_replace(1225, 1, GetJsonHex("assign2_target_max_l", a));            //copy Assigns
        GT100_default_replace(1226, 1, GetJsonHex("assign2_source", a));                  //copy Assigns
        GT100_default_replace(1227, 1, GetJsonHex("assign2_source_mode", a));             //copy Assigns
        GT100_default_replace(1228, 1, GetJsonHex("assign2_act_range_lo", a));            //copy Assigns
        GT100_default_replace(1229, 1, GetJsonHex("assign2_act_range_hi", a));            //copy Assigns
        GT100_default_replace(1230, 1, GetJsonHex("assign2_int_pdl_trigger", a));         //copy Assigns
        GT100_default_replace(1231, 1, GetJsonHex("assign2_int_pdl_time", a));            //copy Assigns
        GT100_default_replace(1232, 1, GetJsonHex("assign2_int_pdl_curve", a));           //copy Assigns
        GT100_default_replace(1233, 1, GetJsonHex("assign2_wave_rate", a));               //copy Assigns
        GT100_default_replace(1234, 1, GetJsonHex("assign2_waveform", a));                //copy Assigns
        GT100_default_replace(1251, 1, GetJsonHex("assign3_on_off", a));                  //copy Assigns
        //GT100_default_replace(1252, 2, GetJsonHex("assign3_target", a));                  //copy Assigns
        GT100_default_replace(1252, 1, GetJsonHex("assign3_target_h", a));                //copy Assigns
        GT100_default_replace(1253, 1, GetJsonHex("assign3_target_l", a));                //copy Assigns
        //GT100_default_replace(1254, 2, GetJsonHex("assign3_target_min", a));              //copy Assigns
        GT100_default_replace(1254, 1, GetJsonHex("assign3_target_min_h", a));            //copy Assigns
        GT100_default_replace(1255, 1, GetJsonHex("assign3_target_min_l", a));            //copy Assigns
        //GT100_default_replace(1256, 2, GetJsonHex("assign3_target_max", a));              //copy Assigns
        GT100_default_replace(1256, 1, GetJsonHex("assign3_target_max_h", a));            //copy Assigns
        GT100_default_replace(1257, 1, GetJsonHex("assign3_target_max_l", a));            //copy Assigns
        GT100_default_replace(1258, 1, GetJsonHex("assign3_source", a));                  //copy Assigns
        GT100_default_replace(1259, 1, GetJsonHex("assign3_source_mode", a));             //copy Assigns
        GT100_default_replace(1260, 1, GetJsonHex("assign3_act_range_lo", a));            //copy Assigns
        GT100_default_replace(1261, 1, GetJsonHex("assign3_act_range_hi", a));            //copy Assigns
        GT100_default_replace(1262, 1, GetJsonHex("assign3_int_pdl_trigger", a));         //copy Assigns
        GT100_default_replace(1263, 1, GetJsonHex("assign3_int_pdl_time", a));            //copy Assigns
        GT100_default_replace(1264, 1, GetJsonHex("assign3_int_pdl_curve", a));           //copy Assigns
        GT100_default_replace(1265, 1, GetJsonHex("assign3_wave_rate", a));               //copy Assigns
        GT100_default_replace(1266, 1, GetJsonHex("assign3_waveform", a));                //copy Assigns
        GT100_default_replace(1296, 1, GetJsonHex("assign4_on_off", a));                  //copy Assigns
        //GT100_default_replace(1297, 2, GetJsonHex("assign4_target", a));                  //copy Assigns
        GT100_default_replace(1297, 1, GetJsonHex("assign4_target_h", a));                //copy Assigns
        GT100_default_replace(1298, 1, GetJsonHex("assign4_target_l", a));                //copy Assigns
        //GT100_default_replace(1299, 2, GetJsonHex("assign4_target_min", a));              //copy Assigns
        GT100_default_replace(1299, 1, GetJsonHex("assign4_target_min_h", a));            //copy Assigns
        GT100_default_replace(1300, 1, GetJsonHex("assign4_target_min_l", a));            //copy Assigns
        //GT100_default_replace(1301, 2, GetJsonHex("assign4_target_max", a));              //copy Assigns
        GT100_default_replace(1301, 1, GetJsonHex("assign4_target_max_h", a));            //copy Assigns
        GT100_default_replace(1302, 1, GetJsonHex("assign4_target_max_l", a));            //copy Assigns
        GT100_default_replace(1303, 1, GetJsonHex("assign4_source", a));                  //copy Assigns
        GT100_default_replace(1304, 1, GetJsonHex("assign4_source_mode", a));             //copy Assigns
        GT100_default_replace(1305, 1, GetJsonHex("assign4_act_range_lo", a));            //copy Assigns
        GT100_default_replace(1306, 1, GetJsonHex("assign4_act_range_hi", a));            //copy Assigns
        GT100_default_replace(1307, 1, GetJsonHex("assign4_int_pdl_trigger", a));         //copy Assigns
        GT100_default_replace(1308, 1, GetJsonHex("assign4_int_pdl_time", a));            //copy Assigns
        GT100_default_replace(1309, 1, GetJsonHex("assign4_int_pdl_curve", a));           //copy Assigns
        GT100_default_replace(1310, 1, GetJsonHex("assign4_wave_rate", a));               //copy Assigns
        GT100_default_replace(1311, 1, GetJsonHex("assign4_waveform", a));                //copy Assigns
        GT100_default_replace(1328, 1, GetJsonHex("assign5_on_off", a));                  //copy Assigns
        //GT100_default_replace(1329, 2, GetJsonHex("assign5_target", a));                  //copy Assigns
        GT100_default_replace(1329, 1, GetJsonHex("assign5_target_h", a));                //copy Assigns
        GT100_default_replace(1330, 1, GetJsonHex("assign5_target_l", a));                //copy Assigns
        GT100_default_replace(1331, 1, GetJsonHex("assign5_target_min_h", a));            //copy Assigns
        GT100_default_replace(1332, 1, GetJsonHex("assign5_target_min_l", a));            //copy Assigns
        //GT100_default_replace(1333, 2, GetJsonHex("assign5_target_max", a));              //copy Assigns
        GT100_default_replace(1333, 1, GetJsonHex("assign5_target_max_h", a));            //copy Assigns
        GT100_default_replace(1334, 1, GetJsonHex("assign5_target_max_l", a));            //copy Assigns
        GT100_default_replace(1335, 1, GetJsonHex("assign5_source", a));                  //copy Assigns
        GT100_default_replace(1336, 1, GetJsonHex("assign5_source_mode", a));             //copy Assigns
        GT100_default_replace(1337, 1, GetJsonHex("assign5_act_range_lo", a));            //copy Assigns
        GT100_default_replace(1338, 1, GetJsonHex("assign5_act_range_hi", a));            //copy Assigns
        GT100_default_replace(1339, 1, GetJsonHex("assign5_int_pdl_trigger", a));         //copy Assigns
        GT100_default_replace(1340, 1, GetJsonHex("assign5_int_pdl_time", a));            //copy Assigns
        GT100_default_replace(1341, 1, GetJsonHex("assign5_int_pdl_curve", a));           //copy Assigns
        GT100_default_replace(1342, 1, GetJsonHex("assign5_wave_rate", a));               //copy Assigns
        GT100_default_replace(1343, 1, GetJsonHex("assign5_waveform", a));                //copy Assigns
        GT100_default_replace(1360, 1, GetJsonHex("assign6_on_off", a));                  //copy Assigns
        //GT100_default_replace(1361, 2, GetJsonHex("assign6_target", a));                  //copy Assigns
        GT100_default_replace(1361, 1, GetJsonHex("assign6_target_h", a));                //copy Assigns
        GT100_default_replace(1362, 1, GetJsonHex("assign6_target_l", a));                //copy Assigns
        //GT100_default_replace(1363, 2, GetJsonHex("assign6_target_min", a));              //copy Assigns
        GT100_default_replace(1363, 1, GetJsonHex("assign6_target_min_h", a));            //copy Assigns
        GT100_default_replace(1364, 1, GetJsonHex("assign6_target_min_l", a));            //copy Assigns
        //GT100_default_replace(1365, 2, GetJsonHex("assign6_target_max", a));              //copy Assigns
        GT100_default_replace(1365, 1, GetJsonHex("assign6_target_max_h", a));            //copy Assigns
        GT100_default_replace(1366, 1, GetJsonHex("assign6_target_max_l", a));            //copy Assigns
        GT100_default_replace(1367, 1, GetJsonHex("assign6_source", a));                  //copy Assigns
        GT100_default_replace(1368, 1, GetJsonHex("assign6_source_mode", a));             //copy Assigns
        GT100_default_replace(1369, 1, GetJsonHex("assign6_act_range_lo", a));            //copy Assigns
        GT100_default_replace(1370, 1, GetJsonHex("assign6_act_range_hi", a));            //copy Assigns
        GT100_default_replace(1371, 1, GetJsonHex("assign6_int_pdl_trigger", a));         //copy Assigns
        GT100_default_replace(1372, 1, GetJsonHex("assign6_int_pdl_time", a));            //copy Assigns
        GT100_default_replace(1373, 1, GetJsonHex("assign6_int_pdl_curve", a));           //copy Assigns
        GT100_default_replace(1374, 1, GetJsonHex("assign6_wave_rate", a));               //copy Assigns
        GT100_default_replace(1375, 1, GetJsonHex("assign6_waveform", a));                //copy Assigns
        GT100_default_replace(1392, 1, GetJsonHex("assign7_on_off", a));                  //copy Assigns
        //GT100_default_replace(1393, 2, GetJsonHex("assign7_target", a));                  //copy Assigns
        GT100_default_replace(1393, 1, GetJsonHex("assign7_target_h", a));                //copy Assigns
        GT100_default_replace(1394, 1, GetJsonHex("assign7_target_l", a));                //copy Assigns
        //GT100_default_replace(1395, 2, GetJsonHex("assign7_target_min", a));              //copy Assigns
        GT100_default_replace(1395, 1, GetJsonHex("assign7_target_min_h", a));            //copy Assigns
        GT100_default_replace(1396, 1, GetJsonHex("assign7_target_min_l", a));            //copy Assigns
        //GT100_default_replace(1397, 2, GetJsonHex("assign7_target_max", a));              //copy Assigns
        GT100_default_replace(1397, 1, GetJsonHex("assign7_target_max_h", a));            //copy Assigns
        GT100_default_replace(1398, 1, GetJsonHex("assign7_target_max_l", a));            //copy Assigns
        GT100_default_replace(1399, 1, GetJsonHex("assign7_source", a));                  //copy Assigns
        GT100_default_replace(1400, 1, GetJsonHex("assign7_source_mode", a));             //copy Assigns
        GT100_default_replace(1401, 1, GetJsonHex("assign7_act_range_lo", a));            //copy Assigns
        GT100_default_replace(1402, 1, GetJsonHex("assign7_act_range_hi", a));            //copy Assigns
        GT100_default_replace(1403, 1, GetJsonHex("assign7_int_pdl_trigger", a));         //copy Assigns
        GT100_default_replace(1404, 1, GetJsonHex("assign7_int_pdl_time", a));            //copy Assigns
        GT100_default_replace(1405, 1, GetJsonHex("assign7_int_pdl_curve", a));           //copy Assigns
        GT100_default_replace(1406, 1, GetJsonHex("assign7_wave_rate", a));               //copy Assigns
        GT100_default_replace(1407, 1, GetJsonHex("assign7_waveform", a));                //copy Assigns
        GT100_default_replace(1437, 1, GetJsonHex("assign8_on_off", a));                  //copy Assigns
        //GT100_default_replace(1438, 2, GetJsonHex("assign8_target", a));                  //copy Assigns
        GT100_default_replace(1438, 1, GetJsonHex("assign8_target_h", a));                //copy Assigns
        GT100_default_replace(1439, 1, GetJsonHex("assign8_target_l", a));                //copy Assigns
        //GT100_default_replace(1440, 2, GetJsonHex("assign8_target_min", a));              //copy Assigns
        GT100_default_replace(1440, 1, GetJsonHex("assign8_target_min_h", a));            //copy Assigns
        GT100_default_replace(1441, 1, GetJsonHex("assign8_target_min_l", a));            //copy Assigns
        //GT100_default_replace(1442, 2, GetJsonHex("assign8_target_max", a));              //copy Assigns
        GT100_default_replace(1442, 1, GetJsonHex("assign8_target_max_h", a));            //copy Assigns
        GT100_default_replace(1443, 1, GetJsonHex("assign8_target_max_l", a));            //copy Assigns
        GT100_default_replace(1444, 1, GetJsonHex("assign8_source", a));                  //copy Assigns
        GT100_default_replace(1445, 1, GetJsonHex("assign8_source_mode", a));             //copy Assigns
        GT100_default_replace(1446, 1, GetJsonHex("assign8_act_range_lo", a));            //copy Assigns
        GT100_default_replace(1447, 1, GetJsonHex("assign8_act_range_hi", a));            //copy Assigns
        GT100_default_replace(1448, 1, GetJsonHex("assign8_int_pdl_trigger", a));         //copy Assigns
        GT100_default_replace(1449, 1, GetJsonHex("assign8_int_pdl_time", a));            //copy Assigns
        GT100_default_replace(1450, 1, GetJsonHex("assign8_int_pdl_curve", a));           //copy Assigns
        GT100_default_replace(1451, 1, GetJsonHex("assign8_wave_rate", a));               //copy Assigns
        GT100_default_replace(1452, 1, GetJsonHex("assign8_waveform", a));                //copy Assigns
        GT100_default_replace(1469, 1, GetJsonHex("assign_common_input_sens", a));        //copy Assign common Input sense

        //int addr10 = 2267;
        GT100_default_replace(2283, 1, GetJsonHex("fx1_acsim_high", a));              //copy FX1 60 00 10 10
        GT100_default_replace(2284, 1, GetJsonHex("fx1_acsim_body", a));              //copy FX1
        GT100_default_replace(2285, 1, GetJsonHex("fx1_acsim_low", a));               //copy FX1
        GT100_default_replace(2287, 1, GetJsonHex("fx1_acsim_level", a));             //copy FX1
        GT100_default_replace(2289, 1, GetJsonHex("fx1_rotary2_balance", a));         //copy FX1
        GT100_default_replace(2290, 1, GetJsonHex("fx1_rotary2_speed_sel", a));       //copy FX1
        GT100_default_replace(2291, 1, GetJsonHex("fx1_rotary2_rate_slow", a));       //copy FX1
        GT100_default_replace(2292, 1, GetJsonHex("fx1_rotary2_rate_fast", a));       //copy FX1
        GT100_default_replace(2293, 1, GetJsonHex("fx1_rotary2_risetime", a));        //copy FX1
        GT100_default_replace(2294, 1, GetJsonHex("fx1_rotary2_falltime", a));        //copy FX1
        GT100_default_replace(2295, 1, GetJsonHex("fx1_rotary2_depth", a));           //copy FX1
        GT100_default_replace(2296, 1, GetJsonHex("fx1_rotary2_level", a));           //copy FX1
        GT100_default_replace(2297, 1, GetJsonHex("fx1_rotary2_direct_mix", a));      //copy FX1
        GT100_default_replace(2298, 1, GetJsonHex("fx2_acsim_high", a));                 //copy FX2
        GT100_default_replace(2299, 1, GetJsonHex("fx2_acsim_body", a));                 //copy FX2
        GT100_default_replace(2300, 1, GetJsonHex("fx2_acsim_low", a));                  //copy FX2
        GT100_default_replace(2302, 1, GetJsonHex("fx2_acsim_level", a));                //copy FX2
        GT100_default_replace(2304, 1, GetJsonHex("fx2_rotary2_balance", a));            //copy FX2
        GT100_default_replace(2305, 1, GetJsonHex("fx2_rotary2_speed_sel", a));          //copy FX2
        GT100_default_replace(2306, 1, GetJsonHex("fx2_rotary2_rate_slow", a));          //copy FX2
        GT100_default_replace(2307, 1, GetJsonHex("fx2_rotary2_rate_fast", a));          //copy FX2
        GT100_default_replace(2308, 1, GetJsonHex("fx2_rotary2_risetime", a));           //copy FX2
        GT100_default_replace(2309, 1, GetJsonHex("fx2_rotary2_falltime", a));           //copy FX2
        GT100_default_replace(2310, 1, GetJsonHex("fx2_rotary2_depth", a));              //copy FX2
        GT100_default_replace(2311, 1, GetJsonHex("fx2_rotary2_level", a));              //copy FX2
        GT100_default_replace(2312, 1, GetJsonHex("fx2_rotary2_direct_mix", a));         //copy FX2
        GT100_default_replace(2314, 1, GetJsonHex("prm_fx2_teraecho_mode", a));          //copy FX2
        GT100_default_replace(2315, 1, GetJsonHex("prm_fx2_teraecho_time", a));          //copy FX2
        GT100_default_replace(2316, 1, GetJsonHex("prm_fx2_teraecho_feedback", a));      //copy FX2
        GT100_default_replace(2317, 1, GetJsonHex("prm_fx2_teraecho_tone", a));          //copy FX2
        GT100_default_replace(2318, 1, GetJsonHex("prm_fx2_teraecho_effect_level", a));  //copy FX2
        GT100_default_replace(2319, 1, GetJsonHex("prm_fx2_teraecho_direct_mix", a));    //copy FX2
        GT100_default_replace(2320, 1, GetJsonHex("prm_fx2_teraecho_hold", a));          //copy FX2
        GT100_default_replace(2322, 1, GetJsonHex("prm_fx2_overtone_detune", a));        //copy FX2
        GT100_default_replace(2323, 1, GetJsonHex("prm_fx2_overtone_tone", a));          //copy FX2
        GT100_default_replace(2324, 1, GetJsonHex("prm_fx2_overtone_upper_level", a));   //copy FX2
        GT100_default_replace(2325, 1, GetJsonHex("prm_fx2_overtone_lower_level", a));   //copy FX2
        GT100_default_replace(2326, 1, GetJsonHex("prm_fx2_overtone_direct_level", a));  //copy FX2


        GT100_default_replace(2328, 1, GetJsonHex("fx1_phaser90e_script", a));    //copy
        GT100_default_replace(2329, 1, GetJsonHex("fx1_phaser90e_speed", a));     //copy
        GT100_default_replace(2330, 1, GetJsonHex("fx1_flanger117e_manual", a));  //copy
        GT100_default_replace(2331, 1, GetJsonHex("fx1_flanger117e_width", a));   //copy
        GT100_default_replace(2332, 1, GetJsonHex("fx1_flanger117e_speed", a));   //copy
        GT100_default_replace(2333, 1, GetJsonHex("fx1_flanger117e_regen", a));   //copy
        GT100_default_replace(2334, 1, GetJsonHex("fx2_phaser90e_script", a));    //copy
        GT100_default_replace(2335, 1, GetJsonHex("fx2_phaser90e_speed", a));     //copy
        GT100_default_replace(2336, 1, GetJsonHex("fx2_flanger117e_manual", a));  //copy
        GT100_default_replace(2337, 1, GetJsonHex("fx2_flanger117e_width", a));   //copy
        GT100_default_replace(2338, 1, GetJsonHex("fx2_flanger117e_speed", a));   //copy
        GT100_default_replace(2339, 1, GetJsonHex("fx2_flanger117e_regen", a));   //copy
        GT100_default_replace(2340, 1, GetJsonHex("delay_vtg_lpf", a));           //copy
        GT100_default_replace(2341, 1, GetJsonHex("delay_vtg_filter", a));        //copy
        GT100_default_replace(2342, 1, GetJsonHex("delay_vtg_feedback_phase", a));//copy
        GT100_default_replace(2343, 1, GetJsonHex("delay_vtg_effect_phase", a));  //copy
        GT100_default_replace(2344, 1, GetJsonHex("delay_vtg_mod_sw", a));        //copy
        GT100_default_replace(2345, 1, GetJsonHex("delay2_on_off", a));           //copy
        GT100_default_replace(2346, 1, GetJsonHex("delay2_type", a));             //copy
//        GT100_default_replace(2347, 2, GetJsonHex("delay2_delay_time", a));       //copy
        GT100_default_replace(2347, 1, GetJsonHex("delay2_delay_time_h", a));     //copy
        GT100_default_replace(2348, 1, GetJsonHex("delay2_delay_time_l", a));     //copy
        GT100_default_replace(2349, 1, GetJsonHex("delay2_f_back", a));           //copy
        GT100_default_replace(2350, 1, GetJsonHex("delay2_high_cut", a));         //copy
        GT100_default_replace(2351, 1, GetJsonHex("delay2_effect_level", a));     //copy
        GT100_default_replace(2352, 1, GetJsonHex("delay2_direct_mix", a));       //copy
        GT100_default_replace(2353, 1, GetJsonHex("delay2_tap_time", a));         //copy
//      GT100_default_replace(2354, 2, GetJsonHex("delay2_d1_time", a));          //copy
        GT100_default_replace(2354, 1, GetJsonHex("delay2_d1_time_h", a));        //copy
        GT100_default_replace(2355, 1, GetJsonHex("delay2_d1_time_l", a));        //copy
        GT100_default_replace(2356, 1, GetJsonHex("delay2_d1_f_back", a));        //copy
        GT100_default_replace(2357, 1, GetJsonHex("delay2_d1_hi_cut", a));        //copy
        GT100_default_replace(2358, 1, GetJsonHex("delay2_d1_level", a));         //copy
//        GT100_default_replace(2359, 2, GetJsonHex("delay2_d2_time", a));          //copy
        GT100_default_replace(2359, 1, GetJsonHex("delay2_d2_time_h", a));        //copy
        GT100_default_replace(2360, 1, GetJsonHex("delay2_d2_time_l", a));        //copy
        GT100_default_replace(2361, 1, GetJsonHex("delay2_d2_f_back", a));        //copy
        GT100_default_replace(2362, 1, GetJsonHex("delay2_d2_hi_cut", a));        //copy
        GT100_default_replace(2363, 1, GetJsonHex("delay2_d2_level", a));         //copy
        GT100_default_replace(2364, 1, GetJsonHex("delay2_mod_rate", a));         //copy
        GT100_default_replace(2365, 1, GetJsonHex("delay2_mod_depth", a));        //copy
        GT100_default_replace(2366, 1, GetJsonHex("delay2_vtg_lpf", a));          //copy
        GT100_default_replace(2367, 1, GetJsonHex("delay2_vtg_filter", a));       //copy
        GT100_default_replace(2368, 1, GetJsonHex("delay2_vtg_feedback_phase", a));//copy
        GT100_default_replace(2369, 1, GetJsonHex("delay2_vtg_effect_phase", a)); //copy
        GT100_default_replace(2370, 1, GetJsonHex("delay2_vtg_mod_sw", a));       //copy
        GT100_default_replace(2371, 1, GetJsonHex("fx1_wah95e_pedal_pos", a));
        GT100_default_replace(2372, 1, GetJsonHex("fx1_wah95e_pedal_min", a));
        GT100_default_replace(2373, 1, GetJsonHex("fx1_wah95e_pedal_max", a));
        GT100_default_replace(2374, 1, GetJsonHex("fx1_wah95e_effect_level", a));
        GT100_default_replace(2375, 1, GetJsonHex("fx1_wah95e_direct_mix", a));
        GT100_default_replace(2376, 1, GetJsonHex("fx1_dc30_selector", a));
        GT100_default_replace(2377, 1, GetJsonHex("fx1_dc30_input_volume", a));
        GT100_default_replace(2378, 1, GetJsonHex("fx1_dc30_chorus_intensity", a));
        GT100_default_replace(2379, 1, GetJsonHex("fx1_dc30_echo_repeat_rate_h", a));
        GT100_default_replace(2380, 1, GetJsonHex("fx1_dc30_echo_repeat_rate_l", a));
        GT100_default_replace(2381, 1, GetJsonHex("fx1_dc30_echo_intensity", a));
        GT100_default_replace(2382, 1, GetJsonHex("fx1_dc30_echo_volume", a));
        GT100_default_replace(2383, 1, GetJsonHex("fx1_dc30_tone", a));
        GT100_default_replace(2384, 1, GetJsonHex("fx1_dc30_output", a));
        GT100_default_replace(2385, 1, GetJsonHex("fx2_wah95e_pedal_pos", a));
        GT100_default_replace(2386, 1, GetJsonHex("fx2_wah95e_pedal_min", a));
        GT100_default_replace(2387, 1, GetJsonHex("fx2_wah95e_pedal_max", a));
        GT100_default_replace(2388, 1, GetJsonHex("fx2_wah95e_effect_level", a));
        GT100_default_replace(2389, 1, GetJsonHex("fx2_wah95e_direct_mix", a));
        GT100_default_replace(2390, 1, GetJsonHex("fx2_dc30_selector", a));
        GT100_default_replace(2391, 1, GetJsonHex("fx2_dc30_input_volume", a));
        GT100_default_replace(2392, 1, GetJsonHex("fx2_dc30_chorus_intensity", a));
        GT100_default_replace(2393, 1, GetJsonHex("fx2_dc30_echo_repeat_rate_h", a));
        GT100_default_replace(2394, 1, GetJsonHex("fx2_dc30_echo_repeat_rate_l", a));
        //int addr11 = 2408;
        GT100_default_replace(2408, 1, GetJsonHex("fx2_dc30_echo_intensity", a));
        GT100_default_replace(2409, 1, GetJsonHex("fx2_dc30_echo_volume", a));
        GT100_default_replace(2410, 1, GetJsonHex("fx2_dc30_tone", a));
        GT100_default_replace(2411, 1, GetJsonHex("fx2_dc30_output", a));
        GT100_default_replace(2412, 1, GetJsonHex("eq_type", a));
        GT100_default_replace(2413, 1, GetJsonHex("eq_geq_31hz", a));
        GT100_default_replace(2414, 1, GetJsonHex("eq_geq_62hz", a));
        GT100_default_replace(2415, 1, GetJsonHex("eq_geq_125hz", a));
        GT100_default_replace(2416, 1, GetJsonHex("eq_geq_250hz", a));
        GT100_default_replace(2417, 1, GetJsonHex("eq_geq_500hz", a));
        GT100_default_replace(2418, 1, GetJsonHex("eq_geq_1khz", a));
        GT100_default_replace(2419, 1, GetJsonHex("eq_geq_2khz", a));
        GT100_default_replace(2420, 1, GetJsonHex("eq_geq_4khz", a));
        GT100_default_replace(2421, 1, GetJsonHex("eq_geq_8khz", a));
        GT100_default_replace(2422, 1, GetJsonHex("eq_geq_16khz", a));
        GT100_default_replace(2423, 1, GetJsonHex("eq_geq_level", a));

        GT100_default_replace(2423, 1, GetJsonHex("pedal_fx_position", a));
        GT100_default_replace(2423, 1, GetJsonHex("pedal_fx_type", a));
        GT100_default_replace(2423, 1, GetJsonHex("pedal_fx_evh95_position", a));
        GT100_default_replace(2423, 1, GetJsonHex("pedal_fx_evh95_pedal_min", a));
        GT100_default_replace(2423, 1, GetJsonHex("pedal_fx_evh95_pedal_max", a));
        GT100_default_replace(2423, 1, GetJsonHex("pedal_fx_evh95_effect_level", a));
        GT100_default_replace(2423, 1, GetJsonHex("pedal_fx_evh95_direct_mix", a));
        GT100_default_replace(2423, 1, GetJsonHex("fx1_heavy_oct_1oct_level", a));
        GT100_default_replace(2423, 1, GetJsonHex("fx1_heavy_oct_2oct_level", a));
        GT100_default_replace(2423, 1, GetJsonHex("fx1_heavy_oct_direct_mix", a));
        GT100_default_replace(2423, 1, GetJsonHex("fx2_heavy_oct_1oct_level", a));
        GT100_default_replace(2423, 1, GetJsonHex("fx2_heavy_oct_2oct_level", a));
        GT100_default_replace(2423, 1, GetJsonHex("fx2_heavy_oct_direct_mix", a));


        int addr12 = 2549;
        GT100_default_replace(addr12, 1, GetJsonHex("chain_ptn", a));                  //copy KATANA Chain
        GT100_default_replace(addr12+1, 1, GetJsonHex("fxbox_asgn_fx1a_g", a));          //copy
        GT100_default_replace(addr12+2, 1, GetJsonHex("fxbox_asgn_fx1a_r", a));          //copy
        GT100_default_replace(addr12+3, 1, GetJsonHex("fxbox_asgn_fx1a_y", a));          //copy
        GT100_default_replace(addr12+4, 1, GetJsonHex("fxbox_asgn_fx1b_g", a));          //copy
        GT100_default_replace(addr12+5, 1, GetJsonHex("fxbox_asgn_fx1b_r", a));          //copy
        GT100_default_replace(addr12+6, 1, GetJsonHex("fxbox_asgn_fx1b_y", a));          //copy
        GT100_default_replace(addr12+7, 1, GetJsonHex("fxbox_asgn_fx2a_g", a));          //copy
        GT100_default_replace(addr12+8, 1, GetJsonHex("fxbox_asgn_fx2a_r", a));          //copy
        GT100_default_replace(addr12+9, 1, GetJsonHex("fxbox_asgn_fx2a_y", a));          //copy
        GT100_default_replace(addr12+10, 1, GetJsonHex("fxbox_asgn_fx2b_g", a));          //copy
        GT100_default_replace(addr12+11, 1, GetJsonHex("fxbox_asgn_fx2b_r", a));          //copy
        GT100_default_replace(addr12+12, 1, GetJsonHex("fxbox_asgn_fx2b_y", a));          //copy
        GT100_default_replace(addr12+13, 1, GetJsonHex("fxbox_asgn_fx3_g", a));           //copy
        GT100_default_replace(addr12+14, 1, GetJsonHex("fxbox_asgn_fx3_r", a));           //copy
        GT100_default_replace(addr12+15, 1, GetJsonHex("fxbox_asgn_fx3_y", a));           //copy
        GT100_default_replace(addr12+16, 1, GetJsonHex("fxbox_sel_fx1b", a));             //copy
        GT100_default_replace(addr12+17, 1, GetJsonHex("fxbox_sel_fx2a", a));             //copy
        GT100_default_replace(addr12+18, 1, GetJsonHex("fxbox_sel_fx3", a));              //copy
        GT100_default_replace(addr12+19, 1, GetJsonHex("fx_active_ab_fx1", a));           //copy
        GT100_default_replace(addr12+20, 1, GetJsonHex("fx_active_ab_fx2", a));           //copy
        GT100_default_replace(addr12+21, 1, GetJsonHex("fxbox_sel_fx1a", a));             //copy
        GT100_default_replace(addr12+22, 1, GetJsonHex("fxbox_sel_fx2b", a));             //copy
        GT100_default_replace(addr12+23, 1, GetJsonHex("send_return_position", a));       //copy
        GT100_default_replace(addr12+24, 1, GetJsonHex("eq_position", a));                //copy
        GT100_default_replace(addr12+25, 1, GetJsonHex("fxbox_asgn_fx3b_g", a));          //copy
        GT100_default_replace(addr12+26, 1, GetJsonHex("fxbox_asgn_fx3b_r", a));          //copy
        GT100_default_replace(addr12+27, 1, GetJsonHex("fxbox_asgn_fx3b_y", a));          //copy
        GT100_default_replace(addr12+28, 1, GetJsonHex("fxbox_layer_fx3_g", a));          //copy
        GT100_default_replace(addr12+29, 1, GetJsonHex("fxbox_layer_fx3_r", a));          //copy
        GT100_default_replace(addr12+30, 1, GetJsonHex("fxbox_layer_fx3_y", a));          //copy
        GT100_default_replace(addr12+31, 1, GetJsonHex("pedal_function_exp_pedal", a));
        GT100_default_replace(addr12+32, 1, GetJsonHex("pedal_function_gafc_exp1", a));
        GT100_default_replace(addr12+33, 1, GetJsonHex("pedal_function_gafc_exp2", a));

        GT100_default_replace(addr12+48, 1, GetJsonHex("knob_assign_booster", a));
        GT100_default_replace(addr12+49, 1, GetJsonHex("knob_assign_delay", a));
        GT100_default_replace(addr12+50, 1, GetJsonHex("knob_assign_reverb", a));
        GT100_default_replace(addr12+51, 1, GetJsonHex("knob_assign_chorus", a));
        GT100_default_replace(addr12+52, 1, GetJsonHex("knob_assign_flanger", a));
        GT100_default_replace(addr12+53, 1, GetJsonHex("knob_assign_phaser", a));
        GT100_default_replace(addr12+54, 1, GetJsonHex("knob_assign_uni_v", a));
        GT100_default_replace(addr12+55, 1, GetJsonHex("knob_assign_tremolo", a));
        GT100_default_replace(addr12+56, 1, GetJsonHex("knob_assign_vibrato", a));
        GT100_default_replace(addr12+57, 1, GetJsonHex("knob_assign_rotary", a));
        GT100_default_replace(addr12+58, 1, GetJsonHex("knob_assign_ring_mod", a));
        GT100_default_replace(addr12+59, 1, GetJsonHex("knob_assign_slow_gear", a));
        GT100_default_replace(addr12+60, 1, GetJsonHex("knob_assign_slicer", a));
        GT100_default_replace(addr12+61, 1, GetJsonHex("knob_assign_comp", a));
        GT100_default_replace(addr12+62, 1, GetJsonHex("knob_assign_limiter", a));
        GT100_default_replace(addr12+63, 1, GetJsonHex("knob_assign_t_wah", a));
        GT100_default_replace(addr12+64, 1, GetJsonHex("knob_assign_auto_wah", a));
        GT100_default_replace(addr12+65, 1, GetJsonHex("knob_assign_pedal_wah", a));
        GT100_default_replace(addr12+66, 1, GetJsonHex("knob_assign_geq", a));
        GT100_default_replace(addr12+67, 1, GetJsonHex("knob_assign_peq", a));
        GT100_default_replace(addr12+68, 1, GetJsonHex("knob_assign_guitar_sim", a));
        GT100_default_replace(addr12+69, 1, GetJsonHex("knob_assign_ac_guitar_sim", a));
        GT100_default_replace(addr12+70, 1, GetJsonHex("knob_assign_ac_processor", a));
        GT100_default_replace(addr12+71, 1, GetJsonHex("knob_assign_wave_synth", a));
        GT100_default_replace(addr12+72, 1, GetJsonHex("knob_assign_octave", a));
        GT100_default_replace(addr12+73, 1, GetJsonHex("knob_assign_pitch_shifter", a));
        GT100_default_replace(addr12+74, 1, GetJsonHex("knob_assign_harmonist", a));
        GT100_default_replace(addr12+75, 1, GetJsonHex("knob_assign_humanizer", a));
        GT100_default_replace(addr12+76, 1, GetJsonHex("knob_assign_evh_phaser", a));
        GT100_default_replace(addr12+77, 1, GetJsonHex("knob_assign_evh_flanger", a));
        GT100_default_replace(addr12+78, 1, GetJsonHex("knob_assign_wah95e", a));
        GT100_default_replace(addr12+79, 1, GetJsonHex("knob_assign_dc30", a));
        GT100_default_replace(addr12+79, 1, GetJsonHex("knob_assign_heavy_oct", a));

        int addr13 = 2690;
        GT100_default_replace(addr13+0, 1, GetJsonHex("exp_pedal_assign_booster", a));
        GT100_default_replace(addr13+1, 1, GetJsonHex("exp_pedal_assign_delay", a));
        GT100_default_replace(addr13+2, 1, GetJsonHex("exp_pedal_assign_reverb", a));
        GT100_default_replace(addr13+3, 1, GetJsonHex("exp_pedal_assign_chorus", a));
        GT100_default_replace(addr13+4, 1, GetJsonHex("exp_pedal_assign_flanger", a));
        GT100_default_replace(addr13+5, 1, GetJsonHex("exp_pedal_assign_phaser", a));
        GT100_default_replace(addr13+6, 1, GetJsonHex("exp_pedal_assign_uni_v", a));
        GT100_default_replace(addr13+7, 1, GetJsonHex("exp_pedal_assign_tremolo", a));
        GT100_default_replace(addr13+8, 1, GetJsonHex("exp_pedal_assign_vibrato", a));
        GT100_default_replace(addr13+9, 1, GetJsonHex("exp_pedal_assign_rotary", a));
        GT100_default_replace(addr13+10, 1, GetJsonHex("exp_pedal_assign_ring_mod", a));
        GT100_default_replace(addr13+11, 1, GetJsonHex("exp_pedal_assign_slow_gear", a));
        GT100_default_replace(addr13+12, 1, GetJsonHex("exp_pedal_assign_slicer", a));
        GT100_default_replace(addr13+13, 1, GetJsonHex("exp_pedal_assign_comp", a));
        GT100_default_replace(addr13+14, 1, GetJsonHex("exp_pedal_assign_limiter", a));
        GT100_default_replace(addr13+15, 1, GetJsonHex("exp_pedal_assign_t_wah", a));
        GT100_default_replace(addr13+16, 1, GetJsonHex("exp_pedal_assign_auto_wah", a));
        GT100_default_replace(addr13+17, 1, GetJsonHex("exp_pedal_assign_pedal_wah", a));
        GT100_default_replace(addr13+18, 1, GetJsonHex("exp_pedal_assign_geq", a));
        GT100_default_replace(addr13+19, 1, GetJsonHex("exp_pedal_assign_peq", a));
        GT100_default_replace(addr13+20, 1, GetJsonHex("exp_pedal_assign_guitar_sim", a));
        GT100_default_replace(addr13+21, 1, GetJsonHex("exp_pedal_assign_ac_guitar_sim", a));
        GT100_default_replace(addr13+22, 1, GetJsonHex("exp_pedal_assign_ac_processor", a));
        GT100_default_replace(addr13+23, 1, GetJsonHex("exp_pedal_assign_wave_synth", a));
        GT100_default_replace(addr13+24, 1, GetJsonHex("exp_pedal_assign_octave", a));
        GT100_default_replace(addr13+25, 1, GetJsonHex("exp_pedal_assign_pitch_shifter", a));
        GT100_default_replace(addr13+26, 1, GetJsonHex("exp_pedal_assign_harmonist", a));
        GT100_default_replace(addr13+27, 1, GetJsonHex("exp_pedal_assign_humanizer", a));
        GT100_default_replace(addr13+28, 1, GetJsonHex("exp_pedal_assign_evh_phaser", a));
        GT100_default_replace(addr13+29, 1, GetJsonHex("exp_pedal_assign_evh_flanger", a));
        GT100_default_replace(addr13+30, 1, GetJsonHex("exp_pedal_assign_wah95e", a));
        GT100_default_replace(addr13+31, 1, GetJsonHex("exp_pedal_assign_dc30", a));
        GT100_default_replace(addr13+32, 1, GetJsonHex("exp_pedal_assign_booster_min", a));
        GT100_default_replace(addr13+33, 1, GetJsonHex("exp_pedal_assign_booster_max", a));
        GT100_default_replace(addr13+34, 1, GetJsonHex("exp_pedal_assign_delay_min_h", a));
        GT100_default_replace(addr13+35, 1, GetJsonHex("exp_pedal_assign_delay_min_l", a));
        GT100_default_replace(addr13+36, 1, GetJsonHex("exp_pedal_assign_delay_max_h", a));
        GT100_default_replace(addr13+37, 1, GetJsonHex("exp_pedal_assign_delay_max_l", a));
        GT100_default_replace(addr13+38, 1, GetJsonHex("exp_pedal_assign_reverb_min_h", a));
        GT100_default_replace(addr13+39, 1, GetJsonHex("exp_pedal_assign_reverb_min_l", a));
        GT100_default_replace(addr13+40, 1, GetJsonHex("exp_pedal_assign_reverb_max_h", a));
        GT100_default_replace(addr13+41, 1, GetJsonHex("exp_pedal_assign_reverb_max_l", a));
        GT100_default_replace(addr13+42, 1, GetJsonHex("exp_pedal_assign_chorus_min", a));
        GT100_default_replace(addr13+43, 1, GetJsonHex("exp_pedal_assign_chorus_max", a));
        GT100_default_replace(addr13+44, 1, GetJsonHex("exp_pedal_assign_flanger_min", a));
        GT100_default_replace(addr13+45, 1, GetJsonHex("exp_pedal_assign_flanger_max", a));
        GT100_default_replace(addr13+46, 1, GetJsonHex("exp_pedal_assign_phaser_min", a));
        GT100_default_replace(addr13+47, 1, GetJsonHex("exp_pedal_assign_phaser_max", a));
        GT100_default_replace(addr13+48, 1, GetJsonHex("exp_pedal_assign_uni_v_min", a));
        GT100_default_replace(addr13+49, 1, GetJsonHex("exp_pedal_assign_uni_v_max", a));
        GT100_default_replace(addr13+50, 1, GetJsonHex("exp_pedal_assign_tremolo_min", a));
        GT100_default_replace(addr13+51, 1, GetJsonHex("exp_pedal_assign_tremolo_max", a));
        GT100_default_replace(addr13+52, 1, GetJsonHex("exp_pedal_assign_vibrato_min", a));
        GT100_default_replace(addr13+53, 1, GetJsonHex("exp_pedal_assign_vibrato_max", a));
        GT100_default_replace(addr13+54, 1, GetJsonHex("exp_pedal_assign_rotary_min", a));
        GT100_default_replace(addr13+55, 1, GetJsonHex("exp_pedal_assign_rotary_max", a));
        GT100_default_replace(addr13+56, 1, GetJsonHex("exp_pedal_assign_ring_mod_min", a));
        GT100_default_replace(addr13+57, 1, GetJsonHex("exp_pedal_assign_ring_mod_max", a));
        GT100_default_replace(addr13+58, 1, GetJsonHex("exp_pedal_assign_slow_gear_min", a));
        GT100_default_replace(addr13+59, 1, GetJsonHex("exp_pedal_assign_slow_gear_max", a));
        GT100_default_replace(addr13+60, 1, GetJsonHex("exp_pedal_assign_slicer_min", a));
        GT100_default_replace(addr13+61, 1, GetJsonHex("exp_pedal_assign_slicer_max", a));
        GT100_default_replace(addr13+62, 1, GetJsonHex("exp_pedal_assign_comp_min", a));
        GT100_default_replace(addr13+63, 1, GetJsonHex("exp_pedal_assign_comp_max", a));
        GT100_default_replace(addr13+64, 1, GetJsonHex("exp_pedal_assign_limiter_min", a));
        GT100_default_replace(addr13+65, 1, GetJsonHex("exp_pedal_assign_limiter_max", a));
        GT100_default_replace(addr13+66, 1, GetJsonHex("exp_pedal_assign_t_wah_min", a));
        GT100_default_replace(addr13+67, 1, GetJsonHex("exp_pedal_assign_t_wah_max", a));
        GT100_default_replace(addr13+68, 1, GetJsonHex("exp_pedal_assign_auto_wah_min", a));
        GT100_default_replace(addr13+69, 1, GetJsonHex("exp_pedal_assign_auto_wah_max", a));
        GT100_default_replace(addr13+70, 1, GetJsonHex("exp_pedal_assign_pedal_wah_min", a));
        GT100_default_replace(addr13+71, 1, GetJsonHex("exp_pedal_assign_pedal_wah_max", a));
        GT100_default_replace(addr13+72, 1, GetJsonHex("exp_pedal_assign_geq_min", a));
        GT100_default_replace(addr13+73, 1, GetJsonHex("exp_pedal_assign_geq_max", a));
        GT100_default_replace(addr13+74, 1, GetJsonHex("exp_pedal_assign_peq_min", a));
        GT100_default_replace(addr13+75, 1, GetJsonHex("exp_pedal_assign_peq_max", a));
        GT100_default_replace(addr13+76, 1, GetJsonHex("exp_pedal_assign_guitar_sim_min", a));
        GT100_default_replace(addr13+77, 1, GetJsonHex("exp_pedal_assign_guitar_sim_max", a));
        GT100_default_replace(addr13+78, 1, GetJsonHex("exp_pedal_assign_ac_guitar_sim_min", a));
        GT100_default_replace(addr13+79, 1, GetJsonHex("exp_pedal_assign_ac_guitar_sim_max", a));
        GT100_default_replace(addr13+80, 1, GetJsonHex("exp_pedal_assign_ac_processor_min", a));
        GT100_default_replace(addr13+81, 1, GetJsonHex("exp_pedal_assign_ac_processor_max", a));
        GT100_default_replace(addr13+82, 1, GetJsonHex("exp_pedal_assign_wave_synth_min", a));
        GT100_default_replace(addr13+83, 1, GetJsonHex("exp_pedal_assign_wave_synth_max", a));
        GT100_default_replace(addr13+84, 1, GetJsonHex("exp_pedal_assign_octave_min", a));
        GT100_default_replace(addr13+85, 1, GetJsonHex("exp_pedal_assign_octave_max", a));
        GT100_default_replace(addr13+86, 1, GetJsonHex("exp_pedal_assign_pitch_shifter_min_h", a));
        GT100_default_replace(addr13+87, 1, GetJsonHex("exp_pedal_assign_pitch_shifter_min_l", a));
        GT100_default_replace(addr13+88, 1, GetJsonHex("exp_pedal_assign_pitch_shifter_max_h", a));
        GT100_default_replace(addr13+89, 1, GetJsonHex("exp_pedal_assign_pitch_shifter_max_l", a));
        GT100_default_replace(addr13+90, 1, GetJsonHex("exp_pedal_assign_harmonist_min_h", a));
        GT100_default_replace(addr13+91, 1, GetJsonHex("exp_pedal_assign_harmonist_min_l", a));
        GT100_default_replace(addr13+92, 1, GetJsonHex("exp_pedal_assign_harmonist_max_h", a));
        GT100_default_replace(addr13+93, 1, GetJsonHex("exp_pedal_assign_harmonist_max_l", a));
        GT100_default_replace(addr13+94, 1, GetJsonHex("exp_pedal_assign_humanizer_min", a));
        GT100_default_replace(addr13+95, 1, GetJsonHex("exp_pedal_assign_humanizer_max", a));
        GT100_default_replace(addr13+96, 1, GetJsonHex("exp_pedal_assign_evh_phaser_min", a));
        GT100_default_replace(addr13+97, 1, GetJsonHex("exp_pedal_assign_evh_phaser_max", a));
        GT100_default_replace(addr13+98, 1, GetJsonHex("exp_pedal_assign_evh_flanger_min", a));
        GT100_default_replace(addr13+99, 1, GetJsonHex("exp_pedal_assign_evh_flanger_max", a));
        GT100_default_replace(addr13+100, 1, GetJsonHex("exp_pedal_assign_wah95e_min", a));
        GT100_default_replace(addr13+101, 1, GetJsonHex("exp_pedal_assign_wah95e_max", a));
        GT100_default_replace(addr13+102, 1, GetJsonHex("exp_pedal_assign_dc30_min_h", a));
        GT100_default_replace(addr13+103, 1, GetJsonHex("exp_pedal_assign_dc30_min_l", a));
        GT100_default_replace(addr13+104, 1, GetJsonHex("exp_pedal_assign_dc30_max_h", a));
        GT100_default_replace(addr13+105, 1, GetJsonHex("exp_pedal_assign_dc30_max_l", a));
        GT100_default_replace(addr13+106, 1, GetJsonHex("exp_pedal_assign_heavy_oct", a));
        GT100_default_replace(addr13+107, 1, GetJsonHex("exp_pedal_assign_heavy_oct_min", a));
        GT100_default_replace(addr13+108, 1, GetJsonHex("exp_pedal_assign_heavy_oct_max", a));

        int addr14 = 2831;
        GT100_default_replace(addr14+0, 1, GetJsonHex("gafc_exp1_assign_booster", a));
        GT100_default_replace(addr14+1, 1, GetJsonHex("gafc_exp1_assign_delay", a));
        GT100_default_replace(addr14+2, 1, GetJsonHex("gafc_exp1_assign_reverb", a));
        GT100_default_replace(addr14+3, 1, GetJsonHex("gafc_exp1_assign_chorus", a));
        GT100_default_replace(addr14+4, 1, GetJsonHex("gafc_exp1_assign_flanger", a));
        GT100_default_replace(addr14+5, 1, GetJsonHex("gafc_exp1_assign_phaser", a));
        GT100_default_replace(addr14+6, 1, GetJsonHex("gafc_exp1_assign_uni_v", a));
        GT100_default_replace(addr14+7, 1, GetJsonHex("gafc_exp1_assign_tremolo", a));
        GT100_default_replace(addr14+8, 1, GetJsonHex("gafc_exp1_assign_vibrato", a));
        GT100_default_replace(addr14+9, 1, GetJsonHex("gafc_exp1_assign_rotary", a));
        GT100_default_replace(addr14+10, 1, GetJsonHex("gafc_exp1_assign_ring_mod", a));
        GT100_default_replace(addr14+11, 1, GetJsonHex("gafc_exp1_assign_slow_gear", a));
        GT100_default_replace(addr14+12, 1, GetJsonHex("gafc_exp1_assign_slicer", a));
        GT100_default_replace(addr14+13, 1, GetJsonHex("gafc_exp1_assign_comp", a));
        GT100_default_replace(addr14+14, 1, GetJsonHex("gafc_exp1_assign_limiter", a));
        GT100_default_replace(addr14+15, 1, GetJsonHex("gafc_exp1_assign_t_wah", a));
        GT100_default_replace(addr14+16, 1, GetJsonHex("gafc_exp1_assign_auto_wah", a));
        GT100_default_replace(addr14+17, 1, GetJsonHex("gafc_exp1_assign_pedal_wah", a));
        GT100_default_replace(addr14+18, 1, GetJsonHex("gafc_exp1_assign_geq", a));
        GT100_default_replace(addr14+19, 1, GetJsonHex("gafc_exp1_assign_peq", a));
        GT100_default_replace(addr14+20, 1, GetJsonHex("gafc_exp1_assign_guitar_sim", a));
        GT100_default_replace(addr14+21, 1, GetJsonHex("gafc_exp1_assign_ac_guitar_sim", a));
        GT100_default_replace(addr14+22, 1, GetJsonHex("gafc_exp1_assign_ac_processor", a));
        GT100_default_replace(addr14+23, 1, GetJsonHex("gafc_exp1_assign_wave_synth", a));
        GT100_default_replace(addr14+24, 1, GetJsonHex("gafc_exp1_assign_octave", a));
        GT100_default_replace(addr14+25, 1, GetJsonHex("gafc_exp1_assign_pitch_shifter", a));
        GT100_default_replace(addr14+26, 1, GetJsonHex("gafc_exp1_assign_harmonist", a));
        GT100_default_replace(addr14+27, 1, GetJsonHex("gafc_exp1_assign_humanizer", a));
        GT100_default_replace(addr14+28, 1, GetJsonHex("gafc_exp1_assign_evh_phaser", a));
        GT100_default_replace(addr14+29, 1, GetJsonHex("gafc_exp1_assign_evh_flanger", a));
        GT100_default_replace(addr14+30, 1, GetJsonHex("gafc_exp1_assign_wah95e", a));
        GT100_default_replace(addr14+31, 1, GetJsonHex("gafc_exp1_assign_dc30", a));
        GT100_default_replace(addr14+32, 1, GetJsonHex("gafc_exp1_assign_booster_min", a));
        GT100_default_replace(addr14+33, 1, GetJsonHex("gafc_exp1_assign_booster_max", a));
        GT100_default_replace(addr14+34, 1, GetJsonHex("gafc_exp1_assign_delay_min_h", a));
        GT100_default_replace(addr14+35, 1, GetJsonHex("gafc_exp1_assign_delay_min_l", a));
        GT100_default_replace(addr14+36, 1, GetJsonHex("gafc_exp1_assign_delay_max_h", a));
        GT100_default_replace(addr14+37, 1, GetJsonHex("gafc_exp1_assign_delay_max_l", a));
        GT100_default_replace(addr14+38, 1, GetJsonHex("gafc_exp1_assign_reverb_min_h", a));
        GT100_default_replace(addr14+39, 1, GetJsonHex("gafc_exp1_assign_reverb_min_l", a));
        GT100_default_replace(addr14+40, 1, GetJsonHex("gafc_exp1_assign_reverb_max_h", a));
        GT100_default_replace(addr14+41, 1, GetJsonHex("gafc_exp1_assign_reverb_max_l", a));
        GT100_default_replace(addr14+42, 1, GetJsonHex("gafc_exp1_assign_chorus_min", a));
        GT100_default_replace(addr14+43, 1, GetJsonHex("gafc_exp1_assign_chorus_max", a));
        GT100_default_replace(addr14+44, 1, GetJsonHex("gafc_exp1_assign_flanger_min", a));
        GT100_default_replace(addr14+45, 1, GetJsonHex("gafc_exp1_assign_flanger_max", a));
        GT100_default_replace(addr14+46, 1, GetJsonHex("gafc_exp1_assign_phaser_min", a));
        GT100_default_replace(addr14+47, 1, GetJsonHex("gafc_exp1_assign_phaser_max", a));
        GT100_default_replace(addr14+48, 1, GetJsonHex("gafc_exp1_assign_uni_v_min", a));
        GT100_default_replace(addr14+49, 1, GetJsonHex("gafc_exp1_assign_uni_v_max", a));
        GT100_default_replace(addr14+50, 1, GetJsonHex("gafc_exp1_assign_tremolo_min", a));
        GT100_default_replace(addr14+51, 1, GetJsonHex("gafc_exp1_assign_tremolo_max", a));
        GT100_default_replace(addr14+52, 1, GetJsonHex("gafc_exp1_assign_vibrato_min", a));
        GT100_default_replace(addr14+53, 1, GetJsonHex("gafc_exp1_assign_vibrato_max", a));
        GT100_default_replace(addr14+54, 1, GetJsonHex("gafc_exp1_assign_rotary_min", a));
        GT100_default_replace(addr14+55, 1, GetJsonHex("gafc_exp1_assign_rotary_max", a));
        GT100_default_replace(addr14+56, 1, GetJsonHex("gafc_exp1_assign_ring_mod_min", a));
        GT100_default_replace(addr14+57, 1, GetJsonHex("gafc_exp1_assign_ring_mod_max", a));
        GT100_default_replace(addr14+58, 1, GetJsonHex("gafc_exp1_assign_slow_gear_min", a));
        GT100_default_replace(addr14+59, 1, GetJsonHex("gafc_exp1_assign_slow_gear_max", a));
        GT100_default_replace(addr14+60, 1, GetJsonHex("gafc_exp1_assign_slicer_min", a));
        GT100_default_replace(addr14+61, 1, GetJsonHex("gafc_exp1_assign_slicer_max", a));
        GT100_default_replace(addr14+62, 1, GetJsonHex("gafc_exp1_assign_comp_min", a));
        GT100_default_replace(addr14+63, 1, GetJsonHex("gafc_exp1_assign_comp_max", a));
        GT100_default_replace(addr14+64, 1, GetJsonHex("gafc_exp1_assign_limiter_min", a));
        GT100_default_replace(addr14+65, 1, GetJsonHex("gafc_exp1_assign_limiter_max", a));
        GT100_default_replace(addr14+66, 1, GetJsonHex("gafc_exp1_assign_t_wah_min", a));
        GT100_default_replace(addr14+67, 1, GetJsonHex("gafc_exp1_assign_t_wah_max", a));
        GT100_default_replace(addr14+68, 1, GetJsonHex("gafc_exp1_assign_auto_wah_min", a));
        GT100_default_replace(addr14+69, 1, GetJsonHex("gafc_exp1_assign_auto_wah_max", a));
        GT100_default_replace(addr14+70, 1, GetJsonHex("gafc_exp1_assign_pedal_wah_min", a));
        GT100_default_replace(addr14+71, 1, GetJsonHex("gafc_exp1_assign_pedal_wah_max", a));
        GT100_default_replace(addr14+72, 1, GetJsonHex("gafc_exp1_assign_geq_min", a));
        GT100_default_replace(addr14+73, 1, GetJsonHex("gafc_exp1_assign_geq_max", a));
        GT100_default_replace(addr14+74, 1, GetJsonHex("gafc_exp1_assign_peq_min", a));
        GT100_default_replace(addr14+75, 1, GetJsonHex("gafc_exp1_assign_peq_max", a));
        GT100_default_replace(addr14+76, 1, GetJsonHex("gafc_exp1_assign_guitar_sim_min", a));
        GT100_default_replace(addr14+77, 1, GetJsonHex("gafc_exp1_assign_guitar_sim_max", a));
        GT100_default_replace(addr14+78, 1, GetJsonHex("gafc_exp1_assign_ac_guitar_sim_min", a));
        GT100_default_replace(addr14+79, 1, GetJsonHex("gafc_exp1_assign_ac_guitar_sim_max", a));
        GT100_default_replace(addr14+80, 1, GetJsonHex("gafc_exp1_assign_ac_processor_min", a));
        GT100_default_replace(addr14+81, 1, GetJsonHex("gafc_exp1_assign_ac_processor_max", a));
        GT100_default_replace(addr14+82, 1, GetJsonHex("gafc_exp1_assign_wave_synth_min", a));
        GT100_default_replace(addr14+83, 1, GetJsonHex("gafc_exp1_assign_wave_synth_max", a));
        GT100_default_replace(addr14+84, 1, GetJsonHex("gafc_exp1_assign_octave_min", a));
        GT100_default_replace(addr14+85, 1, GetJsonHex("gafc_exp1_assign_octave_max", a));
        GT100_default_replace(addr14+86, 1, GetJsonHex("gafc_exp1_assign_pitch_shifter_min_h", a));
        GT100_default_replace(addr14+87, 1, GetJsonHex("gafc_exp1_assign_pitch_shifter_min_l", a));
        GT100_default_replace(addr14+88, 1, GetJsonHex("gafc_exp1_assign_pitch_shifter_max_h", a));
        GT100_default_replace(addr14+89, 1, GetJsonHex("gafc_exp1_assign_pitch_shifter_max_l", a));
        GT100_default_replace(addr14+90, 1, GetJsonHex("gafc_exp1_assign_harmonist_min_h", a));
        GT100_default_replace(addr14+91, 1, GetJsonHex("gafc_exp1_assign_harmonist_min_l", a));
        GT100_default_replace(addr14+92, 1, GetJsonHex("gafc_exp1_assign_harmonist_max_h", a));
        GT100_default_replace(addr14+93, 1, GetJsonHex("gafc_exp1_assign_harmonist_max_l", a));
        GT100_default_replace(addr14+94, 1, GetJsonHex("gafc_exp1_assign_humanizer_min", a));
        GT100_default_replace(addr14+95, 1, GetJsonHex("gafc_exp1_assign_humanizer_max", a));
        GT100_default_replace(addr14+96, 1, GetJsonHex("gafc_exp1_assign_evh_phaser_min", a));
        GT100_default_replace(addr14+97, 1, GetJsonHex("gafc_exp1_assign_evh_phaser_max", a));
        GT100_default_replace(addr14+98, 1, GetJsonHex("gafc_exp1_assign_evh_flanger_min", a));
        GT100_default_replace(addr14+99, 1, GetJsonHex("gafc_exp1_assign_evh_flanger_max", a));
        GT100_default_replace(addr14+100, 1, GetJsonHex("gafc_exp1_assign_wah95e_min", a));
        GT100_default_replace(addr14+101, 1, GetJsonHex("gafc_exp1_assign_wah95e_max", a));
        GT100_default_replace(addr14+102, 1, GetJsonHex("gafc_exp1_assign_dc30_min_h", a));
        GT100_default_replace(addr14+103, 1, GetJsonHex("gafc_exp1_assign_dc30_min_l", a));
        GT100_default_replace(addr14+104, 1, GetJsonHex("gafc_exp1_assign_dc30_max_h", a));
        GT100_default_replace(addr14+105, 1, GetJsonHex("gafc_exp1_assign_dc30_max_l", a));
        GT100_default_replace(addr14+106, 1, GetJsonHex("gafc_exp1_assign_heavy_oct", a));
        GT100_default_replace(addr14+107, 1, GetJsonHex("gafc_exp1_assign_heavy_oct_min", a));
        GT100_default_replace(addr14+108, 1, GetJsonHex("gafc_exp1_assign_heavy_oct_max", a));

        int addr15 = 2972;
        GT100_default_replace(addr15+0, 1, GetJsonHex("gafc_exp2_assign_booster", a));
        GT100_default_replace(addr15+1, 1, GetJsonHex("gafc_exp2_assign_delay", a));
        GT100_default_replace(addr15+2, 1, GetJsonHex("gafc_exp2_assign_reverb", a));
        GT100_default_replace(addr15+3, 1, GetJsonHex("gafc_exp2_assign_chorus", a));
        GT100_default_replace(addr15+4, 1, GetJsonHex("gafc_exp2_assign_flanger", a));
        GT100_default_replace(addr15+5, 1, GetJsonHex("gafc_exp2_assign_phaser", a));
        GT100_default_replace(addr15+6, 1, GetJsonHex("gafc_exp2_assign_uni_v", a));
        GT100_default_replace(addr15+7, 1, GetJsonHex("gafc_exp2_assign_tremolo", a));
        GT100_default_replace(addr15+8, 1, GetJsonHex("gafc_exp2_assign_vibrato", a));
        GT100_default_replace(addr15+9, 1, GetJsonHex("gafc_exp2_assign_rotary", a));
        GT100_default_replace(addr15+10, 1, GetJsonHex("gafc_exp2_assign_ring_mod", a));
        GT100_default_replace(addr15+11, 1, GetJsonHex("gafc_exp2_assign_slow_gear", a));
        GT100_default_replace(addr15+12, 1, GetJsonHex("gafc_exp2_assign_slicer", a));
        GT100_default_replace(addr15+13, 1, GetJsonHex("gafc_exp2_assign_comp", a));
        GT100_default_replace(addr15+14, 1, GetJsonHex("gafc_exp2_assign_limiter", a));
        GT100_default_replace(addr15+15, 1, GetJsonHex("gafc_exp2_assign_t_wah", a));
        GT100_default_replace(addr15+16, 1, GetJsonHex("gafc_exp2_assign_auto_wah", a));
        GT100_default_replace(addr15+17, 1, GetJsonHex("gafc_exp2_assign_pedal_wah", a));
        GT100_default_replace(addr15+18, 1, GetJsonHex("gafc_exp2_assign_geq", a));
        GT100_default_replace(addr15+19, 1, GetJsonHex("gafc_exp2_assign_peq", a));
        GT100_default_replace(addr15+20, 1, GetJsonHex("gafc_exp2_assign_guitar_sim", a));
        GT100_default_replace(addr15+21, 1, GetJsonHex("gafc_exp2_assign_ac_guitar_sim", a));
        GT100_default_replace(addr15+22, 1, GetJsonHex("gafc_exp2_assign_ac_processor", a));
        GT100_default_replace(addr15+23, 1, GetJsonHex("gafc_exp2_assign_wave_synth", a));
        GT100_default_replace(addr15+24, 1, GetJsonHex("gafc_exp2_assign_octave", a));
        GT100_default_replace(addr15+25, 1, GetJsonHex("gafc_exp2_assign_pitch_shifter", a));
        GT100_default_replace(addr15+26, 1, GetJsonHex("gafc_exp2_assign_harmonist", a));
        GT100_default_replace(addr15+27, 1, GetJsonHex("gafc_exp2_assign_humanizer", a));
        GT100_default_replace(addr15+28, 1, GetJsonHex("gafc_exp2_assign_evh_phaser", a));
        GT100_default_replace(addr15+29, 1, GetJsonHex("gafc_exp2_assign_evh_flanger", a));
        GT100_default_replace(addr15+30, 1, GetJsonHex("gafc_exp2_assign_wah95e", a));
        GT100_default_replace(addr15+31, 1, GetJsonHex("gafc_exp2_assign_dc30", a));
        GT100_default_replace(addr15+32, 1, GetJsonHex("gafc_exp2_assign_booster_min", a));
        GT100_default_replace(addr15+33, 1, GetJsonHex("gafc_exp2_assign_booster_max", a));
        GT100_default_replace(addr15+34, 1, GetJsonHex("gafc_exp2_assign_delay_min_h", a));
        GT100_default_replace(addr15+35, 1, GetJsonHex("gafc_exp2_assign_delay_min_l", a));
        GT100_default_replace(addr15+36, 1, GetJsonHex("gafc_exp2_assign_delay_max_h", a));
        GT100_default_replace(addr15+37, 1, GetJsonHex("gafc_exp2_assign_delay_max_l", a));
        GT100_default_replace(addr15+38, 1, GetJsonHex("gafc_exp2_assign_reverb_min_h", a));
        GT100_default_replace(addr15+39, 1, GetJsonHex("gafc_exp2_assign_reverb_min_l", a));
        GT100_default_replace(addr15+40, 1, GetJsonHex("gafc_exp2_assign_reverb_max_h", a));
        GT100_default_replace(addr15+41, 1, GetJsonHex("gafc_exp2_assign_reverb_max_l", a));
        GT100_default_replace(addr15+42, 1, GetJsonHex("gafc_exp2_assign_chorus_min", a));
        GT100_default_replace(addr15+43, 1, GetJsonHex("gafc_exp2_assign_chorus_max", a));
        GT100_default_replace(addr15+44, 1, GetJsonHex("gafc_exp2_assign_flanger_min", a));
        GT100_default_replace(addr15+45, 1, GetJsonHex("gafc_exp2_assign_flanger_max", a));
        GT100_default_replace(addr15+46, 1, GetJsonHex("gafc_exp2_assign_phaser_min", a));
        GT100_default_replace(addr15+47, 1, GetJsonHex("gafc_exp2_assign_phaser_max", a));
        GT100_default_replace(addr15+48, 1, GetJsonHex("gafc_exp2_assign_uni_v_min", a));
        GT100_default_replace(addr15+49, 1, GetJsonHex("gafc_exp2_assign_uni_v_max", a));
        GT100_default_replace(addr15+50, 1, GetJsonHex("gafc_exp2_assign_tremolo_min", a));
        GT100_default_replace(addr15+51, 1, GetJsonHex("gafc_exp2_assign_tremolo_max", a));
        GT100_default_replace(addr15+52, 1, GetJsonHex("gafc_exp2_assign_vibrato_min", a));
        GT100_default_replace(addr15+53, 1, GetJsonHex("gafc_exp2_assign_vibrato_max", a));
        GT100_default_replace(addr15+54, 1, GetJsonHex("gafc_exp2_assign_rotary_min", a));
        GT100_default_replace(addr15+55, 1, GetJsonHex("gafc_exp2_assign_rotary_max", a));
        GT100_default_replace(addr15+56, 1, GetJsonHex("gafc_exp2_assign_ring_mod_min", a));
        GT100_default_replace(addr15+57, 1, GetJsonHex("gafc_exp2_assign_ring_mod_max", a));
        GT100_default_replace(addr15+58, 1, GetJsonHex("gafc_exp2_assign_slow_gear_min", a));
        GT100_default_replace(addr15+59, 1, GetJsonHex("gafc_exp2_assign_slow_gear_max", a));
        GT100_default_replace(addr15+60, 1, GetJsonHex("gafc_exp2_assign_slicer_min", a));
        GT100_default_replace(addr15+61, 1, GetJsonHex("gafc_exp2_assign_slicer_max", a));
        GT100_default_replace(addr15+62, 1, GetJsonHex("gafc_exp2_assign_comp_min", a));
        GT100_default_replace(addr15+63, 1, GetJsonHex("gafc_exp2_assign_comp_max", a));
        GT100_default_replace(addr15+64, 1, GetJsonHex("gafc_exp2_assign_limiter_min", a));
        GT100_default_replace(addr15+65, 1, GetJsonHex("gafc_exp2_assign_limiter_max", a));
        GT100_default_replace(addr15+66, 1, GetJsonHex("gafc_exp2_assign_t_wah_min", a));
        GT100_default_replace(addr15+67, 1, GetJsonHex("gafc_exp2_assign_t_wah_max", a));
        GT100_default_replace(addr15+68, 1, GetJsonHex("gafc_exp2_assign_auto_wah_min", a));
        GT100_default_replace(addr15+69, 1, GetJsonHex("gafc_exp2_assign_auto_wah_max", a));
        GT100_default_replace(addr15+70, 1, GetJsonHex("gafc_exp2_assign_pedal_wah_min", a));
        GT100_default_replace(addr15+71, 1, GetJsonHex("gafc_exp2_assign_pedal_wah_max", a));
        GT100_default_replace(addr15+72, 1, GetJsonHex("gafc_exp2_assign_geq_min", a));
        GT100_default_replace(addr15+73, 1, GetJsonHex("gafc_exp2_assign_geq_max", a));
        GT100_default_replace(addr15+74, 1, GetJsonHex("gafc_exp2_assign_peq_min", a));
        GT100_default_replace(addr15+75, 1, GetJsonHex("gafc_exp2_assign_peq_max", a));
        GT100_default_replace(addr15+76, 1, GetJsonHex("gafc_exp2_assign_guitar_sim_min", a));
        GT100_default_replace(addr15+77, 1, GetJsonHex("gafc_exp2_assign_guitar_sim_max", a));
        GT100_default_replace(addr15+78, 1, GetJsonHex("gafc_exp2_assign_ac_guitar_sim_min", a));
        GT100_default_replace(addr15+79, 1, GetJsonHex("gafc_exp2_assign_ac_guitar_sim_max", a));
        GT100_default_replace(addr15+80, 1, GetJsonHex("gafc_exp2_assign_ac_processor_min", a));
        GT100_default_replace(addr15+81, 1, GetJsonHex("gafc_exp2_assign_ac_processor_max", a));
        GT100_default_replace(addr15+82, 1, GetJsonHex("gafc_exp2_assign_wave_synth_min", a));
        GT100_default_replace(addr15+83, 1, GetJsonHex("gafc_exp2_assign_wave_synth_max", a));
        GT100_default_replace(addr15+84, 1, GetJsonHex("gafc_exp2_assign_octave_min", a));
        GT100_default_replace(addr15+85, 1, GetJsonHex("gafc_exp2_assign_octave_max", a));
        GT100_default_replace(addr15+86, 1, GetJsonHex("gafc_exp2_assign_pitch_shifter_min_h", a));
        GT100_default_replace(addr15+87, 1, GetJsonHex("gafc_exp2_assign_pitch_shifter_min_l", a));
        GT100_default_replace(addr15+88, 1, GetJsonHex("gafc_exp2_assign_pitch_shifter_max_h", a));
        GT100_default_replace(addr15+89, 1, GetJsonHex("gafc_exp2_assign_pitch_shifter_max_l", a));
        GT100_default_replace(addr15+90, 1, GetJsonHex("gafc_exp2_assign_harmonist_min_h", a));
        GT100_default_replace(addr15+91, 1, GetJsonHex("gafc_exp2_assign_harmonist_min_l", a));
        GT100_default_replace(addr15+92, 1, GetJsonHex("gafc_exp2_assign_harmonist_max_h", a));
        GT100_default_replace(addr15+93, 1, GetJsonHex("gafc_exp2_assign_harmonist_max_l", a));
        GT100_default_replace(addr15+94, 1, GetJsonHex("gafc_exp2_assign_humanizer_min", a));
        GT100_default_replace(addr15+95, 1, GetJsonHex("gafc_exp2_assign_humanizer_max", a));
        GT100_default_replace(addr15+96, 1, GetJsonHex("gafc_exp2_assign_evh_phaser_min", a));
        GT100_default_replace(addr15+97, 1, GetJsonHex("gafc_exp2_assign_evh_phaser_max", a));
        GT100_default_replace(addr15+98, 1, GetJsonHex("gafc_exp2_assign_evh_flanger_min", a));
        GT100_default_replace(addr15+99, 1, GetJsonHex("gafc_exp2_assign_evh_flanger_max", a));
        GT100_default_replace(addr15+100, 1, GetJsonHex("gafc_exp2_assign_wah95e_min", a));
        GT100_default_replace(addr15+101, 1, GetJsonHex("gafc_exp2_assign_wah95e_max", a));
        GT100_default_replace(addr15+102, 1, GetJsonHex("gafc_exp2_assign_dc30_min_h", a));
        GT100_default_replace(addr15+103, 1, GetJsonHex("gafc_exp2_assign_dc30_min_l", a));
        GT100_default_replace(addr15+104, 1, GetJsonHex("gafc_exp2_assign_dc30_max_h", a));
        GT100_default_replace(addr15+105, 1, GetJsonHex("gafc_exp2_assign_dc30_max_l", a));
        GT100_default_replace(addr15+106, 1, GetJsonHex("gafc_exp2_assign_heavy_oct", a));
        GT100_default_replace(addr15+107, 1, GetJsonHex("gafc_exp2_assign_heavy_oct_min", a));
        GT100_default_replace(addr15+108, 1, GetJsonHex("gafc_exp2_assign_heavy_oct_max", a));

        temp.clear();
        temp.resize(200);
        temp.fill(' ');
        temp.prepend(GetJsonArray("note", a));  //copy note array
        GT100_default_replace(1658, 32, temp.mid(0, 32));
        GT100_default_replace(1703, 128, temp.mid(32, 128));

        SysxIO *sysxIO = SysxIO::Instance();
        sysxIO->setFileSource("Structure", GT100_default);
        sysxIO->setFileName(this->fileName);
        this->fileSource = sysxIO->getFileSource();
    }
//****************************************************************************************************************
    else if(device.contains("ME-80"))
    {
        temp.append(char(GetJsonValue("name1", a).toInt()) );  //copy patch name
        temp.append(char(GetJsonValue("name2", a).toInt()) );  //copy patch name
        temp.append(char(GetJsonValue("name3", a).toInt()) );  //copy patch name
        temp.append(char(GetJsonValue("name4", a).toInt()) );  //copy patch name
        temp.append(char(GetJsonValue("name5", a).toInt()) );  //copy patch name
        temp.append(char(GetJsonValue("name6", a).toInt()) );  //copy patch name
        temp.append(char(GetJsonValue("name7", a).toInt()) );  //copy patch name
        temp.append(char(GetJsonValue("name8", a).toInt()) );  //copy patch name
        temp.append(char(GetJsonValue("name9", a).toInt()) );  //copy patch name
        temp.append(char(GetJsonValue("name10", a).toInt()) ); //copy patch name
        temp.append(char(GetJsonValue("name11", a).toInt()) ); //copy patch name
        temp.append(char(GetJsonValue("name12", a).toInt()) ); //copy patch name
        temp.append(char(GetJsonValue("name13", a).toInt()) ); //copy patch name
        temp.append(char(GetJsonValue("name14", a).toInt()) ); //copy patch name
        temp.append(char(GetJsonValue("name15", a).toInt()) ); //copy patch name
        temp.append(char(GetJsonValue("name16", a).toInt()) ); //copy patch name
        GT100_default_replace(11, 16, temp );                  //copy patch name

        GT100_default_replace(43, 1, GetJsonHex("comp_sw", a));     //copy comp
        GT100_default_replace(44, 1, GetJsonHex("comp_type", a));       //copy comp
        GT100_default_replace(45, 1, GetJsonHex("comp1", a));    //copy comp
        GT100_default_replace(46, 1, GetJsonHex("comp2", a));     //copy comp
        GT100_default_replace(47, 1, GetJsonHex("comp3", a));       //copy comp
        GT100_default_replace(59, 1, GetJsonHex("odds_sw", a));              //copy dist
        GT100_default_replace(60, 1, GetJsonHex("odds_type", a));                //copy dist
        GT100_default_replace(61, 1, GetJsonHex("odds1", a));               //copy dist
        GT100_default_replace(63, 1, GetJsonHex("odds2", a));                //copy dist
        GT100_default_replace(66, 1, GetJsonHex("odds3", a));        //copy dist
        GT100_default_replace(91, 1, GetJsonHex("amp_sw", a));                 //copy pre A
        GT100_default_replace(92, 1, GetJsonHex("amp_type", a));                   //copy pre A
        GT100_default_replace(93, 1, GetJsonHex("amp1", a));                   //copy pre A
        GT100_default_replace(95, 1, GetJsonHex("amp2", a));                   //copy pre A
        GT100_default_replace(96, 1, GetJsonHex("amp3", a));                 //copy pre A
        GT100_default_replace(97, 1, GetJsonHex("amp4", a));                 //copy pre A
        GT100_default_replace(99, 1, GetJsonHex("amp5", a));                  //copy pre A
        GT100_default_replace(957, 1, GetJsonHex("ns_thresh", a));                  //copy NS
        GT100_default_replace(812, 1, GetJsonHex("dly_sw", a));                  //copy DD
        GT100_default_replace(813, 1, GetJsonHex("dly_type", a));                  //copy DD
        GT100_default_replace(814, 1, GetJsonHex("delay_bpm_h", a));                  //copy DD
        GT100_default_replace(815, 1, GetJsonHex("delay_bpm_l", a));                  //copy DD
        GT100_default_replace(816, 1, GetJsonHex("dly2", a));                  //copy DD
        GT100_default_replace(817, 1, GetJsonHex("dly1", a));                  //copy DD
        GT100_default_replace(818, 1, GetJsonHex("dly3", a));                  //copy DD
        GT100_default_replace(873, 1, GetJsonHex("rev_sw", a));               //copy RV
        GT100_default_replace(874, 1, GetJsonHex("rev_type", a));             //copy RV
        GT100_default_replace(881, 1, GetJsonHex("rev", a));                  //copy RV

        GT100_default_replace(200, 1, GetJsonHex("fx2_sw", a));                 //copy EQ
        GT100_default_replace(201, 1, GetJsonHex("fx2_2", a));                //copy EQ
        GT100_default_replace(216, 1, GetJsonHex("mod_sw", a));                  //copy FX1
        GT100_default_replace(217, 1, GetJsonHex("mod_type", a));                  //copy FX1
        GT100_default_replace(219, 1, GetJsonHex("mod1", a));                  //copy FX1
        GT100_default_replace(231, 1, GetJsonHex("mod1", a));                  //copy FX1
        GT100_default_replace(237, 1, GetJsonHex("mod1", a));                  //copy FX1
        GT100_default_replace(302, 1, GetJsonHex("mod1", a));                  //copy FX1

        GT100_default_replace(510, 1, GetJsonHex("fx2_sw", a));                  //copy FX2
        GT100_default_replace(511, 1, GetJsonHex("fx2_type", a));                  //copy FX2

        GT100_default_replace(889, 1, GetJsonHex("pdlfx_sw", a));                  //copy PedalFX


        /*
        "fx2_1":"53",
        "fx2_2":"50",
        "fx2_3":"49",
        "fx2_4":"48",
        "ctl_target_h":"00",
        "ctl_target_l":"00",
        "ctrl_knob_value":"100",
        "ctl_mode":"1",
        "modulation_bpm_h":"06",
        "modulation_bpm_l":"14",
        "pdlfx_type":"6",
        "mod2":"60",
        "mod3":"40",
        "orderNumber":1,
        "note":null,*/

        SysxIO *sysxIO = SysxIO::Instance();
        QString area = "Structure";
        sysxIO->setFileSource(area, GT100_default);
        sysxIO->setFileName(this->fileName);
        this->fileSource = sysxIO->getFileSource();

    }else{

        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle(QObject::tr("Unknown Device Patch File"));
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setTextFormat(Qt::RichText);
        QString msgText;
        msgText.append("This device type currrently not supported<br>");
        msgText.append("Device type recognised as "+device+"<br>");
        msgBox->setText(msgText);
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->exec();

    };


}
