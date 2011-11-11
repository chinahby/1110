; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CExecuteDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "execute.h"
LastPage=0

ClassCount=3
Class1=CExecuteApp
Class2=CExecuteDlg

ResourceCount=2
Resource1=IDD_EXECUTE_DIALOG
Class3=CSelComDlg
Resource2=IDD_DIALOG_COM

[CLS:CExecuteApp]
Type=0
BaseClass=CWinApp
HeaderFile=Execute.h
ImplementationFile=Execute.cpp
LastObject=CExecuteApp

[CLS:CExecuteDlg]
Type=0
BaseClass=CDialog
HeaderFile=ExecuteDlg.h
ImplementationFile=ExecuteDlg.cpp
Filter=W
LastObject=CExecuteDlg

[DLG:IDD_EXECUTE_DIALOG]
Type=1
Class=CExecuteDlg
ControlCount=1
Control1=IDC_STATIC,static,1342177283

[DLG:IDD_DIALOG_COM]
Type=1
Class=CSelComDlg
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDC_COMBO_SELCOM,combobox,1344339970
Control3=IDC_STATIC_SELECT,static,1342308352

[CLS:CSelComDlg]
Type=0
HeaderFile=SelComDlg.h
ImplementationFile=SelComDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=IDC_COMBO_SELCOM
VirtualFilter=dWC

