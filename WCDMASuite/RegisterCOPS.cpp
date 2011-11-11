// RegisterCOPS.cpp : implementation file
//

#include "stdafx.h"
#include "sunnycat.h"
#include "RegisterCOPS.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegisterCOPS dialog


CRegisterCOPS::CRegisterCOPS(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CRegisterCOPS::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRegisterCOPS)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CRegisterCOPS::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegisterCOPS)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegisterCOPS, CBmpBkDialog)
	//{{AFX_MSG_MAP(CRegisterCOPS)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegisterCOPS message handlers

BOOL CRegisterCOPS::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	/* ÉèÖÃ±³¾°Í¼Æ¬*/ 
	CBitmap bmp; 
	bmp.LoadBitmap(IDB_BITMAP_SELNETWORK); 
	m_brush.CreatePatternBrush(&bmp); 
	bmp.DeleteObject();

	/* ÉèÖÃ¿Ø¼þ×ÖÌå */
	m_font.CreateFont(13,0,0,0,FW_NORMAL,FALSE,FALSE,0,   
		ANSI_CHARSET,OUT_DEFAULT_PRECIS,   
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,   
		DEFAULT_PITCH|FF_SWISS,_T("ËÎÌå"));
	GetDlgItem(IDC_STATIC_COPS_REGISTER)->SetWindowText(LoadStringEx(IDS_REGISTER_NETWORK));
	GetDlgItem(IDC_STATIC_COPS_REGISTER)->SetFont(&m_font);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	              // EXCEPTION: OCX Property Pages should return FALSE
}


HBRUSH CRegisterCOPS::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CBmpBkDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if (pWnd == this) 
	{
		return (HBRUSH)m_brush; 
	}
	
	// TODO: Return a different brush if the default is not desired
	return (HBRUSH)m_brush;
}

