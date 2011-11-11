// SelComDlg.cpp : implementation file
//

#include "stdafx.h"
#include "execute.h"
#include "SelComDlg.h"
#include "DevEnum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelComDlg dialog


CSelComDlg::CSelComDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelComDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelComDlg)
	m_strCom = _T("");
	//}}AFX_DATA_INIT
}


void CSelComDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelComDlg)
	DDX_CBString(pDX, IDC_COMBO_SELCOM, m_strCom);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelComDlg, CDialog)
	//{{AFX_MSG_MAP(CSelComDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelComDlg message handlers

extern CString g_strLanguage;

BOOL CSelComDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CWnd* pWnd = GetDlgItem(IDC_COMBO_SELCOM);
	CComboBox* pBox = (CComboBox*) pWnd;
	pBox->ResetContent();
//	CPortList portList;
//	size_t sSize = portList.Count();
//	for(size_t i=0; i<sSize; ++i)
//	{
//		const TPortInfo& info = portList.GetAt(i);
//		if(info.strCOM.Find("COM") != -1)
//		{	
//			pBox->AddString(info.strCOM);
//		}
//		
//	}
	LPCSTR strCOM[] =
	{
		"COM1",
		"COM2",
		"COM3",
		"COM4",
		"COM5",
		"COM6",
		"COM7",
		"COM8",
		"COM9",
		"COM10",
		"COM11",
		"COM12",
		"COM13",
		"COM14",
		"COM15",
		"COM16",
		"COM17",
		"COM18",
		"COM19",
		"COM20"
	};
	
	for(size_t i=0; i<20; ++i)
	{		
		pBox->AddString(strCOM[i]);			
	} 
	pBox->SetCurSel(0);

	CString strLanguage = g_strLanguage;
	pWnd = GetDlgItem(IDC_STATIC_SELECT);
	if(strLanguage == "-Ru")
	{
		pWnd->SetWindowText("§±§à§Ø§Ñ§Ý§å§Û§ã§ä§Ñ, §Ó§í§Ò§Ö§â§Ú§ä§Ö COM£º");
		SetWindowText("§°§Õ§à§Ò§â§ñ§ä§î COM:");
		(GetDlgItem(IDOK))->SetWindowText("OK");
	}
	else if(strLanguage == "-En")
	{
		pWnd->SetWindowText("Please select COM:");
		SetWindowText("Select COM:");
		(GetDlgItem(IDOK))->SetWindowText("OK");
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
