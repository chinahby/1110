# Microsoft Developer Studio Project File - Name="SunnyCat" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SunnyCat - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "SunnyCat.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "SunnyCat.mak" CFG="SunnyCat - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "SunnyCat - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "SunnyCat - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SunnyCat - Win32 Release"

# PROP BASE Use_MFC 6
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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /D "_UNICODE" /D "CDROM" /Fr /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Porting/Release/Porting.lib winmm.lib rasapi32.lib shlwapi.lib msimg32.lib setupapi.lib rpcrt4.lib Wininet.lib data/sqlite3.lib wavedll.lib /nologo /entry:"wWinMainCRTStartup " /subsystem:windows /machine:I386 /out:"Output/SunnyCat.exe"
# SUBTRACT LINK32 /incremental:yes /map /debug /nodefaultlib

!ELSEIF  "$(CFG)" == "SunnyCat - Win32 Debug"

# PROP BASE Use_MFC 6
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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "UNICODE" /D "_UNICODE" /D "CDROM" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x804 /fo"SunnyCat.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Porting\Debug\Porting.lib Msimg32.lib winmm.lib rasapi32.lib shlwapi.lib setupapi.lib data\sqlite3.lib rpcrt4.lib Wininet.lib wavedll.lib /nologo /entry:"wWinMainCRTStartup " /subsystem:windows /debug /machine:I386 /out:"output\SunnyCat.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "SunnyCat - Win32 Release"
# Name "SunnyCat - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\MFC\Include\AFX.INL"
# End Source File
# Begin Source File

SOURCE=.\ApnDlg.h
# End Source File
# Begin Source File

SOURCE=.\ApnEdit.h
# End Source File
# Begin Source File

SOURCE=.\CMobilePacket.h
# End Source File
# Begin Source File

SOURCE=.\CodingConv.h
# End Source File
# Begin Source File

SOURCE=.\contacts.h
# End Source File
# Begin Source File

SOURCE=.\CWndChart.h
# End Source File
# Begin Source File

SOURCE=.\DlgIPDNS.h
# End Source File
# Begin Source File

SOURCE=.\fluxwarndlg.h
# End Source File
# Begin Source File

SOURCE=.\font.h
# End Source File
# Begin Source File

SOURCE=.\GenMsgBox.h
# End Source File
# Begin Source File

SOURCE=.\ImportRepeatPrompt.h
# End Source File
# Begin Source File

SOURCE=.\InitAT.h
# End Source File
# Begin Source File

SOURCE=.\public\IWaveLib.h
# End Source File
# Begin Source File

SOURCE=.\MobileManager.h
# End Source File
# Begin Source File

SOURCE=.\myifstream.h
# End Source File
# Begin Source File

SOURCE=.\OurSetupapi.h
# End Source File
# Begin Source File

SOURCE=.\PBEdit.h
# End Source File
# Begin Source File

SOURCE=.\Porting.h
# End Source File
# Begin Source File

SOURCE=.\RegisterCOPS.h
# End Source File
# Begin Source File

SOURCE=.\RepeatDialog.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SelectNetworkDlg.h
# End Source File
# Begin Source File

SOURCE=.\SetBaudCmdpacket.h
# End Source File
# Begin Source File

SOURCE=.\SMS7Bit.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SunnyCatDlg.h
# End Source File
# Begin Source File

SOURCE=.\ThreadWaitAuotRun.h
# End Source File
# Begin Source File

SOURCE=.\Update.h
# End Source File
# Begin Source File

SOURCE=.\UpdateProgressDlg.h
# End Source File
# Begin Source File

SOURCE=.\WaitCOPS.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\About.cpp
# End Source File
# Begin Source File

SOURCE=.\ApnDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ApnEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\CMobilePacket.cpp
# End Source File
# Begin Source File

SOURCE=.\CodingConv.cpp
# End Source File
# Begin Source File

SOURCE=.\contacts.cpp
# End Source File
# Begin Source File

SOURCE=.\CWndChart.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgIPDNS.cpp
# End Source File
# Begin Source File

SOURCE=.\FluxWarnDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\font.cpp
# End Source File
# Begin Source File

SOURCE=.\GenMsgBox.cpp
# End Source File
# Begin Source File

SOURCE=.\ImportRepeatPrompt.cpp
# End Source File
# Begin Source File

SOURCE=.\InitAT.cpp
# End Source File
# Begin Source File

SOURCE=.\mobilemanager.cpp
# End Source File
# Begin Source File

SOURCE=.\PBEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\PinDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PinDialog.h
# End Source File
# Begin Source File

SOURCE=.\PINEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\PINEdit.h
# End Source File
# Begin Source File

SOURCE=.\PinOperDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\PinOperDialog.h
# End Source File
# Begin Source File

SOURCE=.\RegisterCOPS.cpp
# End Source File
# Begin Source File

SOURCE=.\RemovePrompt.cpp
# End Source File
# Begin Source File

SOURCE=.\RepeatDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectNetworkDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetBaudCmdpacket.cpp
# End Source File
# Begin Source File

SOURCE=.\SMS7Bit.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE="C:\Program Files\Microsoft Visual Studio\VC98\MFC\SRC\STRCORE.CPP"
# End Source File
# Begin Source File

SOURCE=.\SunnyCat.cpp
# End Source File
# Begin Source File

SOURCE=.\SunnyCat.rc
# End Source File
# Begin Source File

SOURCE=.\SunnyCatDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ThreadWaitAuotRun.cpp
# End Source File
# Begin Source File

SOURCE=.\Update.cpp
# End Source File
# Begin Source File

SOURCE=.\UpdateProgressDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\WaitCOPS.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\number\0.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\11.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\12.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\5.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\7.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\8.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\9.bmp
# End Source File
# Begin Source File

SOURCE=".\res\bar-VolAdjustBack.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\bar-VolAdjustBtn.bmp"
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bitmap2.bmp
# End Source File
# Begin Source File

SOURCE=.\Output\skin\bottom_network.bmp
# End Source File
# Begin Source File

SOURCE=.\Output\skin\botton_button.bmp
# End Source File
# Begin Source File

SOURCE=.\res\button_blue.bmp
# End Source File
# Begin Source File

SOURCE=.\res\button_gray.bmp
# End Source File
# Begin Source File

SOURCE=.\Output\skin\button_nrefresh.bmp
# End Source File
# Begin Source File

SOURCE=.\Output\skin\button_nregister.bmp
# End Source File
# Begin Source File

SOURCE=.\Output\skin\button_refresh.bmp
# End Source File
# Begin Source File

SOURCE=.\Output\skin\button_register.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\call.bmp
# End Source File
# Begin Source File

SOURCE=.\res\call\call0.bmp
# End Source File
# Begin Source File

SOURCE=.\res\call\call1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\call\call2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\call\call3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\call\call4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\call\call5.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\callDown.bmp
# End Source File
# Begin Source File

SOURCE=.\res\call\callInAndOut.bmp
# End Source File
# Begin Source File

SOURCE=.\res\clear.bmp
# End Source File
# Begin Source File

SOURCE=.\res\connect.bmp
# End Source File
# Begin Source File

SOURCE=.\res\disableCon.bmp
# End Source File
# Begin Source File

SOURCE=.\res\disableCut.bmp
# End Source File
# Begin Source File

SOURCE=.\res\disconnect.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\down0.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\down11.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\down12.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\down2.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\down3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\down4.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\down5.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\down6.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\down7.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\down8.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\down9.bmp
# End Source File
# Begin Source File

SOURCE=.\res\down_down.bmp
# End Source File
# Begin Source File

SOURCE=.\res\down_no.bmp
# End Source File
# Begin Source File

SOURCE=.\res\down_over.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\downClear.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\downOne.bmp
# End Source File
# Begin Source File

SOURCE=.\res\EDGE.bmp
# End Source File
# Begin Source File

SOURCE=".\res\Exit-over.bmp"
# End Source File
# Begin Source File

SOURCE=.\res\Exit.bmp
# End Source File
# Begin Source File

SOURCE=.\res\geemodem1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\geemodem3.bmp
# End Source File
# Begin Source File

SOURCE=".\res\Geemodem_CDMA-2.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Geemodem_CDMA-3.bmp"
# End Source File
# Begin Source File

SOURCE=.\res\GPRS.bmp
# End Source File
# Begin Source File

SOURCE=.\res\GSM.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\hangoff.bmp
# End Source File
# Begin Source File

SOURCE=.\res\number\hangupDown.bmp
# End Source File
# Begin Source File

SOURCE=.\res\login.bmp
# End Source File
# Begin Source File

SOURCE=.\res\LOGO.bmp
# End Source File
# Begin Source File

SOURCE=.\res\longbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\msg.bmp
# End Source File
# Begin Source File

SOURCE=.\res\notRegister.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Online.bmp
# End Source File
# Begin Source File

SOURCE=.\res\press.bmp
# End Source File
# Begin Source File

SOURCE=.\res\pressOutlook.bmp
# End Source File
# Begin Source File

SOURCE=".\res\QQ截图未命名.bmp"
# End Source File
# Begin Source File

SOURCE=.\Output\register.bmp
# End Source File
# Begin Source File

SOURCE=.\res\regSucceess.bmp
# End Source File
# Begin Source File

SOURCE=.\res\roaming.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SAMLLTOOLBAR.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ShareImageList.bmp
# End Source File
# Begin Source File

SOURCE=".\res\Signal-0.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Signal-1.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Signal-2.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Signal-3.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Signal-4.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Signal-5.bmp"
# End Source File
# Begin Source File

SOURCE=.\res\smalllongbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\sms.bmp
# End Source File
# Begin Source File

SOURCE=.\res\SunnyCat.ico
# End Source File
# Begin Source File

SOURCE=.\res\SunnyCat.rc2
# End Source File
# Begin Source File

SOURCE=.\res\SunnyCatApp.ico
# End Source File
# Begin Source File

SOURCE=.\res\SuperPhone.ico
# End Source File
# Begin Source File

SOURCE=.\res\TOOLBAR.bmp
# End Source File
# Begin Source File

SOURCE=.\res\WEWINS.bmp
# End Source File
# Begin Source File

SOURCE=".\res\number\清除.bmp"
# End Source File
# End Group
# Begin Group "语音"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\AcceptCmdPacket.cpp
# End Source File
# Begin Source File

SOURCE=.\AcceptCmdPacket.h
# End Source File
# Begin Source File

SOURCE=.\CallCmdPacket.cpp
# End Source File
# Begin Source File

SOURCE=.\CallCmdPacket.h
# End Source File
# Begin Source File

SOURCE=.\CallDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\CallDialog.h
# End Source File
# Begin Source File

SOURCE=.\CallManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CallManager.h
# End Source File
# Begin Source File

SOURCE=.\Data\CallRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\Data\CallRecord.h
# End Source File
# Begin Source File

SOURCE=.\CHangupCmdPacket.cpp
# End Source File
# Begin Source File

SOURCE=.\CHangupCmdPacket.h
# End Source File
# Begin Source File

SOURCE=.\DialNumEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\DialNumEdit.h
# End Source File
# Begin Source File

SOURCE=.\Data\PhoneBook.cpp
# End Source File
# Begin Source File

SOURCE=.\Data\PhoneBook.h
# End Source File
# Begin Source File

SOURCE=.\Data\phonebookembed.cpp
# End Source File
# Begin Source File

SOURCE=.\Data\phonebookembed.h
# End Source File
# Begin Source File

SOURCE=.\Data\phonebookpc.cpp
# End Source File
# Begin Source File

SOURCE=.\Data\phonebookpc.h
# End Source File
# Begin Source File

SOURCE=.\phonenumedit.cpp
# End Source File
# Begin Source File

SOURCE=.\phonenumedit.h
# End Source File
# Begin Source File

SOURCE=.\PROGRESSWND.CPP
# End Source File
# End Group
# Begin Group "网络"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\InfoConnect.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoConnect.h
# End Source File
# Begin Source File

SOURCE=.\infodialup.cpp
# End Source File
# Begin Source File

SOURCE=.\infodialup.h
# End Source File
# Begin Source File

SOURCE=.\NetDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\NetDialog.h
# End Source File
# Begin Source File

SOURCE=.\NetStatByTime.h
# End Source File
# Begin Source File

SOURCE=.\rasclient.cpp
# End Source File
# Begin Source File

SOURCE=.\rasclient.h
# End Source File
# Begin Source File

SOURCE=.\rasspdm.cpp
# End Source File
# Begin Source File

SOURCE=.\rasspdm.h
# End Source File
# Begin Source File

SOURCE=.\SetNetConnectDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetNetConnectDlg.h
# End Source File
# End Group
# Begin Group "短信"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\bitsarray.cpp
# End Source File
# Begin Source File

SOURCE=.\bitsarray.h
# End Source File
# Begin Source File

SOURCE=.\dlgmsg.h
# End Source File
# Begin Source File

SOURCE=.\NewSmsDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\NewSmsDialog.h
# End Source File
# Begin Source File

SOURCE=.\PDU.cpp
# End Source File
# Begin Source File

SOURCE=.\PDU.h
# End Source File
# Begin Source File

SOURCE=.\Data\SMS.cpp
# End Source File
# Begin Source File

SOURCE=.\Data\SMS.h
# End Source File
# Begin Source File

SOURCE=.\SMSDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\SMSDialog.h
# End Source File
# Begin Source File

SOURCE=.\Data\SMSEmbed.cpp
# End Source File
# Begin Source File

SOURCE=.\Data\SMSEmbed.h
# End Source File
# Begin Source File

SOURCE=.\Data\SMSManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Data\SMSManager.h
# End Source File
# Begin Source File

SOURCE=.\Data\SMSPC.cpp
# End Source File
# Begin Source File

SOURCE=.\Data\SMSPC.h
# End Source File
# Begin Source File

SOURCE=.\TheadSendSMS.cpp
# End Source File
# Begin Source File

SOURCE=.\TheadSendSMS.h
# End Source File
# End Group
# Begin Group "联系人"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\EnFileDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\EnFileDialog.h
# End Source File
# Begin Source File

SOURCE=.\LinkManDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\LinkManDialog.h
# End Source File
# Begin Source File

SOURCE=.\SkinWin\MyBitmap.h
# End Source File
# Begin Source File

SOURCE=.\NewCardDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NewCardDlg.h
# End Source File
# Begin Source File

SOURCE=.\NewCardForSIM.cpp
# End Source File
# Begin Source File

SOURCE=.\NewCardForSIM.h
# End Source File
# Begin Source File

SOURCE=.\NewClassDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\NewClassDlg.h
# End Source File
# Begin Source File

SOURCE=.\OtherDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\OtherDialog.h
# End Source File
# Begin Source File

SOURCE=.\OwnDrawComboBox.h
# End Source File
# Begin Source File

SOURCE=.\PROGRESSWND.H
# End Source File
# Begin Source File

SOURCE=.\registryex.h
# End Source File
# Begin Source File

SOURCE=.\ResizeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\SelectClassDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectLinkmanDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SelectLinkmanDlg.h
# End Source File
# Begin Source File

SOURCE=.\threadloadpb.cpp
# End Source File
# Begin Source File

SOURCE=.\threadloadpb.h
# End Source File
# Begin Source File

SOURCE=.\WinAddrBook.cpp
# End Source File
# Begin Source File

SOURCE=.\WinAddrBook.h
# End Source File
# End Group
# Begin Group "杂项"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\About.h
# End Source File
# Begin Source File

SOURCE=.\ATCmdQ.cpp
# End Source File
# Begin Source File

SOURCE=.\ATCmdQ.h
# End Source File
# Begin Source File

SOURCE=.\ATResponse.cpp
# End Source File
# Begin Source File

SOURCE=.\ATResponse.h
# End Source File
# Begin Source File

SOURCE=.\BitmapEdit.cpp
# End Source File
# Begin Source File

SOURCE=.\BitmapEdit.h
# End Source File
# Begin Source File

SOURCE=.\BmpBkDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\BmpBkDialog.h
# End Source File
# Begin Source File

SOURCE=.\CDllLoader.h
# End Source File
# Begin Source File

SOURCE=.\CDMATime.cpp
# End Source File
# Begin Source File

SOURCE=.\CDMATime.h
# End Source File
# Begin Source File

SOURCE=.\ColorBtn.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorBtn.h
# End Source File
# Begin Source File

SOURCE=.\COMMonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\COMMonitor.h
# End Source File
# Begin Source File

SOURCE=.\comparser.cpp
# End Source File
# Begin Source File

SOURCE=.\comparser.h
# End Source File
# Begin Source File

SOURCE=.\CriticalSecMon.h
# End Source File
# Begin Source File

SOURCE=.\Data\Data.cpp
# End Source File
# Begin Source File

SOURCE=.\Data\Data.h
# End Source File
# Begin Source File

SOURCE=.\DevEnum.cpp
# End Source File
# Begin Source File

SOURCE=.\DevEnum.h
# End Source File
# Begin Source File

SOURCE=.\dlgmsg.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProgress.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgProgress.h
# End Source File
# Begin Source File

SOURCE=.\dlgprogressEx.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgprogressEx.h
# End Source File
# Begin Source File

SOURCE=.\SkinWin\EnBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\SkinWin\EnBitmap.h
# End Source File
# Begin Source File

SOURCE=.\EnhancedHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\EnhancedHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\enhancedlistctrl.cpp
# End Source File
# Begin Source File

SOURCE=.\enhancedlistctrl.h
# End Source File
# Begin Source File

SOURCE=.\GlobalDataCenter.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalDataCenter.h
# End Source File
# Begin Source File

SOURCE=.\LABEL.CPP
# End Source File
# Begin Source File

SOURCE=.\logservice.cpp
# End Source File
# Begin Source File

SOURCE=.\logservice.h
# End Source File
# Begin Source File

SOURCE=.\mobileatproto.cpp
# End Source File
# Begin Source File

SOURCE=.\mobileatproto.h
# End Source File
# Begin Source File

SOURCE=.\SkinWin\MyBitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\OwnDrawComboBox.cpp
# End Source File
# Begin Source File

SOURCE=.\registryex.cpp
# End Source File
# Begin Source File

SOURCE=.\ResizeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\RoundButton.cpp
# End Source File
# Begin Source File

SOURCE=.\RoundButton.h
# End Source File
# Begin Source File

SOURCE=.\SelectClassDlg.h
# End Source File
# Begin Source File

SOURCE=.\serialport.cpp
# End Source File
# Begin Source File

SOURCE=.\serialport.h
# End Source File
# Begin Source File

SOURCE=.\SetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetDlg.h
# End Source File
# Begin Source File

SOURCE=.\SetManager.cpp
# End Source File
# Begin Source File

SOURCE=.\SetManager.h
# End Source File
# Begin Source File

SOURCE=.\SetRingDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SetRingDlg.h
# End Source File
# Begin Source File

SOURCE=.\SkinWin\SkinButton.cpp
# End Source File
# Begin Source File

SOURCE=.\SkinWin\SkinButton.h
# End Source File
# Begin Source File

SOURCE=.\SkinWin\SkinWin.cpp
# End Source File
# Begin Source File

SOURCE=.\SkinWin\SkinWin.h
# End Source File
# Begin Source File

SOURCE=.\SortHeaderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\SortHeaderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\SortListCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\SortListCtrl.h
# End Source File
# Begin Source File

SOURCE=.\SplitterControl.cpp
# End Source File
# Begin Source File

SOURCE=.\SplitterControl.h
# End Source File
# Begin Source File

SOURCE=.\SpSilderCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\SpSilderCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Data\sqlite3.h
# End Source File
# Begin Source File

SOURCE=.\stringlink.cpp
# End Source File
# Begin Source File

SOURCE=.\stringlink.h
# End Source File
# Begin Source File

SOURCE=.\SkinWin\Subclass.cpp
# End Source File
# Begin Source File

SOURCE=.\SkinWin\Subclass.h
# End Source File
# Begin Source File

SOURCE=.\SunnyCat.h
# End Source File
# Begin Source File

SOURCE=.\SysSetDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SysSetDlg.h
# End Source File
# Begin Source File

SOURCE=.\TabSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\TabSheet.h
# End Source File
# Begin Source File

SOURCE=.\Thread.cpp
# End Source File
# Begin Source File

SOURCE=.\Thread.h
# End Source File
# Begin Source File

SOURCE=.\TurnCallDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TurnCallDlg.h
# End Source File
# Begin Source File

SOURCE=.\Util.cpp
# End Source File
# Begin Source File

SOURCE=.\Util.h
# End Source File
# End Group
# Begin Group "历史记录"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Data\ConnectRecord.cpp
# End Source File
# Begin Source File

SOURCE=.\Data\ConnectRecord.h
# End Source File
# Begin Source File

SOURCE=.\LABEL.H
# End Source File
# Begin Source File

SOURCE=.\NetStatByTime.cpp
# End Source File
# Begin Source File

SOURCE=.\RecordDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\RecordDialog.h
# End Source File
# Begin Source File

SOURCE=.\RecordManager.cpp
# End Source File
# Begin Source File

SOURCE=.\RecordManager.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# Begin Source File

SOURCE=.\output\resource\resource_cn.txt
# End Source File
# Begin Source File

SOURCE=.\output\resource\resource_en.txt
# End Source File
# End Target
# End Project
# Section SunnyCat : {E9E0751F-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcTick
# 	2:10:HeaderFile:vctick.h
# 	2:8:ImplFile:vctick.cpp
# End Section
# Section SunnyCat : {E9E07513-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcSeriesCollection
# 	2:10:HeaderFile:vcseriescollection.h
# 	2:8:ImplFile:vcseriescollection.cpp
# End Section
# Section SunnyCat : {E9E07523-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcValueScale
# 	2:10:HeaderFile:vcvaluescale.h
# 	2:8:ImplFile:vcvaluescale.cpp
# End Section
# Section SunnyCat : {E9E074EE-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcLightSources
# 	2:10:HeaderFile:vclightsources.h
# 	2:8:ImplFile:vclightsources.cpp
# End Section
# Section SunnyCat : {E9E074F2-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcView3d
# 	2:10:HeaderFile:vcview3d.h
# 	2:8:ImplFile:vcview3d.cpp
# End Section
# Section SunnyCat : {E9E0752A-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcDataPoints
# 	2:10:HeaderFile:vcdatapoints.h
# 	2:8:ImplFile:vcdatapoints.cpp
# End Section
# Section SunnyCat : {E9E074E2-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcBackdrop
# 	2:10:HeaderFile:vcbackdrop.h
# 	2:8:ImplFile:vcbackdrop.cpp
# End Section
# Section SunnyCat : {E9E074DE-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcFill
# 	2:10:HeaderFile:vcfill.h
# 	2:8:ImplFile:vcfill.cpp
# End Section
# Section SunnyCat : {E9E0750A-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcLabels
# 	2:10:HeaderFile:vclabels.h
# 	2:8:ImplFile:vclabels.cpp
# End Section
# Section SunnyCat : {BEF6E003-A874-101A-8BBA-00AA00300CAB}
# 	2:5:Class:COleFont
# 	2:10:HeaderFile:font.h
# 	2:8:ImplFile:font.cpp
# End Section
# Section SunnyCat : {E9E07517-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcCategoryScale
# 	2:10:HeaderFile:vccategoryscale.h
# 	2:8:ImplFile:vccategoryscale.cpp
# End Section
# Section SunnyCat : {E9E07527-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcPlot
# 	2:10:HeaderFile:vcplot.h
# 	2:8:ImplFile:vcplot.cpp
# End Section
# Section SunnyCat : {E9E0750E-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcDataPoint
# 	2:10:HeaderFile:vcdatapoint.h
# 	2:8:ImplFile:vcdatapoint.cpp
# End Section
# Section SunnyCat : {E9E074E6-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcFootnote
# 	2:10:HeaderFile:vcfootnote.h
# 	2:8:ImplFile:vcfootnote.cpp
# End Section
# Section SunnyCat : {E9E074CD-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcRect
# 	2:10:HeaderFile:vcrect.h
# 	2:8:ImplFile:vcrect.cpp
# End Section
# Section SunnyCat : {E9E074D6-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcLocation
# 	2:10:HeaderFile:vclocation.h
# 	2:8:ImplFile:vclocation.cpp
# End Section
# Section SunnyCat : {E9E07502-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcSeriesPosition
# 	2:10:HeaderFile:vcseriesposition.h
# 	2:8:ImplFile:vcseriesposition.cpp
# End Section
# Section SunnyCat : {E9E074D1-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcPen
# 	2:10:HeaderFile:vcpen.h
# 	2:8:ImplFile:vcpen.cpp
# End Section
# Section SunnyCat : {3A2B370A-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CMSChart
# 	2:10:HeaderFile:mschart.h
# 	2:8:ImplFile:mschart.cpp
# End Section
# Section SunnyCat : {E9E07506-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcStatLine
# 	2:10:HeaderFile:vcstatline.h
# 	2:8:ImplFile:vcstatline.cpp
# End Section
# Section SunnyCat : {E9E0751D-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcIntersection
# 	2:10:HeaderFile:vcintersection.h
# 	2:8:ImplFile:vcintersection.cpp
# End Section
# Section SunnyCat : {E9E074D5-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcTextLayout
# 	2:10:HeaderFile:vctextlayout.h
# 	2:8:ImplFile:vctextlayout.cpp
# End Section
# Section SunnyCat : {E9E07511-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcSeries
# 	2:10:HeaderFile:vcseries.h
# 	2:8:ImplFile:vcseries.cpp
# End Section
# Section SunnyCat : {E9E074DC-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcShadow
# 	2:10:HeaderFile:vcshadow.h
# 	2:8:ImplFile:vcshadow.cpp
# End Section
# Section SunnyCat : {E9E074EC-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcLightSource
# 	2:10:HeaderFile:vclightsource.h
# 	2:8:ImplFile:vclightsource.cpp
# End Section
# Section SunnyCat : {E9E074FC-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcWall
# 	2:10:HeaderFile:vcwall.h
# 	2:8:ImplFile:vcwall.cpp
# End Section
# Section SunnyCat : {E9E074E0-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcFrame
# 	2:10:HeaderFile:vcframe.h
# 	2:8:ImplFile:vcframe.cpp
# End Section
# Section SunnyCat : {E9E07521-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcAxisScale
# 	2:10:HeaderFile:vcaxisscale.h
# 	2:8:ImplFile:vcaxisscale.cpp
# End Section
# Section SunnyCat : {E9E074F0-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcLight
# 	2:10:HeaderFile:vclight.h
# 	2:8:ImplFile:vclight.cpp
# End Section
# Section SunnyCat : {E9E07515-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcAxisTitle
# 	2:10:HeaderFile:vcaxistitle.h
# 	2:8:ImplFile:vcaxistitle.cpp
# End Section
# Section SunnyCat : {E9E07525-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcAxis
# 	2:10:HeaderFile:vcaxis.h
# 	2:8:ImplFile:vcaxis.cpp
# End Section
# Section SunnyCat : {E9E0750C-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcDataPointLabel
# 	2:10:HeaderFile:vcdatapointlabel.h
# 	2:8:ImplFile:vcdatapointlabel.cpp
# End Section
# Section SunnyCat : {E9E074C9-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcCoor
# 	2:10:HeaderFile:vccoor.h
# 	2:8:ImplFile:vccoor.cpp
# End Section
# Section SunnyCat : {E9E074E4-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcTitle
# 	2:10:HeaderFile:vctitle.h
# 	2:8:ImplFile:vctitle.cpp
# End Section
# Section SunnyCat : {E9E074F4-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcPlotBase
# 	2:10:HeaderFile:vcplotbase.h
# 	2:8:ImplFile:vcplotbase.cpp
# End Section
# Section SunnyCat : {E9E074CB-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcLCoor
# 	2:10:HeaderFile:vclcoor.h
# 	2:8:ImplFile:vclcoor.cpp
# End Section
# Section SunnyCat : {E9E074D8-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcFont
# 	2:10:HeaderFile:vcfont.h
# 	2:8:ImplFile:vcfont.cpp
# End Section
# Section SunnyCat : {E9E07504-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcSeriesMarker
# 	2:10:HeaderFile:vcseriesmarker.h
# 	2:8:ImplFile:vcseriesmarker.cpp
# End Section
# Section SunnyCat : {E9E074CF-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcColor
# 	2:10:HeaderFile:vccolor.h
# 	2:8:ImplFile:vccolor.cpp
# End Section
# Section SunnyCat : {E9E0751B-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcAxisGrid
# 	2:10:HeaderFile:vcaxisgrid.h
# 	2:8:ImplFile:vcaxisgrid.cpp
# End Section
# Section SunnyCat : {E9E074D3-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcMarker
# 	2:10:HeaderFile:vcmarker.h
# 	2:8:ImplFile:vcmarker.cpp
# End Section
# Section SunnyCat : {E9E074E8-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcLegend
# 	2:10:HeaderFile:vclegend.h
# 	2:8:ImplFile:vclegend.cpp
# End Section
# Section SunnyCat : {E9E074DA-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcBrush
# 	2:10:HeaderFile:vcbrush.h
# 	2:8:ImplFile:vcbrush.cpp
# End Section
# Section SunnyCat : {E9E074EA-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcDataGrid
# 	2:10:HeaderFile:vcdatagrid.h
# 	2:8:ImplFile:vcdatagrid.cpp
# End Section
# Section SunnyCat : {E9E074FA-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcWeighting
# 	2:10:HeaderFile:vcweighting.h
# 	2:8:ImplFile:vcweighting.cpp
# End Section
# Section SunnyCat : {3A2B370C-BA0A-11D1-B137-0000F8753F5D}
# 	2:21:DefaultSinkHeaderFile:mschart.h
# 	2:16:DefaultSinkClass:CMSChart
# End Section
# Section SunnyCat : {E9E07508-BA0A-11D1-B137-0000F8753F5D}
# 	2:5:Class:CVcLabel
# 	2:10:HeaderFile:vclabel.h
# 	2:8:ImplFile:vclabel.cpp
# End Section
