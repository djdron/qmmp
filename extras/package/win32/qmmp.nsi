
;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  ;Name and file
  Name "Qt-based Multimedia Player"
  OutFile "qmmp-0.7.6-win32.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\Qt-based Multimedia Player"
  
  ;Get installation folder from registry if available
  ;InstallDirRegKey HKCU "Software\Modern UI Test" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE ".\COPYING.txt"
  #!insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"
  !insertmacro MUI_LANGUAGE "Ukrainian"
  !insertmacro MUI_LANGUAGE "Russian"

;--------------------------------
;Installer Sections

Section "-General Section"

  SetOutPath "$INSTDIR"

  File /r *.exe
  File /r *.dll
  File /r *.txt
  File /r *.png
  File /r *.bmp
  File /r *.theme
  File /r *.qm
  File /r *.conf
  
  ;ADD YOUR OWN FILES HERE...
  
  ;Store installation folder
  ;WriteRegStr HKCU "Software\Modern UI Test" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Qmmp" "DisplayName" "Qt-based Multimedia Player"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Qmmp" "UninstallString" '"$INSTDIR\Uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Qmmp" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Qmmp" "NoRepair" 1

SectionEnd

Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Qt-based Multimedia Player"
  CreateShortCut "$SMPROGRAMS\Qt-based Multimedia Player\Uninstall.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Qt-based Multimedia Player\Qmmp.lnk" "$INSTDIR\qmmp.exe" "" "$INSTDIR\qmmp.exe" 0
  
SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  ;LangString DESC_SecDummy ${LANG_ENGLISH} "A test section."

  ;Assign language strings to sections
  ;!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  ;  !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
  ;!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...

  ; Remove directories used
  RMDir /r "$SMPROGRAMS\Qt-based Multimedia Player"
  RMDir /r "$INSTDIR"

  Delete "$INSTDIR\Uninstall.exe"


  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Qmmp"
  DeleteRegKey HKLM SOFTWARE\Qmmp

SectionEnd