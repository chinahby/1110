# Microsoft Developer Studio Generated NMAKE File, Based on Porting.dsp
!IF "$(CFG)" == ""
CFG=Porting - Win32 Debug USB
!MESSAGE No configuration specified. Defaulting to Porting - Win32 Debug USB.
!ENDIF 

!IF "$(CFG)" != "Porting - Win32 Release" && "$(CFG)" != "Porting - Win32 Debug" && "$(CFG)" != "Porting - Win32 Debug USB" && "$(CFG)" != "Porting - Win32 Release USB"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Porting.mak" CFG="Porting - Win32 Debug USB"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Porting - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Porting - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Porting - Win32 Debug USB" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Porting - Win32 Release USB" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "Porting - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\output\Porting.dll"


CLEAN :
	-@erase "$(INTDIR)\PCMCIA.obj"
	-@erase "$(INTDIR)\Porting.obj"
	-@erase "$(INTDIR)\Porting.pch"
	-@erase "$(INTDIR)\registryex.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\USBModem.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\Porting.exp"
	-@erase "$(OUTDIR)\Porting.lib"
	-@erase "..\output\Porting.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "../" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PORTING_EXPORTS" /D "PCMCIA_CARD" /Fp"$(INTDIR)\Porting.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Porting.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\Porting.pdb" /machine:I386 /out:"../output/Porting.dll" /implib:"$(OUTDIR)\Porting.lib" 
LINK32_OBJS= \
	"$(INTDIR)\PCMCIA.obj" \
	"$(INTDIR)\Porting.obj" \
	"$(INTDIR)\registryex.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\USBModem.obj"

"..\output\Porting.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Porting - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\output\Porting.dll" "$(OUTDIR)\Porting.bsc"


CLEAN :
	-@erase "$(INTDIR)\PCMCIA.obj"
	-@erase "$(INTDIR)\PCMCIA.sbr"
	-@erase "$(INTDIR)\Porting.obj"
	-@erase "$(INTDIR)\Porting.pch"
	-@erase "$(INTDIR)\Porting.sbr"
	-@erase "$(INTDIR)\registryex.obj"
	-@erase "$(INTDIR)\registryex.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\USBModem.obj"
	-@erase "$(INTDIR)\USBModem.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Porting.bsc"
	-@erase "$(OUTDIR)\Porting.exp"
	-@erase "$(OUTDIR)\Porting.lib"
	-@erase "$(OUTDIR)\Porting.pdb"
	-@erase "..\output\Porting.dll"
	-@erase "..\output\Porting.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "../" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PORTING_EXPORTS" /D "PCMCIA_CARD" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Porting.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Porting.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\PCMCIA.sbr" \
	"$(INTDIR)\Porting.sbr" \
	"$(INTDIR)\registryex.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\USBModem.sbr"

"$(OUTDIR)\Porting.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\Porting.pdb" /debug /machine:I386 /out:"../output/Porting.dll" /implib:"$(OUTDIR)\Porting.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\PCMCIA.obj" \
	"$(INTDIR)\Porting.obj" \
	"$(INTDIR)\registryex.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\USBModem.obj"

"..\output\Porting.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Porting - Win32 Debug USB"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\output\Porting.dll" "$(OUTDIR)\Porting.bsc"


CLEAN :
	-@erase "$(INTDIR)\PCMCIA.obj"
	-@erase "$(INTDIR)\PCMCIA.sbr"
	-@erase "$(INTDIR)\Porting.obj"
	-@erase "$(INTDIR)\Porting.pch"
	-@erase "$(INTDIR)\Porting.sbr"
	-@erase "$(INTDIR)\registryex.obj"
	-@erase "$(INTDIR)\registryex.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\USBModem.obj"
	-@erase "$(INTDIR)\USBModem.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\Porting.bsc"
	-@erase "$(OUTDIR)\Porting.exp"
	-@erase "$(OUTDIR)\Porting.lib"
	-@erase "$(OUTDIR)\Porting.pdb"
	-@erase "..\output\Porting.dll"
	-@erase "..\output\Porting.ilk"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "../" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PORTING_EXPORTS" /D "USB_MODEM" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Porting.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Porting.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\PCMCIA.sbr" \
	"$(INTDIR)\Porting.sbr" \
	"$(INTDIR)\registryex.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\USBModem.sbr"

"$(OUTDIR)\Porting.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /pdb:"$(OUTDIR)\Porting.pdb" /debug /machine:I386 /out:"../output/Porting.dll" /implib:"$(OUTDIR)\Porting.lib" /pdbtype:sept 
LINK32_OBJS= \
	"$(INTDIR)\PCMCIA.obj" \
	"$(INTDIR)\Porting.obj" \
	"$(INTDIR)\registryex.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\USBModem.obj"

"..\output\Porting.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Porting - Win32 Release USB"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\output\PortingUSB.dll"


CLEAN :
	-@erase "$(INTDIR)\PCMCIA.obj"
	-@erase "$(INTDIR)\Porting.obj"
	-@erase "$(INTDIR)\Porting.pch"
	-@erase "$(INTDIR)\registryex.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\USBModem.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\PortingUSB.exp"
	-@erase "$(OUTDIR)\PortingUSB.lib"
	-@erase "..\output\PortingUSB.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "../" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PORTING_EXPORTS" /D "USB_MODEM" /Fp"$(INTDIR)\Porting.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Porting.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /pdb:"$(OUTDIR)\PortingUSB.pdb" /machine:I386 /out:"../output/PortingUSB.dll" /implib:"$(OUTDIR)\PortingUSB.lib" 
LINK32_OBJS= \
	"$(INTDIR)\PCMCIA.obj" \
	"$(INTDIR)\Porting.obj" \
	"$(INTDIR)\registryex.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\USBModem.obj"

"..\output\PortingUSB.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("Porting.dep")
!INCLUDE "Porting.dep"
!ELSE 
!MESSAGE Warning: cannot find "Porting.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "Porting - Win32 Release" || "$(CFG)" == "Porting - Win32 Debug" || "$(CFG)" == "Porting - Win32 Debug USB" || "$(CFG)" == "Porting - Win32 Release USB"
SOURCE=.\PCMCIA.cpp

!IF  "$(CFG)" == "Porting - Win32 Release"


"$(INTDIR)\PCMCIA.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Porting.pch"


!ELSEIF  "$(CFG)" == "Porting - Win32 Debug"


"$(INTDIR)\PCMCIA.obj"	"$(INTDIR)\PCMCIA.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Porting.pch"


!ELSEIF  "$(CFG)" == "Porting - Win32 Debug USB"


"$(INTDIR)\PCMCIA.obj"	"$(INTDIR)\PCMCIA.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Porting.pch"


!ELSEIF  "$(CFG)" == "Porting - Win32 Release USB"


"$(INTDIR)\PCMCIA.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Porting.pch"


!ENDIF 

SOURCE=..\Porting.cpp

!IF  "$(CFG)" == "Porting - Win32 Release"


"$(INTDIR)\Porting.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Porting.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Porting - Win32 Debug"


"$(INTDIR)\Porting.obj"	"$(INTDIR)\Porting.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Porting.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Porting - Win32 Debug USB"


"$(INTDIR)\Porting.obj"	"$(INTDIR)\Porting.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Porting.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Porting - Win32 Release USB"


"$(INTDIR)\Porting.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Porting.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\registryex.cpp

!IF  "$(CFG)" == "Porting - Win32 Release"


"$(INTDIR)\registryex.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Porting.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Porting - Win32 Debug"


"$(INTDIR)\registryex.obj"	"$(INTDIR)\registryex.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Porting.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Porting - Win32 Debug USB"


"$(INTDIR)\registryex.obj"	"$(INTDIR)\registryex.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Porting.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "Porting - Win32 Release USB"


"$(INTDIR)\registryex.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Porting.pch"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "Porting - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "../" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PORTING_EXPORTS" /D "PCMCIA_CARD" /Fp"$(INTDIR)\Porting.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\Porting.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Porting - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "../" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PORTING_EXPORTS" /D "PCMCIA_CARD" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Porting.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\Porting.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Porting - Win32 Debug USB"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "../" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PORTING_EXPORTS" /D "USB_MODEM" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Porting.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\Porting.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Porting - Win32 Release USB"

CPP_SWITCHES=/nologo /MT /W3 /GX /O2 /I "../" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PORTING_EXPORTS" /D "USB_MODEM" /Fp"$(INTDIR)\Porting.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\Porting.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\USBModem.cpp

!IF  "$(CFG)" == "Porting - Win32 Release"


"$(INTDIR)\USBModem.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Porting.pch"


!ELSEIF  "$(CFG)" == "Porting - Win32 Debug"


"$(INTDIR)\USBModem.obj"	"$(INTDIR)\USBModem.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Porting.pch"


!ELSEIF  "$(CFG)" == "Porting - Win32 Debug USB"


"$(INTDIR)\USBModem.obj"	"$(INTDIR)\USBModem.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Porting.pch"


!ELSEIF  "$(CFG)" == "Porting - Win32 Release USB"


"$(INTDIR)\USBModem.obj" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\Porting.pch"


!ENDIF 


!ENDIF 

