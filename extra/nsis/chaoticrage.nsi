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

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages

Page directory
Page instfiles

;--------------------------------

; The stuff to install
Section "" ;No components page, name is not important

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File /r files\*.*
  
SectionEnd ; end the section
