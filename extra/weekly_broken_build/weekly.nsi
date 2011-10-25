; chaoticrage.nsi
;
; Installer script for Choatic Rage (Weekly Broken Build)

;--------------------------------

!define /date MyTIMESTAMP "%Y-%m-%d"

; The name of the installer
Name "CR WBB ${MyTIMESTAMP}"

; The file to write
OutFile "cr-wbb-${MyTIMESTAMP}.exe"

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

;--------------------------------

; The stuff to install
Section "Weekly Broken Build: ${MyTIMESTAMP}"
  
  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File /r files\*.*

SectionEnd ; end the section

