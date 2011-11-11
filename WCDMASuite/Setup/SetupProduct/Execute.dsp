# Microsoft Developer Studio Project File - Name="Execute" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Execute - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Execute.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Execute.mak" CFG="Execute - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Execute - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Execute - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Execute - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "../../" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "CDROM" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 msimg32.lib setupapi.lib rasapi32.lib winmm.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"LIBC.lib" /out:"../../output/SetupProduct.exe"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "Execute - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "CDROM" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 msimg32.lib setupapi.lib rasapi32.lib winmm.lib /nologo /subsystem:windows /debug /machine:I386 /out:"../../output/SetupProduct.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Execute - Win32 Release"
# Name "Execute - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ChgModemThread.cpp
# End Source File
# Begin Source File

SOURCE=.\COM.cpp
# End Source File
# Begin Source File

SOURCE=..\..\comparser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\DevEnum.cpp
# End Source File
# Begin Source File

SOURCE=.\Execute.cpp
# End Source File
# Begin Source File

SOURCE=.\Execute.rc
# End Source File
# Begin Source File

SOURCE=.\ExecuteDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\HWDetect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\InfoConnect.cpp
# End Source File
# Begin Source File

SOURCE=.\LOG.cpp
# End Source File
# Begin Source File

SOURCE=.\MC315.cpp
# End Source File
# Begin Source File

SOURCE=.\MyProgressBar.cpp
# End Source File
# Begin Source File

SOURCE=.\OSVersion.cpp
# End Source File
# Begin Source File

SOURCE=".\PCMCIA-Helper.cpp"
# End Source File
# Begin Source File

SOURCE=.\rasclient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\rasspdm.cpp
# End Source File
# Begin Source File

SOURCE=.\ResourceDir.cpp
# End Source File
# Begin Source File

SOURCE=.\SelComDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SignatureMonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=..\..\stringlink.cpp
# End Source File
# Begin Source File

SOURCE=.\StringTable.cpp
# End Source File
# Begin Source File

SOURCE=.\Thread.cpp
# End Source File
# Begin Source File

SOURCE=.\UChg.cpp
# End Source File
# Begin Source File

SOURCE=".\USB-Driver.cpp"
# End Source File
# Begin Source File

SOURCE=.\WizFind2K.cpp
# End Source File
# Begin Source File

SOURCE=.\WizFindVista.cpp
# End Source File
# Begin Source File

SOURCE=.\WizFindXP.cpp
# End Source File
# Begin Source File

SOURCE=.\WndFuncs.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ChgModemThread.h
# End Source File
# Begin Source File

SOURCE=..\..\comparser.h
# End Source File
# Begin Source File

SOURCE=..\..\DevEnum.h
# End Source File
# Begin Source File

SOURCE=.\devioctl.h
# End Source File
# Begin Source File

SOURCE=.\DirWalker.h
# End Source File
# Begin Source File

SOURCE=.\DllModule.h
# End Source File
# Begin Source File

SOURCE=.\Execute.h
# End Source File
# Begin Source File

SOURCE=.\ExecuteDlg.h
# End Source File
# Begin Source File

SOURCE=..\..\InfoConnect.h
# End Source File
# Begin Source File

SOURCE=.\log.h
# End Source File
# Begin Source File

SOURCE=.\MyProgressBar.h
# End Source File
# Begin Source File

SOURCE=.\ntddscsi.h
# End Source File
# Begin Source File

SOURCE=.\OSVersion.h
# End Source File
# Begin Source File

SOURCE=".\PCMCIA-Helper.h"
# End Source File
# Begin Source File

SOURCE=.\rasclient.h
# End Source File
# Begin Source File

SOURCE=..\..\rasspdm.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SelComDlg.h
# End Source File
# Begin Source File

SOURCE=.\SignatureMonitor.h
# End Source File
# Begin Source File

SOURCE=.\spti.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=..\..\stringlink.h
# End Source File
# Begin Source File

SOURCE=.\StringTable.h
# End Source File
# Begin Source File

SOURCE=.\Thread.h
# End Source File
# Begin Source File

SOURCE=.\UChg.h
# End Source File
# Begin Source File

SOURCE=.\WndFuncs.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Execute.ico
# End Source File
# Begin Source File

SOURCE=.\res\Execute.rc2
# End Source File
# Begin Source File

SOURCE=.\res\Execute_1.ico
# End Source File
# End Group
# End Target
# End Project
