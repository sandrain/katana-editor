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

#ifndef SYSXWRITER_H
#define SYSXWRITER_H

#include <QString>
#include <QList>
#include "SysxIO.h"	

class sysxWriter
{

public:
    sysxWriter();
    ~sysxWriter();
    void setFile(QString fileName);
    bool readFile();
    SysxData getFileSource();
    SysxData getSystemSource();
    void writeSYX(QString fileName);
    void writeSMF(QString fileName);
    void writeGCL(QString fileName);
    void writeTSL(QString fileName);
    void hexToTSL();
    void writeLoaderFile(QString fileName);
    void writeSystemFile(QString fileName);
    QString getFileName();
    QList<QString> patchList;
    int index;
    void convertFromTSL(int patch);
    QByteArray TSL_default;

public slots:
    void patchIndex(int listIndex);

private:
    QString fileName;
    SysxData fileSource;
    SysxData systemSource;
    QByteArray data;
    QByteArray GT100_default;
    void convertFromGT10();
    void convertFromGT10B();
    void convertFromGT8();
    void translate10to100();
    void translate10Bto100();
    void convertFromGT6B();
    void GT100_default_replace(int pos, int size, QByteArray value);
    QString headerType;
    int ArrayToInt(QByteArray Array);
    QByteArray IntToArray(int value);
    QByteArray GetJsonArray(QByteArray text, int pos);
    QByteArray GetJsonValue(QByteArray text, int pos);
    QByteArray GetJsonHex(QByteArray text, int pos);
    QByteArray GetJsonString(QByteArray text, int pos);
    void AppendTSL(QByteArray hex, const char* Json_name);
    void TextTSL(QByteArray hex, const char* Json_name);
    void TextTSL_mk2(QByteArray hex, const char* Json_name);
    bool mk2;
};

#endif // SYSXWRITER_H
