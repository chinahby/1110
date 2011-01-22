# Microsoft Developer Studio Project File - Name="widgets" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=widgets - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "widgets.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "widgets.mak" CFG="widgets - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "widgets - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "widgets - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Perforce Project"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "widgets - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /WX /GX /O2 /I "./mif" /I "./inc" /I "./src" /I "../xmod" /I "$(BREWDIR)/inc" /I "../forms/inc" /I "../util" /I "../widgets/inc" /I "$(BREWPK)/inc" /I "$(BREWDIR)/../OEM/inc" /I "..\locales\inc\bid" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WIDGET_EXPORTS" /FD /c
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

!ELSEIF  "$(CFG)" == "widgets - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "widgets___Win32_Debug"
# PROP BASE Intermediate_Dir "widgets___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WIDGET_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /WX /Gm /GX /ZI /Od /I "./src" /I "./mif" /I "./inc" /I "../xmod" /I "$(BREWDIR)/inc" /I "../forms/inc" /I "../util" /I "../widgets/inc" /I "$(BREWPK)/inc" /I "$(BREWDIR)/../OEM/inc" /D "AEE_SIMULATOR" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "WIDGET_EXPORTS" /FR /FD /GZ /c
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
# ADD LINK32 /nologo /dll /debug /machine:I386 /out:"widgets.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "widgets - Win32 Release"
# Name "widgets - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\AEEBase.c
# End Source File
# Begin Source File

SOURCE=.\src\Bitmap32.c
# End Source File
# Begin Source File

SOURCE=.\src\BitmapWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\BlendWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\Border.c
# End Source File
# Begin Source File

SOURCE=.\src\BorderWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\CardContainer.c
# End Source File
# Begin Source File

SOURCE=.\src\CaretWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\CheckWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\ConstraintContainer.c
# End Source File
# Begin Source File

SOURCE=.\src\ContainerBase.c
# End Source File
# Begin Source File

SOURCE=.\src\ControllerBase.c
# End Source File
# Begin Source File

SOURCE=.\src\CursorWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\DateTimeWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\DecoratorWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\DefaultLocale.c
# End Source File
# Begin Source File

SOURCE=.\src\DisplayCanvas.c
# End Source File
# Begin Source File

SOURCE=.\src\DrawDecoratorWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\FileSource.c
# End Source File
# Begin Source File

SOURCE=.\src\FontBidiUtil.c
# End Source File
# Begin Source File

SOURCE=.\src\FontMapModel.c
# End Source File
# Begin Source File

SOURCE=.\src\GridContainer.c
# End Source File
# Begin Source File

SOURCE=.\src\HFontOutline.c
# End Source File
# Begin Source File

SOURCE=.\src\ImageBase.c
# End Source File
# Begin Source File

SOURCE=.\src\ImageStaticWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\ImageWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\ListWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\MenuModel.c
# End Source File
# Begin Source File

SOURCE=.\src\ModelBase.c
# End Source File
# Begin Source File

SOURCE=.\src\modres.c
# End Source File
# Begin Source File

SOURCE=.\src\ProgressWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\PropContainer.c
# End Source File
# Begin Source File

SOURCE=.\src\ResDecoderCache.c
# End Source File
# Begin Source File

SOURCE=.\src\ResFile.c
# End Source File
# Begin Source File

SOURCE=.\src\ResObj.c
# End Source File
# Begin Source File

SOURCE=.\src\RootContainer.c
# End Source File
# Begin Source File

SOURCE=.\src\ScrollWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\SliderWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\SoftkeyWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\StaticWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\TabWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\TextController.c
# End Source File
# Begin Source File

SOURCE=.\src\TextFormatter.c
# End Source File
# Begin Source File

SOURCE=.\src\TextLayout.c
# End Source File
# Begin Source File

SOURCE=.\src\TextModel.c
# End Source File
# Begin Source File

SOURCE=.\src\TextWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\TitleWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\Transition.c
# End Source File
# Begin Source File

SOURCE=.\src\Vector.c
# End Source File
# Begin Source File

SOURCE=.\src\VectorModel.c
# End Source File
# Begin Source File

SOURCE=.\src\ViewportWidget.c
# End Source File
# Begin Source File

SOURCE=.\src\widget_mod.c
# End Source File
# Begin Source File

SOURCE=.\src\WidgetBase.c
# End Source File
# Begin Source File

SOURCE=.\src\WidgetContBase.c
# End Source File
# Begin Source File

SOURCE=.\src\wutil.c
# End Source File
# Begin Source File

SOURCE=.\src\XYContainer.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\inc\AEEArrayModel.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEBase.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEBitmapWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEBlendWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEBorderWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEECanvas.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEECardContainer.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEECaretWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEECheckWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEConstraintContainer.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEContainer.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEECursorWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEDateTimeWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEDecorator.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEDisplayCanvas.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEDrawDecoratorWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEDrawHandler.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEFontBidiUtil.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEFontMapModel.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEGridContainer.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEHandler.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEHFont.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEHFontOutline.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEImageStaticWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEImageWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEListModel.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEListWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEELocale.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEMenuModel.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEModel.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEProgressWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEPropContainer.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEResFile.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEERichTextModel.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEERootContainer.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEScrollWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEESliderWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEESoftkeyWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEStaticWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEETextController.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEETextFormatter.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEETextLayout.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEETextModel.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEETextWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEETitleWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEETransition.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEVectorModel.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEViewportWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEWidget.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEWModel.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEWProperties.h
# End Source File
# Begin Source File

SOURCE=.\inc\AEEXYContainer.h
# End Source File
# Begin Source File

SOURCE=.\src\Bitmap32.h
# End Source File
# Begin Source File

SOURCE=.\src\BitmapWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\BlendWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\Border.h
# End Source File
# Begin Source File

SOURCE=.\src\BorderWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\CardContainer.h
# End Source File
# Begin Source File

SOURCE=.\src\CaretWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\CheckWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\ConstraintContainer.h
# End Source File
# Begin Source File

SOURCE=.\src\ContainerBase.h
# End Source File
# Begin Source File

SOURCE=.\src\ControllerBase.h
# End Source File
# Begin Source File

SOURCE=.\src\CursorWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\DateTimeWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\DecoratorWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\DefaultLocale.h
# End Source File
# Begin Source File

SOURCE=.\src\DisplayCanvas.h
# End Source File
# Begin Source File

SOURCE=.\src\DrawDecoratorWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\DrawLines.h
# End Source File
# Begin Source File

SOURCE=.\src\FontMapModel.h
# End Source File
# Begin Source File

SOURCE=.\src\GridContainer.h
# End Source File
# Begin Source File

SOURCE=.\src\HFontOutline.h
# End Source File
# Begin Source File

SOURCE=.\src\ImageBase.h
# End Source File
# Begin Source File

SOURCE=.\src\ImageStaticWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\ImageWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\ListWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\MenuModel.h
# End Source File
# Begin Source File

SOURCE=.\src\ModelBase.h
# End Source File
# Begin Source File

SOURCE=.\src\ProgressWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\PropContainer.h
# End Source File
# Begin Source File

SOURCE=.\src\ResDecoderCache.h
# End Source File
# Begin Source File

SOURCE=.\src\ResFile.h
# End Source File
# Begin Source File

SOURCE=.\src\ResObj.h
# End Source File
# Begin Source File

SOURCE=.\src\RootContainer.h
# End Source File
# Begin Source File

SOURCE=.\src\ScrollWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\SliderWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\SoftkeyWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\StaticWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\TabWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\TextController.h
# End Source File
# Begin Source File

SOURCE=.\src\TextFormatter.h
# End Source File
# Begin Source File

SOURCE=.\src\TextLayout.h
# End Source File
# Begin Source File

SOURCE=.\src\TextModel.h
# End Source File
# Begin Source File

SOURCE=.\src\TextWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\TitleWidget.h
# End Source File
# Begin Source File

SOURCE=.\src\Transition.h
# End Source File
# Begin Source File

SOURCE=.\src\Vector.h
# End Source File
# Begin Source File

SOURCE=.\src\VectorModel.h
# End Source File
# Begin Source File

SOURCE=.\src\ViewportWidget.h
# End Source File
# Begin Source File

SOURCE=.\mif\widget_res.h
# End Source File
# Begin Source File

SOURCE=.\src\WidgetBase.h
# End Source File
# Begin Source File

SOURCE=.\src\WidgetContBase.h
# End Source File
# Begin Source File

SOURCE=.\src\wlint.h
# End Source File
# Begin Source File

SOURCE=.\src\wutil.h
# End Source File
# Begin Source File

SOURCE=.\src\XYContainer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\mif\widgets.mfx

!IF  "$(CFG)" == "widgets - Win32 Release"

# Begin Custom Build - $(BREWSDKTOOLSDIR)\ResourceEditor\brewrc.exe -o ..\$(TargetName).mif -h .\mif\widget_res.h -i mif $(InputPath)
TargetName=widgets
InputPath=.\mif\widgets.mfx

BuildCmds= \
	"$(BREWSDKTOOLSDIR)\ResourceEditor\brewrc.exe" -o ..\$(TargetName).mif -h .\mif\widget_res.h -i mif $(InputPath)

"..\$(TargetName).mif" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

".\mif\widget_res.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "widgets - Win32 Debug"

# Begin Custom Build - $(BREWSDKTOOLSDIR)\ResourceEditor\brewrc.exe -o ..\$(TargetName).mif -h .\mif\widget_res.h -i mif/en $(InputPath)
TargetName=widgets
InputPath=.\mif\widgets.mfx

BuildCmds= \
	"$(BREWSDKTOOLSDIR)\ResourceEditor\brewrc.exe" -o ..\$(TargetName).mif -h .\mif\widget_res.h -i mif $(InputPath)

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
