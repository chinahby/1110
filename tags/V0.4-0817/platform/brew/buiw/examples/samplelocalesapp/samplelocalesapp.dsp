# Microsoft Developer Studio Project File - Name="samplelocalesapp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=samplelocalesapp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "samplelocalesapp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "samplelocalesapp.mak" CFG="samplelocalesapp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "samplelocalesapp - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "samplelocalesapp - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Perforce Project"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "samplelocalesapp - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SSAVERS_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /WX /GX /O2 /I "../../xmod" /I "$(BUITDIR)/widgets/inc" /I "$(BREWDIR)/inc" /I "$(BREWPK)/inc" /I "$(BREWDIR)/../OEM/inc" /D "NDEBUG" /D "AEE_SIMULATOR" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SSAVERS_EXPORTS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "samplelocalesapp - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SSAVERS_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /WX /Gm /GX /ZI /Od /I "../../xmod" /I "$(BUITDIR)/widgets/inc" /I "$(BREWDIR)/inc" /I "$(BREWPK)/inc" /I "$(BREWDIR)/../OEM/inc" /D "AEE_SIMULATOR" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SSAVERS_EXPORTS" /FR"./" /Fo"./" /Fd"./" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /dll /debug /machine:I386 /nodefaultlib:"libcd" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "samplelocalesapp - Win32 Release"
# Name "samplelocalesapp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\samplelocalesapp.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\bid\samplelocalesapp.BID
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\mif\samplelocalesapp.mfx

!IF  "$(CFG)" == "samplelocalesapp - Win32 Release"

# Begin Custom Build - $(BREWSDKTOOLSDIR)\ResourceEditor\brewrc.exe -o ..\$(TargetName).mif -nh -i mif -i ..\ $(InputPath)
TargetName=samplelocalesapp
InputPath=.\mif\samplelocalesapp.mfx

"..\$(TargetName).mif" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(BREWSDKTOOLSDIR)\ResourceEditor\brewrc.exe" -o ..\$(TargetName).mif -nh -i mif -i ..\ $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "samplelocalesapp - Win32 Debug"

# Begin Custom Build - $(BREWSDKTOOLSDIR)\ResourceEditor\brewrc.exe -o ..\$(TargetName).mif -nh -i mif -i ..\ $(InputPath)
TargetName=samplelocalesapp
InputPath=.\mif\samplelocalesapp.mfx

"..\$(TargetName).mif" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(BREWSDKTOOLSDIR)\ResourceEditor\brewrc.exe" -o ..\$(TargetName).mif -nh -i mif -i ..\ $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Module Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\xmod\xmod.h
# End Source File
# Begin Source File

SOURCE=..\..\xmod\xmodimpl.c
# End Source File
# Begin Source File

SOURCE=..\..\xmod\xmodstub.c
# End Source File
# End Group
# End Target
# End Project
