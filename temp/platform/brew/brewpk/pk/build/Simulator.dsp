# Microsoft Developer Studio Project File - Name="Simulator" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Simulator - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Simulator.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Simulator.mak" CFG="Simulator - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Simulator - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Simulator - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Simulator - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\inc" /I "..\..\sdk\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "T_WINNT" /D "UNICODE" /D "_UNICODE" /D "AEE_SIMULATOR" /D "AEE_STATIC" /D "AEE_OEM_EXPORTS" /D "FEATURE_APP_MANAGER" /D "FEATURE_BREW_DOWNLOAD" /D "FEATURE_COMMERCIAL_SIMULATOR" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "..\inc" /i "..\..\inc" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 quartz.lib winmm.lib msacm32.lib olepro32.lib strmiids.lib Ws2_32.lib gpsserver.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"LIBCMT" /out:"../bin/vs60/BREW_Simulator.exe"

!ELSEIF  "$(CFG)" == "Simulator - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GX /ZI /Od /I "..\inc" /I "..\..\sdk\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "T_WINNT" /D "UNICODE" /D "_UNICODE" /D "AEE_SIMULATOR" /D "AEE_STATIC" /D "AEE_OEM_EXPORTS" /D "FEATURE_APP_MANAGER" /D "FEATURE_BREW_DOWNLOAD" /D "FEATURE_COMMERCIAL_SIMULATOR" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /i "..\inc" /i "..\..\inc" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib quartz.lib winmm.lib msacm32.lib olepro32.lib strmiids.lib Ws2_32.lib gpsserver.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libcmt.lib" /out:"../bin/vs60/BREW_Simulator.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Simulator - Win32 Release"
# Name "Simulator - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\SimulatorApp.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Simulator.rc
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409 /i "..\..\sdk\inc"
# SUBTRACT RSC /i "..\inc" /i "..\..\inc"
# End Source File
# End Group
# Begin Group "Generic OEM Sources"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\bin\en\256Color\aeecontrols.bar

!IF  "$(CFG)" == "Simulator - Win32 Release"

# Begin Custom Build
InputDir=\Program Files\BREW 3.1.5\pk\bin\en\256Color
InputPath=..\bin\en\256Color\aeecontrols.bar

"$(InputDir)\aeecontrolsbar.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\utils\bin2src.exe -s$(InputPath) -dfs:/sys/en/aeecontrols.bar -o..\bin\en\256Color\aeecontrolsbar.c

# End Custom Build

!ELSEIF  "$(CFG)" == "Simulator - Win32 Debug"

# Begin Custom Build
InputDir=\Program Files\BREW 3.1.5\pk\bin\en\256Color
InputPath=..\bin\en\256Color\aeecontrols.bar

"$(InputDir)\aeecontrolsbar.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\utils\bin2src.exe -s$(InputPath) -dfs:/sys/en/aeecontrols.bar -o..\bin\en\256Color\aeecontrolsbar.c

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\bin\en\256Color\aeecontrolsbar.c
# End Source File
# Begin Source File

SOURCE=..\inc\AEEResArbiter.h
# End Source File
# Begin Source File

SOURCE=..\src\OEMAddrBook.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMAppFrame.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMAppFuncs_Sim.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMBitmap1.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMBitmap12.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMBitmap16.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMBitmap18.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMBitmap2.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMBitmap24.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMBitmap8.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMBitmap_common.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMConstFiles.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMDB.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMDefaults.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMDeviceNotifier.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMDisp.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMFlip.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMHeap.c
# End Source File
# Begin Source File

SOURCE=..\src\oemitextctl.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMMCFMIF_10888.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMMCFMIF_10889.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMMCFMIF_10890.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMMCFMIF_10891.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMMCFMIF_10892.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMMCFMIF_16225.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMMCFMIF_18067.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMMCFMIF_19240.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMMCFMIF_19917.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMMCFMIF_19918.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMMCFMIF_19919.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMMCFMIF_19920.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMMCFMIF_19922.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMMCFMIF_19923.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMMCFMIF_19924.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMMedia.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMMediaFormats.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMMemCache.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMModTable.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMModTableExt.c
# End Source File
# Begin Source File

SOURCE=..\bin\en\256Color\oemmsgs.bar

!IF  "$(CFG)" == "Simulator - Win32 Release"

# Begin Custom Build
InputDir=\Program Files\BREW 3.1.5\pk\bin\en\256Color
InputPath=..\bin\en\256Color\oemmsgs.bar

"$(InputDir)\oemmsgsbar.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\utils\bin2src -s$(InputPath) -dfs:/sys/en/oemmsgs.bar -o..\bin\en\256Color\oemmsgsbar.c

# End Custom Build

!ELSEIF  "$(CFG)" == "Simulator - Win32 Debug"

# Begin Custom Build
InputDir=\Program Files\BREW 3.1.5\pk\bin\en\256Color
InputPath=..\bin\en\256Color\oemmsgs.bar

"$(InputDir)\oemmsgsbar.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\utils\bin2src -s$(InputPath) -dfs:/sys/en/oemmsgs.bar -o..\bin\en\256Color\oemmsgsbar.c

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\bin\en\256Color\oemmsgsbar.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMNativeNotifier.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMObjectMgr.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMRegistry.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMResArbiter.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMStringFuncs.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMText.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMUSBHMSMIF_26108.c
# End Source File
# Begin Source File

SOURCE=..\src\OEMUSBHMSMIF_26109.c
# End Source File
# End Group
# Begin Group "Core Apps"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "ViewApp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\examples\ViewApp\ViewApp.c
# End Source File
# Begin Source File

SOURCE=..\examples\ViewApp\ViewApp.mif

!IF  "$(CFG)" == "Simulator - Win32 Release"

# Begin Custom Build
InputDir=\Program Files\BREW 3.1.5\pk\examples\ViewApp
InputPath=..\examples\ViewApp\ViewApp.mif

"$(InputDir)\viewappmif.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\utils\bin2src -s$(InputPath) -dfs:/mif/viewapp.mif -o..\examples\viewapp\viewappmif.c

# End Custom Build

!ELSEIF  "$(CFG)" == "Simulator - Win32 Debug"

# Begin Custom Build
InputDir=\Program Files\BREW 3.1.5\pk\examples\ViewApp
InputPath=..\examples\ViewApp\ViewApp.mif

"$(InputDir)\viewappmif.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\utils\bin2src -s$(InputPath) -dfs:/mif/viewapp.mif -o..\examples\viewapp\viewappmif.c

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\examples\ViewApp\viewappmif.c
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\res\Simulator.ico
# End Source File
# Begin Source File

SOURCE=..\bin\vs60\WinEmu.lib
# End Source File
# Begin Source File

SOURCE=..\bin\vs60\png.lib
# End Source File
# Begin Source File

SOURCE=..\bin\vs60\BrewWin.lib
# End Source File
# Begin Source File

SOURCE=..\bin\vs60\BREWEncoding.lib
# End Source File
# Begin Source File

SOURCE=..\bin\vs60\BREWResCore.lib
# End Source File
# Begin Source File

SOURCE=..\lib\Simulator\BREWAppMgrLib.lib
# End Source File
# End Target
# End Project
