# Microsoft Developer Studio Generated NMAKE File, Based on ResLib.dsp
!IF "$(CFG)" == ""
CFG=ResLib - Win32 Debug
!MESSAGE No configuration specified. Defaulting to ResLib - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "ResLib - Win32 Release" && "$(CFG)" != "ResLib - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ResLib.mak" CFG="ResLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ResLib - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ResLib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "ResLib - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\output\ResLib.dll"


CLEAN :
	-@erase "$(INTDIR)\SunnyCat.res"
	-@erase "$(OUTDIR)\ResLib.exp"
	-@erase "$(OUTDIR)\ResLib.lib"
	-@erase "..\output\ResLib.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RESLIB_EXPORTS" /Fp"$(INTDIR)\ResLib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\SunnyCat.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ResLib.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\ResLib.pdb" /machine:I386 /out:"../output/ResLib.dll" /implib:"$(OUTDIR)\ResLib.lib" /NOENTRY 
LINK32_OBJS= \
	"$(INTDIR)\SunnyCat.res"

"..\output\ResLib.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "ResLib - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "..\output\ResLib.dll"


CLEAN :
	-@erase "$(INTDIR)\SunnyCat.res"
	-@erase "$(OUTDIR)\ResLib.exp"
	-@erase "$(OUTDIR)\ResLib.lib"
	-@erase "$(OUTDIR)\ResLib.pdb"
	-@erase "..\output\ResLib.dll"
	-@erase "..\output\ResLib.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RESLIB_EXPORTS" /Fp"$(INTDIR)\ResLib.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC=rc.exe
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\SunnyCat.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\ResLib.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\ResLib.pdb" /debug /machine:I386 /out:"../output//ResLib.dll" /implib:"$(OUTDIR)\ResLib.lib" /pdbtype:sept /NOENTRY 
LINK32_OBJS= \
	"$(INTDIR)\SunnyCat.res"

"..\output\ResLib.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("ResLib.dep")
!INCLUDE "ResLib.dep"
!ELSE 
!MESSAGE Warning: cannot find "ResLib.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "ResLib - Win32 Release" || "$(CFG)" == "ResLib - Win32 Debug"
SOURCE=..\SunnyCat.rc

!IF  "$(CFG)" == "ResLib - Win32 Release"


"$(INTDIR)\SunnyCat.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x804 /fo"$(INTDIR)\SunnyCat.res" /i "\SunnyCat" /d "NDEBUG" $(SOURCE)


!ELSEIF  "$(CFG)" == "ResLib - Win32 Debug"


"$(INTDIR)\SunnyCat.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) /l 0x804 /fo"$(INTDIR)\SunnyCat.res" /i "\SunnyCat" /d "_DEBUG" $(SOURCE)


!ENDIF 


!ENDIF 

