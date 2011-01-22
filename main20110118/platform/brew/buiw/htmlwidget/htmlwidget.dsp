# Microsoft Developer Studio Project File - Name="htmlwidget" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=htmlwidget - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "htmlwidget.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "htmlwidget.mak" CFG="htmlwidget - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "htmlwidget - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "htmlwidget - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Perforce Project"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "htmlwidget - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FORMS_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /WX /GX /O2 /I "./inc" /I ".." /I "../xmod" /I "../widgets/inc" /I "../widgets/src" /I "$(BREWDIR)/inc" /I "$(BREWPK)/inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FORMS_EXPORTS" /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "htmlwidget - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FORMS_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /WX /Gm /GX /ZI /Od /I "../widgets/src" /I "./inc" /I ".." /I "../xmod" /I "../widgets/inc" /I "$(BREWDIR)/inc" /I "$(BREWPK)/inc" /D "AEE_SIMULATOR" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FORMS_EXPORTS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /dll /debug /machine:I386 /out:"htmlwidget.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "htmlwidget - Win32 Release"
# Name "htmlwidget - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\widgets\src\AEEBase.c
# End Source File
# Begin Source File

SOURCE=..\widgets\src\Border.c
# End Source File
# Begin Source File

SOURCE=.\src\charset.c
# End Source File
# Begin Source File

SOURCE=.\src\DocModel.c
# End Source File
# Begin Source File

SOURCE=.\src\dox.c
# End Source File
# Begin Source File

SOURCE=.\src\doxparse.c
# End Source File
# Begin Source File

SOURCE=.\src\DoxPos.c
# End Source File
# Begin Source File

SOURCE=..\widgets\src\FontBidiUtil.c
# End Source File
# Begin Source File

SOURCE=.\src\FormControl.c
# End Source File
# Begin Source File

SOURCE=.\src\HtmlViewModel.c
# End Source File
# Begin Source File

SOURCE=.\src\HtmlWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\hwidget_mod.c
# End Source File
# Begin Source File

SOURCE=.\src\hwidget_util.c
# End Source File
# Begin Source File

SOURCE=..\widgets\src\ImageBase.c
# End Source File
# Begin Source File

SOURCE=..\widgets\src\ModelBase.c
# End Source File
# Begin Source File

SOURCE=.\src\SourceAStream.c
# End Source File
# Begin Source File

SOURCE=..\widgets\src\WidgetBase.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE="$(BUITDIR)\widgets\inc\AEEBase.h"
# End Source File
# Begin Source File

SOURCE=.\inc\AEEDocModel.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEHtmlViewModel.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEHtmlWidget.h
# End Source File
# Begin Source File

SOURCE="$(BREWDIR)\inc\AEEStdLib.h"
# End Source File
# Begin Source File

SOURCE="$(BUITDIR)\widgets\inc\Border.h"
# End Source File
# Begin Source File

SOURCE=.\src\charset.h
# End Source File
# Begin Source File

SOURCE=.\src\DocModel.h
# End Source File
# Begin Source File

SOURCE=.\src\dox.h
# End Source File
# Begin Source File

SOURCE=.\src\doxhtml.h
# End Source File
# Begin Source File

SOURCE=.\src\doxparse.h
# End Source File
# Begin Source File

SOURCE=.\src\DoxPos.h
# End Source File
# Begin Source File

SOURCE=.\src\FormControl.h
# End Source File
# Begin Source File

SOURCE=.\src\HtmlViewModel.h
# End Source File
# Begin Source File

SOURCE=.\src\HtmlWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\lnode.h
# End Source File
# Begin Source File

SOURCE="$(BUITDIR)\widgets\src\ModelBase.h"
# End Source File
# Begin Source File

SOURCE=.\src\SourceAStream.h
# End Source File
# Begin Source File

SOURCE="$(BUITDIR)\widgets\src\WidgetBase.h"
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\mif\htmlwidget.mfx

!IF  "$(CFG)" == "htmlwidget - Win32 Release"

# Begin Custom Build - $(BREWSDKTOOLSDIR)\ResourceEditor\brewrc.exe -o ..\$(TargetName).mif -nh -i mif $(InputPath)
TargetName=htmlwidget
InputPath=.\mif\htmlwidget.mfx

"..\$(TargetName).mif" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(BREWSDKTOOLSDIR)\ResourceEditor\brewrc.exe" -o ..\$(TargetName).mif -nh -i mif $(InputPath)

# End Custom Build

!ELSEIF  "$(CFG)" == "htmlwidget - Win32 Debug"

# Begin Custom Build - $(BREWSDKTOOLSDIR)\ResourceEditor\brewrc.exe -o ..\$(TargetName).mif -nh -i mif $(InputPath)
TargetName=htmlwidget
InputPath=.\mif\htmlwidget.mfx

"..\$(TargetName).mif" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(BREWSDKTOOLSDIR)\ResourceEditor\brewrc.exe" -o ..\$(TargetName).mif -nh -i mif $(InputPath)

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Module Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\xmod\xmod.h
# End Source File
# Begin Source File

SOURCE=..\xmod\xmodimpl.c
# End Source File
# Begin Source File

SOURCE=..\xmod\xmodstub.c
# End Source File
# End Group
# End Target
# End Project
