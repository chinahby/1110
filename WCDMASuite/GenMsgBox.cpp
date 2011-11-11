// GenMsgBox.cpp : implementation file
//

#include "stdafx.h"
#include "GenMsgBox.h"
//#include "SuperPhoneDialog.h"
#include "GlobalDataCenter.h"
#include "SunnyCatDlg.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGenMsgBox dialog


CGenMsgBox::CGenMsgBox(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CGenMsgBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGenMsgBox)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_dwProperty   = MB_OK|MB_ICONEXCLAMATION ;
	m_bDrawToolBar = false ;
	m_bDrawEditFrm = false ;
	m_dwDefBtnID   = IDOK ;
	m_nBtnCount    = 0 ;
}


void CGenMsgBox::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenMsgBox)
	DDX_Control(pDX, IDC_EDIT_MSGTXT, m_EditTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGenMsgBox, CBmpBkDialog)
	//{{AFX_MSG_MAP(CGenMsgBox)
	ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
	ON_CONTROL_RANGE(BN_CLICKED,IDOK,IDNOTOALL,OnCloseMsgBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenMsgBox message handlers
void CGenMsgBox::OnCloseMsgBox(UINT nBtnID)
{
	EndDialog(nBtnID) ;
}


HICON CGenMsgBox::LoadMsgIcon()
{
	LPCTSTR dwSysIconIndex = IDI_WINLOGO ;
	switch((m_dwProperty>>4)&0x0F)
	{
	case 1:
		dwSysIconIndex = IDI_HAND ;
		break;
	case 2:
		dwSysIconIndex = IDI_QUESTION ;
		break;
	case 3:
		dwSysIconIndex = IDI_EXCLAMATION;
		break;
	case 4:
		dwSysIconIndex = IDI_ASTERISK ;
		break;
	case 8:
		dwSysIconIndex = IDI_APPLICATION ;
		break;
	}
	
	return LoadIcon(NULL,dwSysIconIndex) ;
}

void CGenMsgBox::OnPaint() 
{
	CBmpBkDialog::OnPaint();
	CDC *pDC = GetDC() ;
	HICON hIcon = LoadMsgIcon() ;
	if(hIcon != NULL)
	{
		pDC->DrawIcon(14+2,14+26,hIcon);
	}
	ReleaseDC(pDC) ;
}

BOOL CGenMsgBox::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	if(!m_strCaption.IsEmpty())
	{
		SetWindowText(m_strCaption) ;
		UpdateData(false) ;
	}
	AdjustWindow() ;
	return TRUE; 
}


//DEL void CGenMsgBox::OnIgnore() 
//DEL {
//DEL 	EndDialog(IDIGNORE) ;	
//DEL }

/*
#define MB_OK                       0x00000000L
#define MB_OKCANCEL                 0x00000001L
#define MB_ABORTRETRYIGNORE         0x00000002L
#define MB_YESNOCANCEL              0x00000003L
#define MB_YESNO                    0x00000004L
#define MB_RETRYCANCEL              0x00000005L
*/
void CGenMsgBox::AdjustWindow()
{
	CPaintDC dc(this) ;
	dc.SelectObject(GetFont()) ;
	CSize Size;
	CString strLanguage = GetLanguage();
	if(strLanguage == "-En")
	{
		Size = dc.GetOutputTextExtent(TEXT("0"),1) ;
	}
	else
	{
		Size = dc.GetOutputTextExtent(TEXT("0"),2) ;
	}
	int nLineCount = 1 ;
	int nFind      = 1 ;
	int nMaxLine   = 0 ;
	int nLastFind  = 0 ;
	bool bEndFlag = false ;
	while(!bEndFlag)
	{
		nFind = m_strTitle.Find('\n',nFind+1) ;
		nLineCount++ ;
		if(nFind == -1)
		{
			bEndFlag = true ;
			nFind = m_strTitle.GetLength() ;
		}
		if(nMaxLine < nFind - nLastFind)
		{
			nMaxLine = nFind - nLastFind ;
		}
		nLastFind = nFind;
	}
	if(nMaxLine == 0)
		nMaxLine = m_strTitle.GetLength() ;

	CRect rc(55,20+26,55+Size.cx*nMaxLine+5,20+26+Size.cy*nLineCount) ;
#if 0
	m_staTitle.Create(m_strTitle,WS_VISIBLE|WS_CHILD,rc,this) ;
	m_staTitle.SetFont(GetFont()) ;
	m_EditTitle.ShowWindow(SW_HIDE);
#else
	MultiLineString(m_strTitle) ;
	m_EditTitle.MoveWindow(rc) ;
	m_EditTitle.SetFont(GetFont()) ;
	m_EditTitle.SetWindowText(m_strTitle) ;
#endif
	int nStaWid =  rc.Width()  + 10 ;
	int nStaHei =  rc.Height() + 10 ;
	int nToCreateID[10] = {0}; 


/*	static char *pBtnTitles[] = 
	{
		"确定(&O)",
		"取消(&C)",
		"退出(&A)",
		"重试(&R)",
		"忽略(&I)",
		"是(&Y)",
		"否(&N)",
		"关闭(&E)",
		"帮助(&H)",
		"全是(&A)",
		"全否(&U)",
	};
*/
	static CString pBtnTitles[] = 
	{	
		LoadStringEx(IDS_MSGBOX_OK),
		LoadStringEx(IDS_MSGBOX_CANCEL),
		LoadStringEx(IDS_MSGBOX_EXIT),
		LoadStringEx(IDS_MSGBOX_RETRY),
		LoadStringEx(IDS_MSGBOX_IGNO),
		LoadStringEx(IDS_MSGBOX_YES),
		LoadStringEx(IDS_MSGBOX_NO),
		LoadStringEx(IDS_MSGBOX_CLOSE),
		LoadStringEx(IDS_MSGBOX_HELP),
		LoadStringEx(IDS_MSGBOX_YESALL),
		LoadStringEx(IDS_MSGBOX_NOALL),			
	};

	switch(m_dwProperty&0x0F)
	{
	case 0:
		nToCreateID[0] = IDOK ;
		break ;
	case 1:
		nToCreateID[0] = IDOK ;
		nToCreateID[1] = IDCANCEL ;
		break ;
	case 2:
		nToCreateID[0] = IDABORT ;
		nToCreateID[1] = IDRETRY ;
		nToCreateID[2] = IDIGNORE;
		break ;
	case 3:
		nToCreateID[0] = IDYES ;
		nToCreateID[1] = IDNO  ;
		nToCreateID[2] = IDCANCEL ;
		break ;
	case 4:
		nToCreateID[0] = IDYES;
		nToCreateID[1] = IDNO ;
		break ;
	case 5:
		nToCreateID[0] = IDRETRY ;
		nToCreateID[1] = IDCANCEL;
		break ;

	case MB_YESNOALL:
		nToCreateID[0] = IDYES ;
		nToCreateID[1] = IDYESTOALL  ;
		nToCreateID[2] = IDNO ;
		nToCreateID[3] = IDNOTOALL ;
		break;

	case MB_YESNOALLCANCEL:
		nToCreateID[0] = IDYES ;
		nToCreateID[1] = IDYESTOALL  ;
		nToCreateID[2] = IDNO ;
		nToCreateID[3] = IDNOTOALL ;
		nToCreateID[4] = IDCANCEL ;
		break;
	}
	for(int i=0; i<7; i++)
	{
		if(nToCreateID[i] != 0)
		{
			m_nBtnCount++ ;
		}
	}
	GetWindowRect(rc);
	int nWinHeight = nStaHei+45+46 ;
	int nWinWidth  = m_nBtnCount*80+60;
	if(nStaWid+60 > nWinWidth)
	{
		nWinWidth = nStaWid+60;
	}
	if(nWinHeight < 125)
	{
		nWinHeight = 125 ;
	}
	if( nWinWidth< 100)
	{
		nWinWidth = 100 ;
	}
	int nLimitWidth = m_strCaption.GetLength()*Size.cx + 60 ;
	
	if( nWinWidth<nLimitWidth )
	{
		nWinWidth = nLimitWidth ;
	}
	rc.bottom = rc.top +nWinHeight ;
	rc.right  = rc.left+nWinWidth  ;
	MoveWindow(&rc) ;
	ScreenToClient(rc) ;
	int nBeginX = (rc.Width()-m_nBtnCount*82)/2+14 ;
	int nBeginY = rc.bottom-42;
	int nIndex  = 0 ;
	for(i=0; i<m_nBtnCount; i++)
	{
		nIndex  =  nToCreateID[i]-1;
		CRect tmpRc(nBeginX+81*i,nBeginY,nBeginX+81*i+68,nBeginY+25) ;
		m_arrBtns[i].Create(pBtnTitles[nIndex],WS_VISIBLE|WS_CHILD|WS_TABSTOP,tmpRc,this,nToCreateID[i]) ;
		m_arrBtns[i].SetFont(GetFont()) ;
		m_arrBtns[i].SetHitOver() ;
		m_arrBtns[i].SetRoundRect();
	}
}


BOOL CGenMsgBox::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_SPACE)
		{
			CWnd *pWnd = GetFocus() ;
			EndDialog(pWnd->GetDlgCtrlID()) ;
			return TRUE ;
		}
	}
	return CBmpBkDialog::PreTranslateMessage(pMsg);
}

void CGenMsgBox::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CBmpBkDialog::OnShowWindow(bShow, nStatus);
	if(bShow)
	{
		DWORD dwDefIndex = (m_dwProperty>>8)&0x0F;
		if(dwDefIndex>=m_nBtnCount)
			dwDefIndex = 0 ;
		m_arrBtns[dwDefIndex].ModifyStyle(0,BS_DEFPUSHBUTTON) ;
		m_arrBtns[dwDefIndex].SetFocus() ;
	}
}

BOOL CGenMsgBox::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	return CBmpBkDialog::OnCommand(wParam, lParam);
}
