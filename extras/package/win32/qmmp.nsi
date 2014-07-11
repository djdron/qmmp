
;--------------------------------
;Disable description area

  !define MUI_COMPONENTSPAGE_NODESC
     
;--------------------------------
;Include Modern UI

  
  !include "MUI2.nsh"

;--------------------------------
;General

  ;Name and file
  Name "Qt-based Multimedia Player"
  OutFile "qmmp-0.8.0-win32.exe"

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
  !insertmacro MUI_PAGE_COMPONENTS
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
  
  RMDir /r "$INSTDIR"
  
  File *.txt qmmp.exe unzip.exe *.dll *.conf

  SetOutPath "$INSTDIR\plugins"
  File  /r plugins\*.dll
  
  SetOutPath "$INSTDIR\projectM"
  File /r projectM\*.inp projectM\*.ttf projectM\*.milk

  SetOutPath "$INSTDIR\themes"
  File /r themes\*.png themes\*.theme

  SetOutPath "$INSTDIR\translations"
  File /r translations\*.qm
  
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

Section "Extra skins"
  SetOutPath "$INSTDIR\skins"
  File /r skins\*.txt skins\*.png skins\*.cur
SectionEnd

Section /o "TagLib with RusXMMS patch"
  SetOutPath "$INSTDIR"
  File rusxmms\*.dll
SectionEnd



Section "Start Menu Shortcuts"

  SetShellVarContext all
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

  SetShellVarContext all

  ; Remove file associations
  NsExec::Exec '"$INSTDIR\qmmp.exe" --uninstall'

  ; Remove directories used
  RMDir /r "$SMPROGRAMS\Qt-based Multimedia Player"
  RMDir /r "$INSTDIR"

  Delete "$INSTDIR\Uninstall.exe"


  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Qmmp"
  DeleteRegKey HKLM "SOFTWARE\Qmmp"

SectionEnd
