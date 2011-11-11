// NewClassDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SunnyCat.h"
#include "SunnyCatDlg.h"
#include "NewClassDlg.h"
#include "SelectClassDlg.h"
#include "Util.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewClassDlg dialog


CNewClassDlg::CNewClassDlg(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CNewClassDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewClassDlg)
	m_strClassName = _T("");
	m_strClassVoicePath = _T("");
	//}}AFX_DATA_INIT
	m_bDrawToolBar = false;
	isNewClass = true;
}


void CNewClassDlg::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewClassDlg)
	DDX_Control(pDX, IDC_EDIT_CLASSNAME, m_ctrlClassName);
	DDX_Text(pDX, IDC_EDIT_CLASSNAME, m_strClassName);
	DDV_MaxChars(pDX, m_strClassName, 20);
	DDX_Text(pDX, IDC_EDIT_SP_RING, m_strClassVoicePath);
	DDV_MaxChars(pDX, m_strClassVoicePath, 300);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewClassDlg, CBmpBkDialog)
	//{{AFX_MSG_MAP(CNewClassDlg)
	ON_BN_CLICKED(IDC_BUTTON_SELRING, OnButtonSelring)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewClassDlg message handlers




BOOL CNewClassDlg::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UpdateData(true);
	if(m_strClassName == "")
	{
		isNewClass = true;
		SetWindowText(LoadStringEx(IDS_NEW_CLASS));
	}
	else
	{
		isNewClass = false;
		SetWindowText(LoadStringEx(IDS_EDIT_CLASS));
		if(((CSunnyCatDlg *)AfxGetMainWnd())->m_linkManDlg.m_currTreeItemData == 6)
		{
			m_ctrlClassName.EnableWindow(false);
		}
	}
	
	GetDlgItem(IDC_STATIC_CLASSNAME)->SetWindowText(LoadStringEx(IDS_CLASS_NAME));
	GetDlgItem(IDC_STATIC_CLASS_RING)->SetWindowText(LoadStringEx(IDS_RING_PATH));
	GetDlgItem(IDC_BUTTON_SELRING)->SetWindowText(LoadStringEx(IDS_BUTTON_SET));
	GetDlgItem(IDOK)->SetWindowText(LoadStringEx(IDS_OK));
	GetDlgItem(IDCANCEL)->SetWindowText(LoadStringEx(IDS_CANCEL));
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewClassDlg::OnButtonSelring() 
{
	// TODO: Add your control notification handler code here
	CString strFilter = LoadStringEx(IDS_WAVEFILE_TYPE);
	CString strFilename;
	strFilter += "|*.wav||";
	CFileDialog dlg(
		true,		//OPEN 
		TEXT("*.wav"),	//Default filter extern name
		strFilename, //file name
		OFN_EXPLORER | OFN_FILEMUSTEXIST,//flag
		strFilter,
		this);
	
	if(dlg.DoModal() == IDOK)
	{
		CWnd* pWnd = NULL;
		pWnd = GetDlgItem(IDC_EDIT_SP_RING);

		pWnd->SetWindowText(dlg.GetPathName());
		
		m_strClassVoicePath = dlg.GetPathName();
	}
}

void CNewClassDlg::OnOK() 
{
	// TODO: Add extra validation here
	UpdateData(true);	
	if(m_strClassName == "")
	{
		GMessageBox(LoadStringEx(IDS_INPUT_CLASSNAME));
		return;
	}
	CPhonebookPCClass *m_PBClass = ((CSunnyCatDlg *)AfxGetMainWnd())->m_linkManDlg.m_PBClass;
	int num = m_PBClass->Size();
	for (int i = 0; i < num; ++i)
	{
		if (m_strClassName.CompareNoCase(m_PBClass->GetAt(i).strClassName)==0)
		{
			GMessageBox(LoadStringEx(IDS_NEWCLASSEXIST));
			return;
		}
	}
	if(isNewClass)
	{	
		CreateClass();
	}	
	
	CBmpBkDialog::OnOK();
}

void CNewClassDlg::CreateClass()
{
		UpdateData(false);
		TPBClass pbClass;	
		pbClass.nParentID = 0;
		pbClass.strClassName = m_strClassName;
		pbClass.strVoicePath = m_strClassVoicePath;
		m_pPhonebook.Add(pbClass);		
}
