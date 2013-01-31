; chaoticrage.nsi
;
; Installer script for Choatic Rage

;--------------------------------

; The name of the installer
Name "Chaotic Rage - exe only"

; The file to write
OutFile "chaoticrage-win32-exeonly.exe"

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
Section "EXE (required)"
  
  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File ..\msvc2010\Debug\chaoticrage.exe

SectionEnd ; end the section