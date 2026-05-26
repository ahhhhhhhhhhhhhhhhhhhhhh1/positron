OutFile "Positron Installer.exe"
InstallDir "$PROGRAMFILES\positron"

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