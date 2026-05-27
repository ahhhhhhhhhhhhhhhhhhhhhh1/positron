OutFile "Positron Installer.exe"
InstallDir "$PROGRAMFILES\positron"
!include "FileFunc.nsh"

Var NoGUI

Function .onInit
  ${GetParameters} $R0
  ${GetOptions} $R0 "/NOGUI" $R1
  IfErrors done
    StrCpy $NoGUI 1
    SetSilent silent
  done:
FunctionEnd

Section "Main"

    SetOutPath $INSTDIR

    File "positron.exe"
    SetOutPath "$INSTDIR\chrome"
    File /r "chrome\*"

    WriteUninstaller "$INSTDIR\uninstall.exe"

    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Positron" \
    "DisplayName" "Positron"

    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Positron" \
    "UninstallString" "$INSTDIR\uninstall.exe"
SectionEnd

Section "Uninstall"
    Delete "$INSTDIR\Positron uninstaller.exe"
    RMDir /r "$INSTDIR"

    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Positron"
SectionEnd