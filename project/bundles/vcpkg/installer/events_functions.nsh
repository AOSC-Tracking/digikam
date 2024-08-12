;; ============================================================
 ;
 ; This file is a part of digiKam project
 ; https://www.digikam.org
 ;
 ; Date        : 2005-01-01
 ; Description : Functions to catch NSIS events.
 ;               Note: NSIS >= 3 is required to be compatible with Windows 10.
 ;
 ; SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 ;
 ; SPDX-License-Identifier: GPL-2.0-or-later
 ;
 ; ============================================================ ;;

!ifndef EVENTS_FUNCTIONS_INCLUDED
!define EVENTS_FUNCTIONS_INCLUDED

Function .onInit

    ;Do not permit to install 64 bits to 32 bits.

    ${IfNot} ${RunningX64}

        MessageBox MB_OK|MB_ICONSTOP|MB_TOPMOST|MB_SETFOREGROUND "This is the 64 bits installer, and it cannot be installed to a 32 bits system."
        SetErrorLevel 2
        Quit

    ${EndIf}

    ;Default installation folder depending of target architecture.

    ${If} $InstDir == "" ; Don't override setup.exe /D=c:\custom\dir
        StrCpy $InstDir "$PROGRAMFILES64\${MY_PRODUCT}"
    ${EndIf}

    Push $0
    UserInfo::GetAccountType
    Pop $0

    ${If} $0 != "admin"

        ;Require admin rights on NT4+

        MessageBox MB_OK|MB_ICONSTOP|MB_TOPMOST|MB_SETFOREGROUND "Administrator privileges required!$\r$\n$\r$\nPlease restart the installer using an administrator account."
        SetErrorLevel 740 ; ERROR_ELEVATION_REQUIRED
        Quit

    ${EndIf}

    Pop $0

    Push $R0
    Push $R1
    Push $R2

    checkUninstallRequired:

    ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MY_PRODUCT}" "UninstallString"
    ${StrRep} $R0 $R0 '"' "" ; Remove double-quotes so Delete and RMDir work properly and we can extract the path
    StrCmp $R0 "" done

    ${IfNot} ${FileExists} $R0

        DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MY_PRODUCT}"
        Goto checkUninstallRequired

    ${EndIf}

    ;Get path

    ${StrStrAdv} $R1 $R0 "\" "<" "<" "0" "0" "0"

    ReadRegStr $R2 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${MY_PRODUCT}" "DisplayName" ; DisplayName contains version

    ;TODO: need to internationalize string (see VLC / clementine / etc)

    MessageBox MB_YESNO|MB_ICONEXCLAMATION|MB_TOPMOST|MB_SETFOREGROUND "$R2 is currently installed but only a single instance of ${MY_PRODUCT} can be installed at any time.$\r$\n$\r$\n\
        Note: only the application binary files will be removed from your system at this stage. Your collections and database files kept untouched.$\r$\n$\r$\n\
        Important: database schemas can be upgraded between major releases, so it's recommended to backup all database files before to start a new digiKam session after this install.$\r$\n$\r$\n\
        Do you want to uninstall the current instance of ${MY_PRODUCT} and continue installing ${MY_PRODUCT} ${VERSION}?" /SD IDYES IDNO noInstall

    ;Run the uninstaller

    ClearErrors

    IfSilent 0 notSilent

    ExecWait '"$R0" /S _?=$R1' ; Do not copy the uninstaller to a temp file
    Goto uninstDone

    notSilent:

        ExecWait '"$R0" _?=$R1' ; Do not copy the uninstaller to a temp file

    uninstDone:

        IfErrors checkUninstallRequired
        Delete "$R0" ; If uninstall successful, remove uninstaller
        RMDir "$R1" ; remove previous install directory
        Goto checkUninstallRequired

    noInstall:
        Abort

    done:
        Pop $R2
        Pop $R1
        Pop $R0

FunctionEnd

;-------------------------------------------

Function .onInstSuccess

    nsExec::ExecToLog /TIMEOUT=2000 '"$instdir\kbuildsycoca6.exe --noincremental"'

FunctionEnd

;-------------------------------------------------------------------------------------

Function functionFinishRun

    ; Execute the file with non-elevated rights.

    Exec '"$WINDIR\explorer.exe" "$instdir\digikam.exe"'

FunctionEnd

!endif ;EVENTS_FUNCTIONS_INCLUDED
