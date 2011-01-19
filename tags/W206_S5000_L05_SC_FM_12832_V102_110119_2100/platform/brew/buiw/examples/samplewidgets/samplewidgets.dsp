# Microsoft Developer Studio Project File - Name="samplewidgets" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=samplewidgets - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "samplewidgets.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "samplewidgets.mak" CFG="samplewidgets - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "samplewidgets - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "samplewidgets - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Perforce Project"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "samplewidgets - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WIDGET_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /WX /GX /O2 /I "./inc" /I "../../xmod" /I "$(BUITDIR)/widgets/inc" /I "$(BUITDIR)/widgets/src" /I "$(BREWDIR)/inc" /I "$(BREWPK)/inc" /I "$(BREWDIR)/../OEM/inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WIDGET_EXPORTS" /FD /c
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

!ELSEIF  "$(CFG)" == "samplewidgets - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "samplewidgets___Win32_Debug"
# PROP BASE Intermediate_Dir "samplewidgets___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WIDGET_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /WX /Gm /GX /ZI /Od /I "$(BUITDIR)/widgets/src" /I "./inc" /I "../../xmod" /I "$(BUITDIR)/widgets/inc" /I "$(BREWDIR)/inc" /I "$(BREWPK)/inc" /I "$(BREWDIR)/../OEM/inc" /D "AEE_SIMULATOR" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WIDGET_EXPORTS" /FR /FD /GZ /c
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
# ADD LINK32 /nologo /dll /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "samplewidgets - Win32 Release"
# Name "samplewidgets - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\widgets\src\AEEBase.c
# End Source File
# Begin Source File

SOURCE=.\src\CustomCaretWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\CustomContainer.c
# End Source File
# Begin Source File

SOURCE=.\src\CustomController.c
# End Source File
# Begin Source File

SOURCE=.\src\CustomModel.c
# End Source File
# Begin Source File

SOURCE=.\src\CustomWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\samplewidgets_mod.c
# End Source File
# Begin Source File

SOURCE=.\src\samplewidgets_util.c
# End Source File
# End Group
# Begin Group "Sample Base Classes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\CustomContainerBase.c
# End Source File
# Begin Source File

SOURCE=.\src\CustomControllerBase.c
# End Source File
# Begin Source File

SOURCE=.\src\CustomModelBase.c
# End Source File
# Begin Source File

SOURCE=.\src\CustomWidgetBase.c
# End Source File
# End Group
# Begin Group "Public Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\inc\CustomCaretWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\CustomContainer.h
# End Source File
# Begin Source File

SOURCE=.\inc\CustomController.h
# End Source File
# Begin Source File

SOURCE=.\inc\CustomModel.h
# End Source File
# Begin Source File

SOURCE=.\inc\CustomWidget.h
# End Source File
# End Group
# Begin Group "Private Headers"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\inc\CustomCaretWidget_priv.h
# End Source File
# Begin Source File

SOURCE=.\inc\CustomContainer_priv.h
# End Source File
# Begin Source File

SOURCE=.\inc\CustomContainerBase.h
# End Source File
# Begin Source File

SOURCE=.\inc\CustomController_priv.h
# End Source File
# Begin Source File

SOURCE=.\inc\CustomControllerBase.h
# End Source File
# Begin Source File

SOURCE=.\inc\CustomModel_priv.h
# End Source File
# Begin Source File

SOURCE=.\inc\CustomModelBase.h
# End Source File
# Begin Source File

SOURCE=.\inc\CustomWidget_priv.h
# End Source File
# Begin Source File

SOURCE=.\inc\CustomWidgetBase.h
# End Source File
# Begin Source File

SOURCE=.\inc\samplewidgets_util.h
# End Source File
# End Group
# Begin Group "bid Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\inc\bid\AEECLSID_CUSTOMCARETWIDGET.bid
# End Source File
# Begin Source File

SOURCE=.\inc\bid\AEECLSID_CUSTOMCONTAINER.BID
# End Source File
# Begin Source File

SOURCE=.\inc\bid\AEECLSID_CUSTOMCONTROLLER.BID
# End Source File
# Begin Source File

SOURCE=.\inc\bid\AEECLSID_CUSTOMMODEL.BID
# End Source File
# Begin Source File

SOURCE=.\inc\bid\AEECLSID_CUSTOMWIDGET.bid
# End Source File
# Begin Source File

SOURCE=.\inc\bid\AEEIID_CUSTOMCONTAINER.BID
# End Source File
# Begin Source File

SOURCE=.\inc\bid\AEEIID_CUSTOMMODEL.BID
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\mif\samplewidgets.mfx

!IF  "$(CFG)" == "samplewidgets - Win32 Release"

# Begin Custom Build - .$(BREWSDKTOOLSDIR)/ResourceEditor/brewrc  -o ..\$(TargetName).mif -h .\mif\widget_res.h -i mif $(InputPath)
TargetName=samplewidgets
InputPath=.\mif\samplewidgets.mfx

BuildCmds= \
	"$(BREWSDKTOOLSDIR)/ResourceEditor/brewrc" -o ..\$(TargetName).mif -h .\mif\widget_res.h -i mif $(InputPath)

"..\$(TargetName).mif" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".\mif\widget_res.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "samplewidgets - Win32 Debug"

# Begin Custom Build - $(BREWSDKTOOLSDIR)/ResourceEditor/brewrc  -o ..\$(TargetName).mif -h .\mif\widget_res.h -i mif/en $(InputPath)
TargetName=samplewidgets
InputPath=.\mif\samplewidgets.mfx

BuildCmds= \
	"$(BREWSDKTOOLSDIR)/ResourceEditor/brewrc"  -o ..\$(TargetName).mif -h .\mif\widget_res.h -i mif $(InputPath)

"..\$(TargetName).mif" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".\mif\widget_res.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
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
