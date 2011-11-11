// TurnCallDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SunnyCat.h"
#include "TurnCallDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTurnCallDlg dialog


CTurnCallDlg::CTurnCallDlg(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CTurnCallDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTurnCallDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
		m_bDrawToolBar = false;
}


void CTurnCallDlg::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTurnCallDlg)
	DDX_Control(pDX, IDCANCEL, m_cancel);
	DDX_Control(pDX, IDOK, m_ok);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTurnCallDlg, CBmpBkDialog)
	//{{AFX_MSG_MAP(CTurnCallDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTurnCallDlg message handlers
