; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "KatanaFxFloorBoard"
!define PRODUCT_VERSION "20200503"
!define PRODUCT_PUBLISHER "Colin Willcocks (gumtown)"
!define PRODUCT_WEB_SITE "https://sourceforge.net/projects/fxfloorboard"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\KATANAFxFloorBoard.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "license.txt"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\Katana_FxFloorBoard_FW4_Windows-setup"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\readme.txt"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "Katana_FxFloorBoard_FW4_Windows-setup.exe"
InstallDir "$PROGRAMFILES\KatanaFxFloorBoard"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "readme.txt"
  File "license.txt"
  File "libwinpthread-1.dll"
  File "libstdc++-6.dll"
  File "libgcc_s_dw2-1.dll"
  File "preferences.xml"
  File "KATANAFxFloorBoard.exe"
  CreateDirectory "$SMPROGRAMS\KatanaFxFloorBoard"
  CreateShortCut "$SMPROGRAMS\KatanaFxFloorBoard\KatanaFxFloorBoard.lnk" "$INSTDIR\KATANAFxFloorBoard.exe"
  CreateShortCut "$DESKTOP\KatanaFxFloorBoard.lnk" "$INSTDIR\KATANAFxFloorBoard.exe"
  SetOutPath "$INSTDIR\help"
  SetOverwrite try
  File "help\help.html"
  SetOutPath "$INSTDIR\help\help_files"
  File "help\help_files\colorschememapping.xml"
  File "help\help_files\filelist.xml"
  File "help\help_files\image001.png"
  File "help\help_files\image002.gif"
  File "help\help_files\image003.png"
  File "help\help_files\image004.gif"
  File "help\help_files\item0010.xml"
  File "help\help_files\props011.xml"
  File "help\help_files\themedata.thmx"
  SetOutPath "$INSTDIR\help"
  File "help\New Microsoft Word Document.docx"
  SetOutPath "$INSTDIR\Init Patches"
  File "Init Patches\INIT_Bad to the skull.tsl"
  File "Init Patches\INIT_Blues Wah.tsl"
  File "Init Patches\INIT_Bohemian Queen.tsl"
  File "Init Patches\INIT_Crazy railway.tsl"
  File "Init Patches\INIT_Dead or Alive.tsl"
  File "Init Patches\INIT_Foxy Woman.tsl"
  File "Init Patches\INIT_Heavy Dist.tsl"
  File "Init Patches\INIT_Hotel west coast.tsl"
  File "Init Patches\INIT_Killing MAchine.tsl"
  File "Init Patches\INIT_Panama Lick.tsl"
  File "Init Patches\INIT_Pink Wall.tsl"
  File "Init Patches\INIT_Plenty of love.tsl"
  File "Init Patches\INIT_Purple Fog.tsl"
  File "Init Patches\INIT_Sneaky - BG Drive.tsl"
  File "Init Patches\INIT_Sneaky - BG-Lead.tsl"
  File "Init Patches\INIT_Sneaky - Bognor Uber.tsl"
  File "Init Patches\INIT_Sneaky - Clean Twin.tsl"
  File "Init Patches\INIT_Sneaky - Combo Crunch.tsl"
  File "Init Patches\INIT_Sneaky - Core Metal.tsl"
  File "Init Patches\INIT_Sneaky - Deluxe Crunch.tsl"
  File "Init Patches\INIT_Sneaky - Extreme Lead.tsl"
  File "Init Patches\INIT_Sneaky - High Gain Stack.tsl"
  File "Init Patches\INIT_Sneaky - JC-120.tsl"
  File "Init Patches\INIT_Sneaky - Match Drive.tsl"
  File "Init Patches\INIT_Sneaky - MS1959 I+II.tsl"
  File "Init Patches\INIT_Sneaky - MS1959 I.tsl"
  File "Init Patches\INIT_Sneaky - Natural Clean.tsl"
  File "Init Patches\INIT_Sneaky - Orange RB.tsl"
  File "Init Patches\INIT_Sneaky - Power Drive.tsl"
  File "Init Patches\INIT_Sneaky - Pro Crunch.tsl"
  File "Init Patches\INIT_Sneaky - Rectifier Modern.tsl"
  File "Init Patches\INIT_Sneaky - Rectifier Vintage.tsl"
  File "Init Patches\INIT_Sneaky - Stack Crunch.tsl"
  File "Init Patches\INIT_Sneaky - T Amp Lead.tsl"
  File "Init Patches\INIT_Sneaky - Tweed.tsl"
  File "Init Patches\INIT_Sneaky - VO Drive.tsl"
  File "Init Patches\INIT_Sneaky - VO Lead.tsl"
  File "Init Patches\INIT_Solo Delay.tsl"
  File "Init Patches\INIT_Sweet Child.tsl"
  File "Init Patches\INIT_Wind cries.tsl"
  SetOutPath "$INSTDIR\patches"
  File "patches\@mo cleans.tsl"
  File "patches\ACDC.tsl"
  File "patches\Axe-Man K100C Patches.tsl"
  File "patches\Buterfly&Zebras.syx"
  File "patches\Current Load out. .tsl"
  File "patches\FxFloorBoard.tsl"
  File "patches\goodiez.tsl"
  File "patches\higainfromh3ll.tsl"
  File "patches\Jesse's noise.tsl"
  File "patches\Loader Collection.tsl"
  File "patches\phaser_lead.tsl"
  File "patches\rfier.tsl"
  File "patches\Shreddin' Lead.tsl"
  File "patches\SneakyAmpsBlank.tsl"
  File "patches\Tom Live Set.tsl"
  File "patches\Tone Central Collection.tsl"
SectionEnd

Section -AdditionalIcons
  SetOutPath $INSTDIR
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\KatanaFxFloorBoard\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\KatanaFxFloorBoard\Uninstall.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\KATANAFxFloorBoard.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\KATANAFxFloorBoard.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\patches\Tone Central Collection.tsl"
  Delete "$INSTDIR\patches\Tom Live Set.tsl"
  Delete "$INSTDIR\patches\SneakyAmpsBlank.tsl"
  Delete "$INSTDIR\patches\Shreddin' Lead.tsl"
  Delete "$INSTDIR\patches\rfier.tsl"
  Delete "$INSTDIR\patches\phaser_lead.tsl"
  Delete "$INSTDIR\patches\Loader Collection.tsl"
  Delete "$INSTDIR\patches\Jesse's noise.tsl"
  Delete "$INSTDIR\patches\higainfromh3ll.tsl"
  Delete "$INSTDIR\patches\goodiez.tsl"
  Delete "$INSTDIR\patches\FxFloorBoard.tsl"
  Delete "$INSTDIR\patches\Current Load out. .tsl"
  Delete "$INSTDIR\patches\Buterfly&Zebras.syx"
  Delete "$INSTDIR\patches\Axe-Man K100C Patches.tsl"
  Delete "$INSTDIR\patches\ACDC.tsl"
  Delete "$INSTDIR\patches\@mo cleans.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Wind cries.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sweet Child.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Solo Delay.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - VO Lead.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - VO Drive.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - Tweed.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - T Amp Lead.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - Stack Crunch.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - Rectifier Vintage.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - Rectifier Modern.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - Pro Crunch.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - Power Drive.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - Orange RB.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - Natural Clean.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - MS1959 I.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - MS1959 I+II.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - Match Drive.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - JC-120.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - High Gain Stack.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - Extreme Lead.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - Deluxe Crunch.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - Core Metal.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - Combo Crunch.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - Clean Twin.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - Bognor Uber.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - BG-Lead.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Sneaky - BG Drive.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Purple Fog.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Plenty of love.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Pink Wall.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Panama Lick.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Killing MAchine.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Hotel west coast.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Heavy Dist.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Foxy Woman.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Dead or Alive.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Crazy railway.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Bohemian Queen.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Blues Wah.tsl"
  Delete "$INSTDIR\Init Patches\INIT_Bad to the skull.tsl"
  Delete "$INSTDIR\help\New Microsoft Word Document.docx"
  Delete "$INSTDIR\help\help_files\themedata.thmx"
  Delete "$INSTDIR\help\help_files\props011.xml"
  Delete "$INSTDIR\help\help_files\item0010.xml"
  Delete "$INSTDIR\help\help_files\image004.gif"
  Delete "$INSTDIR\help\help_files\image003.png"
  Delete "$INSTDIR\help\help_files\image002.gif"
  Delete "$INSTDIR\help\help_files\image001.png"
  Delete "$INSTDIR\help\help_files\filelist.xml"
  Delete "$INSTDIR\help\help_files\colorschememapping.xml"
  Delete "$INSTDIR\help\help.html"
  Delete "$INSTDIR\KATANAFxFloorBoard.exe"
  Delete "$INSTDIR\libgcc_s_dw2-1.dll"
  Delete "$INSTDIR\libstdc++-6.dll"
  Delete "$INSTDIR\libwinpthread-1.dll"
  Delete "$INSTDIR\license.txt"
  Delete "$INSTDIR\readme.txt"

  Delete "$SMPROGRAMS\KatanaFxFloorBoard\Uninstall.lnk"
  Delete "$SMPROGRAMS\KatanaFxFloorBoard\Website.lnk"
  Delete "$DESKTOP\KatanaFxFloorBoard.lnk"
  Delete "$SMPROGRAMS\KatanaFxFloorBoard\KatanaFxFloorBoard.lnk"

  RMDir "$SMPROGRAMS\KatanaFxFloorBoard"
  RMDir "$INSTDIR\patches"
  RMDir "$INSTDIR\Init Patches"
  RMDir "$INSTDIR\help\help_files"
  RMDir "$INSTDIR\help"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd