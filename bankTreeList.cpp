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
#include <QLayout>
#include <QMessageBox>
#include <QFile>
#include <QDataStream>
#include <QByteArray>
#include <QTimer>
#include "bankTreeList.h"
#include "Preferences.h"
#include "MidiTable.h"
#include "SysxIO.h"
#include "globalVariables.h"

// Platform-dependent sleep routines.
#ifdef Q_OS_WIN
#include <windows.h>
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds )
#else // Unix variants
#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

bankTreeList::bankTreeList(QWidget *parent)
    : QWidget(parent)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Font", "ratio").toDouble(&ok);
    load_last_file();
    QFont font( "Arial", 9*ratio, QFont::Bold);
    font.setStretch(90);
    this->treeList = newTreeList();
    this->treeList->setObjectName("banklist");
    this->treeList->setFont(font);
    QObject::connect(treeList, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(setOpenItems(QTreeWidgetItem*)));

    QObject::connect(treeList, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(setItemClicked(QTreeWidgetItem*, int)));

    QObject::connect(treeList, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(setItemDoubleClicked(QTreeWidgetItem*, int)));

    QObject::connect(this, SIGNAL(updateSignal()), this->parent(), SIGNAL(updateSignal()));

    QVBoxLayout *treeListLayout = new QVBoxLayout;
    treeListLayout->addWidget(treeList);
    treeListLayout->setMargin(0);
    treeListLayout->setSpacing(0);
    setLayout(treeListLayout);
    updateLoaderNames();

    SysxIO *sysxIO = SysxIO::Instance();
    QObject::connect(this, SIGNAL(setStatusSymbol(int)), sysxIO, SIGNAL(setStatusSymbol(int)));

    QObject::connect(this, SIGNAL(setStatusProgress(int)), sysxIO, SIGNAL(setStatusProgress(int)));

    QObject::connect(this, SIGNAL(setStatusMessage(QString)), sysxIO, SIGNAL(setStatusMessage(QString)));

    QObject::connect(this, SIGNAL(notConnectedSignal()), sysxIO, SIGNAL(notConnectedSignal()));
}

void bankTreeList::updateSize(QRect newrect)
{
    this->setGeometry(newrect);
}

void bankTreeList::setClosedItems(QTreeWidgetItem *item)
{
    Q_UNUSED(item);
}

void bankTreeList::closeChildren(QTreeWidgetItem *item)
{
    Q_UNUSED(item);
}

void bankTreeList::setOpenItems(QTreeWidgetItem *item)
{
    Q_UNUSED(item);
}

QTreeWidget* bankTreeList::newTreeList()
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Font", "ratio").toDouble(&ok);
    const int model = preferences->getPreferences("Midi", "Katana", "model").toInt(&ok);
    int channels = 9;
    if(model==0){ channels=5; }; //kat50 has 4+1 channels, kat100 has 8+1 channels.

    QFont Tfont( "Arial", 11*ratio, QFont::Bold);
    QFont Lfont( "Arial", 9*ratio, QFont::Bold);

    QTreeWidget *newTreeList = new QTreeWidget();
    newTreeList->setColumnCount(1);
    newTreeList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    newTreeList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    QStringList headers;
    headers << QObject::tr("Double-click tree item to load patch");
    newTreeList->setHeaderLabels(headers);

    QTreeWidgetItem *temp = new QTreeWidgetItem(newTreeList);
    temp->setFont(0, Tfont);
    temp->setText(0, "read current patch");
    temp->setWhatsThis(0, tr("Temporary Buffer.<br>a single mouse click will set the Write/Sync button to send to the buffer only,<br>a double click will load the current Katana patch data."));

    user = new QTreeWidgetItem(newTreeList);
    user->setText(0, "KATANA");
    user->setFont(0, Tfont);
    user->setWhatsThis(0, tr("User Banks.<br>expand the Bank to view a section of Banks."));

    for (int c=1; c<=channels; c++)
    {
        patch = new QTreeWidgetItem(user);
        patch->setFont(0, Lfont);
        patch->setText(0, QString("User Patch ").append(QString::number(c, 10)));
        patch->setWhatsThis(0, tr("User Patches.<br>a single mouse click will only change patch<br>a double mouse click will load the select patch from the Katana."));
    };

    //user->addChildren(userBankRanges);
    if( preferences->getPreferences("General", "Loader", "active") == "true"){
        loader = new QTreeWidgetItem(newTreeList);
        loader->setText(0, "LOADER");
        loader->setFont(0, Tfont);
        loader->setWhatsThis(0, tr("User Banks.<br>expand the Bank to view a section of Banks."));

        for (int c=9; c<=127; c++)
        {
            l_patch = new QTreeWidgetItem(loader);
            l_patch->setFont(0, Lfont);
            l_patch->setText(0, QString::number(c, 10).append(QString(": empty ")));
            l_patch->setWhatsThis(0, tr("Loader Patches.<br>a single mouse click will only change patch<br>a double mouse click will load the select patch from the Katana."));
        };
    };
    int x = 0;
    QList< QString > list;
    list.append("KATANA         ");
    list.append("KATANA       A-1");
    list.append("KATANA       A-2");
    if(channels>5){ list.append("KATANA       A-3");
        list.append("KATANA       A-4"); };
    list.append("KATANA       B-1");
    list.append("KATANA       B-2");
    if(channels>5){ list.append("KATANA       B-3");
        list.append("KATANA       B-4"); };

    QTreeWidgetItem *lead = new QTreeWidgetItem(newTreeList);
    lead->setFont(0, Tfont);
    lead->setText(0, "FACTORY");
    lead->setWhatsThis(0, tr("Preset Banks.<br>expand the Bank to view a section of Banks."));

    QList<QTreeWidgetItem *> leadBankRanges;

    for (int c=0; c<channels; c++)
    {
        QTreeWidgetItem* patch = new QTreeWidgetItem(lead);
        patch->setFont(0, Lfont);
        QString pnum = QString::number(c, 10);
        if(c==0){
            patch->setText(0, QString("PANEL: ").append(QString(list.at(x))));
        }else{
            patch->setText(0, QString("P").append(pnum).append(QString(": ")).append(QString(list.at(x))));
        };
        patch->setWhatsThis(0, tr("Preset Patches.<br>a single mouse click will only change patch<br>a double mouse click will load the select patch from the Katana."));
        ++x;
    };

    lead->addChildren(leadBankRanges);
    
    newTreeList->setExpanded(newTreeList->model()->index(1, 0), true);
    newTreeList->setExpanded(newTreeList->model()->index(2, 0), true);
    newTreeList->collapseAll();
    newTreeList->expandItem(user);
    return newTreeList;
}

/*********************** setItemClicked() ***********************************
 * Expands and colapses on a single click and sets patch sellection.
 ****************************************************************************/
void bankTreeList::setItemClicked(QTreeWidgetItem *item, int column)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const int model = preferences->getPreferences("Midi", "Katana", "model").toInt(&ok);

    Q_UNUSED(column);
    int bank = 1;
    int patch = 0;
    SysxIO *sysxIO = SysxIO::Instance();
    if(item->childCount() != 0 && !item->text(0).contains("read"))
    {
        if(item->isExpanded())
        {
            item->setExpanded(false);
        }
        else
        {
            item->setExpanded(true);
        };
    }
    else if (item->childCount() == 0 && sysxIO->isConnected() && sysxIO->deviceReady())
    {
        if (!item->text(0).contains("read"))
        {
            int select = item->parent()->indexOfChild(item);
            if(select==0){patch=4;};
            if(select==1){patch=0;};
            if(select==2){patch=1;};
            if(select==3){patch=2;};
            if(select==4){patch=3;};
            if(select==5){patch=5;};
            if(select==6){patch=6;};
            if(select==7){patch=7;};
            if(select==8){patch=8;};
            if(!item->parent()->text(0).contains("LOADER") && model==0 && (patch==2 || patch==3)){patch = patch+3;};
            if (item->parent()->text(0).contains("FACTORY")) {bank=51;};
            if (item->parent()->text(0).contains("LOADER"))
            {
                patch = item->parent()->indexOfChild(item);
                this->loader->child(patch)->setSelected(false); // unselect patch item.
                if(patch>(sysxIO->patchList.count()))  // check only first empty slot is selected,
                {patch=(sysxIO->patchList.count());};  // as there can not be empty space between patches.
                this->loader->child(patch)->setSelected(true); // select/highlight first empty patch.
                patch=patch+9;
            };
            sysxIO->requestPatchChange(bank, patch); // send the patch change.
        }
        emit patchSelectSignal(bank, patch+1);  // update the patch number display.
    };

}

/*********************** setItemDoubleClicked() *****************************
 * Handles when a patch is double clicked in the tree list. Patch will be
 * loaded into the temp buffer and will tell to request the data afterwards.
 ****************************************************************************/
void bankTreeList::setItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const int model = preferences->getPreferences("Midi", "Katana", "model").toInt(&ok);

    Q_UNUSED(column);
    int bank = 1;
    int patch = 0;
    SysxIO *sysxIO = SysxIO::Instance();

    if(item->childCount() == 0 && sysxIO->deviceReady() && sysxIO->isConnected())
        // Make sure it's a patch (Patches are the last in line so no children).
    {
        emit setStatusSymbol(2);
        emit setStatusMessage(tr("Patch request"));

        sysxIO->setDeviceReady(false);
        if(item->text(0).contains("read"))
        {
            requestPatch();
            emit patchSelectSignal(bank, patch);
        }
        else
        {
            patch = item->parent()->indexOfChild(item);
            if(!item->parent()->text(0).contains("LOADER") && model==0 && (patch==3 || patch==4)){patch = patch+2;};
            if (item->parent()->text(0).contains("FACTORY")) {bank=51;};
            if (item->parent()->text(0).contains("LOADER")) {patch=patch+9;};
            //emit patchSelectSignal(bank, patch+1);
            patch++;
            if(patch<10){requestPatch(bank, patch);};
            if(patch>9){
                if(sysxIO->fileReady==true)
                {
                    file.convertFromTSL(patch-9);
                    emit updateSignal();
                    sysxIO->writeToBuffer();
                };
            };
        };

    };
}
/*********************** requestPatch() *******************************
 * Does the actual requesting of the patch data and hands the
 * reception over to updatePatch function.
 **********************************************************************/
void bankTreeList::requestPatch()
{
    SysxIO *sysxIO = SysxIO::Instance();

    QObject::disconnect(sysxIO, SIGNAL(isChanged()),
                        this, SLOT(requestPatch()));

    if(sysxIO->isConnected())
    {
        QObject::connect(sysxIO, SIGNAL(sysxReply(QString)),	// Connect the result of the request
                         this, SLOT(updatePatch(QString)));					// to updatePatch function.

        emit setStatusSymbol(3);
        emit setStatusMessage(tr("Receiving Patch"));
        sysxIO->requestPatch(0, 0);
    };
}

void bankTreeList::requestPatch(int bank, int patch)
{
    SysxIO *sysxIO = SysxIO::Instance();
    if(sysxIO->isConnected())
    {
        QObject::connect(sysxIO, SIGNAL(sysxReply(QString)),	// Connect the result of the request
                         this, SLOT(updatePatch(QString)));					// to updatePatch function.

        emit setStatusSymbol(3);
        emit setStatusMessage(tr("Receiving Patch"));

        sysxIO->requestPatch(bank, patch);
    };
}

/*********************** updatePatch() *******************************
 * Updates the source of the currently handled patch and set the
 * attributes accordingly.
 *********************************************************************/
void bankTreeList::updatePatch(QString replyMsg)
{
    SysxIO *sysxIO = SysxIO::Instance();

    sysxIO->setDeviceReady(true); // Free the device after finishing interaction.

    QObject::disconnect(sysxIO, SIGNAL(sysxReply(QString)),
                        this, SLOT(updatePatch(QString)));

    if (replyMsg.size()/2 == 2928) {
        QByteArray data;
        QFile file(":default.syx");
        if (file.open(QIODevice::ReadOnly)){ data = file.readAll(); };
        QString sysxBuffer;
        for(int i=0;i<data.size();i++)
        {
            unsigned char byte = (char)data[i];
            unsigned int n = (int)byte;
            QString hex = QString::number(n, 16).toUpper();
            if (hex.length() < 2) hex.prepend("0");
            sysxBuffer.append(hex);
        };

        sysxBuffer.replace(22,  256, replyMsg.mid( 24, 256));  //"00 00 00 00"  replace default patch sections with received data
        sysxBuffer.replace(304, 226, replyMsg.mid(280, 226));  //"00 00 01 00"
        sysxBuffer.replace(530,  30, replyMsg.mid(534,  30));  //"00 00 01 00"
        sysxBuffer.replace(586, 256, replyMsg.mid(564, 256));  //"00 00 02 00"
        sysxBuffer.replace(868, 196, replyMsg.mid(820, 196));  //"00 00 03 00"
        sysxBuffer.replace(1064, 60, replyMsg.mid(1044, 60));  //"00 00 03 00"
        sysxBuffer.replace(1150, 256, replyMsg.mid(1104, 256));//"00 00 04 00"
        sysxBuffer.replace(1432, 166, replyMsg.mid(1360, 166));//"00 00 05 00"
        sysxBuffer.replace(1598, 90, replyMsg.mid(1554, 90));  //"00 00 05 00"
        sysxBuffer.replace(1714, 256, replyMsg.mid(1644, 256));//"00 00 06 00"
        sysxBuffer.replace(1996, 136, replyMsg.mid(1900, 136));//"00 00 07 00"
        sysxBuffer.replace(2132, 120, replyMsg.mid(2064, 120));//"00 00 07 00"
        sysxBuffer.replace(2278, 256, replyMsg.mid(2184, 256));//"00 00 08 00"
        sysxBuffer.replace(2560, 106, replyMsg.mid(2440, 106));//"00 00 09 00"
        sysxBuffer.replace(2574, 150, replyMsg.mid(2574, 150));//"00 00 09 00"
        sysxBuffer.replace(2842, 256, replyMsg.mid(2724, 256));//"00 00 0A 00"
        sysxBuffer.replace(3124,  76, replyMsg.mid(2980,  76));//"00 00 0B 00"
        sysxBuffer.replace(3200, 180, replyMsg.mid(3084, 180));//"00 00 0B 00"
        sysxBuffer.replace(3406, 256, replyMsg.mid(3264, 256));//"00 00 0C 00"
        sysxBuffer.replace(3688,  46, replyMsg.mid(3520,  46));//"00 00 0D 00"
        sysxBuffer.replace(3734, 210, replyMsg.mid(3594, 210));//"00 00 0D 00"
        sysxBuffer.replace(3970, 256, replyMsg.mid(3804, 256));//"00 00 0E 00"
        sysxBuffer.replace(4252,  16, replyMsg.mid(4060,  16));//"00 00 0F 00"
        sysxBuffer.replace(4268, 240, replyMsg.mid(4104, 240));//"00 00 0F 00"
        sysxBuffer.replace(4534, 242, replyMsg.mid(4344, 242));//"00 00 10 00"
        sysxBuffer.replace(4776,  14, replyMsg.mid(4614,  14));//"00 00 10 00"
        sysxBuffer.replace(4816, 256, replyMsg.mid(4628, 256));//"00 00 11 00"
        sysxBuffer.replace(5098, 212, replyMsg.mid(4884, 212));//"00 00 12 00"
        sysxBuffer.replace(5380, 256, replyMsg.mid(5124, 256));//"00 00 13 00"
        sysxBuffer.replace(5662, 226, replyMsg.mid(5380, 226));//"00 00 14 00"
        sysxBuffer.replace(5944, 218, replyMsg.mid(5634, 218));//"00 00 15 00"

        // Add correct checksum to patch strings
        replyMsg.clear();
        QString hex;
        QString sysxEOF;
        int msgLength = sysxBuffer.length()/2;
        for(int i=0;i<msgLength*2;++i)
        {
            hex.append(sysxBuffer.mid(i*2, 2));
            sysxEOF = (sysxBuffer.mid((i*2)+4, 2));
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
                replyMsg.append(hex);
                i=i+2;
                hex.clear();
                sysxEOF.clear();
            };
        };

        emit setStatusMessage(tr("Ready"));

        QString area = "Structure";
        sysxIO->setFileSource(area, replyMsg);		// Set the source to the data received.
        sysxIO->setFileName(tr("Patch from ") + deviceType);	// Set the file name to KATANA patch for the display.
        sysxIO->setDevice(true);				// Patch received from the device so this is set to true.
        sysxIO->setSyncStatus(true);			// We can't be more in sync than right now! :)

        sysxIO->setLoadedBank(sysxIO->getBank());
        sysxIO->setLoadedPatch(sysxIO->getPatch());

        emit updateSignal();
        emit setStatusProgress(0);
    };

    if(!replyMsg.isEmpty() && replyMsg.size()/2 != fullPatchSize)
    {
        //emit notConnectedSignal();				// No message returned so connection must be lost.
        /* NO-REPLY WARNING */
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle(QObject::tr("Warning - Patch data received is incorrect!"));
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setTextFormat(Qt::RichText);
        QString msgText;
        msgText.append("<font size='+1'><b>");
        msgText.append(QObject::tr("Patch data transfer wrong size or data error"));
        msgText.append("<b></font><br>");
        msgText.append(QObject::tr("Please make sure the ") + deviceType + QObject::tr(" is connected correctly and re-try."));
        msgText.append(QObject::tr("<br> data size received = ")+(QString::number(replyMsg.size()/2, 10)));
        msgBox->setText(msgText);
        msgBox->show();
        QTimer::singleShot(3000, msgBox, SLOT(deleteLater()));
        /* END WARNING */
    };
    if(replyMsg.isEmpty())
    {
        //emit notConnectedSignal();				// No message returned so connection must be lost.
        /* NO-REPLY WARNING */
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle(QObject::tr("Warning - Patch data not received!"));
        msgBox->setIcon(QMessageBox::Warning);
        msgBox->setTextFormat(Qt::RichText);
        QString msgText;
        msgText.append("<font size='+1'><b>");
        msgText.append(QObject::tr("Patch data transfer failed, are the correct midi ports selected?"));
        msgText.append("<b></font><br>");
        msgText.append(QObject::tr("Please make sure the ") + deviceType + QObject::tr(" is connected correctly and re-try."));
        msgBox->setText(msgText);
        msgBox->show();
        QTimer::singleShot(3000, msgBox, SLOT(deleteLater()));
        /* END WARNING */
    };

    /*Preferences *preferences = Preferences::Instance(); // Load the preferences.
        if(preferences->getPreferences("Midi", "DBug", "bool")=="true")
        {
        if (replyMsg.size() > 0){
                QString snork;
                        snork.append("<font size='-1'>");
                        //snork.append(tr("{ size="));
                        //snork.append(QString::number(replyMsg.size()/2, 10));
                        //snork.append("}");
                        //snork.append(tr("<br> midi data received"));
                        for(int i=0;i<replyMsg.size();++i)
                        {
                                snork.append(replyMsg.mid(i, 2));
                                snork.append(" ");
                                i++;
                        };
                        snork.replace("F7", "F7<br>");
                        //snork.replace("F0", "{ F0");
                        QString msgText;
                        msgText.append(QObject::tr("<br> data size received = ")+(QString::number(replyMsg.size()/2, 10)));

                        QMessageBox *msgBox = new QMessageBox();
                        msgBox->setWindowTitle(msgText);
                        msgBox->setIcon(QMessageBox::Information);
                        msgBox->setText(snork);
                        msgBox->setStandardButtons(QMessageBox::Ok);
                        msgBox->exec();
                        };
                };*/
    //Preferences *preferences = Preferences::Instance(); // Load the preferences.
    // if(preferences->getPreferences("Midi", "DBug", "bool")=="true")
    {
        if(replyMsg.size()>4000)
        {
            QFile file("saved_patch.syx");
            if (file.open(QIODevice::WriteOnly))
            {
                QByteArray out;
                unsigned int count=0;
                QString data = replyMsg;
                int x = data.size()/2;
                for (int a=0;a<x;++a)
                {
                    QString str = data.at(a*2);
                    str.append(data.at((a*2)+1));
                    bool ok;
                    unsigned int n = str.toInt(&ok, 16);
                    out[count] = (char)n;
                    count++;
                };
                file.write(out);

                /*  QString snork = tr("patch data saved to file as 'saved_patch.syx'  <br>");
                QMessageBox *msgBox = new QMessageBox();
                msgBox->setWindowTitle(deviceType + tr("System data requested"));
                msgBox->setIcon(QMessageBox::Information);
                msgBox->setText(snork);
                msgBox->show();
                QTimer::singleShot(1000, msgBox, SLOT(deleteLater()));*/
            };
        };
    };

}

/********************************** connectedSignal() ****************************
* This slot reloads all patch names of expanded items, if any, on (re)connection to a
* device.
*********************************************************************************/
void bankTreeList::connectedSignal()
{
    //requestPatch(); //load the current temp buffer
    SysxIO *sysxIO = SysxIO::Instance();
    if(/*this->openPatchTreeItems.size() != 0 &&*/ sysxIO->deviceReady() && sysxIO->isConnected())
    {
        sysxIO->setDeviceReady(false);

        this->listIndex = 0;
        this->itemIndex = 0;

        QObject::disconnect(sysxIO, SIGNAL(patchName(QString)),
                            this, SLOT(updatePatchNames(QString)));
        QObject::connect(sysxIO, SIGNAL(patchName(QString)),
                         this, SLOT(updatePatchNames(QString)));

        this->currentPatchTreeItems.clear();
        this->currentPatchTreeItems = this->openPatchTreeItems;
        this->updatePatchNames("");
    }
    else if (sysxIO->deviceReady() && sysxIO->isConnected())
    { requestPatch(); };
    // this->systemRequested=false;
}

/********************************** updateTree() ********************************
* This handles whether we add the newly expanded item to the current job or
* start a new one.
*********************************************************************************/
void bankTreeList::updateTree(QTreeWidgetItem *item)
{
    SysxIO *sysxIO = SysxIO::Instance();
    if(sysxIO->deviceReady() && sysxIO->isConnected())
    {
        sysxIO->setDeviceReady(false);

        this->listIndex = 0;
        this->itemIndex = 0;

        QObject::disconnect(sysxIO, SIGNAL(patchName(QString)),
                            this, SLOT(updatePatchNames(QString)));
        QObject::connect(sysxIO, SIGNAL(patchName(QString)),
                         this, SLOT(updatePatchNames(QString)));

        this->updatePatchNames("");
    }
    else
    {
        this->currentPatchTreeItems.append(item);
    };
}

/***************************** updatePatchNames() ********************************
* This updates the patch names in the treeList of all expanded items. this is
* done when a bank is expanded or when we (re)connect to a device.
*********************************************************************************/
void bankTreeList::updatePatchNames(QString name)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const int model = preferences->getPreferences("Midi", "Katana", "model").toInt(&ok);
    int channels = 9;
    if(model==0){ channels=5; }; //kat50 has 4+1 channels, kat100 has 8+1 channels.

    SysxIO *sysxIO = SysxIO::Instance();
    qApp->setOverrideCursor(Qt::WaitCursor);
    qApp->processEvents();
    if(!name.isEmpty() && sysxIO->isConnected()) //  If not empty we can assume that we did receive a patch name.
    {
        QString patchNum = QString::number(this->listIndex, 10).toUpper();
        if (this->listIndex==0){patchNum="PANEL: "; };
        if (this->listIndex==1){patchNum="CH A1: "; };
        if (this->listIndex==2){patchNum="CH A2: "; };
        if(channels>5){
            if (this->listIndex==3){patchNum="CH A3: "; };
            if (this->listIndex==4){patchNum="CH A4: "; };
            if (this->listIndex==5){patchNum="CH B1: "; };
            if (this->listIndex==6){patchNum="CH B2: "; };
            if (this->listIndex==7){patchNum="CH B3: "; };
            if (this->listIndex==8){patchNum="CH B4: "; };
        }else{
            if (this->listIndex==3){patchNum="CH B1: "; };
            if (this->listIndex==4){patchNum="CH B2: "; };
        };
        this->user->child(this->listIndex)->setText(0, patchNum+name);
        this->listIndex++;
    };

    if(listIndex < channels) // As long as we have items in the list we continue, duh! :)
    {
        int patch = listIndex;
        if(channels==5 && (listIndex==3 || listIndex==4)){patch=patch+2;};
        if(sysxIO->isConnected())
        {
            emit setStatusSymbol(3);
            emit setStatusMessage(tr("Reading names"));
            sysxIO->requestPatchName(1, patch+1); // The patch name request.
        }
        else
        {
            sysxIO->setDeviceReady(true);
            this->listIndex = 0;
            emit setStatusSymbol(1);
            emit setStatusMessage(tr("Ready"));
            emit setStatusProgress(0);

            QObject::disconnect(sysxIO, SIGNAL(patchName(QString)),
                                this, SLOT(updatePatchNames(QString)));
        };
    }
    else {
        sysxIO->setDeviceReady(true);
        emit setStatusSymbol(1);
        emit setStatusMessage(tr("Ready"));

        QObject::disconnect(sysxIO, SIGNAL(patchName(QString)),
                            this, SLOT(updatePatchNames(QString)));

        if(this->systemRequested==false) {requestPatch(); QTimer::singleShot(2000, this, SLOT(systemRequest())); };
    };
    qApp->restoreOverrideCursor ();
}

void bankTreeList::updateLoaderNames()
{
    Preferences *preferences = Preferences::Instance();
    if( preferences->getPreferences("General", "Loader", "active") == "true"){
        SysxIO *sysxIO = SysxIO::Instance();
        if(sysxIO->fileReady==true){
            sysxIO->patchList.clear();
            unsigned int patchCount = sysxIO->data.count("params"); // count the number of times 'params' appears to get patch count
            int pindex = (sysxIO->data.size()/patchCount);
            unsigned int a = 400; // locate patch text start position from the start of the file
            for (unsigned int h=0;h<patchCount;h++)
            {
                QByteArray text ("patchname");
                QByteArray str(((char)34+text+(char)34));
                int start_index = sysxIO->data.indexOf(str, a)+(text.size()+4); //find pointer to start of Json value.
                int end_index = sysxIO->data.indexOf(",", start_index)-start_index; //find pointer to end of value to get the size of the value.
                sysxIO->patchList.append(sysxIO->data.mid(start_index , end_index-1 ));  //copy the Json value and return as a QByteArray.
                a=a+pindex;  // offset is set in front of marker
            };

            for(unsigned int h=0;h<119;h++)
            {
                QString name;
                if(h<patchCount){ name=sysxIO->patchList.at(h); };
                if(h>patchCount-1){ name="empty"; };
                this->loader->child(h)->setText(0, QString::number(h+9)+": "+name);
            };
        };
    };
}

void bankTreeList::systemRequest()
{
    SysxIO *sysxIO = SysxIO::Instance();
    sysxIO->systemDataRequest();
    this->systemRequested=true;
}

void bankTreeList::load_last_file()
{
    Preferences *preferences = Preferences::Instance();
    QString fileName = preferences->getPreferences("General", "Loader", "file");
    SysxIO *sysxIO = SysxIO::Instance();
    if (fileName.isEmpty())
    {
        sysxIO->fileReady=false;
    }else{
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly))
        {
            sysxIO->data = file.readAll();     // read the pre-selected file,
            sysxIO->setFileName(fileName);
            sysxIO->fileReady=true;
        };
    };

}

