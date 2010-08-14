!include MUI2.nsh

!define PRODUCT_NAME 'Arduino IDE'
!define PRODUCT_ORGANIZATION 'MuPuF.org'

OutFile "setup.exe"
Name '${PRODUCT_NAME}'

!insertmacro MUI_PAGE_WELCOME

!insertmacro MUI_PAGE_DIRECTORY

; Installer
InstallDir '$PROGRAMFILES\${PRODUCT_NAME}'
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

; Uninstaller
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

Section Install
        SetOutPath '$INSTDIR'
        WriteUninstaller 'uninstall.exe'
        File /r release/*
        CreateDirectory '$SMPROGRAMS\${PRODUCT_NAME}'
        CreateShortcut '$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk' '$INSTDIR\arduino-ide.exe'
        CreateShortcut '$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk' '$INSTDIR\uninstall.exe'
        WriteRegStr HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}' 'DisplayName' '${PRODUCT_NAME}'
        WriteRegStr HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}' 'Publisher' '${PRODUCT_ORGANIZATION}'
        WriteRegStr HKLM 'Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}' 'UninstallString' '"$INSTDIR\uninstall.exe"'
SectionEnd

Section Uninstall
        RMDir /r '$INSTDIR'
        Delete '$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk'
        Delete '$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk'
        RMDir '$SMPROGRAMS\${PRODUCT_NAME}'
        DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
SectionEnd
