# Microsoft Developer Studio Project File - Name="AEETU" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=AEETU - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AEETU.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AEETU.mak" CFG="AEETU - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AEETU - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "AEETU - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "AEETU"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AEETU - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "win32_Release"
# PROP BASE Intermediate_Dir "win32_Release\OBJ"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\inc" /I "..\..\..\oem\inc" /D "NDEBUG" /D "AEE_SIMULATOR" /D "WIN32" /D "_MBCS" /D "_LIB" /D "UNICODE" /D "_UNICODE" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Release\AEEThreadUtil.lib"
# Begin Custom Build
OutDir=.\Release
InputPath=.\Release\AEEThreadUtil.lib
SOURCE="$(InputPath)"

"..\..\lib\win32\AEEThreadUtil.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /f ..\..\lib\win32\AEEThreadUtil.lib 
	copy $(OutDir)\AEEThreadUtil.lib ..\..\lib\win32\AEEThreadUtil.lib 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "AEETU - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "win32_Debug"
# PROP BASE Intermediate_Dir "win32_Debug\OBJ"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\oem\inc" /I "..\..\inc" /D "AEE_SIMULATOR" /D "_MBCS" /D "_LIB" /D "UNICODE" /D "_UNICODE" /D "WIN32" /D "_DEBUG" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\AEEThreadUtil.lib"
# Begin Custom Build
OutDir=.\Debug
InputPath=.\Debug\AEEThreadUtil.lib
SOURCE="$(InputPath)"

"..\..\lib\win32\AEEThreadUtil.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	del /f ..\..\lib\win32\AEEThreadUtil.lib 
	copy $(OutDir)\AEEThreadUtil.lib ..\..\lib\win32\AEEThreadUtil.lib 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "AEETU - Win32 Release"
# Name "AEETU - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AEECallbackUtil.c
# End Source File
# Begin Source File

SOURCE=.\AEETU_CondVar.c
# End Source File
# Begin Source File

SOURCE=.\AEETU_Connect.c
# End Source File
# Begin Source File

SOURCE=.\AEETU_GetHostByName.c
# End Source File
# Begin Source File

SOURCE=.\AEETU_Mutex.c
# End Source File
# Begin Source File

SOURCE=.\AEETU_Recv.c
# End Source File
# Begin Source File

SOURCE=.\AEETU_Send.c
# End Source File
# Begin Source File

SOURCE=.\AEETU_Sleep.c
# End Source File
# Begin Source File

SOURCE=.\AEETU_Socket.c
# End Source File
# Begin Source File

SOURCE=.\AEETU_TLS.c
# End Source File
# Begin Source File

SOURCE=.\AEETU_Web.c
# End Source File
# Begin Source File

SOURCE=.\AEETU_Yield.c
# End Source File
# End Group
# End Target
# End Project
