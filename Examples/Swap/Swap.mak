# Microsoft Developer Studio Generated NMAKE File, Based on Swap.dsp
!IF "$(CFG)" == ""
CFG=Swap - Win32 Debug MTDLL
!MESSAGE No configuration specified. Defaulting to Swap - Win32 Debug MTDLL.
!ENDIF 

!IF "$(CFG)" != "Swap - Win32 Release" && "$(CFG)" != "Swap - Win32 Debug" && "$(CFG)" != "Swap - Win32 Release MTDLL" && "$(CFG)" != "Swap - Win32 Debug MTDLL"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Swap.mak" CFG="Swap - Win32 Debug MTDLL"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Swap - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Swap - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "Swap - Win32 Release MTDLL" (based on "Win32 (x86) Console Application")
!MESSAGE "Swap - Win32 Debug MTDLL" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Swap - Win32 Release"

OUTDIR=.\build\Release
INTDIR=.\build\Release
# Begin Custom Macros
OutDir=.\build\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\SwapValuation.exe" "$(OUTDIR)\Swap.bsc"

!ELSE 

ALL : "QuantLib - Win32 Release" "$(OUTDIR)\SwapValuation.exe" "$(OUTDIR)\Swap.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"QuantLib - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\swapvaluation.obj"
	-@erase "$(INTDIR)\swapvaluation.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Swap.bsc"
	-@erase "$(OUTDIR)\SwapValuation.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /Ze /W3 /Gi /GR /GX /O2 /Ob2 /I "..\.." /D "NDEBUG" /D "NOMINMAX" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\Swap.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Swap.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\swapvaluation.sbr"

"$(OUTDIR)\Swap.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\SwapValuation.pdb" /machine:I386 /out:"$(OUTDIR)\SwapValuation.exe" /libpath:"..\..\lib\Win32\VisualStudio\\" 
LINK32_OBJS= \
	"$(INTDIR)\swapvaluation.obj" \
	"..\..\lib\Win32\VisualStudio\QuantLib.lib"

"$(OUTDIR)\SwapValuation.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Swap - Win32 Debug"

OUTDIR=.\build\Debug
INTDIR=.\build\Debug
# Begin Custom Macros
OutDir=.\build\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\SwapValuation.exe" "$(OUTDIR)\Swap.bsc"

!ELSE 

ALL : "QuantLib - Win32 Debug" "$(OUTDIR)\SwapValuation.exe" "$(OUTDIR)\Swap.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"QuantLib - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\swapvaluation.obj"
	-@erase "$(INTDIR)\swapvaluation.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Swap.bsc"
	-@erase "$(OUTDIR)\SwapValuation.exe"
	-@erase "$(OUTDIR)\SwapValuation.ilk"
	-@erase "$(OUTDIR)\SwapValuation.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /Ze /W3 /Gm /Gi /GR /GX /ZI /Od /I "..\.." /D "_DEBUG" /D "NOMINMAX" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\Swap.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Swap.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\swapvaluation.sbr"

"$(OUTDIR)\Swap.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\SwapValuation.pdb" /debug /machine:I386 /out:"$(OUTDIR)\SwapValuation.exe" /pdbtype:sept /libpath:"..\..\lib\Win32\VisualStudio\\" 
LINK32_OBJS= \
	"$(INTDIR)\swapvaluation.obj" \
	"..\..\lib\Win32\VisualStudio\QuantLib_d.lib"

"$(OUTDIR)\SwapValuation.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Swap - Win32 Release MTDLL"

OUTDIR=.\build\ReleaseMTDLL
INTDIR=.\build\ReleaseMTDLL
# Begin Custom Macros
OutDir=.\build\ReleaseMTDLL
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\SwapValuation.exe" "$(OUTDIR)\Swap.bsc"

!ELSE 

ALL : "QuantLib - Win32 Release MTDLL" "$(OUTDIR)\SwapValuation.exe" "$(OUTDIR)\Swap.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"QuantLib - Win32 Release MTDLLCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\swapvaluation.obj"
	-@erase "$(INTDIR)\swapvaluation.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Swap.bsc"
	-@erase "$(OUTDIR)\SwapValuation.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /Ze /W3 /Gi /GR /GX /O2 /Ob2 /I "..\.." /D "NDEBUG" /D "NOMINMAX" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\Swap.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Swap.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\swapvaluation.sbr"

"$(OUTDIR)\Swap.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\SwapValuation.pdb" /machine:I386 /out:"$(OUTDIR)\SwapValuation.exe" /libpath:"..\..\lib\Win32\VisualStudio\\" 
LINK32_OBJS= \
	"$(INTDIR)\swapvaluation.obj" \
	"..\..\lib\Win32\VisualStudio\QuantLib_MTDLL.lib"

"$(OUTDIR)\SwapValuation.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Swap - Win32 Debug MTDLL"

OUTDIR=.\build\DebugMTDLL
INTDIR=.\build\DebugMTDLL
# Begin Custom Macros
OutDir=.\build\DebugMTDLL
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\SwapValuation.exe" "$(OUTDIR)\Swap.bsc"

!ELSE 

ALL : "QuantLib - Win32 Debug MTDLL" "$(OUTDIR)\SwapValuation.exe" "$(OUTDIR)\Swap.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"QuantLib - Win32 Debug MTDLLCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\swapvaluation.obj"
	-@erase "$(INTDIR)\swapvaluation.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Swap.bsc"
	-@erase "$(OUTDIR)\SwapValuation.exe"
	-@erase "$(OUTDIR)\SwapValuation.ilk"
	-@erase "$(OUTDIR)\SwapValuation.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /Ze /W3 /Gm /Gi /GR /GX /ZI /Od /I "..\.." /D "_DEBUG" /D "NOMINMAX" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /Fr"$(INTDIR)\\" /Fp"$(INTDIR)\Swap.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Swap.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\swapvaluation.sbr"

"$(OUTDIR)\Swap.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\SwapValuation.pdb" /debug /machine:I386 /out:"$(OUTDIR)\SwapValuation.exe" /pdbtype:sept /libpath:"..\..\lib\Win32\VisualStudio\\" 
LINK32_OBJS= \
	"$(INTDIR)\swapvaluation.obj" \
	"..\..\lib\Win32\VisualStudio\QuantLib_MTDLL_d.lib"

"$(OUTDIR)\SwapValuation.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Swap.dep")
!INCLUDE "Swap.dep"
!ELSE 
!MESSAGE Warning: cannot find "Swap.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Swap - Win32 Release" || "$(CFG)" == "Swap - Win32 Debug" || "$(CFG)" == "Swap - Win32 Release MTDLL" || "$(CFG)" == "Swap - Win32 Debug MTDLL"
SOURCE=.\swapvaluation.cpp

"$(INTDIR)\swapvaluation.obj"	"$(INTDIR)\swapvaluation.sbr" : $(SOURCE) "$(INTDIR)"


!IF  "$(CFG)" == "Swap - Win32 Release"

"QuantLib - Win32 Release" : 
   cd "\Branches\QuantLib"
   $(MAKE) /$(MAKEFLAGS) /F .\QuantLib.mak CFG="QuantLib - Win32 Release" 
   cd ".\Examples\Swap"

"QuantLib - Win32 ReleaseCLEAN" : 
   cd "\Branches\QuantLib"
   $(MAKE) /$(MAKEFLAGS) /F .\QuantLib.mak CFG="QuantLib - Win32 Release" RECURSE=1 CLEAN 
   cd ".\Examples\Swap"

!ELSEIF  "$(CFG)" == "Swap - Win32 Debug"

"QuantLib - Win32 Debug" : 
   cd "\Branches\QuantLib"
   $(MAKE) /$(MAKEFLAGS) /F .\QuantLib.mak CFG="QuantLib - Win32 Debug" 
   cd ".\Examples\Swap"

"QuantLib - Win32 DebugCLEAN" : 
   cd "\Branches\QuantLib"
   $(MAKE) /$(MAKEFLAGS) /F .\QuantLib.mak CFG="QuantLib - Win32 Debug" RECURSE=1 CLEAN 
   cd ".\Examples\Swap"

!ELSEIF  "$(CFG)" == "Swap - Win32 Release MTDLL"

"QuantLib - Win32 Release MTDLL" : 
   cd "\Branches\QuantLib"
   $(MAKE) /$(MAKEFLAGS) /F .\QuantLib.mak CFG="QuantLib - Win32 Release MTDLL" 
   cd ".\Examples\Swap"

"QuantLib - Win32 Release MTDLLCLEAN" : 
   cd "\Branches\QuantLib"
   $(MAKE) /$(MAKEFLAGS) /F .\QuantLib.mak CFG="QuantLib - Win32 Release MTDLL" RECURSE=1 CLEAN 
   cd ".\Examples\Swap"

!ELSEIF  "$(CFG)" == "Swap - Win32 Debug MTDLL"

"QuantLib - Win32 Debug MTDLL" : 
   cd "\Branches\QuantLib"
   $(MAKE) /$(MAKEFLAGS) /F .\QuantLib.mak CFG="QuantLib - Win32 Debug MTDLL" 
   cd ".\Examples\Swap"

"QuantLib - Win32 Debug MTDLLCLEAN" : 
   cd "\Branches\QuantLib"
   $(MAKE) /$(MAKEFLAGS) /F .\QuantLib.mak CFG="QuantLib - Win32 Debug MTDLL" RECURSE=1 CLEAN 
   cd ".\Examples\Swap"

!ENDIF 


!ENDIF 

