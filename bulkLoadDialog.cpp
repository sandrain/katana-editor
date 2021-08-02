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

#include <QtWidgets>
#include <QFile>
#include <QDataStream>
#include <QByteArray>
#include <QMessageBox>
#include "bulkLoadDialog.h"
#include "Preferences.h"
#include "globalVariables.h"

// Platform-dependent sleep routines.
#ifdef Q_OS_WIN
#include <windows.h>
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds )
#else // Unix variants & Mac
#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

bulkLoadDialog::bulkLoadDialog()
{ 
    failed = true;
    QLabel *startListLabel = new QLabel(tr("Starting from"));
    this->startPatchCombo = new QComboBox(this);
    startPatchCombo->setMaxVisibleItems(5);
    QLabel *finishListLabel = new QLabel(tr("Finishing at"));
    this->finishPatchCombo = new QComboBox(this);
    finishPatchCombo->setMaxVisibleItems(5);
    QObject::connect(startPatchCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboValueChanged(int)));
    QObject::connect(finishPatchCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(comboValueChanged(int)));
    QVBoxLayout *comboBoxLayout = new QVBoxLayout;
    comboBoxLayout->addWidget(startListLabel);
    comboBoxLayout->addWidget(startPatchCombo);
    comboBoxLayout->addStretch(1);
    comboBoxLayout->addSpacing(12);
    comboBoxLayout->addWidget(finishListLabel);
    comboBoxLayout->addWidget(finishPatchCombo);

    QGroupBox *patchListGroup = new QGroupBox(tr("Set the Range of Patch data to restore"));
    patchListGroup->setLayout(comboBoxLayout);

    QLabel *startRangeLabel = new QLabel(tr("Start Channel"));
    QLabel *finishRangeLabel = new QLabel(tr("Finish Channel"));

    this->startRangeComboBox = new QComboBox(this);
    startRangeComboBox->setMaxVisibleItems(5);
    QObject::connect(startRangeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboValueChanged(int)));
    this->finishRange = new QLineEdit(this);
    this->finishRange->setReadOnly(true);

    QVBoxLayout *rangeLabelLayout = new QVBoxLayout;
    rangeLabelLayout->addSpacing(12);
    rangeLabelLayout->addWidget(startRangeLabel);
    rangeLabelLayout->addSpacing(12);
    rangeLabelLayout->addWidget(finishRangeLabel);

    QHBoxLayout *spinBoxLayout = new QHBoxLayout;
    spinBoxLayout->addWidget(startRangeComboBox);
    
    QVBoxLayout *rangeBoxLayout = new QVBoxLayout;
    rangeBoxLayout->addSpacing(12);
    rangeBoxLayout->addLayout(spinBoxLayout);
    rangeBoxLayout->addSpacing(12);
    rangeBoxLayout->addWidget(finishRange);

    QHBoxLayout *dataRangeLayout = new QHBoxLayout;
    dataRangeLayout->addSpacing(20);
    dataRangeLayout->addLayout(rangeLabelLayout);
    dataRangeLayout->addLayout(rangeBoxLayout);
    dataRangeLayout->addSpacing(20);

    QGroupBox *patchRangeGroup = new QGroupBox(tr("Set start location of Patch data to restore"));
    patchRangeGroup->setLayout(dataRangeLayout);

    this->startButton = new QPushButton(this);
    this->startButton->setText(tr("Start"));
    connect(startButton, SIGNAL(clicked()), this, SLOT(sendData()));

    this->completedButton = new QPushButton(this);
    this->completedButton->setText(tr("DATA TRANSFER COMPLETED"));
    this->completedButton->hide();
    connect(completedButton, SIGNAL(clicked()), this, SLOT(DialogClose()));

    this->cancelButton = new QPushButton(this);
    this->cancelButton->setText(tr("Cancel"));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(DialogClose()));

    this->progressLabel = new QLabel(this);
    this->progressLabel->setText(tr("Full Restoration may take a few seconds"));
    this->bytesLabel = new QLabel(this);
    this->bytesLabel->setText("");

    this->progressBar = new QProgressBar(this);
    this->progressBar->setTextVisible(false);
    this->progressBar->setRange(0, 100);
    this->progressBar->setValue(0);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addSpacing(40);
    buttonsLayout->addWidget(startButton);
    buttonsLayout->addStretch(1);
    buttonsLayout->addSpacing(20);
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addSpacing(40);

    QHBoxLayout *progressBarLayout = new QHBoxLayout;
    progressBarLayout->addWidget(progressBar);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(patchListGroup, Qt::AlignCenter);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout, Qt::AlignCenter);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(patchRangeGroup, Qt::AlignCenter);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(progressLabel, Qt::AlignCenter);

    mainLayout->addWidget(bytesLabel, Qt::AlignCenter);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(completedButton);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(progressBarLayout, Qt::AlignCenter);
    setLayout(mainLayout);

    setWindowTitle(tr("Bulk File Restoration"));

    bulk.clear();


    SysxIO *sysxIO = SysxIO::Instance();
    QObject::connect(this, SIGNAL(setStatusSymbol(int)), sysxIO, SIGNAL(setStatusSymbol(int)));
    QObject::connect(this, SIGNAL(setStatusProgress(int)), sysxIO, SIGNAL(setStatusProgress(int)));
    QObject::connect(this, SIGNAL(setStatusMessage(QString)), sysxIO, SIGNAL(setStatusMessage(QString)));
    QObject::connect(this, SIGNAL(setdeBugStatusMessage(QString)), sysxIO, SIGNAL(setStatusdBugMessage(QString)));

    Preferences *preferences = Preferences::Instance();
    QString dir = preferences->getPreferences("General", "Files", "dir");

    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Choose a file"),
                dir,
                tr("GT100 Bulk Data File (*.tsl *.gcl *.syx *.mid)"));
    if (!fileName.isEmpty())
    {

        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly))
        {
            this->data = file.readAll();     // read the pre-selected file, copy to 'data'
            QByteArray GCL_default;
            QFile file(":default.syx");           // Read the default KATANA sysx file .
            if (file.open(QIODevice::ReadOnly))
            {	default_data = file.readAll(); };
            QFile GCLfile(":default.gcl");           // Read the default KATANA GCL file .
            if (GCLfile.open(QIODevice::ReadOnly))
            {	GCL_default = GCLfile.readAll(); };
            QFile hexfile(":HexLookupTable.hex");           // Read the HexLookupTable for the SMF header file .
            if (hexfile.open(QIODevice::ReadOnly))
            {	this->hextable = hexfile.readAll(); };

            QByteArray default_header = default_data.mid(0, 7);           // copy header from default.syx
            QByteArray file_header = data.mid(0, 7);                      // copy header from read file.syx
            QByteArray GCL_header = GCL_default.mid(3, 20);                // copy header from default.gcl
            QByteArray SMF_header = hextable.mid(288,18);
            //QByteArray SMF_file = data.mid(0, 18);
            unsigned char r = (char)data[7];     // find patch number in file (msb))
            bool ok;
            int patchNum;
            patchNum = QString::number(r, 16).toUpper().toInt(&ok, 16);
            bool isPatch = false;
            if (patchNum >= 16) { isPatch = true; };    // check the sysx file is a valid patch & not system data.

            bool isHeader = false;
            if (default_header == file_header) {isHeader = true;};
            bool isGCL = false;
            if (data.contains(GCL_header)){isGCL = true; };             // see if file is a GCL type and set isGCL flag.
            bool isSMF = false;
            if (data.contains(SMF_header)) {isSMF = true; };
            failed = false;
            if (isHeader == true && isPatch == true) {loadSYX(); setStatusMessage("is a *.syx file type");  }
            else if (isGCL == true) { loadGCL(); setStatusMessage("is a *.gcl file type"); }
            else if (isSMF == true) { loadSMF(); setStatusMessage("is a *.mid file type"); }
            else if (fileName.contains(".tsl")) { loadTSL(); setStatusMessage("is a Tone Studio file type"); }
            else
            {
                failed = true;
                QMessageBox *msgBox = new QMessageBox();
                msgBox->setWindowTitle(QObject::tr("File Restore Error"));
                msgBox->setIcon(QMessageBox::Warning);
                msgBox->setTextFormat(Qt::RichText);
                QString msgText;
                msgText.append("<font size='+1'><b>");
                msgText.append(QObject::tr("This is not a known ") + deviceType + QObject::tr(" file!"));
                msgText.append("<b></font><br>");
                msgText.append(QObject::tr("Patch file not within allowable parameters or<br>"));
                msgText.append(QObject::tr("file format unknown, please try another file."));
                msgBox->setText(msgText);
                msgBox->show();
                QTimer::singleShot(3000, msgBox, SLOT(deleteLater()));
            };
        };
    };
    if (failed == true)
    {
        this->startButton->hide();
    };
}

bulkLoadDialog::~bulkLoadDialog()
{
}

void bulkLoadDialog::comboValueChanged(int value)
{
    Q_UNUSED(value);
    this->bankStart = this->startRangeComboBox->currentIndex();
    this->startList = this->startPatchCombo->currentIndex();
    this->finishList = this->finishPatchCombo->currentIndex();
    if ((this->finishList-this->startList)>(this->bankStart+(bankTotalUser*patchPerBank)))
    {this->startPatchCombo->setCurrentIndex(this->finishList-(bankTotalUser*patchPerBank)+1);
        /*this->startRangeComboBox->setCurrentIndex(1);*/ };
    if (this->startList > this->finishList) {this->startPatchCombo->setCurrentIndex(finishList); }
    else if (this->finishList < this->startList) {this->finishPatchCombo->setCurrentIndex(startList); };
    int x = (this->bankStart+(this->finishList-this->startList));
    if (x<0) {x=0; } else if (x>((bankTotalUser*patchPerBank)-1))
    {
        x=((bankTotalUser*patchPerBank)-1);
        this->bankStart=((bankTotalUser*patchPerBank)-1)-(this->finishList-this->startList);
        this->startRangeComboBox->setCurrentIndex(((bankTotalUser*patchPerBank)-1)-(this->finishList-this->startList));
    };
    QString text = tr("Finish at CH");
    int y = x/patchPerBank; y = y*patchPerBank; y=x-y;
    text.append(QString::number((x/patchPerBank)+1, 10).toUpper() );
    text.append("-");
    text.append(QString::number(y+1, 10).toUpper() );
    this->finishRange->setText(text);
    //this->startRangeComboBox->setMaxVisibleItems((bankTotalUser*patchPerBank)-(finishList-startList));
}

void bulkLoadDialog::sendData() 
{	
    bankStart = this->startRangeComboBox->currentIndex()+1;
    startList = this->startPatchCombo->currentIndex();
    finishList = this->finishPatchCombo->currentIndex();
    startButton->hide();
    progress = 0;
    patch = 1;
    range = 5/(finishList-startList);

    int z = (this->bankStart);
    QString q;
    QString address;
    QString msg;
    QString v;
    QString addrMSB = "10";
    QString replyMsg;
    for (int a=startList;a<(finishList+1);a++)
    {
        if (z>128) {z=z-128; addrMSB = "11"; };          // next address range when > 10 7F.
        address = QString::number(z-1, 16).toUpper();
        if (address.size()<2){ address.prepend("0"); };
        int b = a*patchSize;                                // multiples of patch size.
        msg = this->bulk.mid(b*2, patchSize*2);            // copy next patch from bulk patch list.
        for (int g=0;g<msg.size()/2;g++)
        {
            v = msg.mid(g*2, 2);
            if (v == "F0") {msg.replace((g*2)+(sysxAddressOffset*2), 2, addrMSB); msg.replace((g*2)+((sysxAddressOffset*2)+2), 2, address); };   // replace the message address
        };
        replyMsg.append(msg);
        ++z;
    };

    QString reBuild = "";       // Add correct checksum to patch strings
    QString sysxEOF = "";
    QString hex = "";
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
    this->bulk = reBuild.simplified().toUpper();
    bank = (bankStart+(patchPerBank-1));
    patch = bank/patchPerBank; patch = patch*patchPerBank; patch=bank-patch;
    steps=0;
    dataSent=0;
    sendSequence();
}

void bulkLoadDialog::sendPatch(QString data)
{
    progress=progress+range;
    bulkStatusProgress(progress);                         // advance the progressbar.

    SysxIO *sysxIO = SysxIO::Instance();
    QObject::connect(sysxIO, SIGNAL(sysxReply(QString)), this, SLOT(sendSequence()));
    int count=100;
    while(!sysxIO->deviceReady() && count>0){SLEEP(20); --count;};
    sysxIO->sendSysx(data);
}

void bulkLoadDialog::sendSequence()
{ 
    SysxIO *sysxIO = SysxIO::Instance();
    QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)), this, SLOT(sendSequence()));
    sysxIO->setDeviceReady(true); // Free the device after finishing interaction.
    msg=bulk.mid(steps*(patchSize*2), (patchSize*2));

    if (steps<((finishList-startList)+1) )
    {
        bool ok;
        QString patchText;
        QString name = msg.mid((sysxNameOffset*2), (nameLength*2));  // get name from loaded patch.
        QList<QString> x;
        for (int b=0;b<nameLength;b++)
        {
            x.append(name.mid(b*2, 2));
        };
        for (int b=0;b<nameLength;b++)
        {
            QString hexStr = x.at(b);
            patchText.append( (char)(hexStr.toInt(&ok, 16)) );      // convert name to readable text characters.
        };
        int bf = (finishList-startList);
        if(steps>bf) {this->completedButton->show();
            this->progressLabel->setText(tr("Bulk data transfer completed!!"));
            this->progress=100;  };
        QString patchNumber = (tr("User CH" ));
        patchNumber.append("-");
        patchNumber.append( QString::number(patch, 10).toUpper() );
        patchNumber.append("     ");
        patchNumber.append(patchText);
        patchText=patchNumber;
        this->progressLabel->setText(patchText);                        //display the patch number and name.

        patchNumber = tr("File transfer size = ");
        dataSent = dataSent+(msg.size()/2);
        patchNumber.append(QString::number(dataSent, 10).toUpper() );
        patchNumber.append(tr(" bytes"));
        this->bytesLabel->setText(patchNumber);                         //display the bulk data size.
        ++steps;
        ++patch;
        if(patch>patchPerBank) {patch=1; bank=bank+patchPerBank;};	  // increment patch.
        sendPatch(msg);                                 //request the next patch.
        setStatusMessage(tr("Sending Data"));
    } else {
        QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)), this, SLOT(sendSequence()));
        sysxIO->setDeviceReady(true); // Free the device after finishing interaction.
        setStatusMessage(tr("Ready"));
        DialogClose();
    };
}


void bulkLoadDialog::updatePatch()
{
    patchCount = sysxPatches.size()/(patchSize);
    QString msgText;
    QString patchText;
    QString patchNumber;
    unsigned char r;
    this->patchList.clear();
    unsigned int a = sysxNameOffset;             // locate patch text start position from the start of the file
    for (int h=0;h<patchCount;h++)
    {
        for (int b=0;b<nameLength;b++)
        {
            r = (char)sysxPatches[a+b];
            patchText.append(r);
        };
        patchNumber = QString::number(h+1, 10).toUpper();
        msgText.append(patchNumber + " : ");
        msgText.append(patchText + "   ");
        this->patchList.append(msgText);
        patchText.clear();
        msgText.clear();
        a=a+patchSize;                                // advance to the next patch in the bulk file.
    };
    this->startPatchCombo->addItems(patchList);            // add patch names to the combobox lists.
    this->finishPatchCombo->addItems(patchList);
    this->finishPatchCombo->setCurrentIndex(patchCount-1);     // set the finish combobox index to the end of the list.
    this->startPatchCombo->setCurrentIndex(0);
    QString text = tr("Finish at CH");
    if (patchCount<patchPerBank) {patchCount=patchPerBank; };
    text.append(QString::number(patchCount/patchPerBank, 10).toUpper() );
    this->finishRange->setText(text);
    QString bnk;
    QString U = "CH";
    for (int x=0; x<bankTotalUser; x++)
    {
        bnk = "U" + QString::number(x+1, 10).toUpper() + ":1";
        this->startRangeComboBox->addItem(bnk);
        bnk = "U" + QString::number(x+1, 10).toUpper() + ":2";
        this->startRangeComboBox->addItem(bnk);
        bnk = "U" + QString::number(x+1, 10).toUpper() + ":3";
        this->startRangeComboBox->addItem(bnk);
        bnk = "U" + QString::number(x+1, 10).toUpper() + ":4";
        this->startRangeComboBox->addItem(bnk);
    };
    this->startRangeComboBox->setCurrentIndex(0);

    QString sysxBuffer;
    for(int i=0;i<sysxPatches.size();i++)
    {
        unsigned char byte = (char)sysxPatches[i];
        unsigned int n = (int)byte;
        QString hex = QString::number(n, 16).toUpper();     // convert QByteArray to QString
        if (hex.length() < 2) hex.prepend("0");
        sysxBuffer.append(hex);
    };
    bulk.append(sysxBuffer);
}

void bulkLoadDialog::bulkStatusProgress(int value)
{
    value=value/2;
    if (value >100) {value = 100;};
    if (value<0) {value = 0; };
    this->progressBar->setValue(value);
}

void bulkLoadDialog::loadGCL()         // ************************************ GCL File Format***************************
{	
    unsigned char msb = (char)data[34];     // find patch count msb bit in GCL file at byte 34
    unsigned char lsb = (char)data[35];     // find patch count lsb bit in GCL file at byte 35
    bool ok;
    int count;
    count = (256*QString::number(msb, 16).toUpper().toInt(&ok, 16)) + (QString::number(lsb, 16).toUpper().toInt(&ok, 16));
    QByteArray marker;
    marker = data.mid(170, 2);      //copy marker key to find "06A5" which marks the start of each patch block
    unsigned int a = data.indexOf(marker, 0); // locate patch start position from the start of the file
    a=a+2;                             // offset is set in front of marker
    for (int h=0;h<count;h++)
    {
        QByteArray temp;
        temp = data.mid(a, 128);
        default_data.replace(11, 128, temp);       //address "00" +
        temp = data.mid(a+128, 128);
        default_data.replace(152, 128, temp);      //address "01" +
        temp = data.mid(a+256, 128);
        default_data.replace(293, 128, temp);      //address "02" +
        temp = data.mid(a+384, 128);
        default_data.replace(434, 128, temp);      //address "03" +
        temp = data.mid(a+512, 128);
        default_data.replace(575, 128, temp);      //address "04" +
        temp = data.mid(a+640, 128);
        default_data.replace(716, 128, temp);       //address "05" +
        temp = data.mid(a+768, 128);
        default_data.replace(857, 128, temp);      //address "06" +
        temp = data.mid(a+896, 128);
        default_data.replace(998, 128, temp);      //address "07" +
        temp = data.mid(a+1024, 128);
        default_data.replace(1139, 128, temp);     //address "08" +
        temp = data.mid(a+1152, 128);
        default_data.replace(1280, 128, temp);     //address "09" +
        temp = data.mid(a+1280, 128);
        default_data.replace(1421, 128, temp);     //address "0A" +
        temp = data.mid(a+1408, 128);
        default_data.replace(1562, 128, temp);     //address "0B" +
        temp = data.mid(a+1536, 128);
        default_data.replace(1703, 128, temp);     //address "0C" +
        temp = data.mid(a+1664, 128);
        default_data.replace(1844, 128, temp);     //address "0D" +
        temp = data.mid(a+1792, 128);
        default_data.replace(1985, 128, temp);     //address "0E" +
        temp = data.mid(a+1920, 128);
        default_data.replace(2126, 128, temp);     //address "0F" +
        temp = data.mid(a+2048, 60);
        default_data.replace(2267, 60, temp);     //address "10" +
        a = data.indexOf(marker, a); // locate patch start position from the start of the file
        a=a+2;                      // offset is set in front of marker
        temp = default_data;
        this->sysxPatches.append(temp.mid(0, patchSize));
    };
    updatePatch();
}

void bulkLoadDialog::loadSYX()        //********************************* SYX File Format *****************************
{	
    sysxPatches = data;
    updatePatch();
}

void bulkLoadDialog::loadSMF()    // **************************** SMF FILE FORMAT ***************************
{	
    QByteArray temp;                         // TRANSLATION of KATANA SMF PATCHES, data read from smf patch **************
    if ( data.at(37) != default_data.at(5) ){    // check if a valid KATANA file
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle(QObject::tr("SMF file import"));
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setTextFormat(Qt::RichText);
        QString msgText;
        msgText.append("<font size='+1'><b>");
        msgText.append(QObject::tr("This is not a KATANA file!"));
        msgText.append("<b></font><br>");
        msgText.append(QObject::tr("this file is a GT-10 or GT-10B version<br>"));
        msgText.append(QObject::tr("*Loading this file may have unpredictable results*."));
        msgBox->setText(msgText);
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->exec();
    };
    int count = (data.size()-32)/2322;
    int a=0;                             // offset is set to first patch
    for (int h=0;h<count;h++)
    {
        temp = data.mid(a+43, 128);            // copy SMF 128 bytes#
        default_data.replace(11, 128, temp);             // replace gt100 address "00"#
        temp = data.mid(a+171, 114);           // copy SMF part1#
        temp.append(data.mid(a+301,14));       // copy SMF part2#
        default_data.replace(152, 128, temp);            // replace gt100 address "01"#
        temp = data.mid(a+315, 128);           // copy SMF part1#
        default_data.replace(293, 128, temp);            // replace gt100 address "02"#
        temp = data.mid(a+443, 100);           // copy SMF part1#
        temp.append(data.mid(a+559,28));       // copy SMF part2#
        default_data.replace(434, 128, temp);            // replace gt100 address "03"#
        temp = data.mid(a+587, 128);           // copy SMF part1#
        default_data.replace(575, 128, temp);            // replace gt100 address "04"#
        temp = data.mid(a+715, 86);            // copy SMF part1#
        temp.append(data.mid(a+817,42));      // copy SMF part2#
        default_data.replace(716, 128, temp);             // replace gt100 address "05"#
        temp = data.mid(a+859, 128);           // copy SMF part1#
        default_data.replace(857,128, temp);             // replace gt100 address "06"#
        temp = data.mid(a+987, 72);            // copy SMF part1#
        temp.append(data.mid(a+1075,56));      // copy SMF part2#
        default_data.replace(998, 128, temp);            // replace gt100 address "07"#
        temp = data.mid(a+1131, 128);          // copy SMF part1#
        default_data.replace(1139,128, temp);            // replace gt100 address "08"#
        temp = data.mid(a+1259, 58);           // copy SMF part1#
        temp.append(data.mid(a+1333,70));      // copy SMF part2#
        default_data.replace(1280, 128, temp);           // replace gt100 address "09"#
        temp = data.mid(a+1403, 128);          // copy SMF part1#
        default_data.replace(1421,128, temp);            // replace gt100 address "0A"#
        temp = data.mid(a+1531, 44);           // copy SMF part1#
        temp.append(data.mid(a+1591,84));      // copy SMF part2#
        default_data.replace(1562, 128, temp);           // replace gt100 address "0B"#
        temp = data.mid(a+1675, 128);          // copy SMF part1#
        default_data.replace(1703,128, temp);            // replace gt100 address "0C"#
        temp = data.mid(a+1803, 30);           // copy SMF part1#
        temp.append(data.mid(a+1849,98));      // copy SMF part2#
        default_data.replace(1844, 128, temp);           // replace gt100 address "0D"#
        temp = data.mid(a+1947, 128);           // copy SMF part1#
        default_data.replace(1985, 128, temp);           // replace gt100 address "0E"#
        temp = data.mid(a+2075, 16);           // copy SMF part1#
        temp.append(data.mid(a+2107,112));      // copy SMF part2
        default_data.replace(2126, 128, temp);           // replace gt100 address "0F"
        temp = data.mid(a+2219, 60);           // copy SMF part1#
        GT100_default.replace(2267, 60, temp);           // replace gt100 address "10"
        a=a+2322;                      // offset is set in front of marker
        temp = default_data;
        this->sysxPatches.append(temp.mid(0, patchSize));
    };
    updatePatch();
}

void bulkLoadDialog::loadTSL()         // ************************************ TSL File Format***************************
{
    QByteArray temp;
    bool skip = false;
    unsigned int pnum = data.count("name2");
    int pindex = data.size()/(pnum);
    QString device=GetJsonValue("device", 1);
    int patchCount = pnum;
    unsigned int a = 0; // locate patch text start position from the start of the file
    for (int h=0;h<patchCount;h++)
    {
        GT100_default=default_data;
        if(device.contains("GT"))
        {
            QByteArray name_temp;
            name_temp.append((char)GetJsonValue("patch_name1", a).toInt() );  //copy patch name
            name_temp.append((char)GetJsonValue("patch_name2", a).toInt() );  //copy patch name
            name_temp.append((char)GetJsonValue("patch_name3", a).toInt() );  //copy patch name
            name_temp.append((char)GetJsonValue("patch_name4", a).toInt() );  //copy patch name
            name_temp.append((char)GetJsonValue("patch_name5", a).toInt() );  //copy patch name
            name_temp.append((char)GetJsonValue("patch_name6", a).toInt() );  //copy patch name
            name_temp.append((char)GetJsonValue("patch_name7", a).toInt() );  //copy patch name
            name_temp.append((char)GetJsonValue("patch_name8", a).toInt() );  //copy patch name
            name_temp.append((char)GetJsonValue("patch_name9", a).toInt() );  //copy patch name
            name_temp.append((char)GetJsonValue("patch_name10", a).toInt() ); //copy patch name
            name_temp.append((char)GetJsonValue("patch_name11", a).toInt() ); //copy patch name
            name_temp.append((char)GetJsonValue("patch_name12", a).toInt() ); //copy patch name
            name_temp.append((char)GetJsonValue("patch_name13", a).toInt() ); //copy patch name
            name_temp.append((char)GetJsonValue("patch_name14", a).toInt() ); //copy patch name
            name_temp.append((char)GetJsonValue("patch_name15", a).toInt() ); //copy patch name
            name_temp.append((char)GetJsonValue("patch_name16", a).toInt() ); //copy patch name
            GT100_default_replace(11, 16, name_temp );                        //copy patch name
            GT100_default_replace(27, 1, GetJsonHex("output_select", a));   //copy output select
            GT100_default_replace(43, 1, GetJsonHex("comp_on_off", a));     //copy comp
            GT100_default_replace(44, 1, GetJsonHex("comp_type", a));       //copy comp
            GT100_default_replace(45, 1, GetJsonHex("comp_sustain", a));    //copy comp
            GT100_default_replace(46, 1, GetJsonHex("comp_attack", a));     //copy comp
            GT100_default_replace(47, 1, GetJsonHex("comp_tone", a));       //copy comp
            GT100_default_replace(48, 1, GetJsonHex("comp_level", a));      //copy comp
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
            GT100_default_replace(102, 1, GetJsonHex("preamp_a_solo_sw", a));               //copy pre A
            GT100_default_replace(103, 1, GetJsonHex("preamp_a_solo_level", a));            //copy pre A
            GT100_default_replace(104, 1, GetJsonHex("preamp_a_sp_type", a));               //copy pre A
            GT100_default_replace(105, 1, GetJsonHex("preamp_a_mic_type", a));              //copy pre A
            GT100_default_replace(106, 1, GetJsonHex("preamp_a_mic_dis", a));               //copy pre A
            GT100_default_replace(107, 1, GetJsonHex("preamp_a_mic_pos", a));               //copy pre A
            GT100_default_replace(108, 1, GetJsonHex("preamp_a_mic_level", a));             //copy pre A
            GT100_default_replace(109, 1, GetJsonHex("preamp_a_direct_mix", a));            //copy pre A
            GT100_default_replace(110, 1, GetJsonHex("preamp_a_custom_type", a));           //copy pre A
            GT100_default_replace(111, 1, GetJsonHex("preamp_a_custom_bottom", a));         //copy pre A
            GT100_default_replace(112, 1, GetJsonHex("preamp_a_custom_edge", a));           //copy pre A
            GT100_default_replace(115, 1, GetJsonHex("preamp_a_custom_preamp_low", a));     //copy pre A
            GT100_default_replace(116, 1, GetJsonHex("preamp_a_custom_preamp_high", a));    //copy pre A
            GT100_default_replace(117, 1, GetJsonHex("preamp_a_custom_char", a));           //copy pre A
            GT100_default_replace(118, 1, GetJsonHex("preamp_a_custom_sp_size", a));        //copy pre A
            GT100_default_replace(119, 1, GetJsonHex("preamp_a_custom_sp_color_low", a));   //copy pre A
            GT100_default_replace(120, 1, GetJsonHex("preamp_a_custom_sp_color_high", a));  //copy pre A
            GT100_default_replace(121, 1, GetJsonHex("preamp_a_custom_sp_num", a));         //copy pre A
            GT100_default_replace(122, 1, GetJsonHex("preamp_a_custom_sp_cabinet", a));     //copy pre A
            GT100_default_replace(152, 1, GetJsonHex("preamp_b_on_off", a));              //copy pre B
            GT100_default_replace(153, 1, GetJsonHex("preamp_b_type", a));                //copy pre B
            GT100_default_replace(154, 1, GetJsonHex("preamp_b_gain", a));                //copy pre B
            GT100_default_replace(155, 1, GetJsonHex("preamp_b_t_comp", a));              //copy pre B
            GT100_default_replace(156, 1, GetJsonHex("preamp_b_bass", a));                //copy pre B
            GT100_default_replace(157, 1, GetJsonHex("preamp_b_middle", a));              //copy pre B
            GT100_default_replace(158, 1, GetJsonHex("preamp_b_treble", a));              //copy pre B
            GT100_default_replace(159, 1, GetJsonHex("preamp_b_presence", a));            //copy pre B
            GT100_default_replace(160, 1, GetJsonHex("preamp_b_level", a));               //copy pre B
            GT100_default_replace(161, 1, GetJsonHex("preamp_b_bright", a));              //copy pre B
            GT100_default_replace(162, 1, GetJsonHex("preamp_b_gain_sw", a));             //copy pre B
            GT100_default_replace(163, 1, GetJsonHex("preamp_b_solo_sw", a));             //copy pre B
            GT100_default_replace(164, 1, GetJsonHex("preamp_b_solo_level", a));          //copy pre B
            GT100_default_replace(165, 1, GetJsonHex("preamp_b_sp_type", a));             //copy pre B
            GT100_default_replace(166, 1, GetJsonHex("preamp_b_mic_type", a));            //copy pre B
            GT100_default_replace(167, 1, GetJsonHex("preamp_b_mic_dis", a));             //copy pre B
            GT100_default_replace(168, 1, GetJsonHex("preamp_b_mic_pos", a));             //copy pre B
            GT100_default_replace(169, 1, GetJsonHex("preamp_b_mic_level", a));           //copy pre B
            GT100_default_replace(170, 1, GetJsonHex("preamp_b_direct_mix", a));          //copy pre B
            GT100_default_replace(171, 1, GetJsonHex("preamp_b_custom_type", a));         //copy pre B
            GT100_default_replace(172, 1, GetJsonHex("preamp_b_custom_bottom", a));       //copy pre B
            GT100_default_replace(173, 1, GetJsonHex("preamp_b_custom_edge", a));         //copy pre B
            GT100_default_replace(176, 1, GetJsonHex("preamp_b_custom_preamp_low", a));   //copy pre B
            GT100_default_replace(177, 1, GetJsonHex("preamp_b_custom_preamp_high", a));  //copy pre B
            GT100_default_replace(178, 1, GetJsonHex("preamp_b_custom_char", a));         //copy pre B
            GT100_default_replace(179, 1, GetJsonHex("preamp_b_custom_sp_size", a));      //copy pre B
            GT100_default_replace(180, 1, GetJsonHex("preamp_b_custom_sp_color_low", a)); //copy pre B
            GT100_default_replace(181, 1, GetJsonHex("preamp_b_custom_sp_color_high", a));//copy pre B
            GT100_default_replace(182, 1, GetJsonHex("preamp_b_custom_sp_num", a));       //copy pre B
            GT100_default_replace(183, 1, GetJsonHex("preamp_b_custom_sp_cabinet", a));   //copy pre B
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
            GT100_default_replace(216, 1, GetJsonHex("fx1_on_off", a));                  //copy FX1
            GT100_default_replace(217, 1, GetJsonHex("fx1_fx_type", a));                  //copy FX1
            GT100_default_replace(218, 1, GetJsonHex("fx1_sub_od_ds_type", a));                  //copy FX1
            GT100_default_replace(219, 1, GetJsonHex("fx1_sub_od_ds_drive", a));                  //copy FX1
            GT100_default_replace(220, 1, GetJsonHex("fx1_sub_od_ds_bottom", a));                  //copy FX1
            GT100_default_replace(221, 1, GetJsonHex("fx1_sub_od_ds_tone", a));                  //copy FX1
            GT100_default_replace(222, 1, GetJsonHex("fx1_sub_od_ds_solo_sw", a));                  //copy FX1
            GT100_default_replace(223, 1, GetJsonHex("fx1_sub_od_ds_solo_level", a));                  //copy FX1
            GT100_default_replace(224, 1, GetJsonHex("fx1_sub_od_ds_effect_level", a));                  //copy FX1
            GT100_default_replace(225, 1, GetJsonHex("fx1_sub_od_ds_direct_mix", a));                  //copy FX1
            GT100_default_replace(228, 1, GetJsonHex("fx1_t_wah_mode", a));                  //copy FX1
            GT100_default_replace(229, 1, GetJsonHex("fx1_t_wah_polar", a));                  //copy FX1
            GT100_default_replace(230, 1, GetJsonHex("fx1_t_wah_sens", a));                  //copy FX1
            GT100_default_replace(231, 1, GetJsonHex("fx1_t_wah_freq", a));                  //copy FX1
            GT100_default_replace(232, 1, GetJsonHex("fx1_t_wah_peak", a));                  //copy FX1
            GT100_default_replace(233, 1, GetJsonHex("fx1_t_wah_direct_mix", a));                  //copy FX1
            GT100_default_replace(234, 1, GetJsonHex("fx1_t_wah_effect_level", a));                  //copy FX1
            GT100_default_replace(236, 1, GetJsonHex("fx1_auto_wah_mode", a));                  //copy FX1
            GT100_default_replace(237, 1, GetJsonHex("fx1_auto_wah_freq", a));                  //copy FX1
            GT100_default_replace(238, 1, GetJsonHex("fx1_auto_wah_peak", a));                  //copy FX1
            GT100_default_replace(239, 1, GetJsonHex("fx1_auto_wah_rate", a));                  //copy FX1
            GT100_default_replace(240, 1, GetJsonHex("fx1_auto_wah_depth", a));                  //copy FX1
            GT100_default_replace(241, 1, GetJsonHex("fx1_auto_wah_direct_mix", a));                  //copy FX1
            GT100_default_replace(242, 1, GetJsonHex("fx1_auto_wah_effect_level", a));                  //copy FX1
            GT100_default_replace(245, 1, GetJsonHex("fx1_sub_wah_type", a));                  //copy FX1
            GT100_default_replace(246, 1, GetJsonHex("fx1_sub_wah_pedal_pos", a));                  //copy FX1
            GT100_default_replace(247, 1, GetJsonHex("fx1_sub_wah_pedal_min", a));                  //copy FX1
            GT100_default_replace(248, 1, GetJsonHex("fx1_sub_wah_pedal_max", a));                  //copy FX1
            GT100_default_replace(249, 1, GetJsonHex("fx1_sub_wah_effect_level", a));                  //copy FX1
            GT100_default_replace(250, 1, GetJsonHex("fx1_sub_wah_direct_mix", a));                  //copy FX1
            GT100_default_replace(251, 1, GetJsonHex("fx1_adv_comp_type", a));                  //copy FX1
            GT100_default_replace(252, 1, GetJsonHex("fx1_adv_comp_sustain", a));                  //copy FX1
            GT100_default_replace(253, 1, GetJsonHex("fx1_adv_comp_attack", a));                  //copy FX1
            GT100_default_replace(254, 1, GetJsonHex("fx1_adv_comp_tone", a));                  //copy FX1
            GT100_default_replace(255, 1, GetJsonHex("fx1_adv_comp_level", a));                  //copy FX1
            GT100_default_replace(257, 1, GetJsonHex("fx1_limiter_type", a));                  //copy FX1
            GT100_default_replace(258, 1, GetJsonHex("fx1_limiter_attack", a));                  //copy FX1
            GT100_default_replace(259, 1, GetJsonHex("fx1_limiter_thresh", a));                  //copy FX1
            GT100_default_replace(260, 1, GetJsonHex("fx1_limiter_ratio", a));                  //copy FX1
            GT100_default_replace(261, 1, GetJsonHex("fx1_limiter_release", a));                  //copy FX1
            GT100_default_replace(262, 1, GetJsonHex("fx1_limiter_level", a));                  //copy FX1
            GT100_default_replace(264, 1, GetJsonHex("fx1_graphic_eq_31hz", a));                  //copy FX1
            GT100_default_replace(265, 1, GetJsonHex("fx1_graphic_eq_62hz", a));                  //copy FX1
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
            GT100_default_replace(857, 1, GetJsonHex("chorus_on_off", a));                  //copy CE
            GT100_default_replace(858, 1, GetJsonHex("chorus_mode", a));                  //copy CE
            GT100_default_replace(859, 1, GetJsonHex("chorus_rate", a));                  //copy CE
            GT100_default_replace(860, 1, GetJsonHex("chorus_depth", a));                  //copy CE
            GT100_default_replace(861, 1, GetJsonHex("chorus_pre_delay", a));                  //copy CE
            GT100_default_replace(862, 1, GetJsonHex("chorus_low_cut", a));                  //copy CE
            GT100_default_replace(863, 1, GetJsonHex("chorus_high_cut", a));                  //copy CE
            GT100_default_replace(864, 1, GetJsonHex("chorus_effect_level", a));                  //copy CE
            GT100_default_replace(865, 1, GetJsonHex("chorus_direct_level", a));                  //copy CE
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
            GT100_default_replace(889, 1, GetJsonHex("pedal_fx_on_off", a));                  //copy PedalFX
            GT100_default_replace(891, 1, GetJsonHex("pedal_fx_pedal_bend_pitch", a));                  //copy PedalFX
            GT100_default_replace(892, 1, GetJsonHex("pedal_fx_pedal_bend_position", a));                  //copy PedalFX
            GT100_default_replace(893, 1, GetJsonHex("pedal_fx_pedal_bend_effect_level", a));                  //copy PedalFX
            GT100_default_replace(894, 1, GetJsonHex("pedal_fx_pedal_bend_direct_mix", a));                  //copy PedalFX
            GT100_default_replace(895, 1, GetJsonHex("pedal_fx_wah_type", a));                  //copy PedalFX
            GT100_default_replace(896, 1, GetJsonHex("pedal_fx_wah_position", a));                  //copy PedalFX
            GT100_default_replace(897, 1, GetJsonHex("pedal_fx_wah_pedal_min", a));                  //copy PedalFX
            GT100_default_replace(898, 1, GetJsonHex("pedal_fx_wah_pedal_max", a));                  //copy PedalFX
            GT100_default_replace(899, 1, GetJsonHex("pedal_fx_wah_effect_level", a));                  //copy PedalFX
            GT100_default_replace(900, 1, GetJsonHex("pedal_fx_wah_direct_mix", a));                  //copy PedalFX
            GT100_default_replace(908, 1, GetJsonHex("foot_volume_volume_curve", a));                  //copy FV
            GT100_default_replace(909, 1, GetJsonHex("foot_volume_volume_min", a));                  //copy FV
            GT100_default_replace(910, 1, GetJsonHex("foot_volume_volume_max", a));                  //copy FV
            GT100_default_replace(911, 1, GetJsonHex("foot_volume_level", a));                  //copy FV
            GT100_default_replace(921, 1, GetJsonHex("divider_mode", a));                  //copy Divide/Mix
            GT100_default_replace(922, 1, GetJsonHex("divider_ch_select", a));                  //copy Divide/Mix
            GT100_default_replace(923, 1, GetJsonHex("divider_ch_a_dynamic", a));                  //copy Divide/Mix
            GT100_default_replace(924, 1, GetJsonHex("divider_ch_a_dynamic_sens", a));                  //copy Divide/Mix
            GT100_default_replace(925, 1, GetJsonHex("divider_ch_a_filter", a));                  //copy Divide/Mix
            GT100_default_replace(926, 1, GetJsonHex("divider_ch_a_cutoff_freq", a));                  //copy Divide/Mix
            GT100_default_replace(927, 1, GetJsonHex("divider_ch_b_dynamic", a));                  //copy Divide/Mix
            GT100_default_replace(928, 1, GetJsonHex("divider_ch_b_dynamic_sens", a));                  //copy Divide/Mix
            GT100_default_replace(929, 1, GetJsonHex("divider_ch_b_filter", a));                  //copy Divide/Mix
            GT100_default_replace(930, 1, GetJsonHex("divider_ch_b_cutoff_freq", a));                  //copy Divide/Mix
            GT100_default_replace(937, 1, GetJsonHex("mixer_mode", a));                  //copy Divide/Mix
            GT100_default_replace(938, 1, GetJsonHex("mixer_ch_a_b_balance", a));                  //copy Divide/Mix
            GT100_default_replace(939, 1, GetJsonHex("mixer_spread", a));                  //copy Divide/Mix
            GT100_default_replace(942, 1, GetJsonHex("send_return_on_off", a));                  //copy S/R
            GT100_default_replace(943, 1, GetJsonHex("send_return_mode", a));                  //copy S/R
            GT100_default_replace(944, 1, GetJsonHex("send_return_send_level", a));                  //copy S/R
            GT100_default_replace(945, 1, GetJsonHex("send_return_return_level", a));                  //copy S/R
            GT100_default_replace(946, 1, GetJsonHex("send_return_adjust", a));                  //copy S/R
            GT100_default_replace(953, 1, GetJsonHex("amp_control", a));                  //copy Amp ctrl
            GT100_default_replace(956, 1, GetJsonHex("ns1_on_off", a));                  //copy NS
            GT100_default_replace(957, 1, GetJsonHex("ns1_threshold", a));                  //copy NS
            GT100_default_replace(958, 1, GetJsonHex("ns1_release", a));                  //copy NS
            GT100_default_replace(959, 1, GetJsonHex("ns1_detect", a));                  //copy NS
            GT100_default_replace(961, 1, GetJsonHex("ns2_on_off", a));                  //copy NS
            GT100_default_replace(962, 1, GetJsonHex("ns2_threshold", a));                  //copy NS
            GT100_default_replace(963, 1, GetJsonHex("ns2_release", a));                  //copy NS
            GT100_default_replace(964, 1, GetJsonHex("ns2_detect", a));                  //copy NS
            GT100_default_replace(969, 1, GetJsonHex("accel_fx_type", a));                  //copy Accel
            GT100_default_replace(970, 1, GetJsonHex("accel_fx_s_bend_pitch", a));                  //copy Accel
            GT100_default_replace(971, 1, GetJsonHex("accel_fx_s_bend_rise_time", a));                  //copy Accel
            GT100_default_replace(972, 1, GetJsonHex("accel_fx_s_bend_fall_time", a));                  //copy Accel
            GT100_default_replace(973, 1, GetJsonHex("accel_fx_laser_beam_rate", a));                  //copy Accel
            GT100_default_replace(974, 1, GetJsonHex("accel_fx_laser_beam_depth", a));                  //copy Accel
            GT100_default_replace(975, 1, GetJsonHex("accel_fx_laser_beam_rise_time", a));                  //copy Accel
            GT100_default_replace(976, 1, GetJsonHex("accel_fx_laser_beam_fall_time", a));                  //copy Accel
            GT100_default_replace(977, 1, GetJsonHex("accel_fx_ring_mod_freq", a));                  //copy Accel
            GT100_default_replace(978, 1, GetJsonHex("accel_fx_ring_mod_rise_time", a));                  //copy Accel
            GT100_default_replace(979, 1, GetJsonHex("accel_fx_ring_mod_fall_time", a));                  //copy Accel
            GT100_default_replace(980, 1, GetJsonHex("accel_fx_ring_mod_ring_level", a));                  //copy Accel
            GT100_default_replace(981, 1, GetJsonHex("accel_fx_ring_mod_octave_level", a));                  //copy Accel
            GT100_default_replace(982, 1, GetJsonHex("accel_fx_ring_mod_direct_mix", a));                  //copy Accel
            GT100_default_replace(983, 1, GetJsonHex("accel_fx_twist_level", a));                  //copy Accel
            GT100_default_replace(984, 1, GetJsonHex("accel_fx_twist_rise_time", a));                  //copy Accel
            GT100_default_replace(998, 1, GetJsonHex("accel_fx_twist_fall_time", a));                  //copy Accel
            GT100_default_replace(999, 1, GetJsonHex("accel_fx_warp_level", a));                  //copy Accel
            GT100_default_replace(1000, 1, GetJsonHex("accel_fx_warp_rise_time", a));                  //copy Accel
            GT100_default_replace(1001, 1, GetJsonHex("accel_fx_warp_fall_time", a));                  //copy Accel
            GT100_default_replace(1002, 1, GetJsonHex("accel_fx_feedbacker_mode", a));                  //copy Accel
            GT100_default_replace(1003, 1, GetJsonHex("accel_fx_feedbacker_depth", a));                  //copy Accel
            GT100_default_replace(1004, 1, GetJsonHex("accel_fx_feedbacker_rise_time", a));                  //copy Accel
            GT100_default_replace(1005, 1, GetJsonHex("accel_fx_feedbacker_octave_rise_time", a));                  //copy Accel
            GT100_default_replace(1006, 1, GetJsonHex("accel_fx_feedbacker_f_back_level", a));                  //copy Accel
            GT100_default_replace(1007, 1, GetJsonHex("accel_fx_feedbacker_octave_f_back_level", a));                  //copy Accel
            GT100_default_replace(1008, 1, GetJsonHex("accel_fx_feedbacker_vib_rate", a));                  //copy Accel
            GT100_default_replace(1009, 1, GetJsonHex("accel_fx_feedbacker_vib_depth", a));                  //copy Accel
            GT100_default_replace(1013, 1, GetJsonHex("patch_category", a));               //copy Master
            GT100_default_replace(1014, 1, GetJsonHex("patch_level", a));                  //copy Master
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
            GT100_default_replace(1062, 1, GetJsonHex("manual_mode_bank_down", a));                      //copy Manual Mode
            GT100_default_replace(1063, 1, GetJsonHex("manual_mode_bank_up", a));                        //copy Manual Mode
            GT100_default_replace(1064, 1, GetJsonHex("manual_mode_number_pedal1", a));                  //copy Manual Mode
            GT100_default_replace(1065, 1, GetJsonHex("manual_mode_number_pedal2", a));                  //copy Manual Mode
            GT100_default_replace(1066, 1, GetJsonHex("manual_mode_number_pedal3", a));                  //copy Manual Mode
            GT100_default_replace(1067, 1, GetJsonHex("manual_mode_number_pedal4", a));                  //copy Manual Mode
            GT100_default_replace(1068, 1, GetJsonHex("manual_mode_phrase_loop", a));                    //copy Manual Mode
            GT100_default_replace(1069, 1, GetJsonHex("manual_mode_accel_ctrl", a));                     //copy Manual Mode
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
            GT100_default_replace(1188, 1, GetJsonHex("assign1_target_h", a));                //copy Assigns
            GT100_default_replace(1189, 1, GetJsonHex("assign1_target_l", a));                //copy Assigns
            GT100_default_replace(1190, 1, GetJsonHex("assign1_target_min_h", a));            //copy Assigns
            GT100_default_replace(1191, 1, GetJsonHex("assign1_target_min_l", a));            //copy Assigns
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
            GT100_default_replace(1220, 1, GetJsonHex("assign2_target_h", a));                //copy Assigns
            GT100_default_replace(1221, 1, GetJsonHex("assign2_target_l", a));                //copy Assigns
            GT100_default_replace(1222, 1, GetJsonHex("assign2_target_min_h", a));            //copy Assigns
            GT100_default_replace(1223, 1, GetJsonHex("assign2_target_min_l", a));            //copy Assigns
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
            GT100_default_replace(1252, 1, GetJsonHex("assign3_target_h", a));                //copy Assigns
            GT100_default_replace(1253, 1, GetJsonHex("assign3_target_l", a));                //copy Assigns
            GT100_default_replace(1254, 1, GetJsonHex("assign3_target_min_h", a));            //copy Assigns
            GT100_default_replace(1255, 1, GetJsonHex("assign3_target_min_l", a));            //copy Assigns
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
            GT100_default_replace(1297, 1, GetJsonHex("assign4_target_h", a));                //copy Assigns
            GT100_default_replace(1298, 1, GetJsonHex("assign4_target_l", a));                //copy Assigns
            GT100_default_replace(1299, 1, GetJsonHex("assign4_target_min_h", a));            //copy Assigns
            GT100_default_replace(1300, 1, GetJsonHex("assign4_target_min_l", a));            //copy Assigns
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
            GT100_default_replace(1329, 1, GetJsonHex("assign5_target_h", a));                //copy Assigns
            GT100_default_replace(1330, 1, GetJsonHex("assign5_target_l", a));                //copy Assigns
            GT100_default_replace(1331, 1, GetJsonHex("assign5_target_min_h", a));            //copy Assigns
            GT100_default_replace(1332, 1, GetJsonHex("assign5_target_min_l", a));            //copy Assigns
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
            GT100_default_replace(1361, 1, GetJsonHex("assign6_target_h", a));                //copy Assigns
            GT100_default_replace(1362, 1, GetJsonHex("assign6_target_l", a));                //copy Assigns
            GT100_default_replace(1363, 1, GetJsonHex("assign6_target_min_h", a));            //copy Assigns
            GT100_default_replace(1364, 1, GetJsonHex("assign6_target_min_l", a));            //copy Assigns
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
            GT100_default_replace(1393, 1, GetJsonHex("assign7_target_h", a));                //copy Assigns
            GT100_default_replace(1394, 1, GetJsonHex("assign7_target_l", a));                //copy Assigns
            GT100_default_replace(1395, 1, GetJsonHex("assign7_target_min_h", a));            //copy Assigns
            GT100_default_replace(1396, 1, GetJsonHex("assign7_target_min_l", a));            //copy Assigns
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
            GT100_default_replace(1438, 1, GetJsonHex("assign8_target_h", a));                //copy Assigns
            GT100_default_replace(1439, 1, GetJsonHex("assign8_target_l", a));                //copy Assigns
            GT100_default_replace(1440, 1, GetJsonHex("assign8_target_min_h", a));            //copy Assigns
            GT100_default_replace(1441, 1, GetJsonHex("assign8_target_min_l", a));            //copy Assigns
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

            GT100_default_replace(2283, 1, GetJsonHex("fx1_acsim_high", a));              //copy FX1
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
            GT100_default_replace(2347, 2, GetJsonHex("delay2_delay_time", a));       //copy
            GT100_default_replace(2347, 1, GetJsonHex("delay2_delay_time_h", a));     //copy
            GT100_default_replace(2348, 1, GetJsonHex("delay2_delay_time_l", a));     //copy
            GT100_default_replace(2349, 1, GetJsonHex("delay2_f_back", a));           //copy
            GT100_default_replace(2350, 1, GetJsonHex("delay2_high_cut", a));         //copy
            GT100_default_replace(2351, 1, GetJsonHex("delay2_effect_level", a));     //copy
            GT100_default_replace(2352, 1, GetJsonHex("delay2_direct_mix", a));       //copy
            GT100_default_replace(2353, 1, GetJsonHex("delay2_tap_time", a));         //copy
            GT100_default_replace(2354, 2, GetJsonHex("delay2_d1_time", a));          //copy
            GT100_default_replace(2354, 1, GetJsonHex("delay2_d1_time_h", a));        //copy
            GT100_default_replace(2355, 1, GetJsonHex("delay2_d1_time_l", a));        //copy
            GT100_default_replace(2356, 1, GetJsonHex("delay2_d1_f_back", a));        //copy
            GT100_default_replace(2357, 1, GetJsonHex("delay2_d1_hi_cut", a));        //copy
            GT100_default_replace(2358, 1, GetJsonHex("delay2_d1_level", a));         //copy
            GT100_default_replace(2359, 2, GetJsonHex("delay2_d2_time", a));          //copy
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
            GT100_default_replace(2379, 1, GetJsonHex("fx1_dc30_echo_repeat_rate", a));
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
            GT100_default_replace(2393, 1, GetJsonHex("fx2_dc30_echo_repeat_rate", a));
            //int addr11 = 2408;
            GT100_default_replace(2408, 1, GetJsonHex("fx2_dc30_echo_intensity", a));
            GT100_default_replace(2409, 1, GetJsonHex("fx2_dc30_echo_volume", a));
            GT100_default_replace(2410, 1, GetJsonHex("fx2_dc30_tone", a));
            GT100_default_replace(2411, 1, GetJsonHex("fx2_dc30_output", a));
            GT100_default_replace(2412, 1, GetJsonHex("eq_type", a));
            GT100_default_replace(2413, 1, GetJsonHex("eq_geq_band1", a));
            GT100_default_replace(2414, 1, GetJsonHex("eq_geq_band2", a));
            GT100_default_replace(2415, 1, GetJsonHex("eq_geq_band3", a));
            GT100_default_replace(2416, 1, GetJsonHex("eq_geq_band4", a));
            GT100_default_replace(2417, 1, GetJsonHex("eq_geq_band5", a));
            GT100_default_replace(2418, 1, GetJsonHex("eq_geq_band6", a));
            GT100_default_replace(2419, 1, GetJsonHex("eq_geq_band7", a));
            GT100_default_replace(2420, 1, GetJsonHex("eq_geq_band8", a));
            GT100_default_replace(2421, 1, GetJsonHex("eq_geq_band9", a));
            GT100_default_replace(2422, 1, GetJsonHex("eq_geq_band10", a));
            GT100_default_replace(2423, 1, GetJsonHex("eq_geq_level", a));
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

            this->sysxPatches.append(GT100_default);


/*          //to fix broken bulk saves
            temp.append(GT100_default.mid(11, 128));       //address "00"
            temp.append(GT100_default.mid(152, 128));      //address "01"
            temp.append(GT100_default.mid(293, 128));      //address "02"
            temp.append(GT100_default.mid(434, 128));      //address "03"
            temp.append(GT100_default.mid(575, 128));      //address "04"
            temp.append(GT100_default.mid(716, 128));      //address "05" +
            temp.append(GT100_default.mid(857, 128));      //address "06" +
            temp.append(GT100_default.mid(998, 128));      //address "07" +
            temp.append(GT100_default.mid(1139, 128));     //address "08" +
            temp.append(GT100_default.mid(1280, 128));     //address "09" +
            temp.append(GT100_default.mid(1421, 128));     //address "0A" +
            temp.append(GT100_default.mid(1562, 128));     //address "0B" +
            temp.append(GT100_default.mid(1703, 128));     //address "0C" +
            temp.append(GT100_default.mid(1844, 128));     //address "0D" +
            temp.append(GT100_default.mid(1985, 128));     //address "0E" +
            temp.append(GT100_default.mid(2126, 128));     //address "0F" +
            temp.append(GT100_default.mid(2267, 60));      //address "10" +

            if(h==patchCount-1)
            {
                QFile xfile("test.txt");
                if (xfile.open(QIODevice::WriteOnly)){  xfile.write(temp); };
                for (int t=0;t<patchCount;t++)
                {
                    QByteArray temp_2;
                    temp_2 = temp.mid(0, 128);
                    GT100_default.replace(11, 128, temp_2);       //address "00" +
                    temp_2 = temp.mid(128, 128);
                    GT100_default.replace(152, 128, temp_2);      //address "01" +
                    temp_2 = temp.mid(256, 128);
                    GT100_default.replace(293, 128, temp_2);      //address "02" +
                    temp_2 = temp.mid(384, 128);
                    GT100_default.replace(434, 128, temp_2);      //address "03" +
                    temp_2 = temp.mid(512, 128);
                    GT100_default.replace(575, 128, temp_2);      //address "04" +
                    temp_2 = temp.mid(640, 128);
                    GT100_default.replace(716, 128, temp_2);      //address "05" +
                    temp_2 = temp.mid(768, 128);
                    GT100_default.replace(857, 128, temp_2);      //address "06" +
                    temp_2 = temp.mid(896, 128);
                    GT100_default.replace(998, 128, temp_2);      //address "07" +
                    temp_2 = temp.mid(1024, 128);
                    GT100_default.replace(1139, 128, temp_2);     //address "08" +
                    temp_2 = temp.mid(1152, 128);
                    GT100_default.replace(1280, 128, temp_2);     //address "09" +
                    temp_2 = temp.mid(1280, 128);
                    GT100_default.replace(1421, 128, temp_2);     //address "0A" +
                    temp_2 = temp.mid(1408, 128);
                    GT100_default.replace(1562, 128, temp_2);     //address "0B" +
                    temp_2 = temp.mid(1536, 128);
                    GT100_default.replace(1703, 128, temp_2);     //address "0C" +
                    temp_2 = temp.mid(1664, 128);
                    GT100_default.replace(1844, 128, temp_2);     //address "0D" +
                    temp_2 = temp.mid(1792, 128);
                    GT100_default.replace(1985, 128, temp_2);     //address "0E" +
                    temp_2 = temp.mid(1920, 128);
                    GT100_default.replace(2126, 128, temp_2);     //address "0F" +
                    temp_2 = temp.mid(2048, 60);
                    GT100_default.replace(2267, 60, temp_2);      //address "10" +
                    this->sysxPatches.append(GT100_default);
                    temp.remove(0,2110);
                };
            }; */

        }
        else if(device.contains("ME-80"))
        {
            temp.append((char)GetJsonValue("name1", a).toInt() );  //copy patch name
            temp.append((char)GetJsonValue("name2", a).toInt() );  //copy patch name
            temp.append((char)GetJsonValue("name3", a).toInt() );  //copy patch name
            temp.append((char)GetJsonValue("name4", a).toInt() );  //copy patch name
            temp.append((char)GetJsonValue("name5", a).toInt() );  //copy patch name
            temp.append((char)GetJsonValue("name6", a).toInt() );  //copy patch name
            temp.append((char)GetJsonValue("name7", a).toInt() );  //copy patch name
            temp.append((char)GetJsonValue("name8", a).toInt() );  //copy patch name
            temp.append((char)GetJsonValue("name9", a).toInt() );  //copy patch name
            temp.append((char)GetJsonValue("name10", a).toInt() ); //copy patch name
            temp.append((char)GetJsonValue("name11", a).toInt() ); //copy patch name
            temp.append((char)GetJsonValue("name12", a).toInt() ); //copy patch name
            temp.append((char)GetJsonValue("name13", a).toInt() ); //copy patch name
            temp.append((char)GetJsonValue("name14", a).toInt() ); //copy patch name
            temp.append((char)GetJsonValue("name15", a).toInt() ); //copy patch name
            temp.append((char)GetJsonValue("name16", a).toInt() ); //copy patch name
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
            this->sysxPatches.append(GT100_default.mid(0, patchSize));
        }else{
            QMessageBox *msgBox = new QMessageBox();
            msgBox->setWindowTitle(QObject::tr("Unknown Device Patch File"));
            msgBox->setIcon(QMessageBox::Warning);
            msgBox->setTextFormat(Qt::RichText);
            QString msgText;
            msgText.append("This device type currrently not supported<br>");
            msgText.append("Device type recognised as = "+device+"<br>");
            msgBox->setText(msgText);
            msgBox->setStandardButtons(QMessageBox::Ok);
            msgBox->exec();
            h=patchCount;
            skip = true;
            DialogClose();
        };
        a=(h+1)*pindex;                      // offset is set in front of marker
    };
    if(!skip) {updatePatch(); };
}

QByteArray bulkLoadDialog::GetJsonValue(QByteArray text, int pos )
{
    QByteArray str(((char)34+text+(char)34));
    int start_index = data.indexOf(str, pos)+(text.size()+3); //find pointer to start of Json value.
    int end_index = data.indexOf(",", start_index)-start_index;                  //find pointer to end of value to get the size of the value.
    QByteArray x = data.mid(start_index , end_index );                 //copy the Json value and return as a QByteArray.
    if(x.at(0)==(char)34) {x.remove(0, 1); };
    if(x.contains((char)34)) {x.truncate(x.indexOf((char)34)); };
    return x;
}

QByteArray bulkLoadDialog::GetJsonHex(QByteArray text, int pos )
{
    QByteArray str(((char)34+text+(char)34));
    int start_index = data.indexOf(str, pos)+(text.size()+3); //find pointer to start of Json value.
    bool ok;
    int end_index = data.indexOf(",", start_index)-start_index;                  //find pointer to end of value to get the size of the value.
    QByteArray x = data.mid(start_index , end_index );
    if(x.at(0)==(char)34) {x.remove(0, 1); };
    if(x.contains((char)34)) {x.truncate(x.indexOf((char)34)); };
    int value =  x.toInt(&ok, 10);
    QByteArray Array;
    Array.append((char)value);
    return Array;                                    // return one byte QByteArray
}

void bulkLoadDialog::GT100_default_replace(int pos, int size, QByteArray value)
{

    if(!value.isEmpty() && !value.isNull())
    {
        GT100_default.replace(pos, size, value);
    };
}

void bulkLoadDialog::DialogClose()
{
    steps = 300;
    SysxIO *sysxIO = SysxIO::Instance();
    QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)), this, SLOT(sendSequence()));
    sysxIO->setDeviceReady(true); // Free the device after finishing interaction.
    setStatusMessage(tr("Ready"));
    close();
    this->deleteLater();
}
