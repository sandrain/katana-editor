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

#include "editWindow.h"
#include "MidiTable.h"
#include "SysxIO.h"
#include "globalVariables.h"
#include "bulkEditDialog.h"
#include "Preferences.h"

editWindow::editWindow(QWidget *parent)
    : QDialog(parent)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);
    const double fratio = preferences->getPreferences("Window", "Font", "ratio").toDouble(&ok);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    if(preferences->getPreferences("Window", "Single", "bool")=="true")
    {
        this->image = QPixmap(":images/editwindow.png");
        this->setFixedSize(image.width()*ratio, image.height()*ratio);

        this->setWindowFlags(Qt::WindowStaysOnTopHint);
    } else { this->image = QPixmap(":images/meshWindow.png"); };

    QFont Cfont( "Arial", 14*fratio, QFont::Bold);

    this->title = new QLabel;
    this->title->setFont(Cfont);
    this->title->setObjectName("title");

    this->comboBoxLabel = new QLabel(tr("PAGE SELECT"));
    this->comboBoxLabel->setObjectName("selectlabel");
    this->comboBoxLabel->setVisible(false);

    this->pageComboBox = new QComboBox;
    this->pageComboBox->setObjectName("largecombo");
    this->pageComboBox->setFont(Cfont);
    this->pageComboBox->setEditable(false);
    this->pageComboBox->setFrame(false);
    this->pageComboBox->setVisible(false);

    this->bulkEdit_Button = new customControlLabel;
    this->bulkEdit_Button->setButton(true);
    this->bulkEdit_Button->setImage(":/images/pushbutton_dark.png");
    this->bulkEdit_Button->setText(tr("Bulk Write"));
    this->bulkEdit_Button->setAlignment(Qt::AlignCenter);
    this->bulkEdit_Button->setWhatsThis(tr("Multiple Patch Edit Button<br>will write only the currently displayed effect part to a selection of User Patches."
                                           "<br>useful for applying common settings to a patch group."));

    this->swap_Button = new customControlLabel;
    this->swap_Button->setButton(true);
    this->swap_Button->setImage(":/images/pushbutton_dark.png");
    this->swap_Button->setText(tr("Pre A/B swap"));
    this->swap_Button->setAlignment(Qt::AlignCenter);
    this->swap_Button->setWhatsThis(tr("Will swap the settings betweem PreAmp A and PreAmp B."));

    this->temp1_Button = new customControlLabel;
    this->temp1_Button->setButton(true);
    this->temp1_Button->setImage(":/images/pushbutton_dark.png");
    this->temp1_Button->setText(tr("ClipBd-1 Paste"));
    this->temp1_Button->setAlignment(Qt::AlignCenter);
    this->temp1_Button->setWhatsThis(tr("Effect Partial Paste Button<br>will paste only the currently displayed effect part from the selected Temp clipboard."
                                        "<br>can be used to quicky compare settings between patches."));

    this->temp2_Button = new customControlLabel;
    this->temp2_Button->setButton(true);
    this->temp2_Button->setImage(":/images/pushbutton_dark.png");
    this->temp2_Button->setText(tr("ClipBd-2 Paste"));
    this->temp2_Button->setAlignment(Qt::AlignCenter);
    this->temp2_Button->setWhatsThis(tr("Effect Partial Paste Button<br>will paste only the currently displayed effect part from the selected Temp clipboard."
                                        "<br>can be used to quicky compare settings between patches."));

    this->temp3_Button = new customControlLabel;
    this->temp3_Button->setButton(true);
    this->temp3_Button->setImage(":/images/pushbutton_dark.png");
    this->temp3_Button->setText(tr("ClipBd-3 Paste"));
    this->temp3_Button->setAlignment(Qt::AlignCenter);
    this->temp3_Button->setWhatsThis(tr("Effect Partial Paste Button<br>will paste only the currently displayed effect part from the selected Temp clipboard."
                                        "<br>can be used to quicky compare settings between patches."));

    this->temp4_Button = new customControlLabel;
    this->temp4_Button->setButton(true);
    this->temp4_Button->setImage(":/images/pushbutton_dark.png");
    this->temp4_Button->setText(tr("ClipBd-4 Paste"));
    this->temp4_Button->setAlignment(Qt::AlignCenter);
    this->temp4_Button->setWhatsThis(tr("Effect Partial Paste Button<br>will paste only the currently displayed effect part from the selected Temp clipboard."
                                        "<br>can be used to quicky compare settings between patches."));

    this->temp5_Button = new customControlLabel;
    this->temp5_Button->setButton(true);
    this->temp5_Button->setImage(":/images/pushbutton_dark.png");
    this->temp5_Button->setText(tr("ClipBd-5 Paste"));
    this->temp5_Button->setAlignment(Qt::AlignCenter);
    this->temp5_Button->setWhatsThis(tr("Effect Partial Paste Button<br>will paste only the currently displayed effect part from the selected Temp clipboard."
                                        "<br>can be used to quicky compare settings between patches."));

    this->assign1_Button = new customControlLabel;
    this->assign1_Button->setButton(true);
    this->assign1_Button->setImage(":/images/pushbutton_dark.png");
    this->assign1_Button->setText(tr("Assign 1 Paste"));
    this->assign1_Button->setAlignment(Qt::AlignCenter);
    this->assign1_Button->setWhatsThis(tr("Assign Paste Button<br>will paste an Assign setting from another Assign from the same patch."
                                          "<br>can be used to copy or shift an Assign setting."));

    this->assign2_Button = new customControlLabel;
    this->assign2_Button->setButton(true);
    this->assign2_Button->setImage(":/images/pushbutton_dark.png");
    this->assign2_Button->setText(tr("Assign 2 Paste"));
    this->assign2_Button->setAlignment(Qt::AlignCenter);
    this->assign2_Button->setWhatsThis(tr("Assign Paste Button<br>will paste an Assign setting from another Assign from the same patch."
                                          "<br>can be used to copy or shift an Assign setting."));

    this->assign3_Button = new customControlLabel;
    this->assign3_Button->setButton(true);
    this->assign3_Button->setImage(":/images/pushbutton_dark.png");
    this->assign3_Button->setText(tr("Assign 3 Paste"));
    this->assign3_Button->setAlignment(Qt::AlignCenter);
    this->assign3_Button->setWhatsThis(tr("Assign Paste Button<br>will paste an Assign setting from another Assign from the same patch."
                                          "<br>can be used to copy or shift an Assign setting."));

    this->assign4_Button = new customControlLabel;
    this->assign4_Button->setButton(true);
    this->assign4_Button->setImage(":/images/pushbutton_dark.png");
    this->assign4_Button->setText(tr("Assign 4 Paste"));
    this->assign4_Button->setAlignment(Qt::AlignCenter);
    this->assign4_Button->setWhatsThis(tr("Assign Paste Button<br>will paste an Assign setting from another Assign from the same patch."
                                          "<br>can be used to copy or shift an Assign setting."));

    this->assign5_Button = new customControlLabel;
    this->assign5_Button->setButton(true);
    this->assign5_Button->setImage(":/images/pushbutton_dark.png");
    this->assign5_Button->setText(tr("Assign 5 Paste"));
    this->assign5_Button->setAlignment(Qt::AlignCenter);
    this->assign5_Button->setWhatsThis(tr("Assign Paste Button<br>will paste an Assign setting from another Assign from the same patch."
                                          "<br>can be used to copy or shift an Assign setting."));

    this->assign6_Button = new customControlLabel;
    this->assign6_Button->setButton(true);
    this->assign6_Button->setImage(":/images/pushbutton_dark.png");
    this->assign6_Button->setText(tr("Assign 6 Paste"));
    this->assign6_Button->setAlignment(Qt::AlignCenter);
    this->assign6_Button->setWhatsThis(tr("Assign Paste Button<br>will paste an Assign setting from another Assign from the same patch."
                                          "<br>can be used to copy or shift an Assign setting."));

    this->assign7_Button = new customControlLabel;
    this->assign7_Button->setButton(true);
    this->assign7_Button->setImage(":/images/pushbutton_dark.png");
    this->assign7_Button->setText(tr("Assign 7 Paste"));
    this->assign7_Button->setAlignment(Qt::AlignCenter);
    this->assign7_Button->setWhatsThis(tr("Assign Paste Button<br>will paste an Assign setting from another Assign from the same patch."
                                          "<br>can be used to copy or shift an Assign setting."));

    this->assign8_Button = new customControlLabel;
    this->assign8_Button->setButton(true);
    this->assign8_Button->setImage(":/images/pushbutton_dark.png");
    this->assign8_Button->setText(tr("Assign 8 Paste"));
    this->assign8_Button->setAlignment(Qt::AlignCenter);
    this->assign8_Button->setWhatsThis(tr("Assign Paste Button<br>will paste an Assign setting from another Assign from the same patch."
                                          "<br>can be used to copy or shift an Assign setting."));

    this->closeButton = new customControlLabel;
    this->closeButton->setButton(true);
    this->closeButton->setImage(":/images/closebutton.png");
    this->closeButton->setWhatsThis(tr("Will close the current edit page window."));

    QHBoxLayout *pastebuttonLayout = new QHBoxLayout;
    pastebuttonLayout->addWidget(this->bulkEdit_Button);
    //pastebuttonLayout->addWidget(this->swap_Button);
    pastebuttonLayout->addWidget(this->temp1_Button);
    pastebuttonLayout->addWidget(this->temp2_Button);
    pastebuttonLayout->addWidget(this->temp3_Button);
    pastebuttonLayout->addWidget(this->temp4_Button);
    pastebuttonLayout->addWidget(this->temp5_Button);

    QHBoxLayout *top4buttonLayout = new QHBoxLayout;
    top4buttonLayout->addSpacing(170*ratio);
    top4buttonLayout->addWidget(this->assign1_Button);
    top4buttonLayout->addWidget(this->assign2_Button);
    top4buttonLayout->addWidget(this->assign3_Button);
    top4buttonLayout->addWidget(this->assign4_Button);

    QHBoxLayout *bottom4buttonLayout = new QHBoxLayout;
    bottom4buttonLayout->addSpacing(170*ratio);
    bottom4buttonLayout->addWidget(this->assign5_Button);
    bottom4buttonLayout->addWidget(this->assign6_Button);
    bottom4buttonLayout->addWidget(this->assign7_Button);
    bottom4buttonLayout->addWidget(this->assign8_Button);

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addLayout(pastebuttonLayout);
    buttonLayout->addSpacing(10*ratio);
    buttonLayout->addLayout(top4buttonLayout);
    buttonLayout->addLayout(bottom4buttonLayout);

    QHBoxLayout *headerLayout = new QHBoxLayout;
    headerLayout->addWidget(this->title);
    headerLayout->addStretch();
    headerLayout->addWidget(this->comboBoxLabel);
    headerLayout->addWidget(this->pageComboBox);
    headerLayout->addStretch();
    headerLayout->addLayout(buttonLayout);
    headerLayout->addStretch();
    headerLayout->addWidget(this->closeButton);

    this->pagesWidget = new QStackedWidget;

    QHBoxLayout *pagesLayout = new QHBoxLayout;
    pagesLayout->addStretch();
    pagesLayout->addWidget(this->pagesWidget);
    pagesLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(headerLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(pagesLayout);
    mainLayout->addStretch();
    setLayout(mainLayout);

    this->tempPage = new editPage;

    QObject::connect(this->pageComboBox, SIGNAL(activated(int)), this->pagesWidget, SLOT(setCurrentIndex(int)));

    QObject::connect(this->pageComboBox, SIGNAL(highlighted(int)), this->pagesWidget, SLOT(setCurrentIndex(int)));

    QObject::connect(this->bulkEdit_Button, SIGNAL(mouseReleased()), this, SLOT(bulkEdit()));
    QObject::connect(this->swap_Button, SIGNAL(mouseReleased()), this, SLOT(swap_pre()));
    QObject::connect(this->temp1_Button, SIGNAL(mouseReleased()), this, SLOT(temp1()));
    QObject::connect(this->temp2_Button, SIGNAL(mouseReleased()), this, SLOT(temp2()));
    QObject::connect(this->temp3_Button, SIGNAL(mouseReleased()), this, SLOT(temp3()));
    QObject::connect(this->temp4_Button, SIGNAL(mouseReleased()), this, SLOT(temp4()));
    QObject::connect(this->temp5_Button, SIGNAL(mouseReleased()), this, SLOT(temp5()));

    QObject::connect(this->assign1_Button, SIGNAL(mouseReleased()), this, SLOT(assign1_paste()));
    QObject::connect(this->assign2_Button, SIGNAL(mouseReleased()), this, SLOT(assign2_paste()));
    QObject::connect(this->assign3_Button, SIGNAL(mouseReleased()), this, SLOT(assign3_paste()));
    QObject::connect(this->assign4_Button, SIGNAL(mouseReleased()), this, SLOT(assign4_paste()));
    QObject::connect(this->assign5_Button, SIGNAL(mouseReleased()), this, SLOT(assign5_paste()));
    QObject::connect(this->assign6_Button, SIGNAL(mouseReleased()), this, SLOT(assign6_paste()));
    QObject::connect(this->assign7_Button, SIGNAL(mouseReleased()), this, SLOT(assign7_paste()));
    QObject::connect(this->assign8_Button, SIGNAL(mouseReleased()), this, SLOT(assign8_paste()));

    QObject::connect(this->closeButton, SIGNAL(mouseReleased()), this, SLOT(hide()));

    QObject::connect(this, SIGNAL( closeWindow() ), this, SLOT(hide()));

    QObject::connect(this, SIGNAL( dialogUpdateSignal() ), this, SLOT( pageUpdateSignal() ));

    //QObject::connect(this, SIGNAL( updateSignal() ), this, SLOT( pageUpdateSignal() ));

    QObject::connect(this->pageComboBox, SIGNAL(activated(int)), this, SLOT(valueChanged(int)));

    this->pageIndex = 0;
}

void editWindow::paintEvent(QPaintEvent *)
{
    Preferences *preferences = Preferences::Instance();
    bool ok;
    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);

    QRectF target(0.0, 0.0, image.width()*ratio, image.height()*ratio);
    QRectF source(0.0, 0.0, image.width(), image.height());

    QPainter painter(this);
    painter.drawPixmap(target, image, source);
}

editWindow::~editWindow()
{

}

void editWindow::setLSB(QString hex1, QString hex2)
{
    this->hex1 = hex1;
    this->hex2 = hex2;
}

void editWindow::setWindow(QString title)
{
    this->title->setText(title);
    this->pagesWidget->setCurrentIndex(0);
}

QString editWindow::getTitle()
{
    return this->title->text();
}

void editWindow::addPage(QString hex1, QString hex2, QString hex3, QString hex4, QString area)
{
    this->hex1 = hex1;
    this->hex2 = hex2;
    this->hex3 = hex3;
    this->area = area;

    if (area.isEmpty()){this->area = "Structure";};
    this->tempPage->setGridLayout();
    this->editPages.append(this->tempPage);
    this->pagesWidget->addWidget(editPages.last());
    this->pages = this->pagesWidget->count();

    QObject::connect(this, SIGNAL( dialogUpdateSignal() ),
                     editPages.last(), SIGNAL( dialogUpdateSignal() ));

    QObject::connect(editPages.last(), SIGNAL( updateSignal() ),
                     this, SIGNAL( updateSignal() ));

    //bool assign_check = false;
    //if(this->area == "Structure" ) {assign_check = true; };

    if (this->area != "Structure" || this->temp_hex1.isEmpty() || this->temp_hex1.contains("void") || this->position == 0)
    {
        this->bulkEdit_Button->hide();
        this->temp1_Button->hide();
        this->temp2_Button->hide();
        this->temp3_Button->hide();
        this->temp4_Button->hide();
        this->temp5_Button->hide();
    };
    if (this->area != "Structure" || (this->position != 182 && this->position != 304))
    {
        this->swap_Button->hide();
    };

    if ((this->temp_hex1 != "08" && this->temp_hex1 != "09" && this->temp_hex1 != "0A") || this->position < 2374 || this->position > 2874)
    {
        this->assign1_Button->hide();
        this->assign2_Button->hide();
        this->assign3_Button->hide();
        this->assign4_Button->hide();
        this->assign5_Button->hide();
        this->assign6_Button->hide();
        this->assign7_Button->hide();
        this->assign8_Button->hide();
    };
    if(this->position == 2374) {this->assign1_Button->hide(); };
    if(this->position == 2438) {this->assign2_Button->hide(); };
    if(this->position == 2502) {this->assign3_Button->hide(); };
    if(this->position == 2592) {this->assign4_Button->hide(); };
    if(this->position == 2656) {this->assign5_Button->hide(); };
    if(this->position == 2720) {this->assign6_Button->hide(); };
    if(this->position == 2784) {this->assign7_Button->hide(); };
    if(this->position == 2874) {this->assign8_Button->hide(); };

    if(hex1 != "void" && hex2 != "void" && hex3 != "void")
    {
        MidiTable *midiTable = MidiTable::Instance();
        Midi items = midiTable->getMidiMap(this->area, hex1, hex2, hex3);

        int itemsCount;
        if(hex4 == "void")
        {
            itemsCount = this->pagesWidget->count() - 1;
        }
        else
        {
            bool ok;
            itemsCount = hex4.toInt(&ok, 16);
        };

        QString item;
        QString desc = items.level.at(itemsCount).desc;
        QString customdesc = items.level.at(itemsCount).customdesc;
        if(!customdesc.isEmpty())
        {
            item = customdesc;
        }
        else
        {
            item = desc;
        };


        this->pageComboBox->addItem(item);
        this->tempPage = new editPage;

        this->pageComboBox->setMaxVisibleItems(this->pages);

        if(this->pages > 1)
        {
            this->pageComboBox->setVisible(true);
            this->comboBoxLabel->setVisible(true);
        };
    };
}

void editWindow::valueChanged(int index)
{
    if(hex1 != "void" && hex2 != "void")
    {
       this->pageIndex = index;
    };
}

void editWindow::pageUpdateSignal()
{
        if(this->pages > 1 && hex1 != "void" && hex2 != "void")
        {
                int index = this->pageIndex;
                this->pageComboBox->setCurrentIndex(index);
                this->pagesWidget->setCurrentIndex(index);
        };
    emit updateSignal();
}

editPage* editWindow::page()
{
    return this->tempPage;
}

void editWindow::closeEvent(QCloseEvent* ce)
{
    ce->accept();
}

void editWindow::hideWindow()
{
    QApplication::beep();
    emit hide();
}

void editWindow::bulkEdit()
{
    SysxIO *sysxIO = SysxIO::Instance();
    if (sysxIO->isConnected())
    {
        bulkEditDialog *editDialog = new bulkEditDialog(this->position, this->length, this->temp_hex1, this->temp_hex3);
        editDialog->exec();
        editDialog->deleteLater();
    }
    else
    {
        QString snork = tr("Ensure connection is active and retry");
        QMessageBox *msgBox = new QMessageBox();
        msgBox->setWindowTitle(deviceType + tr(" not connected !!"));
        msgBox->setIcon(QMessageBox::Information);
        msgBox->setText(snork);
        msgBox->show();
        QTimer::singleShot(3000, msgBox, SLOT(deleteLater()));
    };
}

void editWindow::temp1()
{
    SysxIO *sysxIO = SysxIO::Instance();
    if (!sysxIO->temp1_sysxMsg.isEmpty() && !temp_hex1.isEmpty() && !temp_hex1.contains("void") )
    {
        QString temp = sysxIO->temp1_sysxMsg.mid(this->position, this->length);
        QList< QList<QString> > patchData = sysxIO->getFileSource().hex; // Get the loaded patch data.
        QString addr1 = tempBulkWrite;  // temp address
        QString addr2 = QString::number(0, 16).toUpper();
        QString sysxMsg;
        for(int i=0;i<patchData.size();++i)
        {
            QList<QString> data = patchData.at(i);
            for(int x=0;x<data.size();++x)
            {
                QString hex;
                if(x == sysxAddressOffset)
                { hex = addr1; }
                else if(x == sysxAddressOffset + 1)
                {	hex = addr2; }
                else
                {	hex = data.at(x);	};
                if (hex.length() < 2) hex.prepend("0");
                sysxMsg.append(hex);
            };
        };
        sysxMsg = sysxMsg.replace(this->position, this->length, temp);
        sysxIO->setFileSource("Structure", this->temp_hex1, "00", this->temp_hex3, temp);
        sysxIO->setFileSource("Structure", sysxMsg );
        emit dialogUpdateSignal();
    }
    else
    {
        QApplication::beep();
        sysxIO->emitStatusdBugMessage(tr("patch must be copied to clipboard first"));
    };
}

void editWindow::temp2()
{
    SysxIO *sysxIO = SysxIO::Instance();
    if (!sysxIO->temp2_sysxMsg.isEmpty() && !temp_hex1.isEmpty() && !temp_hex1.contains("void")  )
    {
        QString temp = sysxIO->temp2_sysxMsg.mid(this->position, this->length);
        QList< QList<QString> > patchData = sysxIO->getFileSource().hex; // Get the loaded patch data.
        QString addr1 = tempBulkWrite;  // temp address
        QString addr2 = QString::number(0, 16).toUpper();
        QString sysxMsg;
        for(int i=0;i<patchData.size();++i)
        {
            QList<QString> data = patchData.at(i);
            for(int x=0;x<data.size();++x)
            {
                QString hex;
                if(x == sysxAddressOffset)
                { hex = addr1; }
                else if(x == sysxAddressOffset + 1)
                {	hex = addr2; }
                else
                {	hex = data.at(x);	};
                if (hex.length() < 2) hex.prepend("0");
                sysxMsg.append(hex);
            };
        };
        sysxMsg = sysxMsg.replace(this->position, this->length, temp);
        sysxIO->setFileSource("Structure", this->temp_hex1, "00", this->temp_hex3, temp);
        sysxIO->setFileSource("Structure", sysxMsg );
        emit dialogUpdateSignal();
    }
    else
    {
        QApplication::beep();
        sysxIO->emitStatusdBugMessage(tr("patch must be copied to clipboard first"));
    };
}

void editWindow::temp3()
{
    SysxIO *sysxIO = SysxIO::Instance();
    if (!sysxIO->temp3_sysxMsg.isEmpty() && !temp_hex1.isEmpty() && !temp_hex1.contains("void")  )
    {
        QString temp = sysxIO->temp3_sysxMsg.mid(this->position, this->length);
        QList< QList<QString> > patchData = sysxIO->getFileSource().hex; // Get the loaded patch data.
        QString addr1 = tempBulkWrite;  // temp address
        QString addr2 = QString::number(0, 16).toUpper();
        QString sysxMsg;
        for(int i=0;i<patchData.size();++i)
        {
            QList<QString> data = patchData.at(i);
            for(int x=0;x<data.size();++x)
            {
                QString hex;
                if(x == sysxAddressOffset)
                { hex = addr1; }
                else if(x == sysxAddressOffset + 1)
                {	hex = addr2; }
                else
                {	hex = data.at(x);	};
                if (hex.length() < 2) hex.prepend("0");
                sysxMsg.append(hex);
            };
        };
        sysxMsg = sysxMsg.replace(this->position, this->length, temp);
        sysxIO->setFileSource("Structure", this->temp_hex1, "00", this->temp_hex3, temp);
        sysxIO->setFileSource("Structure", sysxMsg );
        emit dialogUpdateSignal();
    }
    else
    {
        QApplication::beep();
        sysxIO->emitStatusdBugMessage(tr("patch must be copied to clipboard first"));
    };
}

void editWindow::temp4()
{
    SysxIO *sysxIO = SysxIO::Instance();
    if (!sysxIO->temp4_sysxMsg.isEmpty() && !temp_hex1.isEmpty() && !temp_hex1.contains("void")  )
    {
        QString temp = sysxIO->temp4_sysxMsg.mid(this->position, this->length);
        QList< QList<QString> > patchData = sysxIO->getFileSource().hex; // Get the loaded patch data.
        QString addr1 = tempBulkWrite;  // temp address
        QString addr2 = QString::number(0, 16).toUpper();
        QString sysxMsg;
        for(int i=0;i<patchData.size();++i)
        {
            QList<QString> data = patchData.at(i);
            for(int x=0;x<data.size();++x)
            {
                QString hex;
                if(x == sysxAddressOffset)
                { hex = addr1; }
                else if(x == sysxAddressOffset + 1)
                {	hex = addr2; }
                else
                {	hex = data.at(x);	};
                if (hex.length() < 2) hex.prepend("0");
                sysxMsg.append(hex);
            };
        };
        sysxMsg = sysxMsg.replace(this->position, this->length, temp);
        sysxIO->setFileSource("Structure", this->temp_hex1, "00", this->temp_hex3, temp);
        sysxIO->setFileSource("Structure", sysxMsg );
        emit dialogUpdateSignal();
    }
    else
    {
        QApplication::beep();
        sysxIO->emitStatusdBugMessage(tr("patch must be copied to clipboard first"));
    };
}

void editWindow::temp5()
{
    SysxIO *sysxIO = SysxIO::Instance();
    if (!sysxIO->temp5_sysxMsg.isEmpty() && !temp_hex1.isEmpty() && !temp_hex1.contains("void")  )
    {
        QString temp = sysxIO->temp5_sysxMsg.mid(this->position, this->length);
        QList< QList<QString> > patchData = sysxIO->getFileSource().hex; // Get the loaded patch data.
        QString addr1 = tempBulkWrite;  // temp address
        QString addr2 = QString::number(0, 16).toUpper();
        QString sysxMsg;
        for(int i=0;i<patchData.size();++i)
        {
            QList<QString> data = patchData.at(i);
            for(int x=0;x<data.size();++x)
            {
                QString hex;
                if(x == sysxAddressOffset)
                { hex = addr1; }
                else if(x == sysxAddressOffset + 1)
                {	hex = addr2; }
                else
                {	hex = data.at(x);	};
                if (hex.length() < 2) hex.prepend("0");
                sysxMsg.append(hex);
            };
        };
        sysxMsg = sysxMsg.replace(this->position, this->length, temp);
        sysxIO->setFileSource("Structure", this->temp_hex1, "00", this->temp_hex3, temp);
        sysxIO->setFileSource("Structure", sysxMsg );
        emit dialogUpdateSignal();
    }
    else
    {
        QApplication::beep();
        sysxIO->emitStatusdBugMessage(tr("patch must be copied to clipboard first"));
    };
}

void editWindow::swap_pre()
{

}
void editWindow::patchPos(int pos, int len, QString t_hex1, QString t_hex3)
{
    this->position = pos;
    this->length = len;
    this->temp_hex1 = t_hex1;
    this->temp_hex3 = t_hex3;
}

void editWindow::assign1_paste()
{
    start_int = 2374; length_int = 32;
    assign_paste();
}

void editWindow::assign2_paste()

{
    start_int = 2438; length_int = 32;
    assign_paste();
}

void editWindow::assign3_paste()
{
    start_int = 2502; length_int = 32;
    assign_paste();
}

void editWindow::assign4_paste()
{
    start_int = 2592; length_int = 32;
    assign_paste();
}

void editWindow::assign5_paste()
{
    start_int = 2656; length_int = 32;
    assign_paste();
}

void editWindow::assign6_paste()
{
    start_int = 2720; length_int = 32;
    assign_paste();
}

void editWindow::assign7_paste()
{
    start_int = 2784; length_int = 32;
    assign_paste();
}

void editWindow::assign8_paste()
{
    start_int = 2874; length_int = 32;
    assign_paste();
}

void editWindow::assign_paste()
{
    SysxIO *sysxIO = SysxIO::Instance();
    QList< QList<QString> > patchData = sysxIO->getFileSource().hex; // Get the loaded patch data.
    QString addr1 = tempBulkWrite;  // temp address
    QString addr2 = QString::number(0, 16).toUpper();
    QString sysxMsg;
    for(int i=0;i<patchData.size();++i)
    {
        QList<QString> data = patchData.at(i);
        for(int x=0;x<data.size();++x)
        {
            QString hex;
            if(x == sysxAddressOffset)
            { hex = addr1; }
            else if(x == sysxAddressOffset + 1)
            { hex = addr2; }
            else
            { hex = data.at(x);	};
            if (hex.length() < 2) hex.prepend("0");
            sysxMsg.append(hex);
        };
    };
    temp = sysxMsg.mid(start_int, length_int);
    sysxMsg = sysxMsg.replace(this->position, this->length, temp);
    sysxIO->setFileSource("Structure", this->temp_hex1, "00", this->temp_hex3, temp);
    sysxIO->setFileSource("Structure", sysxMsg );
    emit dialogUpdateSignal();
}


