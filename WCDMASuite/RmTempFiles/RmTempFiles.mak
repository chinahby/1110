# Microsoft Developer Studio Generated NMAKE File, Based on RmTempFiles.dsp
!IF "$(CFG)" == ""
CFG=RmTempFiles - Win32 Debug
!MESSAGE No configuration specified. Defaulting to RmTempFiles - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "RmTempFiles - Win32 Release" && "$(CFG)" != "RmTempFiles - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RmTempFiles.mak" CFG="RmTempFiles - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RmTempFiles - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "RmTempFiles - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "RmTempFiles - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\output\RmTemp.exe"


CLEAN :
	-@erase "$(INTDIR)\RmTempFiles.obj"
	-@erase "$(INTDIR)\RmTempFiles.pch"
	-@erase "$(INTDIR)\RmTempFiles.res"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\output\RmTemp.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\RmTempFiles.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\RmTempFiles.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\RmTempFiles.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=../Porting/Release/Porting.lib rasapi32.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\RmTemp.pdb" /machine:I386 /out:"../output/RmTemp.exe" 
LINK32_OBJS= \
	"$(INTDIR)\RmTempFiles.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\RmTempFiles.res"

"..\output\RmTemp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "RmTempFiles - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\output\RmTemp.exe" "$(OUTDIR)\RmTempFiles.bsc"


CLEAN :
	-@erase "$(INTDIR)\RmTempFiles.obj"
	-@erase "$(INTDIR)\RmTempFiles.pch"
	-@erase "$(INTDIR)\RmTempFiles.res"
	-@erase "$(INTDIR)\RmTempFiles.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\RmTemp.pdb"
	-@erase "$(OUTDIR)\RmTempFiles.bsc"
	-@erase "..\output\RmTemp.exe"
	-@erase "..\output\RmTemp.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\RmTempFiles.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

RSC=rc.exe
RSC_PROJ=/l 0x804 /fo"$(INTDIR)\RmTempFiles.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\RmTempFiles.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\RmTempFiles.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\RmTempFiles.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=../Porting/Debug/Porting.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib rasapi32.lib /nologo /subsystem:console /incremental:yes /pdb:"$(OUTDIR)\RmTemp.pdb" /debug /machine:I386 /out:"../output/RmTemp.exe" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\RmTempFiles.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\RmTempFiles.res"

"..\output\RmTemp.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("RmTempFiles.dep")
!INCLUDE "RmTempFiles.dep"
!ELSE 
!MESSAGE Warning: cannot find "RmTempFiles.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "RmTempFiles - Win32 Release" || "$(CFG)" == "RmTempFiles - Win32 Debug"
SOURCE=.\RmTempFiles.cpp

!IF  "$(CFG)" == "RmTempFiles - Win32 Release"


"$(INTDIR)\RmTempFiles.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\RmTempFiles.pch"


!ELSEIF  "$(CFG)" == "RmTempFiles - Win32 Debug"


"$(INTDIR)\RmTempFiles.obj"	"$(INTDIR)\RmTempFiles.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\RmTempFiles.pch"


!ENDIF 

SOURCE=.\RmTempFiles.rc

"$(INTDIR)\RmTempFiles.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "RmTempFiles - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Fp"$(INTDIR)\RmTempFiles.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\RmTempFiles.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "RmTempFiles - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\RmTempFiles.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\RmTempFiles.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

