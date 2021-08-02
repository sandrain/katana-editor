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
#include "mainWindow.h"
#include "Preferences.h"
#include "MidiTable.h"
#include "SysxIO.h"
#include "sysxWriter.h"
#include "customSplashScreen.h"

int main(int argc, char **argv)
{
    //qputenv("QT_SCALE_FACTOR", QByteArray("1"));
    //qputenv("Qt::AA_DisableHighDpiScaling", QByteArray("1"));
    QApplication app(argc, argv);
    app.setOrganizationName("Gumtown");
    app.setApplicationName("Katana_FxFloorBoard");
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    
#ifdef Q_OS_ANDROID
   AndroidAskReadWriteRights(); // before creating the log file

   QDir().setCurrent(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
   QDir().setCurrent("./KatanaFxFloorBoard");
#endif

#ifdef Q_OS_WIN
    QDir().setCurrent(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
#endif

#ifdef Q_OS_IOS
    QDir().setCurrent(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    QDir().setCurrent("./KatanaFxFloorBoard");
#endif

#ifdef Q_OS_MAC
    QDir().setCurrent(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    QDir().setCurrent("../Shared/KatanaFxFloorBoard");
#endif


    Preferences *preferences = Preferences::Instance(); // Load the preferences.
    QString lang = preferences->getPreferences("Language", "Locale", "select");
    bool ok;
    int choice = lang.toInt(&ok, 16);
    // Loading translation
    QTranslator translator;

    if (choice == 6)     {translator.load(":language_ch.qm");  }
    else if (choice ==5) {translator.load(":language_jp.qm"); }
    else if (choice ==4) {translator.load(":language_es.qm"); }
    else if (choice ==3) {translator.load(":language_pt.qm"); }
    else if (choice ==2) {translator.load(":language_ge.qm"); }
    else if (choice ==1) {translator.load(":language_fr.qm"); }
    else { };

    app.installTranslator(&translator);

       int windowWidth;
    if(preferences->getPreferences("Window", "Collapsed", "width").isEmpty())
    {
        windowWidth = preferences->getPreferences("Window", "Collapsed", "defaultwidth").toInt(&ok, 10);
    }
    else
    {
        windowWidth = preferences->getPreferences("Window", "Collapsed", "width").toInt(&ok, 10);
    };
    QDesktopWidget *desktop = new QDesktopWidget;
    QRect screen = desktop->availableGeometry(desktop->primaryScreen());
    int screenWidth = screen.width();                    // returns available screen width
    int screenHeight = screen.height();                  // returns available screen height
    double resolution = screenWidth;
    if(screenHeight>screenWidth) {resolution = screenHeight; };   // if is a tablet on it's side initially
    resolution = resolution/(windowWidth+15);
    QString scaleRatio = QString::number(resolution,'f',2);
    double dpi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
    dpi=(96/dpi)*(resolution); // ratio 1:1 at 96dpi
    QString dpiRatio = QString::number(dpi,'f',2);
    if(preferences->getPreferences("Window", "AutoScale", "bool")=="true")
    {
        preferences->setPreferences("Window", "Font", "ratio", dpiRatio);
        preferences->setPreferences("Window", "Scale", "ratio", scaleRatio);
        preferences->setPreferences("Window", "Position", "x", "1");
        preferences->setPreferences("Window", "Position", "y", "1");
        preferences->savePreferences();
   /* }else{
        const double scale = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);
        dpi = dpi*(scale/1.6);
        dpiRatio = QString::number(dpi,'f',2);
        preferences->setPreferences("Window", "Font", "ratio", dpiRatio);
        preferences->savePreferences();     */
    };

    const double ratio = preferences->getPreferences("Window", "Scale", "ratio").toDouble(&ok);
    const double fratio = preferences->getPreferences("Window", "Font", "ratio").toDouble(&ok);
    QFont Wfont( "Arial", 11*fratio);
    QApplication::setFont(Wfont);

    // Splash Screen setup uses subclassed QSplashScreen for message position control.
    QPixmap splashImage(":images/splash.png");
    QPixmap splashMask(":images/splashmask.png");
    splashImage = splashImage.scaled(429*ratio, 269*ratio, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    splashMask = splashMask.scaled(429*ratio, 269*ratio, Qt::KeepAspectRatio, Qt::SmoothTransformation);


    customSplashScreen *splash = new customSplashScreen(splashImage);
    splash->setMessageRect(QRect(7*ratio, 253*ratio, 415*ratio, 14*ratio), Qt::AlignCenter); // Setting the message position.

    QFont splashFont;
    splashFont.setFamily("Arial");
    splashFont.setBold(true);
    splashFont.setPixelSize(10*ratio);
    splashFont.setStretch(110);

    splash->setFont(splashFont);
    splash->setMask(splashMask);
    //splash->setWindowFlags(Qt::SplashScreen);

    if(preferences->getPreferences("Window", "Splash", "bool")=="true")
    {
        splash->show();
        splash->showStatusMessage(QObject::tr("KATANA Fx Floorboard  - Initializing.. please wait..."));
        splash->progressBar->setValue(10);
    };

    app.processEvents();

    mainWindow window;

    QObject::connect( &window, SIGNAL(closed()), &app, SLOT(quit()) );

    app.processEvents();

    splash->showStatusMessage(QObject::tr("Checking license file..."));
    splash->progressBar->setValue(25);
    if(!QFile("license.txt").exists())
    {
        QFile file(":license.txt" );
        file.copy("license.txt");
        file.close();
    };

    app.processEvents();

    splash->showStatusMessage(QObject::tr("Loading midi mapping..."));
    splash->progressBar->setValue(40);
    MidiTable *midiTable = MidiTable::Instance();
    midiTable->loadMidiMap();

    app.processEvents();

    splash->showStatusMessage(QObject::tr("Initializing main window..."));
    splash->progressBar->setValue(60);
    
        window.setWindowFlags(
                Qt::WindowTitleHint
                | Qt::WindowMinimizeButtonHint
                | Qt::WindowCloseButtonHint
                //| Qt::MSWindowsFixedSizeDialogHint
                );
    window.setWindowIcon(QIcon(":/images/windowicon.png"));

    //app.processEvents();
    QString x_str = preferences->getPreferences("Window", "Position", "x");
    QString y_str = preferences->getPreferences("Window", "Position", "y");

    int windowHeight;
    if(preferences->getPreferences("Window", "Collapsed", "bool")=="true" &&
            preferences->getPreferences("Window", "Restore", "sidepanel")=="true")
    {

        if(preferences->getPreferences("Window", "Collapsed", "width").isEmpty())
        {
            windowWidth = preferences->getPreferences("Window", "Collapsed", "defaultwidth").toInt(&ok, 10);
        }
        else
        {
            windowWidth = preferences->getPreferences("Window", "Collapsed", "width").toInt(&ok, 10);
        };
    }
    else
    {
        if(preferences->getPreferences("Window", "Size", "width").isEmpty())
        {
            windowWidth = preferences->getPreferences("Window", "Size", "minwidth").toInt(&ok, 10);
        }
        else
        {
            windowWidth = preferences->getPreferences("Window", "Size", "width").toInt(&ok, 10);
        };
    }

    app.processEvents();


    if(preferences->getPreferences("Window", "Restore", "window")=="true" && !x_str.isEmpty())
    {
        splash->showStatusMessage(QObject::tr("Restoring window position..."));
        if(preferences->getPreferences("Window", "Size", "height").isEmpty())
        {
            windowHeight = preferences->getPreferences("Window", "Size", "minheight").toInt(&ok, 10);
        }
        else
        {
            windowHeight = preferences->getPreferences("Window", "Size", "minheight").toInt(&ok, 10);
        };


        if(windowHeight>6600) { windowHeight=6600; };
        if(windowHeight<369) { windowHeight=369; };
        if(windowWidth>12300) { windowWidth=12300; };
        if(windowWidth<623) { windowWidth=623; };
        //window.resize(QSize((windowWidth*ratio), (windowHeight*ratio*2)-(22*ratio)));
        window.resize(QSize((windowWidth*ratio), (windowHeight*ratio*1.75)));
        window.move(x_str.toInt(&ok, 10), y_str.toInt(&ok, 10));
    }
    else
    {
        splash->showStatusMessage(QObject::tr("Centering main window..."));
        splash->progressBar->setValue(80);
        QDesktopWidget *desktop = new QDesktopWidget;
        QRect screen = desktop->availableGeometry(desktop->primaryScreen());
        int screenWidth = screen.width();                    // returns available screen width
        int screenHeight = screen.height();                  // returns available screen height

        windowHeight = preferences->getPreferences("Window", "Size", "minheight").toInt(&ok, 10);

        int x = (screenWidth - windowWidth) / 2;
        int y = (screenHeight - windowHeight) / 2;
        window.setGeometry(x, y, window.width(), window.height());
    };

    app.processEvents();

    splash->showStatusMessage(QObject::tr("Finished Initializing..."));
    splash->progressBar->setValue(100);
    window.show();
    splash->finish(&window);

    return app.exec();
}
