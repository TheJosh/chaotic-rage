; chaoticrage.nsi
;
; Installer script for Choatic Rage

;--------------------------------

; The name of the installer
Name "Chaotic Rage"

; The file to write
OutFile "chaoticrage-win32-alphaXX.exe"

; The default installation directory
InstallDir $PROGRAMFILES32\ChaoticRage

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\ChaoticRage" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages

Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "Game (required)"
  
  SectionIn RO

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File /r files\*.*
  
  ; Write the installation path into the registry
  WriteRegStr HKLM "SOFTWARE\ChaoticRage" "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChaoticRage" "DisplayName" "Chaotic Rage"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChaoticRage" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChaoticRage" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChaoticRage" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

SectionEnd ; end the section

Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Chaotic Rage"
  CreateShortCut "$SMPROGRAMS\Chaotic Rage\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Chaotic Rage\Chaotic Rage.lnk" "$INSTDIR\chaoticrage.exe" "" "$INSTDIR\chaoticrage.exe" 0
  
SectionEnd


;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ChaoticRage"
  DeleteRegKey HKLM "SOFTWARE\ChaoticRage"

  ; TODO: Remove files...

  ; Remove shortcuts, if any
  RMDir /r /REBOOTOK "$SMPROGRAMS\Chaotic Rage"

SectionEnd

