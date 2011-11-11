// NewCardDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SunnyCat.h"
#include "NewCardDlg.h"
#include "SelectClassDlg.h"
#include "LinkManDialog.h"
#include "SunnyCatDlg.h"
#include "Util.h"
#include "GlobalDataCenter.h"
#include "RepeatDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewCardDlg dialog


CNewCardDlg::CNewCardDlg(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CNewCardDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewCardDlg)
	m_strCompany = _T("");
	m_strCountry = _T("");
	m_strEmail = _T("");
	m_strFaxPhone = _T("");
	m_strHeadShip = _T("");
	m_strHomePhone = _T("");
	m_strMobilePhone = _T("");
	m_strName = _T("");
	m_strOfficePhone = _T("");
	m_strClassName = _T("");
	m_classID = 6;
	//}}AFX_DATA_INIT
	m_bDrawToolBar = false;
	isNewCard = true;
	m_iRepeat = REPEAT_OK;
}

CNewCardDlg::CNewCardDlg(CWnd* pParent,CString strPhone)
	: CBmpBkDialog(CNewCardDlg::IDD, pParent)
{
	m_strCompany = _T("");
	m_strCountry = _T("");
	m_strEmail = _T("");
	m_strFaxPhone = _T("");
	m_strHeadShip = _T("");
	m_strHomePhone = _T("");
	m_strMobilePhone = _T("");
	m_strName = _T("");
	m_strOfficePhone = _T("");
	m_classID = 6;
	m_strClassName = _T("");
	m_bDrawToolBar = false;
	isNewCard = true;
	m_iRepeat = REPEAT_OK;
	m_strPhone = strPhone;
}


void CNewCardDlg::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewCardDlg)
	DDX_Control(pDX, IDC_EDIT_OFFICEPHONE, m_EditOfficePB);
	DDX_Control(pDX, IDC_EDIT_MOBILEPHONE, m_EditMobilePB);
	DDX_Control(pDX, IDC_EDIT_HOMEPHONE, m_EditHomePB);
	DDX_Control(pDX, IDC_EDIT_FAXPHONE, m_EditFaxPB);
	DDX_Control(pDX, IDC_EDIT_SELCLASS, m_ctrlClassName);
	DDX_Text(pDX, IDC_EDIT_COMPANY, m_strCompany);
	DDV_MaxChars(pDX, m_strCompany, 128);
	DDX_Text(pDX, IDC_EDIT_COURTRY, m_strCountry);
	DDV_MaxChars(pDX, m_strCountry, 128);
	DDX_Text(pDX, IDC_EDIT_EMAIL, m_strEmail);
	DDV_MaxChars(pDX, m_strEmail, 128);
	DDX_Text(pDX, IDC_EDIT_FAXPHONE, m_strFaxPhone);
	DDV_MaxChars(pDX, m_strFaxPhone, 32);
	DDX_Text(pDX, IDC_EDIT_HEADSHIP, m_strHeadShip);
	DDV_MaxChars(pDX, m_strHeadShip, 64);
	DDX_Text(pDX, IDC_EDIT_HOMEPHONE, m_strHomePhone);
	DDV_MaxChars(pDX, m_strHomePhone, 32);
	DDX_Text(pDX, IDC_EDIT_MOBILEPHONE, m_strMobilePhone);
	DDV_MaxChars(pDX, m_strMobilePhone, 32);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDV_MaxChars(pDX, m_strName, 64);
	DDX_Text(pDX, IDC_EDIT_OFFICEPHONE, m_strOfficePhone);
	DDV_MaxChars(pDX, m_strOfficePhone, 32);
	DDX_Text(pDX, IDC_EDIT_SELCLASS, m_strClassName);
	DDV_MaxChars(pDX, m_strClassName, 64);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewCardDlg, CBmpBkDialog)
	//{{AFX_MSG_MAP(CNewCardDlg)
	ON_BN_CLICKED(IDC_BUTTON_SELCARDCLS, OnButtonSelcardcls)
	ON_BN_CLICKED(IDC_BUTTON_SELCLASS, OnButtonSelclass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewCardDlg message handlers

void CNewCardDlg::OnButtonSelcardcls() 
{
	// TODO: Add your control notification handler code here
	CSelectClassDlg m_selClass;
	if(m_selClass.DoModal() == IDOK)
	{
		return;
	}

}

void CNewCardDlg::OnOK() 
{
	// TODO: Add extra validation here
	if(isNewCard)
	{
		/*if(m_strClassName == "")
		{
			MessageBox(LoadStringEx(IDS_INPUT_CLASS));
				return;
		}
		*/

		TPhoneBookItemEx newCard;
		UpdateData(true);
	//	int parentID = ((CSunnyCatDlg *)AfxGetMainWnd())->m_linkManDlg.m_currTreeItemData;
		int parentID = m_classID;
		newCard.nParentID = parentID;
		newCard.strName = m_strName;
		if(m_strName == "")
		{
//			AfxMessageBox(LoadStringEx(IDS_INPUT_NAME));
			GMessageBox(LoadStringEx(IDS_INPUT_NAME));
			return;
		}
		
		
		newCard.strTypeName = m_className;
		newCard.strHomePhone = m_strHomePhone;
		newCard.strEmail = m_strEmail;
		newCard.strFax = m_strFaxPhone;	
		newCard.strMobilePhone = m_strMobilePhone;	
		newCard.strOfficePhone = m_strOfficePhone;
		newCard.strBusiness = m_strHeadShip;
		newCard.strOffice = m_strCompany;
		newCard.strCountry = m_strCountry;
		
		if(m_iRepeat != REPEAT_OK_ALL && m_iRepeat != REPEAT_NO_ALL)
		{
			CPhonebookPC phBookPC;
			if(phBookPC.IsExitRecord(newCard, false))
			{
				CRepeatDialog repeatDlg;
				repeatDlg.DoModal();
				m_iRepeat = repeatDlg.m_repeat;
			}
		}
		
		if(m_iRepeat == REPEAT_OK || m_iRepeat == REPEAT_OK_ALL)
		{
			bool bOk = phonebookPC.Add(newCard);
			if(!bOk)
			{
				GMessageBox(LoadStringEx(IDS_NEW_CARD_FAIL));
				return;
			}
		}
		else if(m_iRepeat == REPEAT_NO_ALL || m_iRepeat == REPEAT_NO)
		{
		
		}
		//((CSunnyCatDlg *)AfxGetMainWnd())->m_linkManDlg.PostMessage(WM_LINKMAN_NOTIFY, 0 ,1);
		((CSunnyCatDlg *)AfxGetMainWnd())->m_linkManDlg.PostMessage(WM_LINKMAN_NOTIFY, 0 ,1);//081106he SendMessage
	((CSunnyCatDlg *)AfxGetMainWnd())->m_linkManDlg.SetTreeText(m_classID);	

	}
//	((CSunnyCatDlg *)AfxGetMainWnd())->m_linkManDlg.PostMessage(WM_LINKMAN_NOTIFY, 0 ,1);//081106he SendMessage
//	((CSunnyCatDlg *)AfxGetMainWnd())->m_linkManDlg.SetTreeText(m_classID);	

	CBmpBkDialog::OnOK();
}

BOOL CNewCardDlg::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
//	m_strClassName = "Î´·ÖÀà"; 
	GetDlgItem(IDC_STATIC_COMMICATION)->SetWindowText(LoadStringEx(IDS_COMMUNITCATION));
	GetDlgItem(IDC_STATIC_SELCLASS)->SetWindowText(LoadStringEx(IDS_SELECT_CLASS));
	GetDlgItem(IDC_STATIC_CARDNAME)->SetWindowText(LoadStringEx(IDS_CARD_NAME));
	GetDlgItem(IDC_STATIC_HOME)->SetWindowText(LoadStringEx(IDS_HOME_NUM));
	GetDlgItem(IDC_STATIC_FAX)->SetWindowText(LoadStringEx(IDS_FAX_NUM));
	GetDlgItem(IDC_STATIC_MOBPHONE)->SetWindowText(LoadStringEx(IDS_MOBILE_NUM));
	GetDlgItem(IDC_STATIC_OFFICE)->SetWindowText(LoadStringEx(IDS_OFFICE_NUM));
	GetDlgItem(IDC_STATIC_ADDRESS)->SetWindowText(LoadStringEx(IDS_ADDRESS_INFO));
	GetDlgItem(IDC_STATIC_COUNTRY)->SetWindowText(LoadStringEx(IDS_COUNTRY));
	GetDlgItem(IDC_STATIC_COMPANYNAME)->SetWindowText(LoadStringEx(IDS_COMPANY));
	GetDlgItem(IDC_STATIC_HEADSHIP)->SetWindowText(LoadStringEx(IDS_HEADSHIP));
	GetDlgItem(IDC_BUTTON_SELCLASS)->SetWindowText(LoadStringEx(IDS_BUTTON_SELECT));
	GetDlgItem(IDOK)->SetWindowText(LoadStringEx(IDS_OK));
	GetDlgItem(IDCANCEL)->SetWindowText(LoadStringEx(IDS_CANCEL));

	UpdateData(false);
	m_strClassName = m_className;
	if(m_strName == "" && m_strMobilePhone == "")
	{
		if(m_className == "")
		{
			m_strClassName = LoadStringEx(IDS_CLASS_NOCLASS);
			m_classID = 6;
		}
		isNewCard = true;
		
		SetWindowText(LoadStringEx(IDS_NEW_CARD));
	}
	else
	{
		isNewCard = false;
		SetWindowText(LoadStringEx(IDS_EDIT_CARD));
	}
	if(m_strPhone != "")
	m_strMobilePhone = m_strPhone;
	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewCardDlg::OnButtonSelclass() 
{
	// TODO: Add your control notification handler code here
	CSelectClassDlg selClass(this);
	if(selClass.DoModal() == IDOK)
	{
		m_strClassName = m_className;
		m_ctrlClassName.SetWindowText(m_strClassName);
		//UpdateData(false);
		return;
	}
}
