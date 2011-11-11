// GenEnhancedDialog.cpp : implementation file
//

#include "stdafx.h"
#include "BmpBkDialog.h"
#include "Util.h"
#include "GlobalDataCenter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBmpBkDialog dialog

#define IDC_BUTTON_CLOSEDLG  6000

static HBITMAP g_hFrmBit[9] ;
static CFont   g_CapFont    ;
static bool    bLoadFlag = false ;

static char *g_szFrmBmpFile[]= 
{
	"ToolBarBk.bmp",
	"PopWinBkCenterTop.bmp",
	"PopWinBkCenterBottom.bmp",
	"PopWinBkLeft.bmp",
	"PopWinBkRight.bmp",

	"PopWinBkLeftTop.bmp",
	"PopWinBkRightTop.bmp",
	"PopWinBkLeftBottom.bmp",
	"PopWinBkRightBottom.bmp",
};

bool  CBmpBkDialog::m_bShiftDown = false ;
bool  CBmpBkDialog::m_bCtrlDown  = false ;
bool  CBmpBkDialog::m_bAltDown	 = false ;


static CString g_strSkinPath ;

CBmpBkDialog::CBmpBkDialog(UINT uResource,CWnd* pParent /*=NULL*/)
	: CDialog(uResource, pParent)
{
	//{{AFX_DATA_INIT(CBmpBkDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_Bkbr.CreateSolidBrush(RGB(255,255,255)) ;
	m_Editbr.CreateSolidBrush(RGB(250,250,250)) ;
	m_nDlgID = uResource ;
	m_pSubWnd= NULL ;
	m_pUpdateEdit = NULL ;
	m_bCreateExitBtn = true ;
	m_nBmbWidth		 = 0 ; 
	m_nBmbHeight	 = 0 ;
	m_bDrawToolBar	 = true ;
	m_bIsMainFuncDlg = false;
	m_bDrawEditFrm   = true ;	
	m_bLButtonDown	 = false;
	m_bEnableEnKey	 = false ;
	m_bDrawRightFrm  = true  ;
	if(!bLoadFlag)
	{
		
	//	g_strSkinPath = GetCurrentPath() + "PhoneSkins\\";
		g_strSkinPath = GetAppPath() + "skin\\PhoneSkins\\";
		for(int i=0; i<9; i++)
		{
			g_hFrmBit[i] = LoadBmpFormFile(g_strSkinPath+g_szFrmBmpFile[i]) ;
		}
		bLoadFlag = true ;
		
	    /*
		for(int i=0; i<9; i++)
		{
			g_hFrmBit[i] = LoadBmpFormFile(g_szFrmBmpFile[i]) ;
		}
		bLoadFlag = true ;
		*/
	}
	m_arrClrBtns.RemoveAll ();
}

CBmpBkDialog::~CBmpBkDialog()
{
}

void CBmpBkDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBmpBkDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBmpBkDialog, CDialog)
	//{{AFX_MSG_MAP(CBmpBkDialog)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_CLOSEDLG, OnButtonCloseDlg)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_HELPINFO()
	ON_WM_TIMER()
	ON_WM_SHOWWINDOW()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBmpBkDialog message handlers

HBRUSH CBmpBkDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	pDC->SetBkMode(TRANSPARENT) ;
	switch(nCtlColor)
	{		
	case CTLCOLOR_EDIT :
		//pDC->SetTextColor(RGB(0,0,100));
		pDC->SetBkColor(RGB(255,255,255));
		return	m_Editbr ;
		break ;
	default:
		break;
	}

	return	m_Bkbr ;
}

BOOL CBmpBkDialog::Create(CWnd *pParentWnd, UINT dlgID) 
{
	return CDialog::Create(m_nDlgID, pParentWnd);
}


void CBmpBkDialog::RedrawEditFrame()
{
	CEdit *pWnd = (CEdit *)GetFocus() ;
	if(IsEditCtrl(pWnd))
	{
		pWnd->UpdateWindow();
	}
}


bool CBmpBkDialog::IsEditCtrl(CWnd *pWnd) 
{
	if(pWnd == NULL)
	{
		return false ;
	}
	TCHAR szClassName[64]={0};
	try
	{
		GetClassName(pWnd->GetSafeHwnd(),szClassName,64) ;
		if(_tcscmp(szClassName,TEXT("Edit")) == 0 )
		{
			return true ;
		}
	}
	catch(...)
	{
		return false ;
	}
	return false ;
}

BOOL CBmpBkDialog::PreTranslateMessage(MSG* pMsg) 
{
	WORD nKeyVal = pMsg->wParam ;
	switch(pMsg->message)
	{
	case WM_MOUSEWHEEL:
		SetTimer(0x1112,10,NULL) ;
	case WM_HSCROLL:
	case WM_VSCROLL:
	case WM_NCLBUTTONDOWN:
		{
			KillTimer(0x1111) ;
			CPoint CurPt ;
			GetCursorPos(&CurPt) ;
			CurPt.x-= 15 ;
			m_pUpdateEdit = WindowFromPoint(CurPt) ;
			if(IsEditCtrl(m_pUpdateEdit))
			{
				if(WM_MOUSEWHEEL != pMsg->message)
					SetTimer(0x1111,10,NULL) ;
			}
			else
			{
				m_pUpdateEdit = NULL ;
			}
		}
		break ;
		
	case 0x0118:
		KillTimer(0x1111) ;
		break ;
		
	case WM_KEYDOWN:
		if(nKeyVal == VK_RETURN)
		{
			if(!m_bEnableEnKey)
			{
				if(!(GetStyle()&WS_POPUP) && !m_bIsMainFuncDlg)
					return TRUE;
			}
		}
		if(nKeyVal == VK_SHIFT) 
			m_bShiftDown = true ;
		if(nKeyVal == VK_CONTROL)
			m_bCtrlDown = true  ;
		{
			CEdit *pFcEdit = (CEdit *)GetFocus() ;
			if(IsEditCtrl(pFcEdit))
			{
				if(m_bCtrlDown && (nKeyVal == 'A' ||nKeyVal == 'a'))
				{
					pFcEdit->SetSel(0,-1) ;
				}
			}
		}
		break;

	case WM_KEYUP:
		if(nKeyVal == VK_SHIFT)
			m_bShiftDown = false ;
		if(nKeyVal == VK_CONTROL)
			m_bCtrlDown = false  ;
		RedrawEditFrame() ;
	case WM_LBUTTONDOWN:
		m_bLButtonDown = true ;
	case WM_RBUTTONDOWN:
		//if(g_pMainDlg != NULL) //hxb
			//g_pMainDlg->ResetShowMainWinTimer() ;//hxb
		break ;
	case WM_LBUTTONUP:
		m_bLButtonDown = false ;
		RedrawEditFrame() ;
		break ;
	case WM_MOUSEMOVE:
		if(m_bLButtonDown)
		{
			CWnd *pEdit = GetFocus() ;
			if(IsEditCtrl(pEdit))
			{
				int nCount = ((CEdit *)pEdit)->GetLineCount() ;
				CRect rc ;
				pEdit->GetWindowRect(rc) ;
				if(rc.Height()<nCount*12)
				{
					pEdit->SetRedraw(false);
					pEdit->RedrawWindow() ;
					pEdit->SetRedraw(true);
				}
			}
		}
		break;

	case WM_CHAR :
		RedrawEditFrame() ;
		break;
	default:
		break;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CBmpBkDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CRect rc ;
	GetWindowRect(rc) ;
	DWORD dwStyle = GetStyle() ;
	SetBackgroundBmp(g_strSkinPath+m_strBkbmpName) ;
	if(m_nBmbWidth>0)
	{
		rc.right =rc.left+m_nBmbWidth  ;
		rc.bottom=rc.top +m_nBmbHeight ;
		MoveWindow(rc) ;
	}
	if(!g_CapFont.GetSafeHandle())
	{
		LOGFONT lf ;
		GetFont()->GetLogFont(&lf) ;
		lf.lfWeight=FW_BOLD ;
		g_CapFont.CreateFontIndirect(&lf) ;
	}
	bool bMoveCtrl = false ;
	CString strTmp  ;
	GetWindowText(strTmp) ;
	if(!strTmp.IsEmpty() && m_strCaption.IsEmpty())
		m_strCaption = strTmp ;
	if(dwStyle&WS_POPUP)
	{
		m_btnClose.Create(LoadStringEx(IDS_CLOSE),WS_CHILD|WS_VISIBLE,rc,this,IDC_BUTTON_CLOSEDLG) ;
		m_btnClose.SetBkColor(RGB(100,100,100), RGB(200,200,200));
		m_btnClose.SetBitmapID(IDB_BITMAP_EXIT, IDB_BITMAP_EXITOVER);
		m_btnClose.SetHitOver() ;
		m_btnClose.SetToolTipTxt(LoadStringEx(IDS_CLOSE)) ;
		if(dwStyle&WS_CAPTION)
		{
			bMoveCtrl = true ;
			m_bDrawToolBar=false ;
			DWORD dwStyle = GetWindowLong(m_hWnd,GWL_EXSTYLE);
			dwStyle &= (~WS_EX_DLGMODALFRAME);
			SetWindowLong(m_hWnd,GWL_EXSTYLE,dwStyle);
			dwStyle = GetWindowLong(m_hWnd,GWL_STYLE);
			dwStyle |= WS_MINIMIZEBOX;
			dwStyle &= (~(WS_CAPTION));
			SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);
			SetWindowPos(this,0,0,rc.Width(),rc.Height(),SWP_NOZORDER|SWP_NOMOVE|SWP_FRAMECHANGED );
			GetWindowRect(rc)  ;
			rc.top-=22 ;
			//ModifyStyle(0,WS_SYSMENU|WS_MINIMIZEBOX);
		}
		rc.top+=8 ;
		rc.left=rc.right-26 ;
		ScreenToClient(rc) ;
		m_btnClose.MoveWindow(rc) ;
		
	}
	
	if(dwStyle&WS_CHILD)
	{
		m_bCreateExitBtn= false ;
	}
	CWnd *pWnd = GetNextWindow(GW_CHILD) ;
	CString strClassName ;
	int nItemID = 0 ;
	
	GetWindowRect(rc) ;
	CRect ctrlRc;
	while(pWnd != NULL)
	{
		if(bMoveCtrl)
		{
			pWnd->GetWindowRect(ctrlRc);
			ctrlRc.bottom+=22;
			ctrlRc.top   +=22;
			ScreenToClient(ctrlRc) ;
			pWnd->MoveWindow(ctrlRc) ;
		}
		
		TCHAR szClass[64] = {0} ;
		GetClassName(pWnd->m_hWnd,szClass,64) ;
		strClassName = szClass ;
		strClassName.MakeUpper() ;
		nItemID = pWnd->GetDlgCtrlID() ;
		pWnd->GetWindowRect(ctrlRc);
		if(ctrlRc.bottom+10 >= rc.bottom)
		{
			ctrlRc.bottom-=5;
			ctrlRc.top	 -=5;
			ScreenToClient(ctrlRc) ;
			pWnd->MoveWindow(ctrlRc) ;
		}

		//TRACE("ID=%d %s \n",nItemID ,strClassName)  ;
		if(strClassName.Find(TEXT("EDIT")) != -1)
		{
			CRect EditRc ;
			m_arrEditIDs.Add(pWnd) ;
			pWnd->GetWindowRect(EditRc) ;
			pWnd->ModifyStyle(WS_BORDER,0);
			pWnd->ModifyStyleEx(WS_EX_STATICEDGE,0);
			if(EditRc.Height()<20)
			{
				pWnd->ModifyStyle(0,ES_MULTILINE);
				ScreenToClient(EditRc) ;
				EditRc.bottom = EditRc.top+14 ;
				////SetWindowPos(pWnd,0,0,EditRc.Width(),EditRc.Height(),SWP_NOMOVE|SWP_NOZORDER) ;
				pWnd->MoveWindow(EditRc) ;
			}
		}
		else if(strClassName.Find(TEXT("BUTTON")) != -1)
		{
			CString strText;
			pWnd->GetWindowText(strText);
			DWORD dwStyle = pWnd->GetStyle()&0xFFFF ;
			//TRACE("Style : 0x%08X == %s \n",dwStyle,strText) ;
			if(dwStyle==0x00 || dwStyle ==0x01 ) 
			{
				CColorBtn *pNewBtn = new CColorBtn() ;
				if(pNewBtn->SubclassDlgItem(nItemID,this))
				{
					pNewBtn->SetHitOver() ;
					m_arrClrBtns.Add(pNewBtn) ;
				}
				else
				{
					delete pNewBtn ;
				}
			}
		}
/*		else if(strClassName.Find("COMBOBOX") != -1)
		{
			COwnDrawComboBox *pCmb = new COwnDrawComboBox() ;
			pCmb->SubclassDlgItem(nItemID,this) ;
			m_arrOwnDrawCmb.Add(pCmb) ;
		}
*/		else if(strClassName.Find(TEXT("SYSLISTVIEW32")) != -1)
		{
			DWORD dwStyle = GetWindowLong(pWnd->GetSafeHwnd(),GWL_EXSTYLE);
			dwStyle &= (~WS_EX_STATICEDGE);
			SetWindowLong(pWnd->GetSafeHwnd(),GWL_EXSTYLE,dwStyle);
			SetWindowPos(pWnd,0,0,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOMOVE|SWP_FRAMECHANGED );
			m_arrListCtrl.Add((CListCtrl *)pWnd) ;
		}
		else if(strClassName.Find(TEXT("SYSTREEVIEW32")) != -1)
		{
			DWORD dwStyle = GetWindowLong(pWnd->GetSafeHwnd(),GWL_EXSTYLE);
			dwStyle &= (~WS_EX_STATICEDGE);
			SetWindowLong(pWnd->GetSafeHwnd(),GWL_EXSTYLE,dwStyle);
			SetWindowPos(pWnd,0,0,0,0,SWP_NOZORDER|SWP_NOSIZE|SWP_NOMOVE|SWP_FRAMECHANGED );
			m_arrTreeCtrl.Add((CTreeCtrl *)pWnd) ;
		}

		pWnd = pWnd->GetNextWindow(GW_HWNDNEXT) ;
	}
	return TRUE; 
}


void CBmpBkDialog::SetBackgroundBmp(LPCTSTR lpBmpName)
{
	CBitmap bk ;
	m_nBmbWidth  = 0 ; 
	m_nBmbHeight = 0 ;
	bk.Attach(LoadBmpFormFile(lpBmpName)) ;
	if(bk.GetSafeHandle())
	{
		m_Bkbr.DeleteObject() ;
		m_Bkbr.CreatePatternBrush(&bk) ;
		BITMAP bm ;
		bk.GetBitmap(&bm) ;
		m_nBmbWidth  = bm.bmWidth  ; 
		m_nBmbHeight = bm.bmHeight ;
		HBITMAP OldBit = (HBITMAP) bk.Detach() ;
		bk.DeleteObject() ;
		DeleteObject(OldBit) ;
	}
}

void CBmpBkDialog::OnButtonCloseDlg()
{
	SendMessage(WM_CLOSE) ;
}

void CBmpBkDialog::OnPaint() 
{
	try
	{
		CPaintDC dc(this); // device context for painting
		int	   i = 0 ;
		CRect  EditRc ;
		static int a = 1 ;
		DWORD dwStyle = GetStyle() ;
		static COLORREF clrFrm = RGB(210,210,210) ;
		CWnd *pTWnd = NULL ;

		if(m_bDrawEditFrm)
		{
			for(i=0;i<m_arrEditIDs.GetSize(); i++)
			{
				pTWnd = m_arrEditIDs[i];
				if(pTWnd->GetSafeHwnd() == NULL)
					continue ;
				if(pTWnd->GetStyle()&WS_VISIBLE)
				{
					pTWnd->GetWindowRect(EditRc) ;
					ScreenToClient(EditRc) ;
					EditRc.InflateRect(a,a,a,a) ;
					dc.Draw3dRect(&EditRc,clrFrm,clrFrm) ;
				}
			}
		}
		for(i=0;i<m_arrListCtrl.GetSize(); i++)
		{
			pTWnd = m_arrListCtrl[i] ;
			if(pTWnd->GetStyle()&WS_VISIBLE)
			{
				pTWnd->GetWindowRect(EditRc) ;
				ScreenToClient(EditRc) ;
				EditRc.InflateRect(a,a,a,m_bIsMainFuncDlg?0:a) ;
				if(m_bIsMainFuncDlg)
				{
					EditRc.top    = 38 ;
					EditRc.bottom = 38+470+2;
				}
				dc.Draw3dRect(&EditRc,clrFrm,clrFrm) ;
			}
		}

		for(i=0;i<m_arrTreeCtrl.GetSize(); i++)
		{
			pTWnd = m_arrTreeCtrl[i] ;
			if(pTWnd->GetStyle()&WS_VISIBLE)
			{
				pTWnd->GetWindowRect(EditRc) ;
				ScreenToClient(EditRc) ;
				EditRc.InflateRect(a,a,a,a) ;
				if(m_bIsMainFuncDlg)
					EditRc.top-=11 ;
				dc.Draw3dRect(&EditRc,clrFrm,clrFrm) ;
			}
		}
		CRect rc ;
		if(dwStyle&WS_POPUP)
		{
			if(m_nBmbHeight == 0 )
			{
				ShowBmpInrect(&dc) ;
			}
			GetClientRect(rc) ;
			rc.InflateRect(-5,-5,-5,-5) ;
			if(!(dwStyle&WS_CAPTION))
				rc.top+=21 ;
			
			dc.Draw3dRect(&rc,clrFrm,clrFrm) ;
		}
		else
		{
			if(m_bIsMainFuncDlg)
			{
				BITMAP  bm ;
				CDC		memdc;
				static  CBitmap bt ;
 				GetClientRect(rc) ;
				memdc.CreateCompatibleDC(&dc);
				CRect  *pRc = &CRect(7,2,rc.Width()-4,2+32) ;
				bt.Attach(g_hFrmBit[0]) ;
				bt.GetBitmap(&bm)  ;
				memdc.SelectObject(&bt) ;
				int x,y,nXPos,nYPos ; 
				for(x=0; x<pRc->Width(); x+=bm.bmWidth)
				{
					for(y=0; y<pRc->Height(); y+=bm.bmHeight)
					{
						nXPos = pRc->left+x ;
						if(nXPos+bm.bmWidth >pRc->right )
						{
							nXPos = pRc->right-bm.bmWidth ;
						}
						nYPos = pRc->top+y ;
						if(nYPos+bm.bmHeight >pRc->bottom )
						{
							nYPos = pRc->bottom-bm.bmHeight ;
						}
						dc.BitBlt(nXPos,nYPos,bm.bmWidth,bm.bmHeight,&memdc,0,0,SRCCOPY) ;
					}
				}
				bt.Detach() ;
				static CPen pen(PS_SOLID,1,clrFrm);//
				CPen *pOldPen = dc.SelectObject(&pen) ;
				dc.MoveTo(2,35)  ;
				dc.LineTo(rc.Width()-2,35) ;
				dc.SelectObject(pOldPen) ;

 				GetClientRect(rc) ;
				rc.InflateRect(-1,-1,-1,-1);
				dc.Draw3dRect(&rc,clrFrm,clrFrm) ;
				if(m_bDrawRightFrm)
				{
					rc.left = 592;
					rc.top  = 39 ;
					rc.right  = 592+189 ;
					rc.bottom = 39+470  ;
					rc.InflateRect(1,1,1,1) ;
					dc.Draw3dRect(&rc,clrFrm,clrFrm) ;
				}
				
				memdc.DeleteDC() ;
				//ReleaseDC(&memdc);
			}
		}
	}
	catch(...)
	{
	}
}

void CBmpBkDialog::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(GetStyle()&WS_POPUP)
	{
		CRect  rc  ;
		GetWindowRect(rc) ;
		ScreenToClient(rc);
		//TRACE("%d,%d,%d,%d  (%d,%d)\n",rc.left,rc.top,rc.right,rc.bottom,point.x,point.y)  ;

		rc.left   += 10 ;
		rc.bottom  = rc.top+20 ;
		rc.right  -= 20 ;
		if(rc.PtInRect(point))
		{
			SendMessage(WM_SYSCOMMAND,0xF012,0);
		}
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CBmpBkDialog::SetColorButtonsInfo(DWORD *lpBtnIDs,TCHAR **lpBmpNames,int nBtnsCount)
{
	CColorBtn *pBtns = NULL ;
	bool bFound = false ;
	for(int i=0,j=0; i<nBtnsCount; i++,j+=2)
	{
		bFound = false ;
		for(int n=0; n<m_arrClrBtns.GetSize(); n++)
		{
			pBtns = m_arrClrBtns[n] ;
			if(pBtns->GetDlgCtrlID() == (int)lpBtnIDs[i] )
			{
				bFound = true ;
				break ;
			}
		}
		if(!bFound)
		{
			pBtns = new CColorBtn() ;
			m_arrClrBtns.Add(pBtns) ;
			pBtns->SubclassDlgItem(lpBtnIDs[i],this);
		}
		pBtns->SetBitmapPath( lpBmpNames[j],
							  lpBmpNames[j+1]) ;
		pBtns->SetHitOver() ;
	}
}


void CBmpBkDialog::OnClose() 
{
	if(m_pSubWnd != NULL)
		::SendMessage(m_pSubWnd->GetSafeHwnd(),WM_CLOSE,0,0) ;
	CDialog::OnClose();
}

void CBmpBkDialog::SetSubDlg(CWnd *pSubWnd)
{
	m_pSubWnd = pSubWnd ;
}

void CBmpBkDialog::RemoveColorBtn(DWORD dwBtnID)
{
	for(int n=0; n<m_arrClrBtns.GetSize(); n++)
	{
		if(m_arrClrBtns[n]->GetSafeHwnd())
		if(m_arrClrBtns[n]->GetDlgCtrlID() == (int)dwBtnID )
		{
			m_arrClrBtns[n]->UnsubclassWindow() ;
			delete m_arrClrBtns[n];
			m_arrClrBtns.RemoveAt(n);
			break ;
		}
	}
}

void CBmpBkDialog::AutoShowEditSrcoll(CEdit *pEdit)
{
	if(pEdit == NULL)
		return ;
	if(pEdit->GetStyle()&ES_MULTILINE)
	{
		CRect rc ;
		bool bShow = false ;
		int nCount = pEdit->GetLineCount() ;
		pEdit->GetWindowRect(rc) ;
		if(rc.Height()<nCount*12)
		{
			bShow = true ;
		}
		if(bShow)
			pEdit->ModifyStyle(0,ES_AUTOVSCROLL) ;
		else
			pEdit->ModifyStyle(ES_AUTOVSCROLL,0) ;
		pEdit->ShowScrollBar(SB_VERT,bShow) ;
	}
}

BOOL CBmpBkDialog::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if(	!m_bCreateExitBtn && wParam == IDCANCEL)
	{
		::PostMessage(GetParent()->m_hWnd,WM_COMMAND,IDCANCEL,0) ;
		return TRUE;
	}
	if(lParam != NULL)
	{
		int nNotifyMsg = HIWORD(wParam) ;
		CEdit *pEdit = (CEdit *)GetDlgItem(LOWORD(wParam)) ;

		if( nNotifyMsg == EN_CHANGE  ||
			nNotifyMsg == EN_HSCROLL ||
			nNotifyMsg == EN_VSCROLL ||
			nNotifyMsg == EN_UPDATE  ||
			nNotifyMsg == EN_SETFOCUS||
			nNotifyMsg == EN_KILLFOCUS) 
		{
			if(pEdit != NULL)
			{
				pEdit->RedrawWindow() ;
				AutoShowEditSrcoll(pEdit)  ;
				if(nNotifyMsg == EN_SETFOCUS)
				{
					CRect rc;
					pEdit->GetClientRect(rc) ;
					ClientToScreen(&rc) ;
					rc.InflateRect(6,6,6,6);
					InvalidateRect(rc) ;
				}
			}
		}
	}
	return CDialog::OnCommand(wParam, lParam);
}

void CBmpBkDialog::ShowBmpInrect(CDC *pDC)
{
	CRect   rc ;
	GetClientRect(rc) ;
	int nHeight = rc.Height();
	int nWidth  = rc.Width() ;

	CRect RectArr[]=
	{
		CRect(7,27,nWidth-7,59),
		CRect(0,0,nWidth,26),
		CRect(0,nHeight-5,nWidth,nHeight),
		CRect(0,0,5,nHeight),
		CRect(nWidth-5,0,nWidth,nHeight),

		CRect(0,0,8,25),
		CRect(nWidth-8,0,nWidth,25),
		CRect(0,nHeight-5,5,nHeight),
		CRect(nWidth-5,nHeight-5,nWidth,nHeight),
	};
	
	int i = 0 ;

	CRect rcSub ;
	m_btnClose.GetWindowRect(rcSub) ;
	ScreenToClient(rcSub) ;
	pDC->ExcludeClipRect(rcSub) ;
	BITMAP  bm ;
	CDC		memdc;
	static  CBitmap bt ;

	memdc.CreateCompatibleDC(pDC);
	for(i=0; i<9; i++)
	{
		if(i==0)
		{
			if(!m_bDrawToolBar )
				continue ;
			GetClientRect(rc) ;
			ScreenToClient(rc);
			rc.left-=5;
			rc.right=rc.Width()-4+2;
			static CPen pen(PS_SOLID,1,RGB(210,210,210));
			pDC->SelectObject(&pen) ;
			pDC->MoveTo(rc.left,35+25);
			pDC->LineTo(rc.right,35+25);
		}
		CRect  *pRc = &RectArr[i] ;
		if(g_hFrmBit[i] != NULL)
		{
			bt.Attach(g_hFrmBit[i]) ;
			bt.GetBitmap(&bm)  ;
			memdc.SelectObject(&bt) ;
			int x,y,nXPos,nYPos;
			for(x=0; x<pRc->Width(); x+=bm.bmWidth)
			{
				for(y=0; y<pRc->Height(); y+=bm.bmHeight)
				{
					nXPos = pRc->left+x ;
					if(nXPos+bm.bmWidth >pRc->right )
					{
						nXPos = pRc->right-bm.bmWidth ;
					}
					nYPos = pRc->top+y ;
					if(nYPos+bm.bmHeight > pRc->bottom+1 )
					{
						nYPos = pRc->bottom-bm.bmHeight ;
					}
					pDC->BitBlt(nXPos,nYPos,bm.bmWidth,bm.bmHeight,&memdc,0,0,SRCCOPY) ;
				}
			}
			bt.Detach() ;
		}
		else
		{
			pDC->FillSolidRect(pRc,RGB(200,196,245)) ;
		}
	}

	if(!m_strTitlebmpName.IsEmpty())
	{
		bt.Attach(LoadBmpFormFile(m_strTitlebmpName));
		bt.GetBitmap(&bm)  ;
		memdc.SelectObject(&bt) ;
		pDC->BitBlt(7,2,bm.bmWidth,bm.bmHeight,&memdc,0,0,SRCCOPY) ;

		HBITMAP OldBit = (HBITMAP) bt.Detach() ;
		bt.DeleteObject() ;
		DeleteObject(OldBit) ;
	}
	else
	{
		GetWindowText(m_strCaption) ;
		if(!m_strCaption.IsEmpty())
		{
			pDC->SetBkMode(TRANSPARENT)   ;
			pDC->SelectObject(&g_CapFont) ;
			pDC->TextOut(10,7,m_strCaption);
		}
	}
	memdc.DeleteDC() ;
	ReleaseDC(&memdc);
}


CColorBtn *CBmpBkDialog::SetColorBtnBmp(DWORD dwBtnID,TCHAR *pBmpName)
{
	CColorBtn *pBtn = NULL ;
	bool bFound = false ;
	int nCount  = m_arrClrBtns.GetSize() ;
	for(int i=0; i<nCount; i++)
	{
		pBtn = m_arrClrBtns[i] ;
		if(pBtn != NULL && pBtn->GetSafeHwnd() && pBtn->GetDlgCtrlID() == (int)dwBtnID )
		{
			bFound = true ;
			TRACE(TEXT("Found %d \n"),dwBtnID) ;
			break ;
		}
	}
	if(!bFound)
	{
		pBtn = new CColorBtn() ;
		m_arrClrBtns.Add(pBtn) ;
		pBtn->SubclassDlgItem(dwBtnID,this);
	}
	CString strBmp(pBmpName) ;
	int nFind = strBmp.ReverseFind('-') ;
	if(nFind >0)
	{
		strBmp = strBmp.Left(nFind) ;
	}
	if(pBtn->GetSafeHwnd())
	{
		pBtn->SetBitmapPath(strBmp+"-Normal.bmp",strBmp+"-Down.bmp") ;
		pBtn->SetHitOver() ;
	}
	return pBtn;
}

void CBmpBkDialog::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	CRect rc ;
	if(m_btnClose.GetSafeHwnd())
	{
		GetWindowRect(rc) ;
		rc.top+=3 ;
		rc.left=rc.right-26 ;
		ScreenToClient(rc)  ;
		m_btnClose.MoveWindow(rc) ;
	}
	if(GetStyle()&WS_POPUP)
	{
		GetWindowRect(rc) ; 
		CRgn rgnWnd ;
		rgnWnd.CreateRoundRectRgn(0,0,rc.Width()+1,rc.Height()+1,18,18) ;
		SetWindowRgn((HRGN)rgnWnd,TRUE) ;
	}
}

CColorBtn *CBmpBkDialog::GetColorBtn(DWORD dwBtnID)
{
	CColorBtn *pBtn = NULL ;
	for(int i=0; i<m_arrClrBtns.GetSize(); i++)
	{
		pBtn = m_arrClrBtns[i] ;
		if(pBtn != NULL && pBtn->GetDlgCtrlID() == (int)dwBtnID )
		{
			return  pBtn;
		}
	}
	return NULL ;
}

void CBmpBkDialog::OnDestroy() 
{
	CDialog::OnDestroy();
	HBRUSH OldBr = (HBRUSH)m_Bkbr.Detach() ;
	m_Bkbr.DeleteObject() ;
	DeleteObject(OldBr) ;
	OldBr = (HBRUSH)m_Editbr.Detach() ;
	m_Editbr.DeleteObject()  ;
	DeleteObject(OldBr) ;

	for(int i=0; i<m_arrClrBtns.GetSize(); i++)
	{
		delete m_arrClrBtns[i] ;
	}
	m_arrClrBtns.RemoveAll() ;
/*	for(i=0; i<m_arrOwnDrawCmb.GetSize(); i++)
	{
		delete m_arrOwnDrawCmb[i] ;
	}	
*/
	m_arrClrBtns.RemoveAll() ;
}

/*COwnDrawComboBox *CBmpBkDialog::GetOwndrawCmb(DWORD dwCmbID)
{
	COwnDrawComboBox *pCmb = NULL ;
	for(int i=0; i<m_arrOwnDrawCmb.GetSize(); i++)
	{
		pCmb = m_arrOwnDrawCmb[i] ;
		if(pCmb != NULL && pCmb->GetDlgCtrlID() == (int)dwCmbID )
		{
			return  pCmb;
		}
	}
	return NULL ;	
}
*/
/*void CBmpBkDialog::DeleteOwnDrawCombox(int nCombID)
{
	COwnDrawComboBox *pCmb = NULL ;
	for(int i=0; i<m_arrOwnDrawCmb.GetSize(); i++)
	{
		pCmb = m_arrOwnDrawCmb[i] ;
		if(pCmb != NULL && pCmb->GetDlgCtrlID() == nCombID )
		{
			pCmb->UnsubclassWindow() ;
			delete  pCmb;
			m_arrOwnDrawCmb.RemoveAt(i) ;
			return ;
		}
	}
}
*/
BOOL CBmpBkDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	return false;
	//return g_pMainDlg->ShowHelpFile(); //hxb
	//return CDialog::OnHelpInfo(pHelpInfo);
}


void CBmpBkDialog::OnTimer(UINT nIDEvent) 
{
	if(m_pUpdateEdit != NULL && m_pUpdateEdit->GetSafeHwnd())
	{
		CRect rc ;
		m_pUpdateEdit->GetWindowRect(rc) ;
		CPoint pos ;
		static CPoint pt(0,0);
		GetCursorPos(&pos) ;
		if(pt.y != pos.y || 0x1112 == nIDEvent || abs(rc.top - pos.y)>3 )
		{
			int nCount = ((CEdit *)m_pUpdateEdit)->GetLineCount() ;
			if(rc.Height()<nCount*12)
			{
				m_pUpdateEdit->RedrawWindow();
				TRACE(TEXT("m_pUpdateEdit->RedrawWindow() \n")) ;
			}
			pt = pos ;
		}
	}
	KillTimer(0x1112) ;
	CDialog::OnTimer(nIDEvent);
}


void CBmpBkDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	for(int i=0;i<m_arrEditIDs.GetSize(); i++)
	{
		AutoShowEditSrcoll((CEdit *)m_arrEditIDs[i]) ;
	}
}


