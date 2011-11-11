// NewSmsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SunnyCat.h"
#include "SunnyCatDlg.h"
#include "NewSmsDialog.h"
#include "SelectLinkmanDlg.h"
#include "Util.h"
#include "DlgProgressEx.h"
#include "TheadSendSMS.h"
#include "SMSDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CNewSmsDialog dialog
//extern std::vector<CString> m_numbers;

CNewSmsDialog::CNewSmsDialog(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CNewSmsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNewSmsDialog)
	m_strSendSmsNum = _T("");
	m_strSendSmsContent = _T("");
	m_strContent = _T("");
	m_strSendNum = _T("");
	//}}AFX_DATA_INIT
	m_bDrawToolBar = false;
	m_strSendSmsNum = strNumbers;
	m_bEditSMS = false;
	m_pParent = pParent;
}

CNewSmsDialog::CNewSmsDialog(CWnd* pParent ,CString strPhone,int nNum)
	: CBmpBkDialog(CNewSmsDialog::IDD, pParent)
{
	m_strSendSmsNum = _T("");
	//m_strSendSmsInfo = _T("");
	m_strSendSmsContent = _T("");
	m_bDrawToolBar = false;

	m_pParent = pParent;

	m_strPhone = strPhone;
	m_nNum = nNum;

	m_bEditSMS = false;

}

void CNewSmsDialog::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNewSmsDialog)
	DDX_Control(pDX, IDC_STATIC_SEND, m_ctrlSendNum);
	DDX_Control(pDX, IDC_STATIC_CONTENT, m_ctrlContent);
	DDX_Control(pDX, IDC_EDIT_SMS_NUM, m_ctrlSmsNum);
	DDX_Text(pDX, IDC_EDIT_SMS_NUM, m_strSendSmsNum);
	DDV_MaxChars(pDX, m_strSendSmsNum, 1024);
	DDX_Text(pDX, IDC_EDIT_SMS_CONTENT, m_strSendSmsContent);
	DDV_MaxChars(pDX, m_strSendSmsContent, 1024);
	DDX_Text(pDX, IDC_STATIC_CONTENT, m_strContent);
	DDX_Text(pDX, IDC_STATIC_SEND, m_strSendNum);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNewSmsDialog, CBmpBkDialog)
	//{{AFX_MSG_MAP(CNewSmsDialog)
	ON_BN_CLICKED(IDC_BUTTON_SMS_SAVE, OnButtonSmsSave)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_SEL_LINKMAN, OnButtonSelLinkman)
	ON_EN_SETFOCUS(IDC_EDIT_SMS_NUM, OnSetfocusEditSmsNum)
	ON_EN_CHANGE(IDC_EDIT_SMS_NUM, OnChangeEditSmsNum)
	ON_EN_CHANGE(IDC_EDIT_SMS_CONTENT, OnChangeEditSmsContent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewSmsDialog message handlers



void CNewSmsDialog::OnButtonSmsSave() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	m_bEditSMS = false;

	vector<CString> contents;
	vector<CString> numbers;
	
	int nfind = 0;
	
	if(m_strSendSmsNum.GetLength() < 1)
		return;
	CString right = m_strSendSmsNum;
	
	if(right.Right(0) != ";")
	{
		right += ";";
	}
	int size = right.GetLength();
	
	while(1)
	{
		nfind = right.Find(TEXT(";"));
		if(nfind >= 0)
		{
			CString strnum = right.Left(nfind);
			strnum.TrimLeft();
			strnum.TrimRight();
			if(strnum != "")
				numbers.push_back(right.Left(nfind));
			right = m_strSendSmsNum.Right(right.GetLength() - right.Left(nfind).GetLength()-1);
			if(right == "")
				break;
		}
		else
		{
			numbers.push_back(right);
			break;
		}
	}


	int numSize = numbers.size();

	TSMSItem item;
	for(int j = 0; j < numSize; j++)
	{
		 item.strPhoneNum = numbers[j];
		 int length = m_strSendSmsContent.GetLength();
		 item.strText = m_strSendSmsContent;
		 item.nMailBoxID = (SMSMailBoxID)SMS_MAILBOX_DRAFT;
		 item.nOrigMailBoxID = (SMSMailBoxID)SMS_MAILBOX_DRAFT;
		 item.nPSMSState = (PSMSType)1;
		 item.tmSMSTime = CTime::GetCurrentTime();
		 ((CSunnyCatDlg *)AfxGetMainWnd())->m_smsDlg.m_PCDraft.AddBack(item);
		 TRACE(TEXT("m_PCDraft.Size() = %d\n"), ((CSunnyCatDlg *)AfxGetMainWnd())->m_smsDlg.m_PCDraft.Size());
	}

	((CSunnyCatDlg *)AfxGetMainWnd())->m_smsDlg.PostMessage(WM_SMSLIST_NOTIFY,0,1);//SMS_PC_DRAFT);
	((CSunnyCatDlg *)AfxGetMainWnd())->m_smsDlg.ClickTreeShowText(5);
	((CSunnyCatDlg *)AfxGetMainWnd())->m_smsDlg.ClickTreeShowText(6);
	 OnOK();

}

void LogString(const char* str,size_t size);
void LogString(const wchar_t* str,size_t size);
void LogString(CString strLog);

void CNewSmsDialog::OnButtonSend() 
{
	//if(!((CSunnyCatDlg*)m_pParent)->m_MobileManager.isAbleCom)
	if(!((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.isAbleCom)
	{
		GMessageBox(LoadStringEx(IDS_NO_SC700_CARD));
		return;
	}

	UpdateData(true);
	/*********************************************************
	if(((CSunnyCatDlg *)AfxGetMainWnd())->m_isConnect)	
	{
		CString strInfo = LoadStringEx(IDS_PROMPT_ONLINE);
		GMessageBox(strInfo);
		return;
	}
	else
	/*********************************************************/
	{
		vector<CString> contents;
		vector<CString> numbers;

		EStringType textType = GetStringType(m_strSendSmsContent);
		///检查字符串长度是否符合要求
		int nLimit = MaxSMSLength(textType);
		CString sendConntent = m_strSendSmsContent;
		while(GetCharNum(m_strSendSmsContent) > nLimit)//划分成多个待发短信
		{
			CString str = GetLeftString(m_strSendSmsContent, nLimit);
			contents.push_back(str);
			m_strSendSmsContent.Delete(0, str.GetLength());
		}
		
		if(!m_strSendSmsContent.IsEmpty() || !contents.size())
		{
			contents.push_back(m_strSendSmsContent);
		}
	
		int nfind = 0;

		if(m_strSendSmsNum.GetLength() < 1)
			return;
	
		CString right = m_strSendSmsNum;
		
		if(right.Right(0) != ";")
		{
			right += ";";
		}
		int size = right.GetLength();
		
		while(1)
		{
			nfind = right.Find(TEXT(";"));
			if(nfind >= 0)
			{
				CString strnum = right.Left(nfind);
				strnum.TrimLeft();
				strnum.TrimRight();
				if(strnum != "")
				numbers.push_back(right.Left(nfind));
				right = m_strSendSmsNum.Right(right.GetLength() - right.Left(nfind).GetLength()-1);
				if(right == "")
					break;
			}
			else
			{
				numbers.push_back(right);
				break;
			}
		}
		if (numbers.size() >= 20)
		{
			if(GMessageBox(LoadStringEx(IDC_MSG_SMSSENDLENLONG), NULL, MB_OKCANCEL|MB_ICONASTERISK) == IDOK)
			{
			}
			else
			{
				return;
			}
		}
		
		CThreadSendSMS thread(((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager,
						numbers,
						contents,
						-1,
						-1,
						textType);

		CDlgProgressEx dlg(thread, this, contents.size()*numbers.size()>1);
		dlg.DoModal();

		LogString("SMS Send end");
		((CSunnyCatDlg *)AfxGetMainWnd())->m_smsDlg.PostMessage(WM_SMSLIST_NOTIFY,0,1);
		LogString("SMS Send end notify");
		((CSunnyCatDlg *)AfxGetMainWnd())->m_smsDlg.ClickTreeShowText(6);
		LogString("SMS Send end click");
		numbers.clear();
		contents.clear();
		m_bEditSMS = false;

		return;
	}

}



void CNewSmsDialog::OnButtonSelLinkman() 
{
	// TODO: Add your control notification handler code here
	CSelectLinkmanDlg m_selectLinkman;
	if(m_selectLinkman.DoModal() == IDOK)
	{
		return;
	}
}

void CNewSmsDialog::UpdateInfo(CString numbers)
{
	m_strSendSmsNum += numbers;
	UpdateData(false);
	OnChangeEditSmsNum();
}

void CNewSmsDialog::OnSetfocusEditSmsNum() 
{
	// TODO: Add your control notification handler code here
	
}

void CNewSmsDialog::OnChangeEditSmsNum() 
{
	m_bEditSMS = true;
	UpdateData(true);
	int size = m_strSendSmsNum.GetLength();
	TCHAR keyTable[] = 
	{
		TEXT('+'),TEXT('0'),TEXT('1'),TEXT('2'),TEXT('3'),
		TEXT('4'),TEXT('5'),TEXT('6'),TEXT('7'),TEXT('8'),TEXT('9'),TEXT(';')
	};

	for(int i = 0; i < size; i++)
	{
		TCHAR key = m_strSendSmsNum[i];
		if(!EnableKey(key, keyTable))
		{		
			GMessageBox(LoadStringEx(IDS_STRING_ERRORNUM));
			return;
		}
	}
	if(m_strSendSmsNum != "")
	{
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SMS_SAVE)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SMS_SAVE)->EnableWindow(FALSE);
	}

	if(!((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.m_bSimExist)
	{
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
	}

}

void CNewSmsDialog::OnCancel() 
{
	// TODO: Add extra cleanup here
	CString strSendNum = m_strSendSmsNum;
	CString strContent = m_strSendSmsContent;

	UpdateData();
	if(m_strSendSmsNum == "")
	{
		CBmpBkDialog::OnCancel();
		return;
	}


	if(m_bEditSMS)
	{
		if((m_strOriSendNum != m_strSendSmsNum)||(m_strOriContent != m_strSendSmsContent))
		{
			if(GMessageBox(LoadStringEx(IDS_IS_SAVE), NULL, MB_OKCANCEL|MB_ICONASTERISK) == IDOK)
			{
				OnButtonSmsSave();
				return;
			}
		}
	}
	
	
	CBmpBkDialog::OnCancel();
}

BOOL CNewSmsDialog::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	m_strOriSendNum = m_strSendSmsNum;
	m_strOriContent = m_strSendSmsContent;

	if(m_strPhone != "")
	{
		m_strSendSmsNum = m_strPhone;
	}
	UpdateData(false);

	GetDlgItem(IDC_BUTTON_SMS_SAVE)->SetWindowText(LoadStringEx(IDS_SAVE_SMS));
	GetDlgItem(IDC_BUTTON_SEND)->SetWindowText(LoadStringEx(IDS_SEND_SMS));
	GetDlgItem(IDCANCEL)->SetWindowText(LoadStringEx(IDS_CANCEL));
	GetDlgItem(IDC_BUTTON_SEL_LINKMAN)->SetWindowText(LoadStringEx(IDS_SMS_RECEIVER));
	GetDlgItem(IDC_STATIC_RECEIVER)->SetWindowText(LoadStringEx(IDS_RECEIVER));
	GetDlgItem(IDC_SMS_NOTICE)->SetWindowText(LoadStringEx(IDS_SMS_NOTICE));
	GetDlgItem(IDC_STATIC_SMS_CONTENT)->SetWindowText(LoadStringEx(IDS_SMS_CONTENT));

	OnChangeEditSmsContent();
	if(m_strSendSmsNum != "" || m_strSendSmsContent != "")
		SetWindowText(LoadStringEx(IDS_EDIT_SMS));
	else
		SetWindowText(LoadStringEx(IDS_NEW_SMS));
	UpdateData();

	if(!((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.m_bSimExist)
	{
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
	}

	if(m_strSendSmsNum == "")
	{
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SMS_SAVE)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_SMS_SAVE)->EnableWindow(TRUE);
	}
	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNewSmsDialog::OnChangeEditSmsContent() 
{
	m_bEditSMS = true;
	UpdateData(true);
	CString content = LoadStringEx(IDS_SMS_INALL) + Util::intToString(m_strSendSmsContent.GetLength());	
	content += LoadStringEx(IDS_SMS_SUM);
	m_ctrlContent.SetWindowText(content);	
}

