// CallDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SunnyCat.h"
#include "CallDialog.h"
#include "GlobalDataCenter.h"
#include "SunnycatDlg.h"
#include "Util.h"
#include "NewCardDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CCallDialog *pCallDialog = NULL;

/////////////////////////////////////////////////////////////////////////////
// CCallDialog dialog

CCallDialog::CCallDialog(CWnd* pParent /*=NULL*/)
: CBmpBkDialog(CCallDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCallDialog)
	
	m_strDialNum = _T("");
	//}}AFX_DATA_INIT
	pCallDialog = this ;

	m_bFirst = TRUE;
}


void CCallDialog::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCallDialog)
	DDX_Control(pDX, IDC_STATIC_CALLIN6, m_ctrlCall6);
	DDX_Control(pDX, IDC_STATIC_CALLIN5, m_ctrlCall5);
	DDX_Control(pDX, IDC_STATIC_CALLIN4, m_ctrlCall4);
	DDX_Control(pDX, IDC_STATIC_CALLIN3, m_ctrlCall3);
	DDX_Control(pDX, IDC_STATIC_CALLIN2, m_ctrlCall2);
	DDX_Control(pDX, IDC_STATIC_CALLIN1, m_ctrlCall1);
	DDX_Control(pDX, IDC_STATIC_CALLIN0, m_ctrlCall0);
	DDX_Control(pDX, IDC_STATIC_SOUND, m_ctrlSound);
	DDX_Control(pDX, IDC_BUTTON_DIAL_NUM, m_ctrlDail);
	DDX_Control(pDX, IDC_BUTTON_HANGUP, m_ctrlHangup);
	DDX_Control(pDX, IDC_BUTTON_DIAL_CLEAR, m_ctrlClear);
	DDX_Control(pDX, IDC_BUTTON_9, m_ctrl9);
	DDX_Control(pDX, IDC_BUTTON_8, m_ctrl8);
	DDX_Control(pDX, IDC_BUTTON_7, m_ctrl7);
	DDX_Control(pDX, IDC_BUTTON_6, m_ctrl6);
	DDX_Control(pDX, IDC_BUTTON_5, m_ctrl5);
	DDX_Control(pDX, IDC_BUTTON_4, m_ctrl4);
	DDX_Control(pDX, IDC_BUTTON_3, m_ctrl3);
	DDX_Control(pDX, IDC_BUTTON_2, m_ctrl2);
	DDX_Control(pDX, IDC_BUTTON_11, m_ctrl11);
	DDX_Control(pDX, IDC_BUTTON_10, m_ctrl10);
	DDX_Control(pDX, IDC_BUTTON_1, m_ctrl1);
	DDX_Control(pDX, IDC_BUTTON_0, m_ctrl0);
	DDX_Text(pDX, IDC_EDIT_DAIL_NUM, m_strDialNum);
	DDV_MaxChars(pDX, m_strDialNum, 100);
	DDX_Control(pDX, IDC_BUTTON_13, m_13Btn);
	DDX_Control(pDX, IDC_BUTTON_14, m_14Btn);
	DDX_Control(pDX, IDC_BUTTON_ADDNUM, m_AddNumBtn);
	//}}AFX_DATA_MAP
}


	BEGIN_MESSAGE_MAP(CCallDialog, CBmpBkDialog)
	//{{AFX_MSG_MAP(CCallDialog)
	ON_BN_CLICKED(IDC_BUTTON_DIAL_NUM, OnButtonDialNum)
	ON_BN_CLICKED(IDC_BUTTON_DIAL_CLEAR, OnButtonDialClear)
	ON_BN_CLICKED(IDC_BUTTON_1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON_2, OnButton2)
	ON_BN_CLICKED(IDC_BUTTON_3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON_4, OnButton4)
	ON_BN_CLICKED(IDC_BUTTON_5, OnButton5)
	ON_BN_CLICKED(IDC_BUTTON_6, OnButton6)
	ON_BN_CLICKED(IDC_BUTTON_7, OnButton7)
	ON_BN_CLICKED(IDC_BUTTON_8, OnButton8)
	ON_BN_CLICKED(IDC_BUTTON_9, OnButton9)
	ON_BN_CLICKED(IDC_BUTTON_10, OnButton10)
	ON_BN_CLICKED(IDC_BUTTON_0, OnButton0)
	ON_BN_CLICKED(IDC_BUTTON_11, OnButton11)
	ON_WM_HSCROLL()
	ON_WM_KEYDOWN()
	ON_BN_CLICKED(IDC_BUTTON_HANGUP, OnButtonHangup)
	ON_WM_SIZE()
	ON_WM_CHAR()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_ADDNUM, OnButtonAddnum)
	ON_BN_CLICKED(IDC_BUTTON_13, OnButton13)
	ON_BN_CLICKED(IDC_BUTTON_14, OnButton14)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCallDialog message handlers


BOOL CCallDialog::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();

	/**************************************************/// modify ui
	CRect temprect(66, 0, 431, 251);
	CWnd::SetWindowPos(NULL, 0, 0, temprect.Width(), temprect.Height(), SWP_NOZORDER|SWP_NOMOVE);
	/**************************************************/

	CString strpath = GetAppPath();
	CString strLanguage = GetLanguage();

	m_ctrl0.SetBitmapPath(strpath+"skin\\NUMBER\\0.bmp",strpath+"skin\\NUMBER\\0_down.bmp",strpath+"skin\\NUMBER\\0_over.bmp");
	m_ctrl1.SetBitmapPath(strpath+"skin\\NUMBER\\1.bmp",strpath+"skin\\NUMBER\\1_down.bmp",strpath+"skin\\NUMBER\\1_over.bmp");
	m_ctrl2.SetBitmapPath(strpath+"skin\\NUMBER\\2.bmp",strpath+"skin\\NUMBER\\2_down.bmp",strpath+"skin\\NUMBER\\2_over.bmp");
	m_ctrl3.SetBitmapPath(strpath+"skin\\NUMBER\\3.bmp",strpath+"skin\\NUMBER\\3_down.bmp",strpath+"skin\\NUMBER\\3_over.bmp");
	m_ctrl4.SetBitmapPath(strpath+"skin\\NUMBER\\4.bmp",strpath+"skin\\NUMBER\\4_down.bmp",strpath+"skin\\NUMBER\\4_over.bmp");
	m_ctrl5.SetBitmapPath(strpath+"skin\\NUMBER\\5.bmp",strpath+"skin\\NUMBER\\5_down.bmp",strpath+"skin\\NUMBER\\5_over.bmp");
	m_ctrl6.SetBitmapPath(strpath+"skin\\NUMBER\\6.bmp",strpath+"skin\\NUMBER\\6_down.bmp",strpath+"skin\\NUMBER\\6_over.bmp");
	m_ctrl7.SetBitmapPath(strpath+"skin\\NUMBER\\7.bmp",strpath+"skin\\NUMBER\\7_down.bmp",strpath+"skin\\NUMBER\\7_over.bmp");
	m_ctrl8.SetBitmapPath(strpath+"skin\\NUMBER\\8.bmp",strpath+"skin\\NUMBER\\8_down.bmp",strpath+"skin\\NUMBER\\8_over.bmp");
	m_ctrl9.SetBitmapPath(strpath+"skin\\NUMBER\\9.bmp",strpath+"skin\\NUMBER\\9_down.bmp",strpath+"skin\\NUMBER\\9_over.bmp");
	m_ctrl10.SetBitmapPath(strpath+"skin\\NUMBER\\10.bmp",strpath+"skin\\NUMBER\\10_down.bmp",strpath+"skin\\NUMBER\\10_over.bmp");

	m_ctrl11.SetBitmapPath(strpath+"skin\\NUMBER\\11.bmp",strpath+"skin\\NUMBER\\11_down.bmp",strpath+"skin\\NUMBER\\11_over.bmp");
	m_13Btn.SetBitmapPath(strpath+"skin\\NUMBER\\12.bmp",strpath+"skin\\NUMBER\\12_down.bmp",strpath+"skin\\NUMBER\\12_over.bmp");
	m_14Btn.SetBitmapPath(strpath+"skin\\NUMBER\\13.bmp",strpath+"skin\\NUMBER\\13_down.bmp",strpath+"skin\\NUMBER\\13_over.bmp");
	m_AddNumBtn.SetBitmapPath(strpath+"skin\\NUMBER\\14.bmp",strpath+"skin\\NUMBER\\14_down.bmp",strpath+"skin\\NUMBER\\14_over.bmp");
	m_ctrlClear.SetBitmapPath(strpath+"skin\\NUMBER\\c.bmp",strpath+"skin\\NUMBER\\c_down.bmp",strpath+"skin\\NUMBER\\c_over.bmp");
	m_ctrlDail.SetBitmapPath(strpath+"skin\\NUMBER\\Call.bmp",strpath+"skin\\NUMBER\\Call_down.bmp",strpath+"skin\\NUMBER\\Call_over.bmp");
	m_ctrlHangup.SetBitmapPath(strpath+"skin\\NUMBER\\hangup.bmp",strpath+"skin\\NUMBER\\hangup_down.bmp",strpath+"skin\\NUMBER\\hangup_over.bmp");


	m_ctrlDail.SetHitOver();
	m_ctrlHangup.SetHitOver();
	m_ctrlClear.SetHitOver();
	m_ctrl9.SetHitOver();
	m_ctrl8.SetHitOver();
	m_ctrl7.SetHitOver();
	m_ctrl6.SetHitOver();
	m_ctrl5.SetHitOver();
	m_ctrl4.SetHitOver();
	m_ctrl3.SetHitOver();
	m_ctrl2.SetHitOver();
	m_ctrl11.SetHitOver();
	m_ctrl10.SetHitOver();
	m_ctrl1.SetHitOver();
	m_ctrl0.SetHitOver();
	m_12Btn.SetHitOver();
	m_13Btn.SetHitOver();
	m_14Btn.SetHitOver();
	m_AddNumBtn.SetHitOver();

	m_editDial.SetTxtColor(RGB(62,137,190),RGB(255,255,255));
	m_editDial.SetSelTxtColor(RGB(221,241,251),RGB(0,0,0));
	m_editDial.SubclassDlgItem(IDC_EDIT_DAIL_NUM,this) ;
	m_editDial.SetFontSize(15);
	
	//	DWORD LValue,RValue;
	//	GetWaveVolume(&LValue,&RValue);
	//m_editDial.SetFocus();
	//=======================
	ShowCallInImag(0);
	//=======================

// delete for huamei 20100128	
/*	
	m_ctrlSound.InitSlider() ;
	m_ctrlSound.SetSLBitmap(IDB_BITMAP_VOLBK,IDB_BITMAP_VOLBTN) ;
	m_ctrlSound.SetSLRange(0,0xFFFF);
	DWORD pos = 21600;
	m_ctrlSound.SetSLPos(pos);
*/	//	m_ctrlDail.SetBitmapID(IDB_BITMAP_DIAL, IDB_BITMAP_DIALDOWN);
	// TODO: Add extra initialization here

	m_ctrlClear.SetToolTipTxt(LoadStringEx(IDS_CALL_CLEAR));
	m_ctrlDail.SetToolTipTxt(LoadStringEx(IDS_CALL_DAIL));
	m_ctrlHangup.SetToolTipTxt(LoadStringEx(IDS_CALL_HANGUP));
	m_13Btn.SetToolTipTxt(LoadStringEx(962));
	m_14Btn.SetToolTipTxt(LoadStringEx(IDS_SAVENUM));
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCallDialog::OnOK()
{
	return;
}

void CCallDialog::OnButtonDialNum() 
{
	//if(!((CSunnyCatDlg*)m_pParent)->m_MobileManager.isAbleCom)
	if(!((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.isAbleCom)
	{
	//	ShowMessageBox(IDS_NO_SC700_CARD);
		GMessageBox(LoadStringEx(IDS_NO_SC700_CARD));
		return;
	}
	/*************************************************************
	if(((CSunnyCatDlg*)AfxGetMainWnd())->m_isConnect)
	{
	//	ShowMessageBox(IDS_PROMPT_ONLINE);
		GMessageBox(LoadStringEx(IDS_PROMPT_ONLINE));
		return;
	}
	/*************************************************************/
	if(((CSunnyCatDlg *)AfxGetMainWnd())->m_bResetting)
	{
		GMessageBox(LoadStringEx(IDS_SEARCH_NET));
		return;
	}
	if(((CSunnyCatDlg *)AfxGetMainWnd())->m_InitAT.IsInitRunning())
	{
		GMessageBox(LoadStringEx(IDS_INIT_RUNNING));
		return;
	}

	m_editDial.SetFocus();
	CallStatus m_status;
	m_status = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_pCallMgr->GetCallStatus();
	if(m_status == CALLSTATE_IDLE)
	{
		TCHAR keyTable[] = 
		{
			TEXT('+'),TEXT('0'),TEXT('1'),TEXT('2'),TEXT('3'),TEXT('4'),TEXT('5'),
			TEXT('6'),TEXT('7'),TEXT('8'),TEXT('9'),TEXT('*'),TEXT('#'),TEXT('p'),TEXT('P')
		};
		UpdateData(true);
		int length = m_strDialNum.GetLength();
		//不允许拨打小于三位的号码
		if(length < 3)
			return;
		//不允许拨打非法号码
		for(int i=0; i<length; i++)
		{
			TCHAR key = m_strDialNum[i];
			if(!EnableKey(key, keyTable))
			{
			//	ShowMessageBox(IDS_STRING_ERRORNUM);
				GMessageBox(LoadStringEx(IDS_STRING_ERRORNUM));
				return;
			}
		}
		m_editDial.SetSel(length, length);
		((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_pCallMgr->Call(m_strDialNum);
//		((CSunnyCatDlg*)AfxGetMainWnd())->StartVoice();
	}
	
	if(m_status == CALLSTATE_INCOMING)
	{		
		((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_pCallMgr->AcceptIncomingCall();
	}	
	
	SetTimer(1,200,0);

	CString strName;
	int nID;	
	CSunnyCatDlg* pDlg = NULL;

	strName.Empty();
	pDlg = (CSunnyCatDlg*)AfxGetMainWnd();
	ASSERT(pDlg != NULL);
	pDlg->FindNameInPhonebook(m_strDialNum,strName,nID);
	if(strName.IsEmpty())		
		pDlg->FlashInfoBar(m_strDialNum);
	else
		pDlg->FlashInfoBar(strName);
	// TODO: Add your control notification handler code here	
}

void CCallDialog::SetParenWnd(CWnd * pParent)
{
	m_pParent = pParent;
}

void CCallDialog::OnButtonDialClear() 
{
	// TODO: Add your control notification handler code here
//	UpdateData(true);
	if(m_tEndTime - m_tBeginTime > LONG_CLEAR_TIME)
	{
		m_strDialNum = _T("");
		UpdateData(false);
		return;
	}

// 	if(length > 0)
//		m_strDialNum = m_strDialNum.Left(length - 1);
// 	UpdateData(false);
	CWnd* pWnd = GetDlgItem(IDC_EDIT_DAIL_NUM);


	if(pWnd != NULL)
		pWnd->PostMessage(WM_KEYDOWN,VK_BACK,0);
	
}


void CCallDialog::OnButton1() 
{
	// TODO: Add your control notification handler code here
	PressKey(TEXT('1'));		
}

void CCallDialog::OnButton2() 
{
	// TODO: Add your control notification handler code here
	PressKey(TEXT('2'));
}

void CCallDialog::OnButton3() 
{
	// TODO: Add your control notification handler code here
	PressKey(TEXT('3'));
}

void CCallDialog::OnButton4() 
{
	// TODO: Add your control notification handler code here
	PressKey(TEXT('4'));
}

void CCallDialog::OnButton5() 
{
	// TODO: Add your control notification handler code here
	PressKey(TEXT('5'));
}

void CCallDialog::OnButton6() 
{
	// TODO: Add your control notification handler code here
	PressKey(TEXT('6'));
}

void CCallDialog::OnButton7() 
{
	// TODO: Add your control notification handler code here
	PressKey(TEXT('7'));
}

void CCallDialog::OnButton8() 
{
	// TODO: Add your control notification handler code here
	PressKey(TEXT('8'));
}

void CCallDialog::OnButton9() 
{
	// TODO: Add your control notification handler code here
	PressKey(TEXT('9'));
}

//*号
void CCallDialog::OnButton10() 
{
	// TODO: Add your control notification handler code here
	PressKey(TEXT('*'));
}

void CCallDialog::OnButton0() 
{
	// TODO: Add your control notification handler code here
	PressKey(TEXT('0'));
}

//#号
void CCallDialog::OnButton11() 
{
	// TODO: Add your control notification handler code here
	PressKey(TEXT('#'));
}


void CCallDialog::PressKey(TCHAR key)
{
	CallStatus callStatus = ((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.m_pCallMgr->GetCallStatus();
	UpdateData(true);
	if(m_strDialNum.GetLength() > 47)
		return;
	int iPos1,iPos2;
	m_editDial.GetSel(iPos1,iPos2); 
	int len = m_strDialNum.GetLength();
	
	if(callStatus == CALLSTATE_IDLE)
	{
		CString strTemp = "";
		strTemp = m_strDialNum.Left(iPos2);
		strTemp += key;
		strTemp += m_strDialNum.Right(len - iPos2);
		m_strDialNum = strTemp;
	}
	if(callStatus == CALLSTATE_OUT_TALKING)
	{
		CString strTemp = "";
		strTemp = m_strDialNum.Left(iPos2);
		strTemp += key;
		strTemp += m_strDialNum.Right(len - iPos2);
		m_strDialNum = strTemp;
		((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_pCallMgr->SendDTMF(key);
	}
	UpdateData(false);
	
    m_editDial.SetSel(iPos2+1,iPos2+1);  

}


void CCallDialog::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	if(pScrollBar->m_hWnd == m_ctrlSound.m_hWnd)
	{
		UINT Pos = m_ctrlSound.GetSLPos();
		
		SetWaveVolume(nPos,Pos);
		int voice = 0;

		CSunnyCatDlg* pDlg = (CSunnyCatDlg*)(AfxGetMainWnd());
		//10个音量级
		for(int i = 1; i <= 4 ; i++)
		{
			if(Pos == 0)
				pDlg->m_MobileManager.SetVolume(1);
			
			if(0 < Pos && Pos <= 6553*i)
			{
				voice = i;
				pDlg->m_MobileManager.SetVolume(voice);
				return;				
			}			
		}	
		
	}
	CBmpBkDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

/*//键盘到按钮的映射
struct KeyButtonMap
{
WPARAM	keycode;	//键盘码
bool	needShift;	//是否与Shift键组合
char	keybutton;	//按钮键值	
};

  static const KeyButtonMap keyMap[] = 
  {
  {'0', false, '0'},
  {VK_NUMPAD0, false, '0'},
  
	{'1', false, '1'},
	{VK_NUMPAD1, false, '1'},
	
	  {'2', false, '2'},
	  {VK_NUMPAD2, false, '2'},
	  
		{'3', false, '3'},
		{VK_NUMPAD3, false, '3'},
		
		  {'4', false, '4'},
		  {VK_NUMPAD4, false, '4'},
		  
			{'5', false, '5'},
			{VK_NUMPAD5, false, '5'},
			
			  {'6', false, '6'},
			  {VK_NUMPAD6, false, '6'},
			  
				{'7', false, '7'},
				{VK_NUMPAD7, false, '7'},
				
				  {'8', false, '8'},
				  {VK_NUMPAD8, false, '8'},
				  
					{'9', false, '9'},
					{VK_NUMPAD9, false, '9'},
					
					  {'R', false, 'R'},
					  {'R', true, 'R'},
					  
						{VK_BACK, false, 'C'},
						{VK_DELETE, false, 'C'},
						
						  {VK_RETURN, false, 'D'},
						  
							{VK_ESCAPE, false, 'H'},
							
							  {'8', true, '*'},//Shift+'8'
							  {VK_MULTIPLY, false, '*'},
							  
								{'3', true, '#'},//Shift+'3'
								
								  {VK_ADD, false, '+'},
								  {187, true, '+'},//Shift+'='
								  };
								  
*/
void CCallDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CBmpBkDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CCallDialog::OnButtonHangup() 
{
	
	// TODO: Add your control notification handler code here
//	CSunnyCatDlg* pDlg = NULL;
//	pDlg = (CSunnyCatDlg*)m_pParent;
	((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.m_pCallMgr->EndCall(true);
	//
	ShowCallInImag(0);
	((CSunnyCatDlg *)AfxGetMainWnd())->FlashInfoBar("");
	m_strDialNum = "";
	UpdateData(false);
	
}

void CCallDialog::ShowCallInImag(int id)
{
	return;
	//信号强度提示
	CStatic* callImag[] = 
	{
		&m_ctrlCall0,
		&m_ctrlCall1,
		&m_ctrlCall2,
		&m_ctrlCall3,
		&m_ctrlCall4,
		&m_ctrlCall5,
		&m_ctrlCall6,	
	};

/*	CRect rcImag;
	m_ctrlCall0.GetWindowRect(rcImag);
	ScreenToClient(rcImag);
*/
//	CRect ImagShow(160, 5, 280, 80);
	for(size_t i=0; i<(sizeof(callImag)/sizeof(callImag[0])); ++i)
	{
//		callImag[i]->MoveWindow(ImagShow);
		callImag[i]->ShowWindow(false);
	}	

	callImag[id]->ShowWindow(true);

}



UINT PlayImagProc(LPVOID pParam)
{
	OVERLAPPED os;

	CCallDialog *callDlg = (CCallDialog *)pParam;
	if(callDlg == NULL)
		return -1;
	os.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (os.hEvent == NULL)
	{
//		AfxMessageBox("Can't create event object!");
		GMessageBox(TEXT("Can't create event object!"));
		return (UINT)-1;
	}
	for(int i = 0; i < 7; i++)
	{		
		callDlg->ShowCallInImag(i);		
		Sleep(150);
		callDlg->ShowCallInImag(0);
	}

	return 0;
}

void CCallDialog::PlayImagThread()
{
	AfxBeginThread(PlayImagProc,this);
}

BOOL CCallDialog::PreTranslateMessage(MSG *pMsg)
{
	
	
	if(pMsg->message == WM_LBUTTONDOWN) 
	{
		int xPos = LOWORD(pMsg->lParam);  // horizontal position of cursor 
		int yPos = HIWORD(pMsg->lParam);  // vertical position of cursor 
        time( &m_tBeginTime );                
	}
	else if(pMsg->message == WM_LBUTTONUP) 
	{
		int xPos = LOWORD(pMsg->lParam);  // horizontal position of cursor 
		int yPos = HIWORD(pMsg->lParam);  // vertical position of cursor 
		
		
        time( &m_tEndTime );                
	}
	
//	if(pMsg->message == WM_CHAR || pMsg->message == WM_KEYDOWN)
// 		m_editDial.SetFocus() ;

	bool handle=FALSE;

	/*if(pMsg->message == WM_KEYDOWN)
	{
		m_editDial.SetFocus() ;
	
		 switch(pMsg->wParam)
		 { 
			 case VK_NUMPAD0:
			 case 48:			
			 case VK_NUMPAD1:
			 case 49:			 
			 case VK_NUMPAD2:
			 case 50:			
			 case VK_NUMPAD3:
			 case 51:			 
			 case VK_NUMPAD4:
			 case 52:			 
			 case VK_NUMPAD5:
			 case 53:			 
			 case VK_NUMPAD6:
			 case 54:			
			 case VK_NUMPAD7:
			 case 55:			
			 case VK_NUMPAD8:
			 case 56:			
			 case VK_NUMPAD9:
			 case 57:			
			 case 42:		
			 case 35:		
			 case 13:		
			 case 27:		
			 default: 
			  break; 
			return TRUE;	//拦截所以按下的键.可以加语句实现部分拦截
		 }
		
	}
*/
	if(pMsg->message == WM_KEYDOWN)
	{
		m_editDial.SetFocus();
		switch(pMsg->wParam)
		{ 
		 case 13:
			 OnButtonDialNum();
			 break;
		 case 27:
			 OnButtonHangup();
			 m_editDial.SetSel(0,0);
			 break;
		}
	}
	if(pMsg->message == WM_CHAR)//处理键释放
	{
		m_editDial.SetFocus();
		 switch(pMsg->wParam)
		 { 
		 case VK_NUMPAD0:
		 case 48:
			 OnButton0();
			 break;
		 case VK_NUMPAD1:
		 case 49:
			OnButton1();
			 break;
		 case VK_NUMPAD2:
		 case 50:
			  OnButton2();
			 break;
		 case VK_NUMPAD3:
		 case 51:
			  OnButton3();
			 break;
		 case VK_NUMPAD4:
		 case 52:
			  OnButton4();
			 break;
		 case VK_NUMPAD5:
		 case 53:
			  OnButton5();
			 break;
		 case VK_NUMPAD6:
		 case 54:
			  OnButton6();
			 break;
		 case VK_NUMPAD7:
		 case 55:
			  OnButton7();
			 break;
		 case VK_NUMPAD8:
		 case 56:
			  OnButton8();
			 break;
		 case VK_NUMPAD9:
		 case 57:
			  OnButton9();
			 break;
		 case 42:
			 OnButton10();
			 break;
		 case 35:
			 OnButton11();
			 break;

		 default: 
			 return CDialog::PreTranslateMessage(pMsg);
		
		 }
		 return TRUE;
		 
	}
    return CDialog::PreTranslateMessage(pMsg);
}

void CCallDialog::Call(CString number)
{
	m_strDialNum = number;
	UpdateData(false);
	OnButtonDialNum();
}

void CCallDialog::OnSize(UINT nType, int cx, int cy) 
{

	CRect rcEdit;
	CRect rcBtn;
	int nWidth = 0;
	int nHeight = 0;


	if(m_bFirst)
	{
		static int i=0;
		if(i >= 2)
			m_bFirst = false;
		i++;
		return;
	}
	if(m_editDial.GetSafeHwnd() != NULL)
	{
		m_editDial.GetWindowRect(rcEdit);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcEdit, 2 );
		m_ctrl0.GetWindowRect(rcBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
		nWidth = rcEdit.Width();
		nHeight = rcEdit.Height();
		rcEdit.left = (cx-nWidth)/2;
		rcEdit.right = rcEdit.left+rcBtn.Width()*5+5*4;
		rcEdit.top = (cy-(rcBtn.bottom-rcEdit.top))/3;
		rcEdit.bottom = rcEdit.top+nHeight;
		m_editDial.MoveWindow(&rcEdit);
	}
	int n1Bottom = 0;
	int n1Width = 0;
	if(m_ctrl1.GetSafeHwnd() != NULL)
	{
		m_ctrl1.GetWindowRect(rcBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
		int nBtnWidth = rcBtn.Width();
		int nBtnHeight = rcBtn.Height();
		n1Width = nBtnWidth;
		rcBtn.left = (cx-nWidth)/2;
		rcBtn.right = rcBtn.left+nBtnWidth;
		rcBtn.top = rcEdit.bottom+10;
		rcBtn.bottom = rcBtn.top+nBtnHeight;
		n1Bottom = rcBtn.bottom;
		m_ctrl1.MoveWindow(rcBtn);
	}
	int n4Bottom=0;
	if(m_ctrl4.GetSafeHwnd() != NULL)
	{
		m_ctrl4.GetWindowRect(rcBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
		int nBtnWidth = rcBtn.Width();
		int nBtnHeight = rcBtn.Height();
		rcBtn.left = (cx-nWidth)/2;
		rcBtn.right = rcBtn.left+nBtnWidth;
		rcBtn.top = n1Bottom+5;
		rcBtn.bottom = rcBtn.top+nBtnHeight;
		n4Bottom = rcBtn.bottom;
		m_ctrl4.MoveWindow(rcBtn);
	}
	int n7Bottom=0;
	if(m_ctrl7.GetSafeHwnd() != NULL)
	{
		m_ctrl7.GetWindowRect(rcBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
		int nBtnWidth = rcBtn.Width();
		int nBtnHeight = rcBtn.Height();
		rcBtn.left = (cx-nWidth)/2;
		rcBtn.right = rcBtn.left+nBtnWidth;
		rcBtn.top = n4Bottom+5;
		rcBtn.bottom = rcBtn.top+nBtnHeight;
		n7Bottom = rcBtn.bottom;
		m_ctrl7.MoveWindow(rcBtn);
	}
	int n10Bottom = 0;
	if(m_ctrl10.GetSafeHwnd() != NULL)
	{
		m_ctrl10.GetWindowRect(rcBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
		int nBtnWidth = rcBtn.Width();
		int nBtnHeight = rcBtn.Height();
		rcBtn.left = (cx-nWidth)/2;
		rcBtn.right = rcBtn.left+nBtnWidth;
		rcBtn.top = n7Bottom+5;
		rcBtn.bottom = rcBtn.top+nBtnHeight;
		n10Bottom = rcBtn.bottom;
		m_ctrl10.MoveWindow(rcBtn);
	}


	if(m_ctrl2.GetSafeHwnd() != NULL)
	{
		m_ctrl2.GetWindowRect(rcBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
		int nBtnWidth = rcBtn.Width();
		int nBtnHeight = rcBtn.Height();
		rcBtn.left = (cx-nWidth)/2+nBtnWidth+5;
		rcBtn.right = rcBtn.left+nBtnWidth;
		rcBtn.top = rcEdit.bottom+10;
		rcBtn.bottom = rcBtn.top+nBtnHeight;
		m_ctrl2.MoveWindow(rcBtn);
	}
	if(m_ctrl5.GetSafeHwnd() != NULL)
	{
		m_ctrl5.GetWindowRect(rcBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
		int nBtnWidth = rcBtn.Width();
		int nBtnHeight = rcBtn.Height();
		rcBtn.left = (cx-nWidth)/2+nBtnWidth+5;
		rcBtn.right = rcBtn.left+nBtnWidth;
		rcBtn.top = n1Bottom+5;
		rcBtn.bottom = rcBtn.top+nBtnHeight;
		m_ctrl5.MoveWindow(rcBtn);
	}
	if(m_ctrl8.GetSafeHwnd() != NULL)
	{
		m_ctrl8.GetWindowRect(rcBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
		int nBtnWidth = rcBtn.Width();
		int nBtnHeight = rcBtn.Height();
		rcBtn.left = (cx-nWidth)/2+nBtnWidth+5;
		rcBtn.right = rcBtn.left+nBtnWidth;
		rcBtn.top = n4Bottom+5;
		rcBtn.bottom = rcBtn.top+nBtnHeight;
		m_ctrl8.MoveWindow(rcBtn);
	}
	if(m_ctrl0.GetSafeHwnd() != NULL)
	{
		m_ctrl0.GetWindowRect(rcBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
		int nBtnWidth = rcBtn.Width();
		int nBtnHeight = rcBtn.Height();
		rcBtn.left = (cx-nWidth)/2+nBtnWidth+5;
		rcBtn.right = rcBtn.left+nBtnWidth;
		rcBtn.top = n7Bottom+5;
		rcBtn.bottom = rcBtn.top+nBtnHeight;
		m_ctrl0.MoveWindow(rcBtn);
	}
	if(m_ctrl3.GetSafeHwnd() != NULL)
	{
		m_ctrl3.GetWindowRect(rcBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
		int nBtnWidth = rcBtn.Width();
		int nBtnHeight = rcBtn.Height();
		rcBtn.left = (cx-nWidth)/2+nBtnWidth*2+5*2;
		rcBtn.right = rcBtn.left+nBtnWidth;
		rcBtn.top = rcEdit.bottom+10;
		rcBtn.bottom = rcBtn.top+nBtnHeight;
		m_ctrl3.MoveWindow(rcBtn);
	}
	if(m_ctrl6.GetSafeHwnd() != NULL)
	{
		m_ctrl6.GetWindowRect(rcBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
		int nBtnWidth = rcBtn.Width();
		int nBtnHeight = rcBtn.Height();
		rcBtn.left = (cx-nWidth)/2+nBtnWidth*2+5*2;
		rcBtn.right = rcBtn.left+nBtnWidth;
		rcBtn.top = n1Bottom+5;
		rcBtn.bottom = rcBtn.top+nBtnHeight;
		m_ctrl6.MoveWindow(rcBtn);
	}
	if(m_ctrl9.GetSafeHwnd() != NULL)
	{
		m_ctrl9.GetWindowRect(rcBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
		int nBtnWidth = rcBtn.Width();
		int nBtnHeight = rcBtn.Height();
		rcBtn.left = (cx-nWidth)/2+nBtnWidth*2+5*2;
		rcBtn.right = rcBtn.left+nBtnWidth;
		rcBtn.top = n4Bottom+5;
		rcBtn.bottom = rcBtn.top+nBtnHeight;
		m_ctrl9.MoveWindow(rcBtn);
	}
	if(m_ctrl11.GetSafeHwnd() != NULL)
	{
		m_ctrl11.GetWindowRect(rcBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
		int nBtnWidth = rcBtn.Width();
		int nBtnHeight = rcBtn.Height();
		rcBtn.left = (cx-nWidth)/2+nBtnWidth*2+5*2;
		rcBtn.right = rcBtn.left+nBtnWidth;
		rcBtn.top = n7Bottom+5;
		rcBtn.bottom = rcBtn.top+nBtnHeight;
		m_ctrl11.MoveWindow(rcBtn);
	}
	if(m_ctrlDail.GetSafeHwnd() != NULL)
	{
		m_ctrlDail.GetWindowRect(rcBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
		int nBtnWidth = rcBtn.Width();
		int nBtnHeight = rcBtn.Height();
		rcBtn.left = (cx-nWidth)/2+nBtnWidth*3+5*3;
		rcBtn.right = rcBtn.left+nBtnWidth;
		rcBtn.top = rcEdit.bottom+10;
		rcBtn.bottom = rcBtn.top+nBtnHeight;
		m_ctrlDail.MoveWindow(rcBtn);
	}
	if(m_ctrlHangup.GetSafeHwnd() != NULL)
	{
		m_ctrlHangup.GetWindowRect(rcBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
		int nBtnWidth = rcBtn.Width();
		int nBtnHeight = rcBtn.Height();
		rcBtn.left = (cx-nWidth)/2+nBtnWidth*4+5*4;
		rcBtn.right = rcBtn.left+nBtnWidth;
		rcBtn.top = rcEdit.bottom+10;
		rcBtn.bottom = rcBtn.top+nBtnHeight;
		m_ctrlHangup.MoveWindow(rcBtn);
	}
	if(m_13Btn.GetSafeHwnd() != NULL)
	{
		m_13Btn.GetWindowRect(rcBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
		int nBtnWidth = rcBtn.Width();
		int nBtnHeight = rcBtn.Height();
		rcBtn.left = (cx-nWidth)/2+nBtnWidth*3+5*3;
		rcBtn.right = rcBtn.left+nBtnWidth;
		rcBtn.top = n4Bottom+5;
		rcBtn.bottom = rcBtn.top+nBtnHeight;
		m_13Btn.MoveWindow(rcBtn);
	}
	if(m_AddNumBtn.GetSafeHwnd() != NULL)
	{
		m_AddNumBtn.GetWindowRect(rcBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
		int nBtnWidth = rcBtn.Width();
		int nBtnHeight = rcBtn.Height();
		rcBtn.left = (cx-nWidth)/2+nBtnWidth*3+5*3;
		rcBtn.right = rcBtn.left+nBtnWidth;
		rcBtn.top = n7Bottom+5;
		rcBtn.bottom = rcBtn.top+nBtnHeight;
		m_AddNumBtn.MoveWindow(rcBtn);
	}
	if(m_ctrlClear.GetSafeHwnd() != NULL)
	{
		m_ctrlClear.GetWindowRect(rcBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
		int nBtnWidth = rcBtn.Width();
		int nBtnHeight = rcBtn.Height();
		rcBtn.left = (cx-nWidth)/2+nBtnWidth*4+5*4;
		rcBtn.right = rcBtn.left+nBtnWidth;
		rcBtn.top = n4Bottom+5;
		rcBtn.bottom = rcBtn.top+nBtnHeight;
		m_ctrlClear.MoveWindow(rcBtn);
	}
	if(m_14Btn.GetSafeHwnd() != NULL)
	{
		m_14Btn.GetWindowRect(rcBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
		int nBtnWidth = rcBtn.Width();
		int nBtnHeight = rcBtn.Height();
		rcBtn.left = (cx-nWidth)/2+nBtnWidth*4+5*4;
		rcBtn.right = rcBtn.left+nBtnWidth;
		rcBtn.top = n7Bottom+5;
		rcBtn.bottom = rcBtn.top+nBtnHeight;
		m_14Btn.MoveWindow(rcBtn);
	}
	if(m_ctrlSound.GetSafeHwnd() != NULL)
	{
		m_ctrlSound.GetWindowRect(rcBtn);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcBtn, 2 );
		int nBtnWidth = rcBtn.Width();
		int nBtnHeight = rcBtn.Height();
		rcBtn.left = (cx-nWidth)/2+n1Width;
		rcBtn.right = rcBtn.left+nBtnWidth;
		rcBtn.top = n10Bottom+15;
		rcBtn.bottom = rcBtn.top+nBtnHeight;
		m_ctrlSound.MoveWindow(rcBtn);
	}
	
}

void CCallDialog::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	switch(nChar)
	{
	case 1:
		OnButton1();
		break;
	}
	CBmpBkDialog::OnChar(nChar, nRepCnt, nFlags);
}

BOOL CCallDialog::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CBmpBkDialog::DestroyWindow();
}

HBRUSH CCallDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CBmpBkDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if(nCtlColor == CTLCOLOR_DLG)
	{
		static HBRUSH brush=CreateSolidBrush(RGB(221,241,251));
		return brush;

	}
	if(nCtlColor == CTLCOLOR_EDIT)
	{
		static HBRUSH brush=CreateSolidBrush(RGB(62,137,190));
		pDC->SetTextColor(RGB(255,255,255));
		return brush;

	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
}

void CCallDialog::OnButtonAddnum() 
{
	// TODO: Add your control notification handler code here
	PressKey(TEXT('+'));
}

void CCallDialog::OnButton13() 
{
	// TODO: Add your control notification handler code here
	((CSunnyCatDlg *)AfxGetMainWnd())->ShowPage(4);
}

void CCallDialog::OnButton14() 
{
	// TODO: Add your control notification handler code here
	CString strNum;
	m_editDial.GetWindowText(strNum);
	if(!strNum.IsEmpty())
	{
		CNewCardDlg dlg(this,strNum);
		dlg.DoModal();
	}
	
}


void CCallDialog::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(!((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.isAbleCom)
	{
		GetDlgItem(IDC_BUTTON_DIAL_NUM)->EnableWindow(TRUE);
		KillTimer(1);
		return;
	}
	/*************************************************************
	if(((CSunnyCatDlg*)AfxGetMainWnd())->m_isConnect)
	{
		KillTimer(1);
		return;
	}
	/*************************************************************/
	
	CallStatus m_status;
	m_status = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_pCallMgr->GetCallStatus();
	if(m_status != CALLSTATE_IDLE && m_status != CALLSTATE_INCOMING)
		GetDlgItem(IDC_BUTTON_DIAL_NUM)->EnableWindow(FALSE);
	else
	{
		KillTimer(1);
		GetDlgItem(IDC_BUTTON_DIAL_NUM)->EnableWindow(TRUE);
	}

	CDialog::OnTimer(nIDEvent);
}

