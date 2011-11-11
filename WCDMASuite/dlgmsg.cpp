#include "stdafx.h"
#include "resource.h"
#include "dlgmsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//---------------------------------------------------------------------------
#define ID_TIMER_POP_WINDOW		10
#define ID_TIMER_CLOSE_WINDOW	11
#define ID_TIMER_DISPLAY_DELAY	12
#define WIN_WIDTH	181
#define WIN_HEIGHT	116
/////////////////////////////////////////////////////////////////////////////
// CDlgMsg

CDlgMsg::CDlgMsg()
{
	m_Bitmap.LoadBitmap(IDB_BITMAP_SHOW); //Load Bitmap
	m_bFlag = false;
	m_strMessage = "The Message To Prompt...";
	m_strCaption = "Monitor Messager";
	
}

CDlgMsg::~CDlgMsg()
{
}

BEGIN_MESSAGE_MAP(CDlgMsg, CWnd)
	//{{AFX_MSG_MAP(CDlgMsg)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_KILLFOCUS()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDlgMsg message handlers
void CDlgMsg::CreateMsgWindow()
{
	KillTimer(ID_TIMER_POP_WINDOW);
	KillTimer(ID_TIMER_CLOSE_WINDOW);
	KillTimer(ID_TIMER_DISPLAY_DELAY);

	SetTimer(ID_TIMER_POP_WINDOW, 20, NULL);
}

void CDlgMsg::SetPromptMessage(LPCTSTR lpszMsg)
{
	m_strMessage = lpszMsg;
}

void CDlgMsg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CDC dcMemory;
	CRect rect;
	GetClientRect(&rect);
    dcMemory.CreateCompatibleDC(NULL);
	dcMemory.SelectObject(&m_Bitmap);
	BITMAP bmBitmap;
	m_Bitmap.GetBitmap(&bmBitmap);

	dc.StretchBlt(0,
		0,
		rect.right-rect.left,//bmBitmap.bmWidth,
		rect.bottom-rect.top,//bmBitmap.bmHeight,    
		&dcMemory, 
		0,
		0,
		bmBitmap.bmWidth,    
		bmBitmap.bmHeight,
		SRCCOPY);

	CFont font;
	font.CreatePointFont(90, TEXT("Impact"));
	dc.SelectObject(&font);
	dc.SetTextColor(RGB(0, 64, 128));
    dc.SetBkMode(TRANSPARENT);
	dc.TextOut(30, 10, m_strCaption);

	rect.top = 30;
	dc.DrawText(m_strMessage, -1, &rect, DT_CENTER|DT_VCENTER);
}

void CDlgMsg::OnTimer(UINT nIDEvent) 
{	
	static int nHeight = 0;
 	int cy = GetSystemMetrics(SM_CYSCREEN);
	int cx = GetSystemMetrics(SM_CXSCREEN);
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA,0,&rect,0);
	int y = rect.bottom-rect.top;
	int x = rect.right-rect.left;
	x = x-WIN_WIDTH;
	
	switch(nIDEvent)
	{
	case ID_TIMER_POP_WINDOW:
		if(nHeight <= WIN_HEIGHT)
		{
			++nHeight;	
			MoveWindow(x,
				y-nHeight,
				WIN_WIDTH,
				WIN_HEIGHT);
			
		    Invalidate(FALSE);
		}
		else
		{
			KillTimer(ID_TIMER_POP_WINDOW);
			SetTimer(ID_TIMER_DISPLAY_DELAY,5000,NULL);
		}
		break;
	case ID_TIMER_CLOSE_WINDOW:
		if(nHeight>=0)
		{
			nHeight--;
			MoveWindow(x,
				y-nHeight,
				WIN_WIDTH,
				nHeight);
		}
		else
		{
			KillTimer(ID_TIMER_CLOSE_WINDOW);
			PostMessage(WM_CLOSE);
			//AfxPostQuitMessage(0);
		}
		break;
	case ID_TIMER_DISPLAY_DELAY:
		KillTimer(ID_TIMER_DISPLAY_DELAY);
		SetTimer(ID_TIMER_CLOSE_WINDOW,20,NULL);
		break;
	}
}

void CDlgMsg::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);

	if(m_bFlag)
		SetTimer(ID_TIMER_DISPLAY_DELAY,3000,NULL);
}

void CDlgMsg::SetPromptCaption(LPCTSTR lpszCaption)
{
	m_strCaption = lpszCaption;
}

CDlgMsg* CDlgMsg::CreateMsgDlg(CFont* pFont)
{
	static CDlgMsg msgWnd;
	if(!msgWnd.GetSafeHwnd())
	{
		msgWnd.CreateEx(0,
		AfxRegisterWndClass(
				0,
				::LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_HAND)),(HBRUSH)(COLOR_DESKTOP+1),NULL),
				TEXT(""),
				WS_POPUP|WS_EX_TOPMOST,
				0,
				0,
				0,//bmBitmap.bmWidth,  //Bitmap Width = Splash Window Width
				0,//bmBitmap.bmHeight, //Bitmap Height = Splash Window Height
				NULL,//AfxGetMainWnd()->GetSafeHwnd(),
				NULL,
				NULL);

		msgWnd.SetFont(pFont);
	}

	return &msgWnd;
}

void CDlgMsg::OnClose() 
{
	CWnd::OnClose();
}
