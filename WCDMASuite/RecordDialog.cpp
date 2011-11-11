// RecordDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SunnyCat.h"
#include "RecordDialog.h"
#include "GlobalDataCenter.h"
#include "SunnyCatDlg.h"
#include "CallDialog.h"
#include "Util.h"
#include "NewSmsDialog.h"
#include "NetStatByTime.h"
#include "NewCardDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecordDialog dialog
//extern CCallDialog *pCallDialog;

CRecordDialog::CRecordDialog(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CRecordDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRecordDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	currItem = 0;
	currItemData = 0;
	g_pSysImageList   = new CImageList();
	m_bFirst = TRUE;
	m_bListFirst = TRUE;
	m_talkItem = NULL;
	m_dialItem = NULL;
	m_missItem = NULL;
	m_allItem = NULL;
}
	
CRecordDialog::~CRecordDialog()
{
	delete g_pSysImageList;
	g_pSysImageList = NULL;
}

void CRecordDialog::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRecordDialog)
	DDX_Control(pDX, IDC_TREE_RECORD, m_ctrlRecordTree);
	DDX_Control(pDX, IDC_LIST_RECORD, m_ctrlRecordList);
	//}}AFX_DATA_MAP
	m_bDrawToolBar  = true ;       //是否加载工具栏背景1
}


BEGIN_MESSAGE_MAP(CRecordDialog, CBmpBkDialog)
	//{{AFX_MSG_MAP(CRecordDialog)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_RECORD, OnRclickListRecord)
	ON_COMMAND(IDR_RECORD_SAVE, OnRecordSave)
	ON_COMMAND(IDR_RECORD_OTHER, OnRecordOther)
	ON_COMMAND(IDR_RECORD_DIAL, OnRecordDial)
	ON_COMMAND(IDR_RECORD_DEL, OnRecordDel)
	ON_COMMAND(IDR_RECORD_CLEAR, OnRecordClear)
	ON_COMMAND(IDR_RECORD_ALL, OnRecordAll)
	ON_NOTIFY(NM_CLICK, IDC_LIST_RECORD, OnClickListRecord)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_RECORD, OnDblclkListRecord)
	ON_BN_CLICKED(IDC_BUTTON_RECORD_DEL, OnButtonRecordDel)
	ON_BN_CLICKED(IDC_BUTTON_RECORD_CLEAR, OnButtonRecordClear)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_RECORD, OnRclickTreeRecord)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_RECORD, OnSelchangedTreeRecord)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_RECORD, OnItemchangedListRecord)
	ON_COMMAND(IDR_RECORD_SMS, OnRecordSms)
	ON_COMMAND(IDM_DELETE, OnDelete)
	ON_COMMAND(IDM_CLEAR, OnClear)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MY_MOVE,OnMyMove)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecordDialog message handlers


BOOL CRecordDialog::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	GetDlgItem(IDC_BUTTON_RECORD_DEL)->SetWindowText(LoadStringEx(IDS_LINKMAN_DEL));
	GetDlgItem(IDC_BUTTON_RECORD_CLEAR)->SetWindowText(LoadStringEx(IDS_LINKMAN_CLEAR));
	InitUI();
	// TODO: Add extra initialization here
//	m_resize.Create( this, TRUE, 400);
//	m_resize.SetGripEnabled( TRUE );
	
//	m_resize.Add(IDC_TREE_RECORD,0,0,0,400);
	//m_resize.Add(IDC_LIST_RECORD,0,0,400,0);

//	m_resize.SetSendMessage();

//	m_resize.SetMinimumTrackingSize(CSize(450,500));
//	m_resize.SetMaximumTrackingSize(CSize(450,500));

	CRect rc;
	CWnd* pWnd;

	pWnd = GetDlgItem(IDC_SPLITTER1);
	pWnd->GetWindowRect(rc);
	ScreenToClient(rc);
	m_wndSplitter1.Create(WS_CHILD | WS_VISIBLE, rc, this, IDC_SPLITTER1);
	m_wndSplitter1.SetRange(140, 260, -1);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CRecordDialog::InitUI()
{
	
	LoadImgList(g_pSysImageList, IDB_LINKMANTREE);
	//初使化历史记录树
	m_ctrlRecordTree.SetImageList(g_pSysImageList, LVSIL_NORMAL);
	HTREEITEM m_recordItem = m_ctrlRecordTree.InsertItem(LoadStringEx(IDS_RECORD_CALL),RECORD_IMAG_CALL,RECORD_IMAG_CALL);

	m_callRecord.ClearVector();
	m_callRecord.GetRecordsByRecordType(CALLRECORD_RECV);
	int size = m_callRecord.Size();
	CString strText= LoadStringEx(IDS_RECORD_RECV);
	CString strNum = "";
	strNum.Format(TEXT("(%d)"), size);
	strText += strNum;
	m_talkItem = m_ctrlRecordTree.InsertItem(strText,RECORD_IMAG_CALL_RECV,RECORD_IMAG_CALL_RECV, m_recordItem);
	m_callRecord.ClearVector();
	m_callRecord.GetRecordsByRecordType(CALLRECORD_DAIL);
	size = m_callRecord.Size();
	strText= LoadStringEx(IDS_RECORD_DAIL);	
	strNum.Format(TEXT("(%d)"), size);
	strText += strNum;
	m_dialItem = m_ctrlRecordTree.InsertItem(strText,RECORD_IMAG_CALL_CALL,RECORD_IMAG_CALL_CALL, m_recordItem);
	m_callRecord.ClearVector();
	m_callRecord.GetRecordsByRecordType(CALLRECORD_MISS);
	size = m_callRecord.Size();
	strText= LoadStringEx(IDS_RECORD_MISS);	
	strNum.Format(TEXT("(%d)"), size);
	strText += strNum;
	m_missItem = m_ctrlRecordTree.InsertItem(strText, RECORD_IMAG_CALL_MISS,RECORD_IMAG_CALL_MISS, m_recordItem);


	
	m_ctrlRecordTree.SetItemData(m_recordItem, CALL_RECORD);
	m_ctrlRecordTree.SetItemData(m_talkItem, CALL_RECORD_RECV);
	m_ctrlRecordTree.SetItemData(m_dialItem, CALL_RECORD_OUTCALL);
	m_ctrlRecordTree.SetItemData(m_missItem, CALL_RECORD_NOTGET);



	HTREEITEM m_netItem = m_ctrlRecordTree.InsertItem(LoadStringEx(IDS_RECORD_NET),NETRECORD_IMAG,NETRECORD_IMAG);
	m_connectRecord.ClearVector();
	m_connectRecord.GetAllRecords();
	size = m_connectRecord.Size();
	strText= LoadStringEx(IDS_RECORD_NETALL);	
	strNum.Format(TEXT("(%d)"), size);
	strText += strNum;
	m_allItem = m_ctrlRecordTree.InsertItem(strText, NETRECORD_IMAG_ALL,NETRECORD_IMAG_ALL,m_netItem);	

	HTREEITEM m_monthItem = m_ctrlRecordTree.InsertItem(LoadStringEx(IDS_RECORD_NETMON),NETRECORD_IMAG_MONTH,NETRECORD_IMAG_MONTH, m_netItem);

	HTREEITEM m_timesItem = m_ctrlRecordTree.InsertItem(LoadStringEx(IDS_RECORD_NETTIME),NETRECORD_IMAG_TIME,NETRECORD_IMAG_TIME, m_netItem);

	m_ctrlRecordTree.SetItemData(m_netItem, NET_RECORD);
	m_ctrlRecordTree.SetItemData(m_allItem, NET_RECORD_ALL);
	m_ctrlRecordTree.SetItemData(m_monthItem, NET_RECORD_MONTH);
	m_ctrlRecordTree.SetItemData(m_timesItem, NET_RECORD_TIME);



	//展开树
	m_ctrlRecordTree.Expand(m_recordItem, TVE_EXPAND);
	m_ctrlRecordTree.Expand(m_netItem, TVE_EXPAND);
	m_ctrlRecordTree.SetBkColor(RGB(255,255,255));
	m_ctrlRecordTree.SetTextColor(RGB(100,100,100));
	
//	m_ctrlRecordTree.SelectItem(m_dialItem);
//	m_ctrlRecordTree.SelectSetFirstVisible(m_dialItem) ;
//	m_ctrlRecordTree.SelectItem(m_talkItem);
	//初使化历史记录列表
	LoadListHead(CALLRECORD);
	//ShowList(CALLRECORD_RECV);
	m_ctrlRecordTree.SelectItem(m_dialItem);

}

void CRecordDialog::ShowTreeText(DWORD currItemData)
{
	CString strText = "";
	CString strNum = "";
	int size = 0;
	switch(currItemData)
	{
		case CALL_RECORD_RECV:
			m_callRecord.ClearVector();
			m_callRecord.GetRecordsByRecordType(CALLRECORD_RECV);
			size = m_callRecord.Size();
			strText= LoadStringEx(IDS_RECORD_RECV);	
			strNum.Format(TEXT("(%d)"), size);
			strText += strNum;
			m_ctrlRecordTree.SetItemText(m_talkItem, strText);
			break;
		case CALL_RECORD_OUTCALL:
			m_callRecord.ClearVector();
			m_callRecord.GetRecordsByRecordType(CALLRECORD_DAIL);
			size = m_callRecord.Size();
			strText= LoadStringEx(IDS_RECORD_DAIL);	
			strNum.Format(TEXT("(%d)"), size);
			strText += strNum;
			m_ctrlRecordTree.SetItemText(m_dialItem, strText);
			break;
		case CALL_RECORD_NOTGET:
			m_callRecord.ClearVector();
			m_callRecord.GetRecordsByRecordType(CALLRECORD_MISS);
			size = m_callRecord.Size();
			strText= LoadStringEx(IDS_RECORD_MISS);	
			strNum.Format(TEXT("(%d)"), size);
			strText += strNum;
			m_ctrlRecordTree.SetItemText(m_missItem, strText);
			break;
		case NET_RECORD_ALL:
			m_connectRecord.ClearVector();
			m_connectRecord.GetAllRecords();
			size = m_connectRecord.Size();
			strText= LoadStringEx(IDS_RECORD_NETALL);	
			strNum.Format(TEXT("(%d)"), size);
			strText += strNum;
			m_ctrlRecordTree.SetItemText(m_allItem, strText);
			break;
		default:
			break;
		
	}
	//	m_ctrlRecordTree.SetItemText(,strText);

}




void CRecordDialog::LoadListHead(int Type)
{
	switch(Type) 
	{
	case CALLRECORD :
		m_ctrlRecordList.SetImageList(g_pSysImageList,LVSIL_SMALL);
		(void)m_ctrlRecordList.SetExtendedStyle( LVS_EX_FULLROWSELECT| LVS_EX_INFOTIP);			//隐藏列表线 
		m_ctrlRecordList.SetHeadings(LoadStringEx(IDS_RECORD_LISTHEAD_CALL));
		m_ctrlRecordList.LoadColumnInfo();	
		m_ctrlRecordList.SetTextBkColor(RGB(247,247,255));
		m_ctrlRecordList.SetTextColor(RGB(100,100,100));
		break;
	case NETRECORD:
		m_ctrlRecordList.SetImageList(g_pSysImageList,LVSIL_SMALL);
		(void)m_ctrlRecordList.SetExtendedStyle( LVS_EX_FULLROWSELECT| LVS_EX_INFOTIP);			//隐藏列表线 
		m_ctrlRecordList.SetHeadings(LoadStringEx(IDS_RECORD_LISTHEAD_NETALL));	
		m_ctrlRecordList.LoadColumnInfo();
		m_ctrlRecordList.SetTextBkColor(RGB(247,247,255));
		m_ctrlRecordList.SetTextColor(RGB(100,100,100));
		break;
	case NETRECORD_MONTH:
		m_ctrlRecordList.SetImageList(g_pSysImageList,LVSIL_SMALL);
		(void)m_ctrlRecordList.SetExtendedStyle( LVS_EX_FULLROWSELECT| LVS_EX_INFOTIP);			//隐藏列表线 
		m_ctrlRecordList.SetHeadings(LoadStringEx(IDS_RECORD_LISTHEAD_NETMON));
		m_ctrlRecordList.LoadColumnInfo();
		m_ctrlRecordList.SetTextBkColor(RGB(247,247,255));
		m_ctrlRecordList.SetTextColor(RGB(100,100,100));
		break;
	case NETRECORD_STATBYTIME:
		m_ctrlRecordList.SetImageList(g_pSysImageList,LVSIL_SMALL);
		(void)m_ctrlRecordList.SetExtendedStyle( LVS_EX_FULLROWSELECT| LVS_EX_INFOTIP);			//隐藏列表线 
		m_ctrlRecordList.SetHeadings(LoadStringEx(IDS_RECORD_LISTHEAD_NETTIME));	
		m_ctrlRecordList.LoadColumnInfo();
		m_ctrlRecordList.SetTextBkColor(RGB(247,247,255));
		m_ctrlRecordList.SetTextColor(RGB(100,100,100));
		break;

	}

}
void CRecordDialog::OnRclickListRecord(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW *pListV = (NM_LISTVIEW *)pNMHDR;
	*pResult = 0;
	
	if(pListV != NULL)
	{
		if(pListV->iItem != -1)
		{
			int nSubItem = pListV->iSubItem;
			if(nSubItem < 1)
			{
				nSubItem = 1;
			}		
		}
	}
	
	if(m_ctrlRecordList.GetItemCount() < 1)
	{
		return;
	}
	int times = 0;
	int nCount = m_ctrlRecordList.GetItemCount();
	
	for(int i = 0; i < nCount ; i++)
	{
		
		if (m_ctrlRecordList.GetItemState(i, LVIS_SELECTED))
		{				
			times++;		
			break;
		}
	}
	if(times < 1)
	{
		SetButtonState(FALSE);
		return;
	}
	SetButtonState(TRUE);
	
	CPoint pt ;	
	CMenu CardMenu, *pPopMenu = NULL ;
	GetCursorPos(&pt);
	if(currItemData <= CALL_RECORD_NOTGET)
		CardMenu.LoadMenu(IDR_MENU_RECORD);
	else if(currItemData <= NET_RECORD_ALL)
		CardMenu.LoadMenu(IDR_MENU_CONNECTRECORD);
	else
	{
		SetButtonState(FALSE);
		return;
	}
	pPopMenu = CardMenu.GetSubMenu(0) ;
	
	if(pPopMenu != NULL)
	{
		if(currItemData <= CALL_RECORD_NOTGET)
		{
			pPopMenu->ModifyMenu(IDR_RECORD_SMS, MF_BYCOMMAND|MF_STRING, IDR_RECORD_SMS , LoadStringEx(IDS_RECORD_SMS));
			pPopMenu->ModifyMenu(IDR_RECORD_DIAL, MF_BYCOMMAND|MF_STRING, IDR_RECORD_DIAL , LoadStringEx(IDS_SMS_DAIL));
			pPopMenu->ModifyMenu(IDR_RECORD_DEL, MF_BYCOMMAND|MF_STRING, IDR_RECORD_DEL , LoadStringEx(IDS_SMS_DEL));
			pPopMenu->ModifyMenu(IDR_RECORD_CLEAR, MF_BYCOMMAND|MF_STRING, IDR_RECORD_CLEAR , LoadStringEx(IDS_CLEAR));
			pPopMenu->ModifyMenu(IDR_RECORD_SAVE, MF_BYCOMMAND|MF_STRING, IDR_RECORD_SAVE , LoadStringEx(IDS_SMS_SAVE));
			pPopMenu->ModifyMenu(IDR_RECORD_ALL, MF_BYCOMMAND|MF_STRING, IDR_RECORD_ALL , LoadStringEx(IDS_SMS_ALL));
		    pPopMenu->ModifyMenu(IDR_RECORD_OTHER, MF_BYCOMMAND|MF_STRING, IDR_RECORD_OTHER , LoadStringEx(IDS_SMS_OTHER));
		
		}
		else if(currItemData <= NET_RECORD_ALL)
		{
			pPopMenu->ModifyMenu(IDM_DELETE, MF_BYCOMMAND|MF_STRING, IDM_DELETE , LoadStringEx(IDS_SMS_DEL));
			pPopMenu->ModifyMenu(IDM_CLEAR, MF_BYCOMMAND|MF_STRING, IDM_CLEAR , LoadStringEx(IDS_CLEAR));
		
		}
		pPopMenu->TrackPopupMenu(TPM_LEFTBUTTON,pt.x,pt.y,this) ;
	}
	

}


//==================================================================
//						右键功能菜单
//==================================================================

void CRecordDialog::OnRecordSave() 
{
	// TODO: Add your command handler code here
	CString saveText = _T("");
	int nCount = m_ctrlRecordList.GetItemCount();
	for(int i = nCount; i >=0 ; i--)
	{		
		if (m_ctrlRecordList.GetItemState(i, LVIS_SELECTED))
		{		
			saveText = m_ctrlRecordList.GetItemText(i,2);
			break;
			//已经存在名片的不存号
			
		}		
		
	}

	CNewCardDlg dlg(this,saveText);
	dlg.DoModal();
	return;
}

void CRecordDialog::OnRecordOther() 
{
	// TODO: Add your command handler code here
	static DWORD dwStaFlag = LVIS_SELECTED|LVIS_FOCUSED|LVIS_ACTIVATING;
	int nCount = m_ctrlRecordList.GetItemCount();
	for(int i = 0; i < nCount; i++)
	{
		if (m_ctrlRecordList.GetItemState(i, LVIS_SELECTED))
		{
			m_ctrlRecordList.SetItemState(i, 0, LVIS_SELECTED);
		}
		else
		m_ctrlRecordList.SetItemState(i,dwStaFlag,dwStaFlag);
	}
}

void CRecordDialog::OnRecordDial() 
{
	// TODO: Add your command handler code here
	CallStatus m_status;
	m_status = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_pCallMgr->GetCallStatus();
	if(m_status != CALLSTATE_IDLE)
		return;
	
	((CSunnyCatDlg *)AfxGetMainWnd())->ShowPage(1);

	int nCount = m_ctrlRecordList.GetItemCount();
	CString strPhone;

	strPhone.Empty();

	for(int i = 0; i < nCount ; i++)
	{
	
		if (m_ctrlRecordList.GetItemState(i, LVIS_SELECTED))
		{	
			strPhone = m_ctrlRecordList.GetItemText(i,CALLRECORD_PHONE);
			break;
		}		
	
		
	}
	pCallDialog->Call(strPhone);
}

void CRecordDialog::OnRecordDel() 
{
	// TODO: Add your command handler code here
	int nCount = m_ctrlRecordList.GetItemCount();
	m_ctrlRecordList.SetRedraw(false);
	for(int i = nCount-1; i >= 0; i--)
	{
	
		if (m_ctrlRecordList.GetItemState(i, LVIS_SELECTED))
		{	
			int id = m_ctrlRecordList.GetItemData_EX(i);
			m_callRecord.Delete(id);			
		
			m_ctrlRecordList.DeleteItem_EX(i);
		}		
	
		
	}
	m_ctrlRecordList.SetRedraw(true);
	ShowTreeText(currItemData);
}

void CRecordDialog::OnRecordClear() 
{
	// TODO: Add your command handler code here
	OnRecordAll(); 
	OnRecordDel();
}

void CRecordDialog::OnRecordAll() 
{
	// TODO: Add your command handler code here
	static DWORD dwStaFlag = LVIS_SELECTED|LVIS_FOCUSED|LVIS_ACTIVATING ;
	int nCount = m_ctrlRecordList.GetItemCount() ;
	for(int i=0; i<nCount; i++)
	{
		m_ctrlRecordList.SetItemState(i,dwStaFlag,dwStaFlag);
	}
}

//==================================================================
//==================================================================

void CRecordDialog::OnClickListRecord(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CRecordDialog::OnDblclkListRecord(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}


void CRecordDialog::OnButtonRecordDel() 
{
	// TODO: Add your control notification handler code here
	if(currItemData < NET_RECORD)
		OnRecordDel();
	else
		OnDelete();
	
}

void CRecordDialog::OnButtonRecordClear() 
{
	// TODO: Add your control notification handler code here	
	OnRecordAll(); 
	if(currItemData < NET_RECORD)
		OnRecordDel();
	else
		OnDelete();
}

void CRecordDialog::OnRclickTreeRecord(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CRecordDialog::OnSelchangedTreeRecord(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	currItem = pNMTreeView->itemNew.hItem;
	DWORD itemData = m_ctrlRecordTree.GetItemData(pNMTreeView->itemNew.hItem);
	// TODO: Add your control notification handler code here
				
	currItemData = itemData;
	switch(itemData)
	{
		//显示通话记录
//	case CALL_RECORD:
	case CALL_RECORD_RECV:
		m_ctrlRecordList.DeleteAllCol();	
		LoadListHead(CALLRECORD);
		ShowList(CALLRECORD_RECV);
		SetButtonState(TRUE);
		break;
	case CALL_RECORD_OUTCALL:
		m_ctrlRecordList.DeleteAllCol();
		LoadListHead(CALLRECORD);
		ShowList(CALLRECORD_DAIL);
		SetButtonState(TRUE);
		break;
	case CALL_RECORD_NOTGET:
		m_ctrlRecordList.DeleteAllCol();	
		LoadListHead(CALLRECORD);
		ShowList(CALLRECORD_MISS);
		SetButtonState(TRUE);
		break;
		
//	case NET_RECORD:
		//列出所有记录
	case NET_RECORD_ALL:	
		m_ctrlRecordList.DeleteAllCol();	
		LoadListHead(NETRECORD);
		ShowListNetRecord();
		SetButtonState(TRUE);
		break;
		//按月列出
	case NET_RECORD_MONTH:
		m_ctrlRecordList.DeleteAllCol();	
		LoadListHead(NETRECORD_MONTH);
		ShowMonthListNetRecord();
		SetButtonState(FALSE);
		break;
		//按时间列出
	case NET_RECORD_TIME:
		m_ctrlRecordList.DeleteAllCol();	
		LoadListHead(NETRECORD_STATBYTIME);
		ShowStatTimeListNetRecord();
		SetButtonState(FALSE);
		
		break;
	}

	ShowTreeText(currItemData);
	int countNum = m_ctrlRecordList.GetItemCount();
	if(countNum > 0)
	{
		m_ctrlRecordList.SelectItem(0, true);
		m_ctrlRecordList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	}
	*pResult = 0;
}

void CRecordDialog::SetButtonState(BOOL bState)
{
	CWnd* pWnd = NULL;

	pWnd = GetDlgItem(IDC_BUTTON_RECORD_DEL);
	pWnd->EnableWindow(bState);
	pWnd = GetDlgItem(IDC_BUTTON_RECORD_CLEAR);
	pWnd->EnableWindow(bState);
}

//显示CallRecord
void CRecordDialog::ShowList(CallRecordType Type)
{
	int iSize;
	CString timeLength;
	int i;
	TCallInfo callInfo;
	CString strName;
	int nID = 0;

	strName.Empty();

	m_ctrlRecordList.SetRedraw(false);
	m_ctrlRecordList.DeleteAllItems_EX();
	//LoadListHead(CALLRECORD);
	m_callRecord.ClearVector();
	m_callRecord.GetRecordsByRecordType(Type);
	iSize = m_callRecord.Size();
	for(i = 0; i < iSize; i++)
	{
		callInfo = m_callRecord.GetAt(i);		
		((CSunnyCatDlg*)AfxGetMainWnd())->FindNameInPhonebook(callInfo.strTelephone,strName,nID);
		//改显示的图片
		if(Type == 0)
		{	
			m_ctrlRecordList.AddItem(3, TEXT(""), strName, callInfo.strTelephone,  SecondToTime(callInfo.period), callInfo.TimeString());
		}
		else
			m_ctrlRecordList.AddItem(Type, TEXT(""), strName, callInfo.strTelephone,  SecondToTime(callInfo.period), callInfo.TimeString());
		strName.Empty();
		m_ctrlRecordList.SetItemData_EX(i, callInfo.nID);
	}
	m_ctrlRecordList.SetRedraw(true);
	if(Type == CALLRECORD_RECV)
	{
		m_ctrlRecordTree.SelectItem(m_talkItem);
		m_ctrlRecordTree.SelectSetFirstVisible(m_talkItem);
		ShowTreeText(CALL_RECORD_RECV);
	}
	if(Type == CALLRECORD_DAIL)
	{
		m_ctrlRecordTree.SelectItem(m_dialItem);
		m_ctrlRecordTree.SelectSetFirstVisible(m_dialItem);
		ShowTreeText(CALL_RECORD_OUTCALL);
	}
	if(Type == CALLRECORD_MISS)
	{
		m_ctrlRecordTree.SelectItem(m_missItem);
		m_ctrlRecordTree.SelectSetFirstVisible(m_missItem);
		ShowTreeText(CALL_RECORD_NOTGET);
	}
}

//显示所有的NetRecord
void CRecordDialog::ShowListNetRecord()
{	
	int iSize;
	TConnectInfo connectInfo;

	CString strDate;
	CString strTime;
	CString strLastTime;
	CString strFlux;
	
	m_ctrlRecordList.SetRedraw(false);
	m_ctrlRecordList.DeleteAllItems_EX();
	//LoadListHead(NETRECORD);
	m_connectRecord.ClearVector();
	m_connectRecord.GetAllRecords();
	iSize = m_connectRecord.Size();
	for(int i = 0; i < iSize; i++)
	{
		connectInfo = m_connectRecord.GetAt(i);
		strLastTime = SecondToTime(connectInfo.iTotalSecond);
		strFlux.Format(TEXT("%.3f"), ((float) connectInfo.Flux()) / 1024);
		m_ctrlRecordList.AddItem(18, TEXT(""), connectInfo.TimeString(),strLastTime, strFlux,"");
		m_ctrlRecordList.SetItemData_EX(i, connectInfo.nID);
	}
	m_ctrlRecordList.SetRedraw(true);
	
	m_ctrlRecordTree.SelectItem(m_allItem);
	m_ctrlRecordTree.SelectSetFirstVisible(m_allItem);
	ShowTreeText(NET_RECORD_ALL);
}

void CRecordDialog::ShowMonthListNetRecord()
{
	TConnectInfo connectInfo;
	m_ctrlRecordList.SetRedraw(false);
	m_ctrlRecordList.DeleteAllItems_EX();
	//LoadListHead(NETRECORD_MONTH);
	m_connectRecord.ClearVector();
	m_connectRecord.GetAllRecords();

	CString strDate;
	CString strLastTime;
	CString strFlux;

	size_t statCount = m_connectRecord.GetMonthStatCount();

	m_ctrlRecordList.SetRedraw(false);
	for(int i = 0; i < statCount; i++)
	{
		CString temp;
		__int64 lFlux;

		m_connectRecord.GetMonthStat(i, strDate, strLastTime, strFlux);
		lFlux = _ttoi64(strFlux);
		temp.Format(TEXT("%.3f"), (double) lFlux / 1024);
		m_ctrlRecordList.AddItem(19,TEXT(""),strDate,strLastTime,temp);
		m_ctrlRecordList.SetItemData_EX(i,i);
		//	m_ctrlRecordList.GetItemCount(), strDate);
		
//		m_ctrlRecordList.SetItemText(iItem, 1, strLastTime);
//		CString temp;

//		m_ctrlRecordList.SetItemText(iItem, 2, temp);
	}
	m_ctrlRecordList.SetRedraw(true);
}

void CRecordDialog::OnItemchangedListRecord(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	//SetButtonState(TRUE);
	*pResult = 0;
}

void CRecordDialog::OnRecordSms() 
{
	// TODO: Add your command handler code here

	int nCount = m_ctrlRecordList.GetItemCount();
	CString strPhone;
	int nNum = 0;

	strPhone.Empty();

	for(int i = 0; i < nCount ; i++)
	{
	
		if (m_ctrlRecordList.GetItemState(i, LVIS_SELECTED))
		{	
			strPhone += m_ctrlRecordList.GetItemText(i,CALLRECORD_PHONE);
			strPhone += ';';

			nNum++;
		}				
	}
	
	CNewSmsDialog smsDlg(&((CSunnyCatDlg*)AfxGetMainWnd())->m_smsDlg,strPhone,nNum);
	smsDlg.DoModal();
}

void CRecordDialog::OnDelete() 
{
	// TODO: Add your command handler code here
	int nCount = m_ctrlRecordList.GetItemCount();
	m_ctrlRecordList.SetRedraw(false);
	for(int i = nCount-1; i >= 0; i--)
	{
	
		if (m_ctrlRecordList.GetItemState(i, LVIS_SELECTED))
		{	
			int id = m_ctrlRecordList.GetItemData_EX(i);
			m_connectRecord.Delete(id);	
			
			m_ctrlRecordList.DeleteItem_EX(i);
		}		
	
		
	}
	m_ctrlRecordList.SetRedraw(true);
	ShowTreeText(currItemData);
}

void CRecordDialog::OnClear() 
{
	// TODO: Add your command handler code here
	OnRecordAll(); 
	OnDelete();
}

void CRecordDialog::ShowStatTimeListNetRecord()
{
	m_ctrlRecordList.SetRedraw(false);
	m_ctrlRecordList.DeleteAllItems_EX();
	//LoadListHead(NETRECORD_STATBYTIME);

	m_ctrlRecordList.SetRedraw(TRUE);

	CNetStatByTime dlg;
	if(dlg.DoModal() == IDOK)
	{
		
		CString strTime = _T("");
		CString strFlux = _T("");

		CConnectRecord ConnectRecord;
		CString temp;
		__int64 lFlux;
		CString	strBegTime;
		CString strEndTime;

		ConnectRecord.GetAllRecords();
		ConnectRecord.GetStatTime(dlg.m_StartTime, dlg.m_EndTime, strTime, strFlux);
		lFlux = _ttoi64(strFlux);
		temp.Format(TEXT("%.3f"), (double) lFlux / 1024);
		m_ctrlRecordList.SetRedraw(false);
		strBegTime = TimeToString(dlg.m_StartTime,TIMEF_YMD);
		strEndTime = TimeToString(dlg.m_EndTime,TIMEF_YMD);
		m_ctrlRecordList.AddItem(0,TEXT(""),strBegTime,strEndTime,strTime,temp);
		m_ctrlRecordList.SetRedraw(TRUE);
	}
}


//void CRecordDialog::OnClickTreeRecord(NMHDR* pNMHDR, LRESULT* pResult) 
//{
//	// TODO: Add your control notification handler code here
//	OnSelchangedTreeRecord(pNMHDR,pResult);
//	*pResult = 0;
//}

void CRecordDialog::DoResize1(int delta)
{
	CSplitterControl::ChangeWidth(&m_ctrlRecordTree, delta);
	CSplitterControl::ChangeWidth(&m_ctrlRecordList, -delta, CW_RIGHTALIGN);
	//CSplitterControl::ChangeWidth(GetDlgItem(IDC_LIST_SHOW_RECORD), -delta, CW_RIGHTALIGN);
	Invalidate();
	UpdateWindow();
}

LRESULT CRecordDialog::OnMyMove(WPARAM wParam,LPARAM lParam)
{
	if(m_wndSplitter1.GetSafeHwnd() != NULL)
	{
		RECT rcTab;
		m_wndSplitter1.GetWindowRect(&rcTab);
		
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcTab, 2 );
		
		RECT rc;
		GetWindowRect(&rc);
		
		if(m_bFirst)
		{
			rcTab.bottom += 10;
			m_bFirst = FALSE;
		}
		else
			rcTab.bottom = rc.bottom-rc.top-170;

	
		m_wndSplitter1.MoveWindow(&rcTab,TRUE);
	}
	

	return TRUE;
}

LRESULT CRecordDialog::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_NOTIFY)
	{
		if (wParam == IDC_SPLITTER1)
		{	
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			DoResize1(pHdr->delta);
		}
	}
	return CBmpBkDialog::DefWindowProc(message, wParam, lParam);
}

void CRecordDialog::OnSize(UINT nType, int cx, int cy) 
{
	CBmpBkDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	RECT rc;
	if(m_ctrlRecordList.GetSafeHwnd() != NULL)
	{	
		m_ctrlRecordList.GetWindowRect(&rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
		rc.right = cx;
		rc.bottom = cy;
		m_ctrlRecordList.MoveWindow(&rc);	
	}

	if(m_ctrlRecordTree.GetSafeHwnd() != NULL)
	{	
		m_ctrlRecordTree.GetWindowRect(&rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );

		rc.bottom = cy;
		m_ctrlRecordTree.MoveWindow(&rc);	
	}
}
