
# to be used with NSIS 1.44 and up
#
# usage:
#       makensis /DLIGHT QuantLib.nsi
# OR
#       makensis QuantLib.nsi


# $Id$


# HEADER CONFIGURATION COMMANDS
!ifdef LIGHT
    Name "QuantLib Light"
    Caption "QuantLib Light - Setup"
    #do not change the name below
    OutFile "..\QuantLib-0.2.1a2-light-inst.exe"
    ComponentText "This will install QuantLib Light on your computer.$\n A more complete version including documentation, examples, source code, etc. can be downloaded from http://quantlib.org"
!else
    Name "QuantLib"
    Caption "QuantLib - Setup"
    #do not change the name below
    OutFile "..\QuantLib-0.2.1a2-full-inst.exe"

    InstType "Full (w/ Source Code)"
    InstType Typical
    InstType Minimal

    ComponentText "This will install QuantLib on your computer"
!endif

SilentInstall normal
CRCCheck on
UninstallText "This will uninstall QuantLib. Hit next to continue."
UninstallExeName "QuantLibUninstall.exe"
LicenseText "You must read the following license before installing:"
LicenseData License.txt
DirShow show
DirText "Please select a location to install QuantLib (or use the default):"
InstallDir $PROGRAMFILES\QuantLib
InstallDirRegKey HKEY_LOCAL_MACHINE SOFTWARE\QuantLib "Install_Dir"
AutoCloseWindow false
ShowInstDetails hide
SetDateSave on

# INSTALLATION EXECUTION COMMANDS


Section "-QuantLib"
SectionIn 1 2 3
# this directory must be created first, or the CreateShortCut will not work
    CreateDirectory "$SMPROGRAMS\QuantLib"
    SetOutPath $INSTDIR
    File "Authors.txt"
    File "Contributors.txt"
    File "History.txt"
    File "LICENSE.txt"
    File "News.txt"
    File "README.txt"
    File "TODO.txt"

    SetOutPath $INSTDIR\lib\Win32\VisualStudio
    File "lib\Win32\VisualStudio\QuantLib.lib"

    SetOutPath $INSTDIR\lib\Win32\Borland
    File "lib\Win32\Borland\QuantLib.lib"

    SetOutPath  $INSTDIR\ql
    File /r "ql\*.hpp"

    SetOutPath  $INSTDIR\ql\Calendars
    File /r "ql\Calendars\*.hpp"

    SetOutPath  $INSTDIR\ql\CashFlows
    File /r "ql\CashFlows\*.hpp"

    SetOutPath  $INSTDIR\ql\DayCounters
    File /r "ql\DayCounters\*.hpp"

    SetOutPath  $INSTDIR\ql\FiniteDifferences
    File /r "ql\FiniteDifferences\*.hpp"

    SetOutPath  $INSTDIR\ql\Indexes
    File /r "ql\Indexes\*.hpp"

    SetOutPath  $INSTDIR\ql\Instruments
    File /r "ql\Instruments\*.hpp"

    SetOutPath  $INSTDIR\ql\Math
    File /r "ql\Math\*.hpp"

    SetOutPath  $INSTDIR\ql\MonteCarlo
    File /r "ql\MonteCarlo\*.hpp"

    SetOutPath  $INSTDIR\ql\Patterns
    File /r "ql\Patterns\*.hpp"

    SetOutPath  $INSTDIR\ql\Pricers
    File /r "ql\Pricers\*.hpp"

    SetOutPath  $INSTDIR\ql\Solvers1D
    File /r "ql\Solvers1D\*.hpp"

    SetOutPath  $INSTDIR\ql\TermStructures
    File /r "ql\TermStructures\*.hpp"

    SetOutPath  $INSTDIR\ql\Utilities
    File /r "ql\Utilities\*.hpp"

    WriteRegStr HKEY_LOCAL_MACHINE \
                "Software\Microsoft\Windows\CurrentVersion\Uninstall\QuantLib" \
                "DisplayName" "QuantLib (remove only)"
    WriteRegStr HKEY_LOCAL_MACHINE \
                "Software\Microsoft\Windows\CurrentVersion\Uninstall\QuantLib" \
                "UninstallString" '"QuantLibUninstall.exe"'
    WriteRegStr HKEY_LOCAL_MACHINE \
                "SOFTWARE\QuantLib" \
                "Install_Dir" "$INSTDIR"
    WriteRegStr HKEY_CURRENT_USER \
                "Environment" \
                "QL_DIR" "$INSTDIR"
    CreateShortCut "$SMPROGRAMS\QuantLib\Uninstall QuantLib.lnk" \
                   "$INSTDIR\QuantLibUninstall.exe" \
                   "" "$INSTDIR\QuantLibUninstall.exe" 0
    CreateShortCut "$SMPROGRAMS\QuantLib\README.txt.lnk" \
                   "$INSTDIR\README.txt"
    CreateShortCut "$SMPROGRAMS\QuantLib\LICENSE.txt.lnk" \
                   "$INSTDIR\LICENSE.txt"
    CreateShortCut "$SMPROGRAMS\QuantLib\What's new.lnk" \
                   "$INSTDIR\News.txt"
SectionEnd


!ifndef LIGHT

#Function .onInit
#  SetOutPath $TEMP
#  File /oname=spltmp.dat "Docs\images\QL-largish.bmp"
#  ReadRegStr $1 HKLM SOFTWARE\NSIS ""
#  StrCpy $2 "$1\splash.exe"
#  File /oname=spltmp.exe $2
#  File /oname=spltmp.exe "E:\Program Files\NSIS\splash.exe"
#  ExecWait '"$TEMP\spltmp.exe" 2000 $HWNDPARENT spltmp.dat'
#  Delete $TEMP\spltmp.exe
#  Delete $TEMP\spltmp.dat
#FunctionEnd

#it doesn't work
#Function .onInstSuccess
#  MessageBox MB_YESNO|MB_ICONQUESTION \
#             "Setup has completed. View Readme.txt now?" \
#             IDNO NoReadme
#    ExecShell open '$INSTDIR\Readme.txt'
#  NoReadme:
#FunctionEnd

Section "Source Code"
SectionIn 1
  SetOutPath $INSTDIR
  File ChangeLog.txt
  File makefile.mak
  File QuantLib.dsp
  File QuantLib.dsw
  File QuantLib.mak
  File QuantLib.nsi

  SetOutPath  $INSTDIR\ql
  File /r "ql\*.cpp"
  File /r "ql\makefile.mak"

  SetOutPath  $INSTDIR\ql\Calendars
  File /r "ql\Calendars\*.cpp"
  File /r "ql\Calendars\makefile.mak"

  SetOutPath  $INSTDIR\ql\CashFlows
  File /r "ql\CashFlows\*.cpp"
  File /r "ql\CashFlows\makefile.mak"

  SetOutPath  $INSTDIR\ql\DayCounters
  File /r "ql\DayCounters\*.cpp"
  File /r "ql\DayCounters\makefile.mak"

  SetOutPath  $INSTDIR\ql\FiniteDifferences
  File /r "ql\FiniteDifferences\*.cpp"
  File /r "ql\FiniteDifferences\makefile.mak"

  SetOutPath  $INSTDIR\ql\Indexes
  File /r "ql\Indexes\*.cpp"
  File /r "ql\Indexes\makefile.mak"

  SetOutPath  $INSTDIR\ql\Instruments
  File /r "ql\Instruments\*.cpp"
  File /r "ql\Instruments\makefile.mak"

  SetOutPath  $INSTDIR\ql\Math
  File /r "ql\Math\*.cpp"
  File /r "ql\Math\makefile.mak"

  SetOutPath  $INSTDIR\ql\MonteCarlo
  File /r "ql\MonteCarlo\*.cpp"
  File /r "ql\MonteCarlo\makefile.mak"

  SetOutPath  $INSTDIR\ql\Pricers
  File /r "ql\Pricers\*.cpp"
  File /r "ql\Pricers\makefile.mak"

  SetOutPath  $INSTDIR\ql\Solvers1D
  File /r "ql\Solvers1D\*.cpp"
  File /r "ql\Solvers1D\makefile.mak"

  SetOutPath  $INSTDIR\ql\TermStructures
  File /r "ql\TermStructures\*.cpp"
  File /r "ql\TermStructures\makefile.mak"

  CreateShortCut "$SMPROGRAMS\QuantLib\QuantLib project workspace.lnk" \
                 "$INSTDIR\QuantLib.dsw"

SectionEnd

Section "Examples"
SectionIn 1 2
    SetOutPath $INSTDIR\Examples
    File /r "Examples\*.txt"
    File /r "Examples\*.mak"
    File /r "Examples\*.dsw"

    SetOutPath $INSTDIR\Examples\DiscreteHedging
    File /r "Examples\DiscreteHedging\*.cpp"
    File /r "Examples\DiscreteHedging\*.dsp"
    File /r "Examples\DiscreteHedging\*.txt"
    File /r "Examples\DiscreteHedging\*.mak"

    SetOutPath $INSTDIR\Examples\Parities
    File /r "Examples\Parities\*.cpp"
    File /r "Examples\Parities\*.dsp"
    File /r "Examples\Parities\*.txt"
    File /r "Examples\Parities\*.mak"

    SetOutPath $INSTDIR\Examples\Swap
    File /r "Examples\Swap\*.cpp"
    File /r "Examples\Swap\*.dsp"
    File /r "Examples\Swap\*.txt"
    File /r "Examples\Swap\*.mak"

    IfFileExists $SMPROGRAMS\QuantLib 0 NoSourceShortCuts
        CreateShortCut "$SMPROGRAMS\QuantLib\Examples workspace.lnk" \
                       "$INSTDIR\Examples\Examples.dsw"
    NoSourceShortCuts:
SectionEnd




SectionDivider

Section "HTML documentation"
SectionIn 1 2
  SetOutPath "$INSTDIR\Docs\html"
  File "Docs\html\*.*"
  CreateShortCut "$INSTDIR\Docs\refman.html.lnk" "$INSTDIR\Docs\html\index.html"
  CreateShortCut "$SMPROGRAMS\QuantLib\Documentation (HTML).lnk" \
                 "$INSTDIR\Docs\html\index.html"
SectionEnd

Section "PDF documentation"
SectionIn 1
  SetOutPath "$INSTDIR\Docs"
  File "Docs\latex\*refman.pdf"
  CreateShortCut "$SMPROGRAMS\QuantLib\Documentation (PDF).lnk" \
                 "$INSTDIR\Docs\refman.pdf"
SectionEnd

Section "PS documentation"
SectionIn 1
  SetOutPath "$INSTDIR\Docs"
  File "Docs\latex\*refman.ps"
  CreateShortCut "$SMPROGRAMS\QuantLib\Documentation (PS).lnk" \
                 "$INSTDIR\Docs\refman.ps"
SectionEnd


Section "Start Menu Group"
SectionIn 1 2 3
  SetOutPath $SMPROGRAMS\QuantLib

#it doesn't work
#  CreateShortCut "$SMPROGRAMS\QuantLib\QuantLib Home Page.lnk" \
#                 "http://quantlib.org/index.html"
#this works
  WriteINIStr "$SMPROGRAMS\QuantLib\QuantLib Home Page.url" \
              "InternetShortcut" "URL" "http://quantlib.org/"

  CreateShortCut "$SMPROGRAMS\QuantLib\QuantLib Directory.lnk" \
                 "$INSTDIR"
SectionEnd

!endif




Section "Uninstall"
    DeleteRegKey HKEY_LOCAL_MACHINE \
        "Software\Microsoft\Windows\CurrentVersion\Uninstall\QuantLib"
    DeleteRegKey HKEY_LOCAL_MACHINE SOFTWARE\QuantLib
    DeleteRegValue HKEY_CURRENT_USER  "Environment" "QL_DIR"
    Delete "$SMPROGRAMS\QuantLib\*.*"
    RMDir "$SMPROGRAMS\QuantLib"
    RMDir /r "$INSTDIR\Examples"
    RMDir /r "$INSTDIR\Docs"
    RMDir /r "$INSTDIR\ql"
    RMDir /r "$INSTDIR\lib"
    RMDir /r "$INSTDIR"
SectionEnd
