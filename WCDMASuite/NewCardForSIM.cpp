// NewCardForSIM.cpp : implementation file
//

#include "stdafx.h"
#include "sunnycat.h"
#include "NewCardForSIM.h"
#include "SunnyCatDlg.h"
#include "GlobalDataCenter.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewCardForSIM dialog


CNewCardForSIM::CNewCardForSIM(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CNewCardForSIM::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewCardForSIM)
	m_strNameSIM = _T("");
	m_strNumSIM = _T("");
//	m_strNameLast = _T("");
//	m_ucNameLen = 0;
	m_ucLenName = 0;
	isNewCard = true;
	//}}AFX_DATA_INIT
}


void CNewCardForSIM::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewCardForSIM)
	DDX_Control(pDX, IDC_EDIT_NUM_SIM, m_EditSimPB);
	DDX_Text(pDX, IDC_EDIT_NAME_SIM, m_strNameSIM);
	DDV_MaxChars(pDX, m_strNameSIM, 12);
	DDX_Text(pDX, IDC_EDIT_NUM_SIM, m_strNumSIM);
	DDV_MaxChars(pDX, m_strNumSIM, 20);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewCardForSIM, CBmpBkDialog)
	//{{AFX_MSG_MAP(CNewCardForSIM)
	ON_EN_CHANGE(IDC_EDIT_NAME_SIM, OnChangeEditNameContent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewCardForSIM message handlers

void CNewCardForSIM::OnOK() 
{
	// TODO: Add extra validation here

	if(isNewCard)
	{
		UpdateData(true);	
		if(m_strNameSIM == "" || m_strNumSIM == "")
		{
//			AfxMessageBox("请输入名片信息！");
			GMessageBox(LoadStringEx(IDS_INPUTCARD_INFORMATION));
			return;
		}

		TPhoneBookItem pbItem;

		pbItem.strName = m_strNameSIM;

		pbItem.strMobilePhone = m_strNumSIM;
		
		bool bOk = ((CSunnyCatDlg *)AfxGetMainWnd())->m_linkManDlg.m_pbookEmbed.Add(pbItem);
		if(!bOk)
		{
			if(((CSunnyCatDlg *)AfxGetMainWnd())->m_linkManDlg.m_pbookEmbed.IsFull())
				GMessageBox(LoadStringEx(IDS_PB_FULL));
			else
			GMessageBox(LoadStringEx(IDS_NEW_CARD_FAIL));
		}

		((CSunnyCatDlg *)AfxGetMainWnd())->m_linkManDlg.PostMessage(WM_LINKMAN_NOTIFY, 0 ,1);
	}

	CDialog::OnOK();
}

BOOL CNewCardForSIM::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();

	SetWindowText(LoadStringEx(IDS_SIM_CARD));
	GetDlgItem(IDC_SIM_NAME)->SetWindowText(LoadStringEx(IDS_SIM_NAME));
	GetDlgItem(IDC_SIM_PHONE)->SetWindowText(LoadStringEx(IDS_SIM_NUM));
	GetDlgItem(IDOK)->SetWindowText(LoadStringEx(IDS_OK));
	GetDlgItem(IDCANCEL)->SetWindowText(LoadStringEx(IDS_CANCEL));


	UpdateData(true);
	if(m_strNameSIM == "" && m_strNumSIM == "")
	{
		isNewCard = true;
		SetWindowText(LoadStringEx(IDS_NEW_CARD));
	}
	else
	{
		isNewCard = false;
		SetWindowText(LoadStringEx(IDS_EDIT_CARD));
	}

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewCardForSIM::OnChangeEditNameContent() 
{
	UpdateData(true);
	if (GetStringType(m_strNameSIM) == STR_DOUBLE_BYTE)
	{
		m_ucLenName = 6;
		if (m_strNameSIM.GetLength() > m_ucLenName)
		{
			GetDlgItem(IDC_EDIT_NAME_SIM)->SetWindowText(m_strNameSIM.Left(m_ucLenName));
		}
	}
	else
	{
		m_ucLenName = 12;
	}
}

