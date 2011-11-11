# Microsoft Developer Studio Project File - Name="ResLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ResLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ResLib.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ResLib - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RESLIB_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RESLIB_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:yes /debug /machine:I386 /out:"../output/ResLib.dll" /NOENTRY

!ELSEIF  "$(CFG)" == "ResLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RESLIB_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "RESLIB_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../output//ResLib.dll" /pdbtype:sept /NOENTRY

!ENDIF 

# Begin Target

# Name "ResLib - Win32 Release"
# Name "ResLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "resource"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\res\dailup\#-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\#-2.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\-2.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\0-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\0-2.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\1-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\1-2.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\2-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\2-2.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\3-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\3-2.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\4-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\4-2.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\5-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\5-2.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\6-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\6-2.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\7-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\7-2.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\8-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\8-2.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\9-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\9-2.bmp"
# End Source File
# Begin Source File

SOURCE=..\res\background.bmp
# End Source File
# Begin Source File

SOURCE="..\res\dailup\Clear-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\Clear-2.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\Close-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\Close-2.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\Dial-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\Dial-2.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\Hangup-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\Hangup-2.bmp"
# End Source File
# Begin Source File

SOURCE=..\res\harrow.cur
# End Source File
# Begin Source File

SOURCE=..\res\max1.bmp
# End Source File
# Begin Source File

SOURCE="..\res\min-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\Min-2.bmp"
# End Source File
# Begin Source File

SOURCE=..\res\min1.bmp
# End Source File
# Begin Source File

SOURCE=..\res\msg.bmp
# End Source File
# Begin Source File

SOURCE=..\res\Online.bmp
# End Source File
# Begin Source File

SOURCE="..\res\dailup\P-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\P-2.bmp"
# End Source File
# Begin Source File

SOURCE=..\res\pcmciaapp.ico
# End Source File
# Begin Source File

SOURCE=..\res\privacy.ico
# End Source File
# Begin Source File

SOURCE="..\res\dailup\RE-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\RE-2.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\Signal-0.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\Signal-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\Signal-2.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\Signal-3.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\Signal-4.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\Signal-5.bmp"
# End Source File
# Begin Source File

SOURCE=..\res\SMSNew.bmp
# End Source File
# Begin Source File

SOURCE=..\res\SMSRead.bmp
# End Source File
# Begin Source File

SOURCE=..\res\SMSUnread.bmp
# End Source File
# Begin Source File

SOURCE=..\res\SysTimeBKG.bmp
# End Source File
# Begin Source File

SOURCE="..\res\dailup\T-1.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\dailup\T-2.bmp"
# End Source File
# End Group
# Begin Source File

SOURCE=..\res\number\0.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\1.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\11.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\12.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\2.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\3.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\4.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\5.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\6.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\7.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\8.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\9.bmp
# End Source File
# Begin Source File

SOURCE="..\res\bar-VolAdjustBack.bmp"
# End Source File
# Begin Source File

SOURCE="..\res\bar-VolAdjustBtn.bmp"
# End Source File
# Begin Source File

SOURCE=..\res\number\call.bmp
# End Source File
# Begin Source File

SOURCE=..\res\call\call0.bmp
# End Source File
# Begin Source File

SOURCE=..\res\call\call1.bmp
# End Source File
# Begin Source File

SOURCE=..\res\call\call2.bmp
# End Source File
# Begin Source File

SOURCE=..\res\call\call3.bmp
# End Source File
# Begin Source File

SOURCE=..\res\call\call4.bmp
# End Source File
# Begin Source File

SOURCE=..\res\call\call5.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\callDown.bmp
# End Source File
# Begin Source File

SOURCE=..\res\call\callInAndOut.bmp
# End Source File
# Begin Source File

SOURCE=..\res\clear.bmp
# End Source File
# Begin Source File

SOURCE=..\res\connect.bmp
# End Source File
# Begin Source File

SOURCE=..\res\disableCon.bmp
# End Source File
# Begin Source File

SOURCE=..\res\disableCut.bmp
# End Source File
# Begin Source File

SOURCE=..\res\disconnect.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\down0.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\down11.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\down12.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\down2.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\down3.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\down4.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\down5.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\down6.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\down7.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\down8.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\down9.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\downClear.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\downOne.bmp
# End Source File
# Begin Source File

SOURCE=..\res\EDGE.bmp
# End Source File
# Begin Source File

SOURCE="..\res\Exit-over.bmp"
# End Source File
# Begin Source File

SOURCE=..\res\Exit.bmp
# End Source File
# Begin Source File

SOURCE=..\res\GPRS.bmp
# End Source File
# Begin Source File

SOURCE=..\res\GSM.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\hangoff.bmp
# End Source File
# Begin Source File

SOURCE=..\res\number\hangupDown.bmp
# End Source File
# Begin Source File

SOURCE=..\res\login.bmp
# End Source File
# Begin Source File

SOURCE=..\res\LOGO.bmp
# End Source File
# Begin Source File

SOURCE=..\res\longbar.bmp
# End Source File
# Begin Source File

SOURCE=..\res\notRegister.bmp
# End Source File
# Begin Source File

SOURCE=..\res\PhoneSkins\PopWinBk_CenterBottom.bmp
# End Source File
# Begin Source File

SOURCE=..\res\PhoneSkins\PopWinBk_CenterTop.bmp
# End Source File
# Begin Source File

SOURCE=..\res\PhoneSkins\PopWinBk_Left.bmp
# End Source File
# Begin Source File

SOURCE=..\res\PhoneSkins\PopWinBk_LeftBottom.bmp
# End Source File
# Begin Source File

SOURCE=..\res\PhoneSkins\PopWinBk_LeftTop.bmp
# End Source File
# Begin Source File

SOURCE=..\res\PhoneSkins\PopWinBk_Right.bmp
# End Source File
# Begin Source File

SOURCE=..\res\PhoneSkins\PopWinBk_RightBottom.bmp
# End Source File
# Begin Source File

SOURCE=..\res\PhoneSkins\PopWinBk_RightTop.bmp
# End Source File
# Begin Source File

SOURCE=..\res\PhoneSkins\PopWinBkCenterBottom.bmp
# End Source File
# Begin Source File

SOURCE=..\res\PhoneSkins\PopWinBkCenterTop.bmp
# End Source File
# Begin Source File

SOURCE=..\res\PhoneSkins\PopWinBkLeft.bmp
# End Source File
# Begin Source File

SOURCE=..\res\PhoneSkins\PopWinBkLeftBottom.bmp
# End Source File
# Begin Source File

SOURCE=..\res\PhoneSkins\PopWinBkLeftTop.bmp
# End Source File
# Begin Source File

SOURCE=..\res\PhoneSkins\PopWinBkRight.bmp
# End Source File
# Begin Source File

SOURCE=..\res\PhoneSkins\PopWinBkRightBottom.bmp
# End Source File
# Begin Source File

SOURCE=..\res\PhoneSkins\PopWinBkRightTop.bmp
# End Source File
# Begin Source File

SOURCE=..\res\press.bmp
# End Source File
# Begin Source File

SOURCE=..\res\pressOutlook.bmp
# End Source File
# Begin Source File

SOURCE=..\res\regSucceess.bmp
# End Source File
# Begin Source File

SOURCE=..\res\roaming.bmp
# End Source File
# Begin Source File

SOURCE=..\res\SAMLLTOOLBAR.bmp
# End Source File
# Begin Source File

SOURCE=..\res\ShareImageList.bmp
# End Source File
# Begin Source File

SOURCE=..\res\smalllongbar.bmp
# End Source File
# Begin Source File

SOURCE=..\res\sms.bmp
# End Source File
# Begin Source File

SOURCE=..\res\SunnyCatApp.ico
# End Source File
# Begin Source File

SOURCE=..\res\TOOLBAR.bmp
# End Source File
# Begin Source File

SOURCE=..\res\PhoneSkins\ToolBarBk.bmp
# End Source File
# Begin Source File

SOURCE=..\res\WEWINS.bmp
# End Source File
# Begin Source File

SOURCE="..\res\number\Çå³ý.bmp"
# End Source File
# End Target
# End Project
