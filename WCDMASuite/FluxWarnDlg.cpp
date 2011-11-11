// FluxWarn.cpp : implementation file
//

#include "stdafx.h"
#include "sunnycat.h"
#include "FluxWarnDlg.h"
#include <atlbase.h>
#include "porting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFluxWarn dialog


CFluxWarnDlg::CFluxWarnDlg(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CFluxWarnDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFluxWarn)
	m_bYN = FALSE;
	m_Date = _T("");
	m_Nm = _T("");
	m_bUncnt = FALSE;
	m_NmUncnt = _T("");
	m_NmWarn = _T("");
	//}}AFX_DATA_INIT
}


void CFluxWarnDlg::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFluxWarn)
	DDX_Control(pDX, IDC_SPIN_WARN, m_WarnSpin);
	DDX_Control(pDX, IDC_SPIN_UNCNT, m_UncntSpin);
	DDX_Control(pDX, IDC_EDIT_NMWARN, m_NmWarnEdit);
	DDX_Control(pDX, IDC_EDIT_NMUNCNT, m_NmcntEdit);
	DDX_Control(pDX, IDC_CHECK_UNCNT, m_UncntCheck);
	DDX_Control(pDX, IDC_SPIN_NM, m_NmSpin);
	DDX_Control(pDX, IDC_SPIN_DATE, m_DateSpin);
	DDX_Control(pDX, IDC_CHECK_YN, m_YNCheck);
	DDX_Control(pDX, IDC_EDIT_NM, m_NmEdit);
	DDX_Control(pDX, IDC_EDIT_DATE, m_DateEdit);
	DDX_Check(pDX, IDC_CHECK_YN, m_bYN);
	DDX_Text(pDX, IDC_EDIT_DATE, m_Date);
	DDV_MaxChars(pDX, m_Date, 2);
	DDX_Text(pDX, IDC_EDIT_NM, m_Nm);
	DDV_MaxChars(pDX, m_Nm, 3);
	DDX_Check(pDX, IDC_CHECK_UNCNT, m_bUncnt);
	DDX_Text(pDX, IDC_EDIT_NMUNCNT, m_NmUncnt);
	DDV_MaxChars(pDX, m_NmUncnt, 5);
	DDX_Text(pDX, IDC_EDIT_NMWARN, m_NmWarn);
	DDV_MaxChars(pDX, m_NmWarn, 5);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFluxWarnDlg, CBmpBkDialog)
	//{{AFX_MSG_MAP(CFluxWarn)
	ON_BN_CLICKED(IDC_CHECK_YN, OnCheckYn)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DATE, OnDeltaposSpinDate)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_NM, OnDeltaposSpinNm)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PERCENT, OnDeltaposSpinWarn)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_UPLIMIT, OnDeltaposSpinUncnt)
	ON_EN_KILLFOCUS(IDC_EDIT_DATE, OnKillfocusEditDate)
	ON_EN_KILLFOCUS(IDC_EDIT_UPLIMIT, OnKillfocusEditUncnt)
	ON_EN_KILLFOCUS(IDC_EDIT_PERCENT, OnKillfocusEditWarn)
	ON_EN_KILLFOCUS(IDC_EDIT_NM, OnKillfocusEditNm)
	ON_BN_CLICKED(IDC_CHECK_UNCNT, OnCheckUncnt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFluxWarn message handlers

BOOL CFluxWarnDlg::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	GetValueFromReg();
	UpdateData(FALSE);

	m_DateSpin.SetBase(10);
	m_DateSpin.SetRange32(1,31);
	m_DateSpin.SetBuddy(&m_DateEdit);
	m_DateSpin.SetPos(_ttoi(m_Date));

	m_UncntSpin.SetBase(10);
	m_UncntSpin.SetRange32(1,99999);
	m_UncntSpin.SetBuddy(&m_NmcntEdit);
	m_UncntSpin.SetPos(_ttol(m_NmUncnt));

	m_WarnSpin.SetBase(10);
	m_WarnSpin.SetRange32(1,99999);
	m_WarnSpin.SetBuddy(&m_NmWarnEdit);
	m_WarnSpin.SetPos(_ttoi(m_NmWarn));

	m_NmSpin.SetBase(10);
	m_NmSpin.SetRange32(1,999);
	m_NmSpin.SetBuddy(&m_NmEdit);
	m_NmSpin.SetPos(_ttoi(m_Nm));


	if(!m_bYN)
	{
		DisableCtrls();
	}
	else
	{
		if(m_UncntCheck.GetCheck() == BST_CHECKED)
			m_NmcntEdit.EnableWindow(TRUE);
		else
			m_NmcntEdit.EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CFluxWarnDlg::GetValueFromReg()
{
	CRegKey reg;
    
    CString regPath = GetConfigString();

	if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS) //打开成功
	{
		reg.Create(HKEY_LOCAL_MACHINE, regPath);		
		if(reg.Open(HKEY_LOCAL_MACHINE, regPath) != ERROR_SUCCESS)
		{
			return TRUE;
		}
	}
	
	DWORD dwValue = 0;

	if(reg.QueryValue(dwValue, TEXT("UseFluxStat")) == ERROR_SUCCESS)
	{
		if(dwValue >0 )
			m_bYN = TRUE;
		else
			m_bYN = FALSE;
	}
	else
	{
		m_bYN = TRUE;
	}
	
	if(reg.QueryValue(dwValue, TEXT("bUncnt")) == ERROR_SUCCESS)
	{
		if(dwValue >0 )
			m_bUncnt = TRUE;
		else
			m_bUncnt = FALSE;
	}
	else
	{
		m_bUncnt = TRUE;
	}


	TCHAR szValue[1024] = {0};
	DWORD nPreCount = sizeof(szValue)-1;
	DWORD nCount = sizeof(szValue)-1;
	if(reg.QueryValue(szValue, TEXT("Date"), & nCount) == ERROR_SUCCESS)
	{
		m_Date = szValue;
	}
	else
	{
		m_Date = TEXT("1");
	}

	nCount = nPreCount;
	if(reg.QueryValue(szValue, TEXT("UncntNM"), & nCount) == ERROR_SUCCESS)
	{
		m_NmUncnt = szValue;
	}
	else
	{
		m_NmUncnt = TEXT("1000");
	}

	nCount = nPreCount;
	if(reg.QueryValue(szValue, TEXT("WarnNM"), & nCount) == ERROR_SUCCESS)
	{
		m_NmWarn = szValue;
	}
	else
	{
		m_NmWarn = TEXT("800");
	}

	nCount = nPreCount;
	if(reg.QueryValue(szValue, TEXT("NM"), & nCount) == ERROR_SUCCESS)
	{
		m_Nm = szValue;
	}
	else
	{
		m_Nm = "10";
	}
	reg.Close();
	return TRUE;

}

void CFluxWarnDlg::EnableCtrls()
{
	m_UncntCheck.EnableWindow(TRUE);
	m_DateEdit.EnableWindow(TRUE);
	m_NmcntEdit.EnableWindow(TRUE);
	m_NmWarnEdit.EnableWindow(TRUE);
	m_NmEdit.EnableWindow(TRUE);

	m_DateSpin.EnableWindow(TRUE);
	m_UncntSpin.EnableWindow(TRUE);
	m_WarnSpin.EnableWindow(TRUE);
	m_NmSpin.EnableWindow(TRUE);

	CWnd* pWnd = NULL;
	pWnd = GetDlgItem(IDC_STATIC_2);
	pWnd->EnableWindow(TRUE);
	pWnd = GetDlgItem(IDC_STATIC_3);
	pWnd->EnableWindow(TRUE);
	pWnd = GetDlgItem(IDC_STATIC_4);
	pWnd->EnableWindow(TRUE);
	pWnd = GetDlgItem(IDC_STATIC_5);
	pWnd->EnableWindow(TRUE);
	pWnd = GetDlgItem(IDC_STATIC_6);
	pWnd->EnableWindow(TRUE);

	pWnd = GetDlgItem(IDC_STATIC_7);
	pWnd->EnableWindow(TRUE);
	pWnd = GetDlgItem(IDC_STATIC_8);
	pWnd->EnableWindow(TRUE);
	pWnd = GetDlgItem(IDC_STATIC_9);
	pWnd->EnableWindow(TRUE);

	if(m_UncntCheck.GetCheck() == BST_CHECKED)
		m_NmcntEdit.EnableWindow(TRUE);
	else
		m_NmcntEdit.EnableWindow(FALSE);
}

void CFluxWarnDlg::DisableCtrls()
{
	m_UncntCheck.EnableWindow(FALSE);
	m_DateEdit.EnableWindow(FALSE);
	m_NmcntEdit.EnableWindow(FALSE);
	m_NmWarnEdit.EnableWindow(FALSE);
	m_NmEdit.EnableWindow(FALSE);

	m_DateSpin.EnableWindow(FALSE);
	m_UncntSpin.EnableWindow(FALSE);
	m_WarnSpin.EnableWindow(FALSE);
	m_NmSpin.EnableWindow(FALSE);

	CWnd* pWnd = NULL;
	pWnd = GetDlgItem(IDC_STATIC_2);
	pWnd->EnableWindow(FALSE);
	pWnd = GetDlgItem(IDC_STATIC_3);
	pWnd->EnableWindow(FALSE);
	pWnd = GetDlgItem(IDC_STATIC_4);
	pWnd->EnableWindow(FALSE);
	pWnd = GetDlgItem(IDC_STATIC_5);
	pWnd->EnableWindow(FALSE);
	pWnd = GetDlgItem(IDC_STATIC_6);
	pWnd->EnableWindow(FALSE);
	
	pWnd = GetDlgItem(IDC_STATIC_7);
	pWnd->EnableWindow(FALSE);
	pWnd = GetDlgItem(IDC_STATIC_8);
	pWnd->EnableWindow(FALSE);
	pWnd = GetDlgItem(IDC_STATIC_9);
	pWnd->EnableWindow(FALSE);
}

void CFluxWarnDlg::OnCheckYn() 
{
	// TODO: Add your control notification handler code here
	if(m_YNCheck.GetCheck() == BST_CHECKED)
	{
		EnableCtrls();
	}
	else
	{
		DisableCtrls();
	}

}

void CFluxWarnDlg::OnDeltaposSpinDate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int noffset = pNMUpDown->iDelta;
	CString strDate = "";
	int nPos = pNMUpDown->iPos+noffset;
	if(nPos < 1)
		nPos = 1;
	if(nPos > 31)
		nPos = 31;
	strDate.Format(TEXT("%d"),nPos);
	m_Date = strDate;
//	UpdateData(FALSE);
//	m_DateEdit.SetWindowText(strDate);

	*pResult = 0;
}

void CFluxWarnDlg::OnDeltaposSpinNm(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int noffset = pNMUpDown->iDelta;
	CString strNm = "";
	int nPos = pNMUpDown->iPos+noffset;
	if(nPos < 0)
		nPos = 0;
	if(nPos > 999)
		nPos = 999;
	strNm.Format(TEXT("%d"),nPos);
	m_NmEdit.SetWindowText(strNm);

	*pResult = 0;
}

void CFluxWarnDlg::OnDeltaposSpinWarn(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int noffset = pNMUpDown->iDelta;
	CString strUpLimit = "";
	int nPos = pNMUpDown->iPos+noffset;
	if(nPos < 0)
		nPos = 0;
	if(nPos > 99999)
		nPos = 99999;
	strUpLimit.Format(TEXT("%d"),nPos);
	m_NmWarnEdit.SetWindowText(strUpLimit);
	
	*pResult = 0;
}

void CFluxWarnDlg::OnDeltaposSpinUncnt(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int noffset = pNMUpDown->iDelta;
	CString strUpLimit = "";
	int nPos = pNMUpDown->iPos+noffset;
	if(nPos < 0)
		nPos = 0;
	if(nPos > 99999)
		nPos = 99999;
	strUpLimit.Format(TEXT("%d"),nPos);
	m_NmcntEdit.SetWindowText(strUpLimit);
	
	*pResult = 0;
}

void CFluxWarnDlg::OnKillfocusEditDate() 
{
	// TODO: Add your control notification handler code here
	CString strText = "";
	m_DateEdit.GetWindowText(strText);
	int nPos = _ttoi(strText);
	if(nPos > 31)
	{
		m_DateEdit.SetWindowText(TEXT("31"));
		nPos = 31;
	}
}

void CFluxWarnDlg::OnKillfocusEditUncnt() 
{
	// TODO: Add your control notification handler code here
	CString strText = "";
	m_NmcntEdit.GetWindowText(strText);
	long nPos = _ttol(strText);
	if(nPos > 99999)
	{
		m_NmcntEdit.SetWindowText(TEXT("99999"));
		nPos = 99999;
	}
//	m_NmWarnEdit.GetWindowText(strText);
//	long nWarnNM = atol(strText);
//	if(nPos < nWarnNM)
//	{
//		GMessageBox(::LoadStringEx(IDS_FLUX_ERRORSET),::LoadStringEx(IDS_FLUX_ERRORSETTITLE),MB_OK|MB_ICONEXCLAMATION);
//		m_NmcntEdit.SetWindowText(strText);
//	}
	
}

void CFluxWarnDlg::OnKillfocusEditWarn() 
{
	// TODO: Add your control notification handler code here
	CString strText = "";
	m_NmWarnEdit.GetWindowText(strText);
	int nPos = _ttol(strText);
	if(nPos > 99999)
	{
		m_NmWarnEdit.SetWindowText(TEXT("99999"));
		nPos = 99999;
	}
//	m_NmcntEdit.GetWindowText(strText);
//	long nUncntNM = atol(strText);
//	if(nPos > nUncntNM)
//	{
//		GMessageBox(::LoadStringEx(IDS_FLUX_ERRORSET),::LoadStringEx(IDS_FLUX_ERRORSETTITLE),MB_OK|MB_ICONEXCLAMATION);
//		m_NmWarnEdit.SetWindowText(strText);
//	}
	
}

void CFluxWarnDlg::OnKillfocusEditNm() 
{
	// TODO: Add your control notification handler code here
	CString strText ;
	m_NmEdit.GetWindowText(strText);
	int nPos = _ttoi(strText);
	if(nPos > 999)
	{
		m_NmEdit.SetWindowText(TEXT("999"));
		nPos = 999;
	}
}

void CFluxWarnDlg::OnCheckUncnt() 
{
	// TODO: Add your control notification handler code here
	if(m_UncntCheck.GetCheck() == BST_CHECKED)
		m_NmcntEdit.EnableWindow(TRUE);
	else
		m_NmcntEdit.EnableWindow(FALSE);
}
