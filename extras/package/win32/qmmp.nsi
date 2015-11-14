;Installer script for Qmmp
;Based on script written by redxii (redxii@users.sourceforge.net)
;Tested/Developed with Unicode NSIS 2.46.5

;--------------------------------
;Disable description area

  !define MUI_COMPONENTSPAGE_NODESC
  
 ;-------------------------------
 ;Defines
 
   !define QMMP_VERSION "0.9.4"
   !define QMMP_DEF_PROGS_KEY "Software\Clients\Media\Qmmp"
   !define QMMP_UNINSTALL_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\Qmmp"
   !define APP_DESCRIPTION "Qt-based multimedia player with support of many formats"
   
     
;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"
  
 ;-------------------------------
 ;Includes
 
  !include WinVer.nsh

;--------------------------------
;General

  ;Name and file
  Name "Qt-based Multimedia Player"
  OutFile "qmmp-${QMMP_VERSION}-win32.exe"

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
;Macros

!macro MacroAllExtensions _action
  !insertmacro ${_action} ".669"
  !insertmacro ${_action} ".8svx"
  !insertmacro ${_action} ".ac3"
  !insertmacro ${_action} ".aif"
  !insertmacro ${_action} ".aiff"
  !insertmacro ${_action} ".amf"
  !insertmacro ${_action} ".ams"
  !insertmacro ${_action} ".ape"
  !insertmacro ${_action} ".au"
  !insertmacro ${_action} ".ay"
  !insertmacro ${_action} ".cue"
  !insertmacro ${_action} ".dbf"
  !insertmacro ${_action} ".dbm"
  !insertmacro ${_action} ".dmfumx"
  !insertmacro ${_action} ".dsm"
  !insertmacro ${_action} ".far"
  !insertmacro ${_action} ".flac"
  !insertmacro ${_action} ".ft2"
  !insertmacro ${_action} ".gms"
  !insertmacro ${_action} ".gym"
  !insertmacro ${_action} ".hes"
  !insertmacro ${_action} ".it"
  !insertmacro ${_action} ".itgz"
  !insertmacro ${_action} ".itr"
  !insertmacro ${_action} ".itz"
  !insertmacro ${_action} ".j2b"
  !insertmacro ${_action} ".kss"
  !insertmacro ${_action} ".m4a"
  !insertmacro ${_action} ".mdbz"
  !insertmacro ${_action} ".mdgz"
  !insertmacro ${_action} ".mdl"
  !insertmacro ${_action} ".mdr"
  !insertmacro ${_action} ".mdz"
  !insertmacro ${_action} ".mod"
  !insertmacro ${_action} ".mp1"
  !insertmacro ${_action} ".mp2"
  !insertmacro ${_action} ".mp3"
  !insertmacro ${_action} ".mpc"
  !insertmacro ${_action} ".mt2"
  !insertmacro ${_action} ".mtm"
  !insertmacro ${_action} ".mus"
  !insertmacro ${_action} ".nsf"
  !insertmacro ${_action} ".nsfe"
  !insertmacro ${_action} ".oga"
  !insertmacro ${_action} ".ogg"
  !insertmacro ${_action} ".opus"
  !insertmacro ${_action} ".P00"
  !insertmacro ${_action} ".prg"
  !insertmacro ${_action} ".psm"
  !insertmacro ${_action} ".ra"
  !insertmacro ${_action} ".s3gz"
  !insertmacro ${_action} ".s3m"
  !insertmacro ${_action} ".s3r"
  !insertmacro ${_action} ".sap"
  !insertmacro ${_action} ".sf"
  !insertmacro ${_action} ".shn"
  !insertmacro ${_action} ".sid"
  !insertmacro ${_action} ".snd"
  !insertmacro ${_action} ".spc"
  !insertmacro ${_action} ".sph"
  !insertmacro ${_action} ".stm"
  !insertmacro ${_action} ".str"
  !insertmacro ${_action} ".tta"
  !insertmacro ${_action} ".ult"
  !insertmacro ${_action} ".vgm"
  !insertmacro ${_action} ".vgz"
  !insertmacro ${_action} ".voc"
  !insertmacro ${_action} ".vqf"
  !insertmacro ${_action} ".w64"
  !insertmacro ${_action} ".wav"
  !insertmacro ${_action} ".wma"
  !insertmacro ${_action} ".wv"
  !insertmacro ${_action} ".xm"
  !insertmacro ${_action} ".xmgz"
  !insertmacro ${_action} ".xmr"
  !insertmacro ${_action} ".xmz"
  !insertmacro ${_action} ".au"
!macroend

!macro WriteRegStrSupportedTypes EXT
  WriteRegStr HKLM  "${QMMP_DEF_PROGS_KEY}\Capabilities\FileAssociations" ${EXT} "QmmpFileAudio"
!macroend

;--------------------------------

;Installer functions

Function RegisterDefaultPrograms
  WriteRegStr HKCR "QmmpFileAudio\DefaultIcon" "" '"$INSTDIR\qmmp.exe",1'
  WriteRegStr HKCR "QmmpFileAudio\shell\enqueue" "" "Enqueue in Qmmp"
  WriteRegStr HKCR "QmmpFileAudio\shell\enqueue\command" "" '"$INSTDIR\qmmp.exe" --enqueue "%1"'
  WriteRegStr HKCR "QmmpFileAudio\shell\open" "FriendlyAppName" "Qt-based Multimedia Player"
  WriteRegStr HKCR "QmmpFileAudio\shell\open\command" "" '"$INSTDIR\qmmp.exe" "%1"'
  ;Modify the list of extensions added in the MacroAllExtensions macro
  WriteRegStr HKLM "${QMMP_DEF_PROGS_KEY}" "" "Qmmp"
  WriteRegStr HKLM "${QMMP_DEF_PROGS_KEY}\Capabilities" "ApplicationDescription" "${APP_DESCRIPTION}"
  WriteRegStr HKLM "${QMMP_DEF_PROGS_KEY}\Capabilities" "ApplicationName" "Qmmp"
  WriteRegStr HKLM "Software\RegisteredApplications" "Qmmp" "${QMMP_DEF_PROGS_KEY}\Capabilities"
  !insertmacro MacroAllExtensions WriteRegStrSupportedTypes
FunctionEnd


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
  WriteRegStr HKLM ${QMMP_UNINSTALL_KEY} "DisplayName" "Qt-based Multimedia Player"
  WriteRegStr HKLM ${QMMP_UNINSTALL_KEY} "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegStr HKLM ${QMMP_UNINSTALL_KEY} "DisplayIcon" "$INSTDIR\qmmp.exe,0"
  WriteRegStr HKLM ${QMMP_UNINSTALL_KEY} "DisplayVersion" "${QMMP_VERSION}"
  WriteRegStr HKLM ${QMMP_UNINSTALL_KEY} "Publisher" "Qmmp Development Team"
  WriteRegStr HKLM ${QMMP_UNINSTALL_KEY} "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM ${QMMP_UNINSTALL_KEY} "URLInfoAbout" "http://qmmp.ylsoftware.com"
  WriteRegStr HKLM ${QMMP_UNINSTALL_KEY} "URLUpdateInfo" "http://qmmp.ylsoftware.com"  
  WriteRegDWORD HKLM ${QMMP_UNINSTALL_KEY} "NoModify" 1
  WriteRegDWORD HKLM ${QMMP_UNINSTALL_KEY} "NoRepair" 1
  
  ;Default Programs Registration (Vista & later)

  ${If} ${AtLeastWinVista}
    Call RegisterDefaultPrograms
  ${EndIf}

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
  DeleteRegKey HKLM "${QMMP_DEF_PROGS_KEY}"
  DeleteRegKey HKCR "QmmpFileAudio"
  DeleteRegValue HKLM "Software\RegisteredApplications" "Qmmp"

SectionEnd
