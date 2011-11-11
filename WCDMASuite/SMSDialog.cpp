// SMSDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SunnyCat.h"
#include "SMSDialog.h"
#include "NewSmsDialog.h"
#include "GlobalDataCenter.h"
#include "SunnyCatDlg.h"
#include "CallDialog.h"
#include "dlgprogressEx.h"
#include "Util.h"
#include "data/SMSPC.h"
#include <vector>
#include "Util.h"
#include "DlgProgressEx.h"
#include "TheadSendSMS.h"
#include "NewCardDlg.h"
#include "ProgressWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSMSDialog dialog

void LogString(const char* str,size_t size);
void LogString(const wchar_t* str,size_t size);
void LogString(CString strLog);

CSMSDialog::CSMSDialog(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CSMSDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSMSDialog)
	m_editSmsShow = _T("");
	m_nCurSelItem = 0;
	m_currTreeItem = 1;
	g_pSysImageList   = new CImageList();
	//}}AFX_DATA_INIT

	m_bFirst = TRUE;
	m_b2First = TRUE;

	m_bListFirst = TRUE;
	m_bEditFirst = TRUE;


	g_pSysList = new CImageList();
}

CSMSDialog::~CSMSDialog()
{
	delete g_pSysImageList;
	g_pSysImageList = NULL;
	delete g_pSysList;
	g_pSysList = NULL;
}

void CSMSDialog::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSMSDialog)
	DDX_Control(pDX, IDC_BUTTON_TRANSMIT, m_TransmitBtn);
	DDX_Control(pDX, IDC_BUTTON_REVERT, m_ReplyBtn);
	DDX_Control(pDX, IDC_BUTTON_DEL, m_DeleteBtn);
	DDX_Control(pDX, IDC_BUTTON_NEWSMS, m_NewBtn);
	DDX_Control(pDX, IDC_EDIT_SHOW, m_ctrlSmsEdit);
	DDX_Control(pDX, IDC_LIST_SMS, m_smsList);
	DDX_Control(pDX, IDC_TREE_SMS, m_smsTree);
	DDX_Text(pDX, IDC_EDIT_SHOW, m_editSmsShow);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSMSDialog, CBmpBkDialog)
	//{{AFX_MSG_MAP(CSMSDialog)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SMS, OnClickListSms)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SMS, OnDblclkListSms)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SMS, OnRclickListSms)
	ON_BN_CLICKED(IDC_BUTTON_NEWSMS, OnButtonNewsms)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SMS, OnItemchangedListSms)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_SMS, OnRclickTreeSms)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SMS, OnSelchangedTreeSms)
	ON_BN_CLICKED(IDC_BUTTON_REVERT, OnButtonRevert)
	ON_BN_CLICKED(IDC_BUTTON_TRANSMIT, OnButtonTransmit)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	ON_COMMAND(IDR_SMS_SAVE, OnSmsSave)
	ON_COMMAND(IDR_SMS_REVERT, OnSmsRevert)
	ON_COMMAND(IDR_SMS_OTHER, OnSmsOther)
	ON_COMMAND(IDR_SMS_DEL, OnSmsDel)
	ON_COMMAND(IDR_SMS_DAIL, OnSmsDail)
	ON_COMMAND(IDR_SMS_CHANG, OnSmsChang)
	ON_COMMAND(IDR_SMS_ALL, OnSmsAll)
	ON_COMMAND(IDR_SMS_RECV, OnSmsRecv)
	ON_COMMAND(IDR_SMS_RECVALL, OnSmsRecvall)
	ON_COMMAND(IDR_SMS_COPY, OnSmsCopy)
	ON_NOTIFY(NM_CLICK, IDC_TREE_SMS, OnClickTreeSms)
	ON_MESSAGE(WM_SMSLIST_NOTIFY, OnSMSListNotify)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_SMS, OnKeydownListSms)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MY_MOVE,OnMyMove)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSMSDialog message handlers

BOOL CSMSDialog::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	/********************************************************/// modify ui
	CRect temprect(110, 0, 431, 251);
	CWnd::SetWindowPos(NULL, 0, 0, temprect.Width(), temprect.Height(), SWP_NOZORDER|SWP_NOMOVE);
	/********************************************************/
	
	InitTree();
	GetDlgItem(IDC_BUTTON_NEWSMS)->SetWindowText(LoadStringEx(IDS_WRITE_SMS));
	GetDlgItem(IDC_BUTTON_REVERT)->SetWindowText(LoadStringEx(IDS_REPLY_SMS));
	GetDlgItem(IDC_BUTTON_TRANSMIT)->SetWindowText(LoadStringEx(IDS_FORWARD_SMS));
	GetDlgItem(IDC_BUTTON_DEL)->SetWindowText(LoadStringEx(IDS_LINKMAN_DEL));
	GetDlgItem(IDC_BUTTON_CLEAR)->SetWindowText(LoadStringEx(IDS_LINKMAN_CLEAR));

	InitUI();

	CRect rc;
	CWnd* pWnd;

	pWnd = GetDlgItem(IDC_SPLITTER1);
	pWnd->GetWindowRect(rc);
	ScreenToClient(rc);
	m_wndSplitter1.Create(WS_CHILD | WS_VISIBLE, rc, this, IDC_SPLITTER1);
	m_wndSplitter1.SetRange(140, 260, -1);
	
	pWnd = GetDlgItem(IDC_SPLITTER2);
	pWnd->GetWindowRect(rc);
	ScreenToClient(rc);
	m_wndSplitter2.Create(WS_CHILD | WS_VISIBLE, rc, this, IDC_SPLITTER2);
	m_wndSplitter2.SetRange(200, 60, -1);

	TRACE(TEXT("CSMSDialog::OnInitDialog:> 1:"));
	if(m_smsList.GetItemCount() > 0)
	{
		ShowSMSContent(0);
		SetDelBtnState(false);		
	}
	else
	{	
		TRACE(TEXT("CSMSDialog::OnInitDialog:> 3: enter the else code"));
		SetDelBtnState(true);

	}

	if(m_currTreeItem == SMS_PC_RECYCLE)
	{
		SetReBtnState(true);
		SetTraBtnState(true);
	}

	SetTraBtnState(true);
	SetReBtnState(true);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CSMSDialog::ClickTreeShowText(int iBox)
{
	int nSize= 0;
	int unSend = 0;
	CString strText;
	int i;
	switch(iBox)
	{
	case SMS_PC_RECV:
		m_PCRecv.ClearVector();
		m_PCRecv.GetRecordsByMailBoxID(SMS_MAILBOX_RECV);
		nSize = m_PCRecv.Size();
		
		for(i = 0; i < nSize; i++)
		{
			if(m_PCRecv.GetAt(i).nPSMSState == SMS_PC_UNREAD)
			{
				unSend++;
			}
		}
		strText = LoadStringEx(IDS_SMS_RECVBOX);
		strText += "(" + Util::intToString(unSend) + "/" + Util::intToString(nSize) + ")";
		m_smsTree.SetItemText(m_recItem, strText);
		break;
	case SMS_PC_SEND:
		m_PCSend.ClearVector();
		m_PCSend.GetRecordsByMailBoxID(SMS_MAILBOX_SEND);
		nSize = m_PCSend.Size();
		strText = LoadStringEx(IDS_SMS_SENDBOX);
		strText += "(" + Util::intToString(nSize) + ")";
		m_smsTree.SetItemText(m_sendItem, strText);
		break;
	case SMS_PC_UNSEND:
		m_PCUnSend.ClearVector();
		m_PCUnSend.GetRecordsByMailBoxID(SMS_MAILBOX_UNSEND);
		nSize = m_PCUnSend.Size();
		strText = LoadStringEx(IDS_SMS_UNSENDBOX);
		strText += "(" + Util::intToString(nSize) + ")";
		m_smsTree.SetItemText(m_unsendItem, strText);
		break;
	case SMS_PC_DRAFT:
		m_PCDraft.ClearVector();
		m_PCDraft.GetRecordsByMailBoxID(SMS_MAILBOX_DRAFT);
		nSize = m_PCDraft.Size();
		strText = LoadStringEx(IDS_SMS_DRAFBOX);
		strText += "(" + Util::intToString(nSize) + ")";
		m_smsTree.SetItemText(m_darftItem, strText);
		break;
	case SMS_PC_RECYCLE:
		m_PCRecyle.ClearVector();
		m_PCRecyle.GetRecordsByMailBoxID(SMS_MAILBOX_RECYLE);
		nSize = m_PCRecyle.Size();
		strText = LoadStringEx(IDS_SMS_RECYBOX);
		strText += "(" + Util::intToString(nSize) + ")";
		m_smsTree.SetItemText(m_recycleItem, strText);
		break;
	default:
		break;
	}
	LogString("ClickTreeShowText end");
	TRACE(TEXT("ClickTreeShowText end IBOX = %d"),iBox);
}

void CSMSDialog::InitTree()
{
	LoadImgList(g_pSysImageList, IDB_LINKMANTREE);	
	//初使化短信树
	m_smsTree.SetRedraw(false);
	m_smsTree.DeleteAllItems();
	m_smsTree.SetImageList(g_pSysImageList, LVSIL_NORMAL);
	m_smsTree.SetTextColor(RGB(100,100,100));
	m_smsTree.SetBkColor(RGB(200,200,200));
	m_pcItem =  m_smsTree.InsertItem(LoadStringEx(IDS_SMS_CENTER),SMS_IMAG_PC,SMS_IMAG_PC);

	m_PCRecv.ClearVector();
	m_PCRecv.GetRecordsByMailBoxID(SMS_MAILBOX_RECV);
	int nSize = m_PCRecv.Size();
	int unSend = 0;
	for(int i = 0; i < nSize; i++)
	{
		if(m_PCRecv.GetAt(i).nPSMSState == SMS_PC_UNREAD)
		{
			unSend++;
		}
	}
	CString strText = LoadStringEx(IDS_SMS_RECVBOX);
	strText += "(" + Util::intToString(unSend) + "/" + Util::intToString(nSize) + ")";
	m_recItem = m_smsTree.InsertItem(strText,SMS_IMAG_RECV,SMS_IMAG_RECV, m_pcItem);

	m_PCSend.ClearVector();
	m_PCSend.GetRecordsByMailBoxID(SMS_MAILBOX_SEND);
	nSize = m_PCSend.Size();
	strText = LoadStringEx(IDS_SMS_SENDBOX);
	strText += "(" + Util::intToString(nSize) + ")";
	m_sendItem = m_smsTree.InsertItem(strText,SMS_IMAG_SEND,SMS_IMAG_SEND, m_pcItem);
	
	m_PCUnSend.ClearVector();
	m_PCUnSend.GetRecordsByMailBoxID(SMS_MAILBOX_UNSEND);
	nSize = m_PCUnSend.Size();
	strText = LoadStringEx(IDS_SMS_UNSENDBOX);
	strText += "(" + Util::intToString(nSize) + ")";
	m_unsendItem = m_smsTree.InsertItem(strText,SMS_IMAG_UNSEND,SMS_IMAG_UNSEND, m_pcItem);
		
	m_PCDraft.ClearVector();
	m_PCDraft.GetRecordsByMailBoxID(SMS_MAILBOX_DRAFT);
	nSize = m_PCDraft.Size();
	strText = LoadStringEx(IDS_SMS_DRAFBOX);
	strText += "(" + Util::intToString(nSize) + ")";
	m_darftItem = m_smsTree.InsertItem(strText,SMS_IMAG_DRAFT,SMS_IMAG_DRAFT, m_pcItem);

	m_PCRecyle.ClearVector();
	m_PCRecyle.GetRecordsByMailBoxID(SMS_MAILBOX_RECYLE);
	nSize = m_PCRecyle.Size();
	strText = LoadStringEx(IDS_SMS_RECYBOX);
	strText += "(" + Util::intToString(nSize) + ")";
	m_recycleItem = m_smsTree.InsertItem(strText,SMS_IMAG_RECYCLE,SMS_IMAG_RECYCLE, m_pcItem);
	
	m_smsTree.SetItemData(m_pcItem, SMS_PC);
	m_smsTree.SetItemData(m_recItem, SMS_PC_RECV);
	m_smsTree.SetItemData(m_sendItem, SMS_PC_SEND);
	m_smsTree.SetItemData(m_unsendItem, SMS_PC_UNSEND);
	m_smsTree.SetItemData(m_darftItem, SMS_PC_DRAFT);
	m_smsTree.SetItemData(m_recycleItem, SMS_PC_RECYCLE);
	
/*	m_smItem = m_smsTree.InsertItem("SIM卡",SMS_IMAG_SIM,SMS_IMAG_SIM);
	HTREEITEM m_recSmItem = m_smsTree.InsertItem("收件箱", SMS_IMAG_RECV,SMS_IMAG_RECV,m_smItem);
	HTREEITEM m_sendSmItem = m_smsTree.InsertItem("已发送",SMS_IMAG_SEND,SMS_IMAG_SEND, m_smItem);
	HTREEITEM m_unsendSmItem = m_smsTree.InsertItem("未发送",SMS_IMAG_UNSEND,SMS_IMAG_UNSEND, m_smItem);


	m_smsTree.SetItemData(m_smItem, SMS_SIM);
	m_smsTree.SetItemData(m_recSmItem, SMS_SIM_RECV);
	m_smsTree.SetItemData(m_sendSmItem, SMS_SIM_SEND);
	m_smsTree.SetItemData(m_unsendSmItem, SMS_SIM_UNSEND);
*/
	//展开树
	m_smsTree.Expand(m_pcItem, TVE_EXPAND);
//	m_smsTree.Expand(m_smItem, TVE_EXPAND);
	m_smsTree.SetBkColor(RGB(255,255,255));
	m_smsTree.SetRedraw(true);
}

void CSMSDialog::InitUI()
{	
	//初使化短信列表	
	CBitmap IconBitmap;
	IconBitmap.LoadBitmap(IDB_LONGBAR);
//	g_pSysImageList->Create(20,20,ILC_MASK|ILC_COLOR24,1,1) ;
	g_pSysList->Create(30,30,ILC_MASK|ILC_COLOR24,1,1) ;
	g_pSysList->Add( &IconBitmap,(COLORREF)0xFFFFFF );
	IconBitmap.DeleteObject();
	
	m_smsList.SetImageList(g_pSysList,LVSIL_SMALL);
	(void)m_smsList.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);			//隐藏列表线 

//	m_smsList.SetHeadings( _T("标识,60; 电话,80;短信内容,100;时间,60") );
	m_smsList.SetHeadings(LoadStringEx(IDS_SMS_LISTHEAD));
	m_smsList.LoadColumnInfo();	
	
//	m_smsList.SetTextBkColor(RGB(247,247,255));
	m_smsList.SetTextColor(RGB(100,100,100));
	m_smsList.SelectItem(0, true);
	
	m_smsTree.SelectItem(m_recItem);
	CFont m_font;
	m_font.CreateFont(20,0,0,0, FW_NORMAL,FALSE,FALSE,0,ANSI_CHARSET, OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
	DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS,TEXT("仿宋"));
//	m_smsList.SetFont(&m_font,FALSE);	
	CFont *oldFont = m_smsList.GetFont();
	m_ctrlSmsEdit.SetFont(oldFont, TRUE);	
	
	CString	strAppPath = GetAppPath()+"\\skin\\sms\\";

	m_NewBtn.SetBitmapPath(strAppPath+"sms_new.bmp",strAppPath+"sms_new_down.bmp",strAppPath+"sms_new_over.bmp");
	m_DeleteBtn.SetBitmapPath(strAppPath+"sms_delete.bmp",strAppPath+"sms_delete_down.bmp",strAppPath+"sms_delete_over.bmp");
//	m_SendBtn.SetBitmapPath(strAppPath+"sms_send.bmp",strAppPath+"sms_send_down.bmp",strAppPath+"sms_send_over.bmp");
	m_ReplyBtn.SetBitmapPath(strAppPath+"sms_reply.bmp",strAppPath+"sms_reply_down.bmp",strAppPath+"sms_reply_over.bmp");
	m_TransmitBtn.SetBitmapPath(strAppPath+"sms_forward.bmp",strAppPath+"sms_forward_down.bmp",strAppPath+"sms_forward_over.bmp");
//	m_SearchBtn.SetBitmapPath(strAppPath+"sms_search.bmp",strAppPath+"sms_search_down.bmp",strAppPath+"sms_search_over.bmp");
	
	m_NewBtn.SetHitOver();
	m_DeleteBtn.SetHitOver();
	m_ReplyBtn.SetHitOver();
	m_TransmitBtn.SetHitOver();

	m_NewBtn.SetToolTipTxt(LoadStringEx(711));
	m_DeleteBtn.SetToolTipTxt(LoadStringEx(216));
	m_ReplyBtn.SetToolTipTxt(LoadStringEx(707));
	m_TransmitBtn.SetToolTipTxt(LoadStringEx(708));

}

LONG CSMSDialog::OnSMSListNotify(WPARAM wParam, LPARAM lParam)
{
	Invalidate(FALSE);
	m_smsList.SetRedraw(false);
	if(m_currTreeItem < 7)
		ShowSMSForPC(m_currTreeItem);
	else
		ShowSMSForSIM(m_currTreeItem);

	m_smsList.SetRedraw(true);
	if(m_smsList.GetItemCount() > 0)
	{
		 SetDelBtnState(false);
		 SetReBtnState(false);
		 SetTraBtnState(false);
	}
	else
	{
		 SetDelBtnState(true);
		 SetReBtnState(true);
		 SetTraBtnState(true);
	}
	Invalidate(TRUE);
	return 0;
}

void CSMSDialog::OnItemchangedListSms(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	m_nCurSelItem = pNMListView->iItem;
	if(m_smsList.GetItemCount() > 0)
	{
		 SetDelBtnState(false);
//		 SetReBtnState(false);
//		 SetTraBtnState(false);
	}
	else
	{

		SetDelBtnState(true);
//		 SetReBtnState(true);
//		 SetTraBtnState(true);
	}
	int selCount = 0;
	for(int i = 0; i < m_smsList.GetItemCount(); i++)
	{
		if(m_smsList.GetItemState(i, LVIS_SELECTED))	
		{
			selCount++;
			if(selCount > 1)
				break;
		}
	}
	if(selCount > 1)
	{
		 SetReBtnState(true);
		 SetTraBtnState(true);
	}
	else
	{
		if(m_currTreeItem == SMS_PC_RECV)
		{
			SetReBtnState(false);
		}
		else
			SetReBtnState(true);
		if(m_currTreeItem == SMS_PC_RECYCLE)
			SetTraBtnState(true);
		else
			SetTraBtnState(false);
	}

	*pResult = 0;
}

void CSMSDialog::OnClickListSms(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

/*	int oldItem = m_nCurSelItem;
	int item = m_smsList.AddItem(SMS_IMAG_RECV, "", m_smsList.GetItemText(m_nCurSelItem,1), m_smsList.GetItemText(m_nCurSelItem,2),
						m_smsList.GetItemText(m_nCurSelItem,3));
	m_smsList.SetItemData_EX(item, m_smsList.GetItemData_EX(m_nCurSelItem));
*/

/*	if(m_currTreeItem == SMS_PC_RECV || m_currTreeItem == SMS_PC)	
	{
		
		m_smsList.SetRedraw(false);
	//	m_smsList.DeleteAllItems_EX();
		m_PCRecv.ClearVector();
		int  imag = 0;
		m_PCRecv.GetRecordsByMailBoxID(SMS_MAILBOX_RECV);		
		int nSize = m_PCRecv.Size();
		
		TSMSItem item;
		for(int i = 0; i < nSize; i++)
		{
			item = m_PCRecv.GetAt(i);
			if(item.iSMSState == SMS_UNREAD)
				imag = SMS_IMAG_UNREAD;
			if(item.iSMSState == SMS_READ)
				imag = SMS_IMAG_RECV;
	
			if(item.iSMSState == SMS_UNREAD)
			{
				m_smsList.DeleteItem_EX(i);
				int selItem = 0;
				selItem = m_smsList.AddItem(imag, "", item.strPhoneNum, item.strText, item.TimeString());
				m_smsList.SetItemData_EX(i, item.iID);
				m_smsList.SetHotItem(selItem);
			
			}

		}
		m_smsList.SetRedraw(true);	
	}
*/
	if(m_smsList.GetItemCount() < 1)
	{
		return;
	}
	//选中个数
	int times = 0;
	for(int i = 0; i < m_smsList.GetItemCount(); i++)
	{

		if(m_smsList.GetItemState(i, LVIS_SELECTED))
			times++;
	}
	if(times < 1)
	{
		return;
	}

	if(m_smsList.GetItemCount() < 1)
		return;

	if(m_currTreeItem == SMS_PC || m_currTreeItem == SMS_PC_RECV)//m_currTreeItem == SMS_SIM_RECV || m_currTreeItem == SMS_SIM)
	{
		
		if(m_smsList.GetItemText(m_nCurSelItem, 0) == TEXT("new"))
		{
		/*	CString number = m_smsList.GetItemText(m_nCurSelItem, 1);
			CString content = m_smsList.GetItemText(m_nCurSelItem, 2);
			CString times = m_smsList.GetItemText(m_nCurSelItem, 3);
		*/
			m_smsList.SetItemText_EX(m_nCurSelItem, 0, TEXT(""));
			m_smsList.SetItemIconIndex(m_nCurSelItem, SMS_IMAG_RECV);
			m_smsList.Update(m_nCurSelItem);
			TRACE(TEXT("m_nCurSelItem = %d\n"),m_nCurSelItem);
			TRACE(TEXT("m_smsList.GetItemData_EX(m_nCurSelItem) = %d\n"),m_smsList.GetItemData_EX(m_nCurSelItem));
			int id = m_PCRecv.FindIndexByID(m_smsList.GetItemData_EX(m_nCurSelItem));
			TSMSItem item =  m_PCRecv.GetAt(id);		// 这句会出错
			TSMSItem node = item;
			node.nPSMSState = (PSMSType)SMS_PC_RECV;
			
			m_PCRecv.UpdateStatus(m_smsList.GetItemData_EX(m_nCurSelItem));
		//	m_PCRecv.Delete(m_nCurSelItem);
		//	m_PCRecv.AddBack(node);
		/*	m_smsList.DeleteItem_EX(m_nCurSelItem);
			int items = m_smsList.AddItem(SMS_IMAG_RECV, "",number, content, times);
			m_nCurSelItem = items;
		*/	
		}
	}

	if(m_currTreeItem == SMS_PC_RECV || m_currTreeItem == SMS_PC)//|| m_currTreeItem == SMS_SIM_RECV || m_currTreeItem == SMS_SIM)
	{
		int nCount = m_smsList.GetItemCount();
		int newSmsSize = 0;		
		for(int i = 0; i < nCount ; i++)
		{	
			if(m_smsList.GetItemText(i,0) == "new")
				newSmsSize++;				
		}
		if(newSmsSize < 1)
		{
			((CSunnyCatDlg *)AfxGetMainWnd())->PostMessage(WM_SMS_READ,0,1);
		}

	}
	
	ShowSMSContent(m_nCurSelItem);
	ClickTreeShowText(SMS_PC_RECV);
	UpdateData(false);
	

	*pResult = 0;
}


void CSMSDialog::ShowSMSContent(int selItem)
{
	if(m_smsList.GetItemCount() < 1)
	{
		TRACE(TEXT("CSMSDialog::OnInitDialog:> ShowSMSContent:>  line 559"));
		return;
	}
	TRACE(TEXT("CSMSDialog::OnInitDialog:> ShowSMSContent:>  line 562"));
	m_editSmsShow.IsEmpty();
	m_editSmsShow = "";
	UpdateData(false);
	if(m_nCurSelItem < 0)
		return;
	m_smsList.SetItemState(selItem,LVIS_SELECTED,LVIS_SELECTED) ;
	m_editSmsShow = LoadStringEx(IDS_SMS_SMSCONTENT);
	m_editSmsShow += " \r\n";

	int id;
	switch(m_currTreeItem)
	{
	case SMS_PC:
	case SMS_PC_RECV:
		id = m_PCRecv.FindIndexByID(m_smsList.GetItemData_EX(selItem));
		m_editSmsShow += m_PCRecv.GetAt(id).strText;
		break;
	case SMS_PC_SEND:
		id = m_PCSend.FindIndexByID(m_smsList.GetItemData_EX(selItem));
		m_editSmsShow += m_PCSend.GetAt(id).strText;
		break;
	case SMS_PC_DRAFT:
		id = m_PCDraft.FindIndexByID(m_smsList.GetItemData_EX(selItem));
		m_editSmsShow += m_PCDraft.GetAt(id).strText;
		break;
	case SMS_PC_RECYCLE:
		id = m_PCRecyle.FindIndexByID(m_smsList.GetItemData_EX(selItem));
		m_editSmsShow += m_PCRecyle.GetAt(id).strText;

		break;
	case SMS_PC_UNSEND:
		id = m_PCUnSend.FindIndexByID(m_smsList.GetItemData_EX(selItem));
		m_editSmsShow += m_PCUnSend.GetAt(id).strText;
		break;

	}
	UpdateData(false);
}

void CSMSDialog::OnDblclkListSms(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	if(m_smsList.GetItemCount() < 1)
	{
		return;
	}
	//选中个数
	int times = 0;
	for(int i = 0; i < m_smsList.GetItemCount(); i++)
	{

		if(m_smsList.GetItemState(i, LVIS_SELECTED))
			times++;
	}
	if(times < 1)
	{
		return;
	}
	if(m_currTreeItem == SMS_PC_DRAFT)
	{
		CNewSmsDialog newSmsDialog(this);
		int id = m_PCDraft.FindIndexByID(m_smsList.GetItemData_EX(m_nCurSelItem));
		newSmsDialog.m_strSendSmsContent = m_PCDraft.GetAt(id).strText;
		newSmsDialog.m_strSendSmsNum = m_smsList.GetItemText(m_nCurSelItem, 2) + ";";
		UpdateData(false);
		//m_PCDraft.Delete(m_smsList.GetItemData_EX(m_nCurSelItem));
		int index = m_smsList.GetItemData_EX(m_nCurSelItem);
		if(newSmsDialog.DoModal() == IDOK)
		{
			//m_PCDraft.Delete(index);
			m_PCDraft.ForeverDelete(index);
			TSMSItem item;
			int		 nSize;
			m_smsList.SetRedraw(false);	
			
			m_smsList.DeleteAllItems_EX();
			m_PCDraft.ClearVector();
			m_PCDraft.GetRecordsByMailBoxID(SMS_MAILBOX_DRAFT);
			nSize = m_PCDraft.Size();
			CString nameAndPhone = _T("");		
			CString strName;
			int nID;
			for(int i = 0; i < nSize; i++)
			{
				item = m_PCDraft.GetAt(i);		
			//	nameAndPhone = 
				((CSunnyCatDlg *)AfxGetMainWnd())->FindNameInPhonebook(item.strPhoneNum, strName, nID);
				m_smsList.AddItem(SMS_IMAG_DRAFT, TEXT(""), strName, item.strPhoneNum, item.strText, item.TimeString());
				m_smsList.SetItemData_EX(i, item.iID);
				strName = _T("");
			}
			m_smsList.SetRedraw(true);

		}
	}
	ClickTreeShowText(5);
	*pResult = 0;
	return;
}

void CSMSDialog::OnRclickListSms(NMHDR* pNMHDR, LRESULT* pResult) 
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

		CPoint pt ;	
		CMenu CardMenu, *pPopMenu = NULL ;
		GetCursorPos(&pt) ;
		CardMenu.LoadMenu(IDR_MENU_SMS);
		pPopMenu = CardMenu.GetSubMenu(0) ;
		
		if(pPopMenu != NULL)
		{	
			
	//		pPopMenu->EnableMenuItem(1,MF_BYPOSITION|MF_GRAYED) ;
	//		pPopMenu->EnableMenuItem(2,MF_BYPOSITION|MF_GRAYED) ;
			if(m_smsList.GetItemCount() < 1)
			{
				return;
			}
			//选中个数
			int times = 0;
			for(int i = 0; i < m_smsList.GetItemCount(); i++)
			{

				if(m_smsList.GetItemState(i, LVIS_SELECTED))
					times++;
			}
			if(times < 1)
			{
				return;
			}

			//==========================================================
			pPopMenu->ModifyMenu(IDR_SMS_CHANG, MF_BYCOMMAND|MF_STRING, IDR_SMS_CHANG , LoadStringEx(IDS_SMS_CHANG));
			pPopMenu->ModifyMenu(IDR_SMS_SAVE, MF_BYCOMMAND|MF_STRING, IDR_SMS_SAVE , LoadStringEx(IDS_SMS_SAVE));
			pPopMenu->ModifyMenu(IDR_SMS_OTHER, MF_BYCOMMAND|MF_STRING, IDR_SMS_OTHER , LoadStringEx(IDS_SMS_OTHER));
			pPopMenu->ModifyMenu(IDR_SMS_DAIL, MF_BYCOMMAND|MF_STRING, IDR_SMS_DAIL , LoadStringEx(IDS_SMS_DAIL));
			pPopMenu->ModifyMenu(IDR_SMS_DEL, MF_BYCOMMAND|MF_STRING, IDR_SMS_DEL , LoadStringEx(IDS_SMS_DEL));
			pPopMenu->ModifyMenu(IDR_SMS_ALL, MF_BYCOMMAND|MF_STRING, IDR_SMS_ALL , LoadStringEx(IDS_SMS_ALL));
			pPopMenu->ModifyMenu(IDR_SMS_COPY, MF_BYCOMMAND|MF_STRING, IDR_SMS_COPY , LoadStringEx(IDS_SMS_COPY));
			pPopMenu->ModifyMenu(IDR_SMS_RECVALL, MF_BYCOMMAND|MF_STRING, IDR_SMS_RECVALL , LoadStringEx(IDS_SMS_RECVALL));
			pPopMenu->ModifyMenu(IDR_SMS_RECV, MF_BYCOMMAND|MF_STRING, IDR_SMS_RECV , LoadStringEx(IDS_SMS_RECV));
			
	/*		if(m_currTreeItem <= 6)
			{
				pPopMenu->EnableMenuItem(3,MF_BYPOSITION|MF_GRAYED);	//070809he 去掉复制
			}
	*/	
			//============================================================
			if(m_currTreeItem == SMS_PC_RECV)
				pPopMenu->ModifyMenu(IDR_SMS_REVERT, MF_BYCOMMAND|MF_STRING, IDR_SMS_REVERT , LoadStringEx(IDS_SMS_REVERT));
			if(m_currTreeItem == SMS_PC_SEND || m_currTreeItem == SMS_PC_UNSEND)
				pPopMenu->ModifyMenu(IDR_SMS_REVERT, MF_BYCOMMAND|MF_STRING, IDR_SMS_REVERT , LoadStringEx(IDS_SMS_RESEND));
			if(m_currTreeItem == SMS_PC_DRAFT || m_currTreeItem == SMS_PC_RECYCLE)
				pPopMenu->ModifyMenu(IDR_SMS_REVERT, MF_BYCOMMAND|MF_STRING, IDR_SMS_REVERT , LoadStringEx(IDS_SMS_SEND));
	
			if(m_currTreeItem != SMS_PC_RECYCLE)
			{
				pPopMenu->EnableMenuItem(6,MF_BYPOSITION|MF_GRAYED);
				pPopMenu->EnableMenuItem(7,MF_BYPOSITION|MF_GRAYED);
			}
			if(m_currTreeItem == SMS_PC_RECYCLE)
			{
				pPopMenu->EnableMenuItem(0,MF_BYPOSITION|MF_GRAYED) ;
					pPopMenu->EnableMenuItem(1,MF_BYPOSITION|MF_GRAYED) ;
						pPopMenu->EnableMenuItem(2,MF_BYPOSITION|MF_GRAYED) ;
				//pPopMenu->EnableMenuItem(3,MF_BYPOSITION|MF_GRAYED);

			}
	//		pPopMenu->EnableMenuItem(4,MF_BYPOSITION|MF_GRAYED) ;
	//		pPopMenu->EnableMenuItem(5,MF_BYPOSITION|MF_GRAYED) ;
	//		pPopMenu->EnableMenuItem(6,MF_BYPOSITION|MF_GRAYED) ;

		
		int nCount = m_smsList.GetItemCount();
		int selCount = 0;
		for(i = 0; i < nCount; i++)
		{
			if (m_smsList.GetItemState(i, LVIS_SELECTED))
			{				
				selCount++;
				if(selCount > 1)
				{
					SetReBtnState(true);
					SetTraBtnState(true);
					pPopMenu->EnableMenuItem(0,MF_BYPOSITION|MF_GRAYED);
					pPopMenu->EnableMenuItem(1,MF_BYPOSITION|MF_GRAYED);
					pPopMenu->EnableMenuItem(2,MF_BYPOSITION|MF_GRAYED);
					pPopMenu->EnableMenuItem(5,MF_BYPOSITION|MF_GRAYED);
					break;
				}
			}		
		}
		/*******************************************************************
		if(((CSunnyCatDlg *)AfxGetMainWnd())->m_isConnect)
		{
			if(m_currTreeItem != SMS_PC_RECV)
				pPopMenu->EnableMenuItem(0,MF_BYPOSITION|MF_GRAYED);
			
			pPopMenu->EnableMenuItem(2,MF_BYPOSITION|MF_GRAYED);
		}
		else
		/*******************************************************************/
		{
			if(!((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.m_bSimExist)
			{	
				if(m_currTreeItem != SMS_PC_RECV)
				pPopMenu->EnableMenuItem(0,MF_BYPOSITION|MF_GRAYED);
				
				pPopMenu->EnableMenuItem(2,MF_BYPOSITION|MF_GRAYED);			
			}
		}

			pPopMenu->TrackPopupMenu(TPM_LEFTBUTTON,pt.x,pt.y,this) ;

		}

	//		OnClickListSms(pNMHDR, pResult); 
}


//===============================================
//				
//===============================================



	





void CSMSDialog::OnDblclkTreeSms(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CSMSDialog::OnRclickTreeSms(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CSMSDialog::OnSelchangedTreeSms(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_currItem = pNMTreeView->itemNew.hItem;
	m_currTreeItem = m_smsTree.GetItemData(pNMTreeView->itemNew.hItem);
//	OnClickTreeSms(pNMHDR, pResult);

	if(m_currTreeItem < 7)
		ShowSMSForPC(m_currTreeItem);
	else
		ShowSMSForSIM(m_currTreeItem);

		ClickTreeShowText(m_currTreeItem);
	m_editSmsShow = "";
	UpdateData(false);
	if(m_smsList.GetItemCount() > 0)
	{
		ShowSMSContent(0);
		SetDelBtnState(false);		
	}
	else
	{
		SetDelBtnState(true);

	}


	if(m_currTreeItem >= SMS_PC_RECV || m_currTreeItem == SMS_PC)
	{
		SetReBtnState(true);
		SetTraBtnState(true);
	}

	*pResult = 0;
}

//=================================================


//=================================================
//			按钮响应
//=================================================

//按钮写短信
void CSMSDialog::OnButtonNewsms() 
{
	// TODO: Add your control notification handler code here
	CNewSmsDialog m_newSms(this);
	if(m_newSms.DoModal() == IDOK)
	{
		return;
	}
}

//按钮回复
void CSMSDialog::OnButtonRevert() 
{
	// TODO: Add your control notification handler code here
	//==================================================2007.6.18he
	/***************************************************
	if(((CSunnyCatDlg *)AfxGetMainWnd())->m_isConnect)
	{		
		GMessageBox(LoadStringEx(IDS_PROMPT_ONLINE));
		return;
	}
	/***************************************************/
	//===================================================
	if(m_currTreeItem != SMS_PC_RECV && m_currTreeItem != SMS_PC 
		&& m_currTreeItem != SMS_SIM && m_currTreeItem != SMS_SIM_RECV)
		return;
	CString number = m_smsList.GetItemText(m_nCurSelItem, 2) + ";";
	int size = number.GetLength();
	CNewSmsDialog newSmsDialog(this, number, size);
//	newSmsDialog.m_strSendSmsNum = m_smsList.GetItemText(m_nCurSelItem, 1);
//	newSmsDialog.m_strSendSmsNum += ";";
	UpdateData(false);
	newSmsDialog.DoModal();
	return;

}

//按钮转发
void CSMSDialog::OnButtonTransmit() 
{
	// TODO: Add your control notification handler code here

	OnSmsChang();
	return;
}

//按钮删除
void CSMSDialog::OnButtonDel() 
{
	// TODO: Add your control notification handler code here
	OnSmsDel();
	return;
}

//按钮清空
void CSMSDialog::OnButtonClear() 
{
	// TODO: Add your control notification handler code here
	OnSmsAll(); 
	OnSmsDel();
}

//==================================================

//==================================================
//			List右键功能菜单
//==================================================

//右键存号
void CSMSDialog::OnSmsSave() 
{
	// TODO: Add your command handler code here
	CString saveText = _T("");
	int nCount = m_smsList.GetItemCount();
	for(int i = nCount; i >=0 ; i--)
	{		
		if (m_smsList.GetItemState(i, LVIS_SELECTED))
		{		
			saveText = m_smsList.GetItemText(i,2);
			break;
			//已经存在名片的不存号
			
		}		
		
	}

	CNewCardDlg dlg(this,saveText);
	dlg.DoModal();
	return;

}

void CSMSDialog::SendSMS(CString strContent,const CString& strNum)
{
	vector<CString> contents;
	vector<CString> numbers;
	
	EStringType textType = GetStringType(strContent);
	///检查字符串长度是否符合要求
	int nLimit = MaxSMSLength(textType);
	CString sendConntent = strContent;
	while(GetCharNum(strContent) > nLimit)//划分成多个待发短信
	{
		CString str = GetLeftString(strContent, nLimit);
		contents.push_back(str);
		strContent.Delete(0, str.GetLength());
	}
	
	if(!strContent.IsEmpty() || !contents.size())
	{
		contents.push_back(strContent);
	}
	
	int nfind = 0;
	
	if(strNum.GetLength() < 1)
		return;
	
	CString right = strNum;
	
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
			right = strNum.Right(right.GetLength() - right.Left(nfind).GetLength()-1);
			if(right == "")
				break;
		}
		else
		{
			numbers.push_back(right);
			break;
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
	
	numbers.clear();
	contents.clear();
}
//右键重发或者回复
void CSMSDialog::OnSmsRevert() 
{
	// TODO: Add your command handler code here
	//==================================================2007.6.18he
	/***************************************************
	if(((CSunnyCatDlg *)AfxGetMainWnd())->m_isConnect)
	{		
		GMessageBox(LoadStringEx(IDS_PROMPT_ONLINE));
		return;
	}
	/***************************************************/
	//===================================================
	vector<CString> contents;
	vector<CString> numbers;
	//只能重发一条信息
	if(m_currTreeItem == SMS_PC_DRAFT ||
		m_currTreeItem == SMS_PC_SEND ||
		m_currTreeItem == SMS_PC_UNSEND)
	{
		int nCount = m_smsList.GetItemCount();		
		for(int i = 0; i < nCount ; i++)
		{			
			if (m_smsList.GetItemState(i, LVIS_SELECTED))
			{	
				int id = m_smsList.GetItemData_EX(i);
			/*	EStringType textType = GetStringType(m_smsList.GetItemText(i, 3));
				contents.push_back(m_smsList.GetItemText(i, 3));
				numbers.push_back(m_smsList.GetItemText(i, 2));
				CThreadSendSMS thread(((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager,
							numbers,
							contents,
							-1,
							-1,
							textType);
				CDlgProgressEx dlg(thread, this, contents.size()*numbers.size()>1);
				dlg.DoModal();*/

				SendSMS(m_smsList.GetItemText(i, 3),m_smsList.GetItemText(i, 2));

				if(m_currTreeItem == SMS_PC_UNSEND)
					m_PCUnSend.ForeverDelete(id);			// 没有在发件箱中添上了条
				PostMessage(WM_SMSLIST_NOTIFY, 0,0);	// MARKER BY LAB686 THIS CODE IS NO USED..
				ClickTreeShowText(m_currTreeItem);
				m_smsList.DeleteItem_EX(i);
				return;

			}
		}		
		
		
	}
	else
	{
		OnButtonRevert();
	}
	return;
}


void CSMSDialog::OnSmsNewmsg() 
{
	// TODO: Add your command handler code here
	OnButtonNewsms();
	return;
}

//删除
void CSMSDialog::OnSmsDel() 
{
	// TODO: Add your command handler code here
	//判断所选联系人的存在位置（ＰＣ、ＳＩＭ）
	//先从ＰＣ和ＳＩＭ卡中删除，如成功再从内存中删除
	//从内存中删除
	int nCount = m_smsList.GetItemCount();
	std::vector<int> index;
	
	//进度显示
	CProgressWnd wndProgress(this, LoadStringEx(IDS_WAITFORDELETE));
	
	wndProgress.GoModal();
	wndProgress.SetRange(0,nCount-1);
	wndProgress.SetText(LoadStringEx(IDS_WAITFORDELETE));
	CString strText;
	
	m_smsList.SetRedraw(false);
	for(int i = nCount; i >=0 ; i--)
	{
		strText.Format(TEXT("%s(%d)..."),LoadStringEx(IDS_WAITFORDELETE), i);
		wndProgress.StepIt();
		wndProgress.PeekAndPump();
		wndProgress.SetText(strText);
		
		if (wndProgress.Cancelled()) 
		{
//			nFailed = countItem -m+1-nCanceled -nOverlapped;
			GMessageBox(LoadStringEx(IDS_DELETEPBCANCEL),LoadStringEx(IDS_PROMPT),MB_ICONEXCLAMATION);
			break;
		}

		if (m_smsList.GetItemState(i, LVIS_SELECTED))
		{		
			index.push_back(i);
			if(m_smsList.GetItemText(i, 3) == m_editSmsShow)
			{
				m_editSmsShow = _T("");					
				UpdateData(false);
			}
			switch(m_currTreeItem)
			{
			case SMS_SIM:
			case SMS_SIM_RECV:	
				index.push_back(m_smsList.GetItemData_EX(i));
				m_EmbedRecv.Delete(i);
				m_smsList.DeleteItem_EX(i);	
				break;
			case SMS_SIM_SEND:
				index.push_back(m_smsList.GetItemData_EX(i));
				m_EmbedSend.Delete(i);
				m_smsList.DeleteItem_EX(i);	
				break;
			case SMS_SIM_UNSEND:
				index.push_back(m_smsList.GetItemData_EX(i));
				m_EmbedDraft.Delete(i);
				m_smsList.DeleteItem_EX(i);	
				break;
			case SMS_PC:
			case SMS_PC_RECV:
				//index.push_back(m_smsList.GetItemData_EX(i))
				m_PCRecv.Delete(m_smsList.GetItemData_EX(i));
				m_smsList.DeleteItem_EX(i);				
				break;
			case SMS_PC_SEND:
				m_PCSend.Delete(m_smsList.GetItemData_EX(i));
				m_smsList.DeleteItem_EX(i);
				break;
			case SMS_PC_UNSEND:
				m_PCUnSend.Delete(m_smsList.GetItemData_EX(i));
				m_smsList.DeleteItem_EX(i);
				break;
			case SMS_PC_DRAFT:
				m_PCDraft.Delete(m_smsList.GetItemData_EX(i));
				m_smsList.DeleteItem_EX(i);
				break;
			case SMS_PC_RECYCLE:
				m_PCRecyle.Delete(m_smsList.GetItemData_EX(i));
				m_smsList.DeleteItem_EX(i);
				break;
			}
				
		}
		
		ClickTreeShowText(m_currTreeItem);	
		ClickTreeShowText(SMS_PC_RECYCLE);
		
	}
		m_smsList.SetRedraw(true);
		
	wndProgress.m_bCancelled = true;
	wndProgress.Hide();
	
	m_editSmsShow = "";
	UpdateData(false);

	SetDelBtnState(true);
	SetReBtnState(true);
	SetTraBtnState(true);
}

//呼叫
void CSMSDialog::OnSmsDail() 
{
	// TODO: Add your command handler code here
	//==================================================2008.11.14he
	/***************************************************
	if(((CSunnyCatDlg *)AfxGetMainWnd())->m_isConnect)
	{		
		GMessageBox(LoadStringEx(IDS_PROMPT_ONLINE));
		return;
	}
	/***************************************************/
	//===================================================
	CallStatus m_status;
	m_status = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_pCallMgr->GetCallStatus();
	if(m_status != CALLSTATE_IDLE)
		return;
	
	int nCount = m_smsList.GetItemCount();
	CString callNum = "";
	for(int i = nCount; i >=0 ; i--)
	{
		
		if (m_smsList.GetItemState(i, LVIS_SELECTED))
		{
			callNum = m_smsList.GetItemText(i, 2);
				break;			
		}
	}

	((CSunnyCatDlg *)AfxGetMainWnd())->ShowPage(2);
	pCallDialog->Call(callNum);
	return;
}

//转发
void CSMSDialog::OnSmsChang() 
{
	//==================================================2008.11.14he
	/********************************************************
	if(((CSunnyCatDlg *)AfxGetMainWnd())->m_isConnect)
	{		
		GMessageBox(LoadStringEx(IDS_PROMPT_ONLINE));
		return;
	}
	/********************************************************/
	//===================================================
	// TODO: Add your command handler code here
	if(m_smsList.GetItemCount() < 1)
		return;
	if(m_currTreeItem == SMS_PC_RECYCLE)
		return;
	CNewSmsDialog newSmsDialog(this);
	if(m_currTreeItem == SMS_PC_RECV)
	{
		int idRecv = m_PCRecv.FindIndexByID(m_smsList.GetItemData_EX(m_nCurSelItem));
		newSmsDialog.m_strSendSmsContent = m_PCRecv.GetAt(idRecv).strText;
	}
	if(m_currTreeItem == SMS_PC_SEND)
	{
		int idSend = m_PCSend.FindIndexByID(m_smsList.GetItemData_EX(m_nCurSelItem));
		newSmsDialog.m_strSendSmsContent = m_PCSend.GetAt(idSend).strText;
	}
	if(m_currTreeItem == SMS_PC_UNSEND)
	{
		int idUnsend = m_PCUnSend.FindIndexByID(m_smsList.GetItemData_EX(m_nCurSelItem));
		newSmsDialog.m_strSendSmsContent = m_PCUnSend.GetAt(idUnsend).strText;
	}
	if(m_currTreeItem == SMS_PC_DRAFT)
	{
		int idDraft = m_PCDraft.FindIndexByID(m_smsList.GetItemData_EX(m_nCurSelItem));
		newSmsDialog.m_strSendSmsContent = m_PCDraft.GetAt(idDraft).strText;
	}
	ClickTreeShowText(m_currTreeItem);
	UpdateData(false);
	
	newSmsDialog.DoModal();

//	AfxMessageBox("转发！");
	return;
}

//全选
void CSMSDialog::OnSmsAll() 
{
	// TODO: Add your command handler code here
	int selCount = 0;
	for(int i = 0; i < m_smsList.GetItemCount(); i++)
	{
		m_smsList.SetItemState(i,LVIS_SELECTED,LVIS_SELECTED) ;
		m_smsList.SetCheck(i) ;
		selCount++;
	}
	if(selCount > 1)
	{
		SetReBtnState(true);
		SetTraBtnState(true);		
	}
	//AfxMessageBox("全选！");
	return;
}

//反选
void CSMSDialog::OnSmsOther() 
{
	// TODO: Add your command handler code here
	static DWORD dwStaFlag = LVIS_SELECTED|LVIS_FOCUSED|LVIS_ACTIVATING;
		int nCount = m_smsList.GetItemCount();
	int selCount = 0;

		for(int i = 0; i < nCount; i++)
		{
			if (m_smsList.GetItemState(i, LVIS_SELECTED))
			{
			
				m_smsList.SetItemState(i, 0, LVIS_SELECTED);
			}
			else
		{
			m_smsList.SetItemState(i,dwStaFlag,dwStaFlag);
			selCount++;
		}
	}
	
	if(selCount > 1)
	{
		SetReBtnState(true);
		SetTraBtnState(true);		
		}
}

void CSMSDialog::ShowSMSForSIM(int iType)
{
	int i;
	int nSize;
	int imag = SMS_IMAG_RECV;
	CString nameAndPhone;
	TSMSItem item;
	int newSmsSum = 0;
	CString strName = _T("");
	int nID;
	switch(iType)
	{
	case SMS_SIM:
	case SMS_SIM_RECV:
		//m_EmbedRecv.Clear();
		m_smsList.SetRedraw(false);
		m_smsList.DeleteAllItems_EX();
		nSize = m_EmbedRecv.Size();
		nameAndPhone = _T("");
		
		for(i = 0; i < nSize; i++)
		{
			item = m_EmbedRecv.GetAt(i);

			if(item.iSMSState == SMS_UNREAD)
			{
				((CSunnyCatDlg *)AfxGetMainWnd())->FindNameInPhonebook(item.strPhoneNum, strName, nID);
				int items = m_smsList.AddItem(SMS_IMAG_UNREAD, TEXT("new"), strName, item.strPhoneNum, item.strText, item.TimeString());
				newSmsSum++;
			}
			else
			{
				((CSunnyCatDlg *)AfxGetMainWnd())->FindNameInPhonebook(item.strPhoneNum, strName, nID);
				int items = m_smsList.AddItem(SMS_IMAG_RECV, TEXT(""), strName, item.strPhoneNum, item.strText, item.TimeString());
			}
			m_smsList.SetItemData_EX(i, item.iID);
			strName = _T("");
		}
		if(newSmsSum > 0)
		{
			//显示新信息提示
		}
		m_smsList.Sort(4,TRUE);
		m_smsList.SetRedraw(true);
		break;
	case SMS_SIM_SEND:
		m_smsList.SetRedraw(false);
		m_smsList.DeleteAllItems_EX();
		nSize = m_EmbedSend.Size();
		nameAndPhone = _T("");
		for(i = 0; i < nSize; i++)
		{
			item = m_EmbedSend.GetAt(i);
			
				imag = SMS_IMAG_SEND;
		
		//	nameAndPhone = 
			((CSunnyCatDlg *)AfxGetMainWnd())->FindNameInPhonebook(item.strPhoneNum, strName, nID);
			m_smsList.AddItem(imag, TEXT(""), strName, item.strPhoneNum, item.strText, item.TimeString());
			m_smsList.SetItemData_EX(i, item.iID);
			strName = _T("");
		}
		m_smsList.SetRedraw(true);
		break;
	case SMS_SIM_UNSEND:
		m_smsList.SetRedraw(false);
		m_smsList.DeleteAllItems_EX();
		nSize = m_EmbedDraft.Size();
		nameAndPhone = _T("");
		for(i = 0; i < nSize; i++)
		{
			item = m_EmbedDraft.GetAt(i);
				imag = SMS_IMAG_UNSEND;
		//	nameAndPhone = 
			((CSunnyCatDlg *)AfxGetMainWnd())->FindNameInPhonebook(item.strPhoneNum, strName, nID);
			m_smsList.AddItem(imag, TEXT(""), strName, item.strPhoneNum, item.strText, item.TimeString());
			m_smsList.SetItemData_EX(i, item.iID);
			strName = _T("");
		}
		m_smsList.SetRedraw(true);
		break;
		
	}
}

void CSMSDialog::ShowSMSForPC(int iType)
{
	TRACE(TEXT("CSMSDialog::ShowSMSForPC :> ENTER\n"));
	int i;
	int nSize;
	int imag = SMS_IMAG_RECV;
	CString nameAndPhone;
	CString strName = _T("");
	int nID;
	int newSmsSum = 0;
	TSMSItem item;
	switch(iType)
	{
	case SMS_PC:
	case SMS_PC_RECV:
		m_smsList.SetRedraw(false);
		m_smsList.DeleteAllItems_EX();
		m_PCRecv.ClearVector();
		m_PCRecv.GetRecordsByMailBoxID(SMS_MAILBOX_RECV);
		nSize = m_PCRecv.Size();
		nameAndPhone = _T("");
		for(i = 0; i < nSize; i++)
		{
			item = m_PCRecv.GetAt(i);
	
			if(item.nPSMSState == SMS_PC_UNREAD)
			{
				((CSunnyCatDlg *)AfxGetMainWnd())->FindNameInPhonebook(item.strPhoneNum, strName, nID);
				int items = m_smsList.AddItem(SMS_IMAG_UNREAD, TEXT("new"), strName, item.strPhoneNum, item.strText, item.TimeString());
				newSmsSum++;
			}
			else
			{
				((CSunnyCatDlg *)AfxGetMainWnd())->FindNameInPhonebook(item.strPhoneNum, strName, nID);
				int items = m_smsList.AddItem(SMS_IMAG_RECV, TEXT(""), strName, item.strPhoneNum, item.strText, item.TimeString());
			}
			m_smsList.SetItemData_EX(i, item.iID);
			strName = _T("");
		}
		if(newSmsSum > 0)
		{
			//显示新信息提示
			((CSunnyCatDlg *)AfxGetMainWnd())->PostMessage(WM_SMS_READ,1,1);
		}
		m_smsList.SetRedraw(true);
		break;
	case SMS_PC_SEND:
		m_smsList.SetRedraw(false);
		m_smsList.DeleteAllItems_EX();
		m_PCSend.ClearVector();
		m_PCSend.GetRecordsByMailBoxID(SMS_MAILBOX_SEND);
		nSize = m_PCSend.Size();
		nameAndPhone = _T("");		
		for(i = 0; i < nSize; i++)
		{
			item = m_PCSend.GetAt(i);
			
				imag = SMS_IMAG_SEND;
		//	nameAndPhone = 
			((CSunnyCatDlg *)AfxGetMainWnd())->FindNameInPhonebook(item.strPhoneNum, strName, nID);
			m_smsList.AddItem(imag, TEXT(""), strName, item.strPhoneNum, item.strText, item.TimeString());
			m_smsList.SetItemData_EX(i, item.iID);
			strName = _T("");
		}
		m_smsList.SetRedraw(true);
		break;
	case SMS_PC_UNSEND:
		m_smsList.SetRedraw(false);
		m_smsList.DeleteAllItems_EX();
		m_PCUnSend.ClearVector();
		m_PCUnSend.GetRecordsByMailBoxID(SMS_MAILBOX_UNSEND);
		nSize = m_PCUnSend.Size();
		nameAndPhone = _T("");
		for(i = 0; i < nSize; i++)
		{
			item = m_PCUnSend.GetAt(i);
		//	nameAndPhone = 
			((CSunnyCatDlg *)AfxGetMainWnd())->FindNameInPhonebook(item.strPhoneNum, strName, nID);
			m_smsList.AddItem(SMS_IMAG_UNSEND, TEXT(""), strName, item.strPhoneNum, item.strText, item.TimeString());
			m_smsList.SetItemData_EX(i, item.iID);
			strName = _T("");
		}
		m_smsList.SetRedraw(true);
		break;

	case SMS_PC_DRAFT:
		TRACE(TEXT("m_PCDraft.Size() = %d\n"), m_PCDraft.Size());
		m_smsList.SetRedraw(false);
		m_smsList.DeleteAllItems_EX();
		m_PCDraft.ClearVector();
		TRACE(TEXT("m_PCDraft.Size() = %d\n"), m_PCDraft.Size());
		m_PCDraft.GetRecordsByMailBoxID(SMS_MAILBOX_DRAFT);
		TRACE(TEXT("m_PCDraft.Size() = %d\n"), m_PCDraft.Size());
		nSize = m_PCDraft.Size();
		TRACE(TEXT("m_PCDraft.Size() = %d\n"), m_PCDraft.Size());
		nameAndPhone = _T("");		
		for(i = 0; i < nSize; i++)
		{
			item = m_PCDraft.GetAt(i);
		//	nameAndPhone = 
			((CSunnyCatDlg *)AfxGetMainWnd())->FindNameInPhonebook(item.strPhoneNum, strName, nID);
			m_smsList.AddItem(SMS_IMAG_DRAFT, TEXT(""), strName, item.strPhoneNum, item.strText, item.TimeString());
			m_smsList.SetItemData_EX(i, item.iID);
			strName = _T("");
		}
		m_smsList.SetRedraw(true);
		break;

	case SMS_PC_RECYCLE:
		m_smsList.SetRedraw(false);
		m_smsList.DeleteAllItems_EX();
		m_PCRecyle.ClearVector();
		m_PCRecyle.GetRecordsByMailBoxID(SMS_MAILBOX_RECYLE);
		nSize = m_PCRecyle.Size();
		nameAndPhone = _T("");		
		for(i = 0; i < nSize; i++)
		{
			item = m_PCRecyle.GetAt(i);
		//	nameAndPhone = 
			((CSunnyCatDlg *)AfxGetMainWnd())->FindNameInPhonebook(item.strPhoneNum, strName, nID);
			m_smsList.AddItem(SMS_IMAG_RECYCLE, TEXT(""), strName, item.strPhoneNum, item.strText, item.TimeString());
			m_smsList.SetItemData_EX(i, item.iID);
			strName = _T("");
		}
		m_smsList.SetRedraw(true);
		break;

	}
	LogString("ShowSMSForPC end");
	TRACE(TEXT("ShowSMSForPC end iType = %d"), iType);
}

struct DeleteSMS
{
	virtual bool Delete(size_t index, bool isDelAll=false,bool isOutbox=false) = 0;
};


class DeleteSMS_PC: public DeleteSMS
{
	CSMS*	m_pSMSList;
	bool isDelAll;
	bool isOutbox;
	bool isPcbox;
public:
	DeleteSMS_PC(CSMS* pSMSList,bool isDelAll,bool isOutbox,bool isPcbox)
		:m_pSMSList(pSMSList),isDelAll(isDelAll),isOutbox(isOutbox),isPcbox(isPcbox)
	{
		ASSERT(m_pSMSList);
	}
	virtual bool Delete(size_t index,bool isDelAll,bool isOutbox)
	{
		//m_pSMSList->Delete(index);
		return true;
	}
};

class DeleteSMS_Embed: public DeleteSMS
{
	CMobileManager& m_Mgr;
	CSMS*	m_pSMSList;
	CString		m_Storage;
	bool isDelAll;
	bool isOutbox;
	bool isPcbox;
protected:
	DeleteSMS_Embed(CMobileManager& mgr, CSMS* smsList, const CString& storage,bool isDelAll,bool isOutbox,bool isPcbox)
		:m_Mgr(mgr), m_pSMSList(smsList), m_Storage(storage),isDelAll(isDelAll),isOutbox(isOutbox),isPcbox(isPcbox)
	{
	}

	virtual bool Delete(size_t index)
	{

		CATCmd cmd;
		cmd = m_Mgr.SendATPacket(CMobileATProto::SMSBoxSelect(m_Storage));//jiangguochao added 20070115
		m_Mgr.WaitForATCompleted(cmd,1000);
		
	
		int iID = index;//m_pSMSList->GetAt(index).iID;
		cmd = m_Mgr.SendATPacket( CMobileATProto::SMSDelete(iID) );
		
		bool bOK = false;
	
			m_Mgr.WaitForATCompleted(cmd,1000);
			bOK = true;	

		return bOK;
	}
};

class DeleteSMS_MT: public DeleteSMS_Embed
{
public:
	DeleteSMS_MT(CMobileManager& mgr, CSMS* smsList,bool isDelAll,bool isOutbox,bool isPcbox)
		:DeleteSMS_Embed(mgr, smsList, "SM",isDelAll,isOutbox,isPcbox)/*jiangguochao changed MT->SM 20070115*/
	{
	}
};

struct DeleteSMS_MO: public DeleteSMS_Embed
{
public:
	DeleteSMS_MO(CMobileManager& mgr, CSMS* smsList,bool isDelAll,bool isOutbox,bool isPcbox)
		:DeleteSMS_Embed(mgr, smsList, "SM",isDelAll,isOutbox,isPcbox)/*jiangguochao changed MT->SM 20070115*/
	{
	}
};

class CThreadDeleteSMS: public CPrgressThread
{
	CListCtrl&	m_ListCtrl;
	DeleteSMS&  m_DeleteObj;
	const std::vector<int>& m_ItemsDel;
	bool isDelAll;
	bool isOutbox;
	bool isPcbox;
public:
	CThreadDeleteSMS(CListCtrl& lstCtrl, DeleteSMS& delObj, const std::vector<int>& items ,bool isDelAll,bool isOutbox,bool isPcbox)
		:m_ListCtrl(lstCtrl), m_DeleteObj(delObj), m_ItemsDel(items),isDelAll(isDelAll),isOutbox(isOutbox),isPcbox(isPcbox)
	{
	}
	virtual void DoWork()
	{
		m_pUI->SetProgress(0, m_ItemsDel.size(), LoadStringEx(IDS_DELETING));
		if((isDelAll) &&(!isPcbox))
		{
			m_pUI->SetProgress(m_ItemsDel.size()/2, m_ItemsDel.size());//20070320
			if(m_DeleteObj.Delete(0,isDelAll,isOutbox))
			{
				m_pUI->SetProgress(m_ItemsDel.size(), m_ItemsDel.size());
			}
			else
			{
				GMessageBox(LoadStringEx(IDS_ERROR_DELETE_FAILED), NULL,MB_OK|MB_ICONINFORMATION);
			}
		}
		else
		{
			for(size_t i=m_ItemsDel.size(); i>0 && !m_bCancel; --i)
			{
				m_pUI->SetProgress(m_ItemsDel.size()-i+1, m_ItemsDel.size());

				int nIndex = m_ItemsDel[i-1];
				if( m_DeleteObj.Delete(nIndex) )
				{
					//m_ListCtrl.DeleteItem( nIndex );
				}
				else
				{
//					ShowMessageBox(IDS_ERROR_DELETE_FAILED, MB_OK|MB_ICONINFORMATION);
					GMessageBox(LoadStringEx(IDS_ERROR_DELETE_FAILED), NULL, MB_OK|MB_ICONINFORMATION);
					break;
				}
			}
		}
	}
};


//右键恢复
void CSMSDialog::OnSmsRecv() 
{
	// TODO: Add your command handler code here
	m_smsList.SetRedraw(false);
	int nCount = m_smsList.GetItemCount();
	for(int i = nCount; i >=0 ; i--)
	{
		
		if (m_smsList.GetItemState(i, LVIS_SELECTED))
		{		
			m_PCRecyle.RestoreOneRecord(m_smsList.GetItemData_EX(i));
			m_smsList.DeleteItem_EX(i);
		}
	}

	ClickTreeShowText(SMS_PC_RECV);
	ClickTreeShowText(SMS_PC_SEND);
	ClickTreeShowText(SMS_PC_UNSEND);
	ClickTreeShowText(SMS_PC_DRAFT);
	ClickTreeShowText(SMS_PC_RECYCLE);
	m_smsList.SetRedraw(true);
	m_editSmsShow = "";
	UpdateData(false);
}

//右键恢复全部
void CSMSDialog::OnSmsRecvall() 
{
	// TODO: Add your command handler code here
	m_smsList.SetRedraw(false);
	m_PCRecyle.RestoreAllRecords();
	m_smsList.DeleteAllItems_EX();
	ClickTreeShowText(SMS_PC_RECV);
	ClickTreeShowText(SMS_PC_SEND);
	ClickTreeShowText(SMS_PC_UNSEND);
	ClickTreeShowText(SMS_PC_DRAFT);
	ClickTreeShowText(SMS_PC_RECYCLE);
	m_smsList.SetRedraw(true);
	m_editSmsShow = "";
	UpdateData(false);
}

//右键拷贝
void CSMSDialog::OnSmsCopy() 
{
	// TODO: Add your command handler code here
	int nCount = m_smsList.GetItemCount();

	TSMSItem pcItem;
	for(int i = nCount; i >=0 ; i--)
{
		
		if (m_smsList.GetItemState(i, LVIS_SELECTED))
	{
			if(m_currTreeItem == SMS_SIM || m_currTreeItem == SMS_SIM_RECV)
			{	
				pcItem.nMailBoxID = SMS_MAILBOX_RECV;
				pcItem.strPhoneNum = m_smsList.GetItemText(i, 2);
				pcItem.strText = m_smsList.GetItemText(i, 3);
				TimeFromString(m_smsList.GetItemText(i, 4),pcItem.tmSMSTime);
				m_PCRecv.AddBack(pcItem);				

			}
			if(m_currTreeItem == SMS_SIM_SEND)
		{	
				pcItem.nMailBoxID = SMS_MAILBOX_SEND;
				pcItem.strPhoneNum = m_smsList.GetItemText(i, 2);
				pcItem.strText = m_smsList.GetItemText(i, 3);
				TimeFromString(m_smsList.GetItemText(i, 4), pcItem.tmSMSTime);
				m_PCSend.AddBack(pcItem);
		}
			if(m_currTreeItem == SMS_SIM_UNSEND)
		{
				pcItem.nMailBoxID = SMS_MAILBOX_UNSEND;
				pcItem.strPhoneNum = m_smsList.GetItemText(i, 2);
				pcItem.strText = m_smsList.GetItemText(i, 3);
				TimeFromString(m_smsList.GetItemText(i, 4),pcItem.tmSMSTime);
				m_PCUnSend.AddBack(pcItem);
		}
			
	}
}
}

void CSMSDialog::OnClickTreeSms(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	m_smsTree.SelectItem(m_currItem) ;
	m_smsTree.SelectSetFirstVisible(m_currItem) ;
	
	if(m_currTreeItem == SMS_PC_RECV || m_currTreeItem == SMS_PC)
	{		
		if(((CSunnyCatDlg *)AfxGetMainWnd())->m_bResetting)
		{
			GMessageBox(LoadStringEx(IDS_SEARCH_NET));
			return;
		}

		if(((CSunnyCatDlg *)AfxGetMainWnd())->m_InitAT.IsInitRunning())
		{
			GMessageBox(LoadStringEx(IDS_INIT_RUNNING));
			return;
		}

		if(!((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.isAbleCom)
		{
			GMessageBox(LoadStringEx(IDS_NO_SC700_CARD));
			return;
		}
		/***************************************************
		if(((CSunnyCatDlg *)AfxGetMainWnd())->m_isConnect)
		{
			return;
		}
		/***************************************************/
		((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.SMSEnum();

		//******************************************************************
		int size = m_EmbedRecv.Size();			// 只读取了一个收件箱
		if(size < 1)
			return;
		TSMSItem item;
		
		for(int i = 0; i < size; i++)
		{
			item.iSMSState = m_EmbedRecv.GetAt(i).iSMSState;
			item.nMailBoxID = (SMSMailBoxID)SMS_MAILBOX_RECV;
			item.nOrigMailBoxID = (SMSMailBoxID)SMS_MAILBOX_RECV;
			item.strPhoneNum = m_EmbedRecv.GetAt(i).strPhoneNum;
			if(item.iSMSState == SMS_UNREAD)
				item.nPSMSState = (PSMSType)SMS_PC_UNREAD;
			else
				item.nPSMSState = (PSMSType)SMS_PC_RECV;
			item.strText = m_EmbedRecv.GetAt(i).strText;
			item.tmSMSTime = m_EmbedRecv.GetAt(i).tmSMSTime;
			m_PCRecv.AddBack(item);		
		}

		CATCmd cmd;
		cmd = ((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.SendATPacket( CMobileATProto::SMSDeleteAll());
		((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.WaitForATCompleted(cmd,2000);

		for(i = 0; i < size; i++)
		{
			m_EmbedRecv.DeleteByID(m_EmbedRecv.GetAt(0).iID);
		}
		ShowSMSForPC(SMS_PC_RECV);
		/******************************************************************/
		/******************************************************************
		size = m_EmbedSend.Size();
		TSMSItem item1;	
		for( i = 0; i < size; i++)
		{
			item1.iSMSState = m_EmbedSend.GetAt(i).iSMSState;
			item1.nMailBoxID = (SMSMailBoxID)SMS_MAILBOX_SEND;
			item1.nOrigMailBoxID = (SMSMailBoxID)SMS_MAILBOX_SEND;
			item1.strPhoneNum = m_EmbedSend.GetAt(i).strPhoneNum;
			item1.nPSMSState = (PSMSType)SMS_PC_SEND;
			item1.strText = m_EmbedSend.GetAt(i).strText;
			item1.tmSMSTime = m_EmbedSend.GetAt(i).tmSMSTime;
			m_PCRecv.AddBack(item1);		
		}

//		CATCmd cmd;
//		cmd = ((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.SendATPacket( CMobileATProto::SMSDeleteAll());
//		((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.WaitForATCompleted(cmd,2000);

		for(i = 0; i < size; i++)
		{
			m_EmbedSend.DeleteByID(m_EmbedSend.GetAt(0).iID);
		}
		ShowSMSForPC(SMS_PC_SEND);
		/******************************************************************/
		/******************************************************************
		size = m_EmbedDraft.Size();
		TSMSItem item2;	
		for( i = 0; i < size; i++)
		{
			item2.iSMSState = m_EmbedDraft.GetAt(i).iSMSState;
			item2.nMailBoxID = (SMSMailBoxID)SMS_MAILBOX_DRAFT;
			item2.nOrigMailBoxID = (SMSMailBoxID)SMS_MAILBOX_DRAFT;
			item2.strPhoneNum = m_EmbedDraft.GetAt(i).strPhoneNum;
			item2.nPSMSState = (PSMSType)SMS_PC_DRAFT;
			item2.strText = m_EmbedDraft.GetAt(i).strText;
			item2.tmSMSTime = m_EmbedDraft.GetAt(i).tmSMSTime;
			m_PCRecv.AddBack(item2);		
		}

//		CATCmd cmd;
//		cmd = ((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.SendATPacket( CMobileATProto::SMSDeleteAll());
//		((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.WaitForATCompleted(cmd,2000);

		for(i = 0; i < size; i++)
		{
			m_EmbedDraft.DeleteByID(m_EmbedDraft.GetAt(0).iID);
		}
		ShowSMSForPC(SMS_PC_DRAFT);
		/******************************************************************/

	}
	ClickTreeShowText(SMS_PC_RECV);
//	ClickTreeShowText(SMS_PC_SEND);
//	ClickTreeShowText(SMS_PC_DRAFT);
	*pResult = 0;
}


//Insert new SMS 
void CSMSDialog::InsertNewSIMSms(TSMSItem item)
{
	TRACE(TEXT("CSMSDialog::InsertNewSIMSms"));
	m_smsTree.SelectItem(m_recItem);
	m_smsTree.SelectSetFirstVisible(m_recItem) ;
	TSMSItem pcItem;
	item.nMailBoxID = (SMSMailBoxID)SMS_MAILBOX_RECV;
	item.nOrigMailBoxID = (SMSMailBoxID)SMS_MAILBOX_RECV;
	item.tmSMSTime = CTime::GetCurrentTime();
	item.nPSMSState = (PSMSType)SMS_PC_UNREAD;
	pcItem = m_PCRecv.AddBack(item);
	TRACE(TEXT("pcItem.iID = %d\n"),pcItem.iID);
//	if(m_currTreeItem == SMS_PC || m_currTreeItem == SMS_PC_RECV)
//	{
	CString strName = _T("");
	int ID;

	((CSunnyCatDlg *)AfxGetMainWnd())->FindNameInPhonebook(pcItem.strPhoneNum, strName, ID);
	TRACE(TEXT("ID = %d\n"),ID);
		int m_item = m_smsList.AddItem(SMS_IMAG_NEWSMS, TEXT("new"), strName, pcItem.strPhoneNum,pcItem.strText,pcItem.TimeString());
		TRACE(TEXT("m_item = %d\n"),m_item);
		int nID = m_PCUnSend.GetMaxID("Select max(SMSID) from SC700_SMS"); 
		if(nID<0)
			nID = 0;
		TRACE(TEXT("m_item = %d\n"),m_item);
		TRACE(TEXT("nID = %d\n"),nID);
		TRACE(TEXT("pcItem.iID = %d\n"),pcItem.iID);
		m_smsList.SetItemData_EX(m_item, nID);
		TRACE(TEXT("m_item = %d\n"),m_item);
		TRACE(TEXT("nID = %d\n"),nID);
		TRACE(TEXT("pcItem.iID = %d\n"),pcItem.iID);
		m_smsList.Sort(4, FALSE);
//	}
	CATCmd cmd;	
	cmd = ((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.SendATPacket( CMobileATProto::SMSDelete(pcItem.iID));
	((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.WaitForATCompleted(cmd,2000);
	
	m_EmbedRecv.DeleteByID(pcItem.iID);
	ClickTreeShowText(SMS_PC_RECV);
}

//Insert unsend SMS
void CSMSDialog::InsertUnsendSms(TSMSItem item)
{
	m_PCUnSend.AddBack(item);

	//ClickTreeShowText(SMS_PC_UNSEND);
}

//Insert send SMS
void CSMSDialog::InsertSendSms(TSMSItem item)
{
	/********************************************************
	m_smsTree.SelectItem(m_sendItem);
	m_smsTree.SelectSetFirstVisible(m_sendItem) ;
	TSMSItem pcItem;
	item.nMailBoxID = (SMSMailBoxID)SMS_MAILBOX_SEND;
	item.nOrigMailBoxID = (SMSMailBoxID)SMS_MAILBOX_SEND;
	item.tmSMSTime = CTime::GetCurrentTime();
	item.nPSMSState = (PSMSType)SMS_PC_SEND;
	pcItem = m_PCSend.AddBack(item);

	CString strName = _T("");
	int ID;

	((CSunnyCatDlg *)AfxGetMainWnd())->FindNameInPhonebook(pcItem.strPhoneNum, strName, ID);
		int m_item = m_smsList.AddItem(SMS_IMAG_SEND, TEXT(""), strName, pcItem.strPhoneNum,pcItem.strText,pcItem.TimeString());
		int nID = m_PCUnSend.GetMaxID("Select max(SMSID) from UCAT_SMS"); 
		if(nID<0)
			nID = 0;
		m_smsList.SetItemData_EX(m_item, nID);
		m_smsList.Sort(4, FALSE);
	ClickTreeShowText(SMS_PC_SEND);
	/********************************************************/
	m_PCSend.AddBack(item);

}


LRESULT CSMSDialog::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_NOTIFY)
	{
		if (wParam == IDC_SPLITTER1)
		{	
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			DoResize1(pHdr->delta);
		}
		else if (wParam == IDC_SPLITTER2)
		{
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			DoResize2(pHdr->delta);
		}
	}
	return CBmpBkDialog::DefWindowProc(message, wParam, lParam);
}

void CSMSDialog::DoResize1(int delta)
{
	CSplitterControl::ChangeWidth(&m_smsTree, delta);
	CSplitterControl::ChangeWidth(&m_smsList, -delta, CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(&m_ctrlSmsEdit, -delta, CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(&m_wndSplitter2, -delta, CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(GetDlgItem(IDC_SPLITTER3), -delta, CW_RIGHTALIGN);
	/*CSplitterControl::ChangeWidth(&m_TransmitBtn, -delta, CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(&m_ReplyBtn, -delta, CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(&m_DeleteBtn, -delta, CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(&m_NewBtn, -delta, CW_RIGHTALIGN);
*/
	CRect rc;
	if(m_NewBtn.GetSafeHwnd() != NULL)
	{		
		m_NewBtn.GetWindowRect(&rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
		int nWidth = rc.Width();
		rc.left = rc.left+delta;
		rc.right = rc.left+nWidth;
		m_NewBtn.MoveWindow(&rc);
	}
	if(m_TransmitBtn.GetSafeHwnd() != NULL)
	{		
		m_TransmitBtn.GetWindowRect(&rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
		int nWidth = rc.Width();
		rc.left = rc.left+delta;
		rc.right = rc.left+nWidth;
		m_TransmitBtn.MoveWindow(&rc);
	}
	if(m_ReplyBtn.GetSafeHwnd() != NULL)
	{		
		m_ReplyBtn.GetWindowRect(&rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
		int nWidth = rc.Width();
		rc.left = rc.left+delta;
		rc.right = rc.left+nWidth;
		m_ReplyBtn.MoveWindow(&rc);
	}
	if(m_DeleteBtn.GetSafeHwnd() != NULL)
	{		
		m_DeleteBtn.GetWindowRect(&rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
		int nWidth = rc.Width();
		rc.left = rc.left+delta;
		rc.right = rc.left+nWidth;
		m_DeleteBtn.MoveWindow(&rc);
	}
	Invalidate();
	UpdateWindow();
}

void CSMSDialog::DoResize2(int delta)
{
	CSplitterControl::ChangeHeight(&m_smsList, delta);
	CSplitterControl::ChangeHeight(&m_ctrlSmsEdit, -delta, CW_BOTTOMALIGN);
	CSplitterControl::ChangeHeight(GetDlgItem(IDC_SPLITTER3), -delta, CW_BOTTOMALIGN);
	Invalidate();
	UpdateWindow();
}



LRESULT CSMSDialog::OnMyMove(WPARAM wParam,LPARAM lParam)
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
			rcTab.bottom  += 10;
			m_bFirst = FALSE;
		}
		else
			rcTab.bottom = rc.bottom-rc.top-170;
	
		m_wndSplitter1.MoveWindow(&rcTab,TRUE);
	}
	
	if(m_wndSplitter2.GetSafeHwnd() != NULL)
	{
		RECT rcTab;
		m_wndSplitter2.GetWindowRect(&rcTab);
		
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcTab, 2 );
		
		RECT rc;
		GetWindowRect(&rc);
		
		if(m_b2First)
		{
			rcTab.right = rc.right-rc.left-44;
			m_b2First = FALSE;
		}
		else
			rcTab.right = rc.right-rc.left;

		rcTab.top = (long)wParam -6 ;
		rcTab.bottom = rcTab.top+5;
	
		m_wndSplitter2.MoveWindow(&rcTab,TRUE);
	}	

	return TRUE;
}

void CSMSDialog::OnSize(UINT nType, int cx, int cy) 
{
	CBmpBkDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	//if(m_wndSplitter2.GetSafeHwnd() != NULL)
	//	m_wndSplitter2.SetRange(200, cy/5, -1);
	RECT rc;
	if(m_smsTree.GetSafeHwnd() != NULL)
	{		
		m_smsTree.GetWindowRect(&rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
		
		rc.bottom = cy;
		m_smsTree.MoveWindow(&rc);
	}

	if(m_smsList.GetSafeHwnd() != NULL)
	{	
		m_smsList.GetWindowRect(&rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
		rc.right = cx;
		rc.bottom = (cy*2)/3;
		
		m_smsList.MoveWindow(&rc);		
	}
	if(m_ctrlSmsEdit.GetSafeHwnd() != NULL)
	{		
		m_ctrlSmsEdit.GetWindowRect(&rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
		
		rc.right = cx;
		rc.top = (cy*2)/3+2;
		rc.bottom = cy;
		m_ctrlSmsEdit.MoveWindow(&rc);
	
		m_ctrlSmsEdit.SetFocus();
	}

	if(m_wndSplitter1.GetSafeHwnd() != NULL)
	{	
		m_wndSplitter1.GetWindowRect(&rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
		rc.bottom = cy;
		rc.right = rc.left+1;
		m_wndSplitter1.MoveWindow(&rc);	
	}
	if(m_wndSplitter2.GetSafeHwnd() != NULL)
	{
		m_wndSplitter2.GetWindowRect(&rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
		rc.right = cx;
		rc.top = (cy*2)/3;
		rc.bottom = (cy*2)/3+1;
		m_wndSplitter2.MoveWindow(&rc);	
	}

}

void CSMSDialog::OnKeydownListSms(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int count = m_smsList.GetItemCount();
	if(count < 1)
		return;
	if(pLVKeyDow->wVKey == VK_UP)
	{
	
		m_nCurSelItem = m_nCurSelItem - 1;
		if(m_nCurSelItem < 0)
		{
			m_nCurSelItem = 0;
			return;
		}
		OnClickListSms(pNMHDR, pResult); 	

	}

	if(pLVKeyDow->wVKey == VK_DOWN)
	{		
		m_nCurSelItem = m_nCurSelItem + 1;
		if(count <= m_nCurSelItem)
		{
			m_nCurSelItem = count - 1;			
			return;
		}
		OnClickListSms(pNMHDR, pResult);
		
	}

	if(pLVKeyDow->wVKey == VK_DELETE)
	{
		OnSmsDel();		
	}

	*pResult = 0;
}

HBRUSH CSMSDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Change any attributes of the DC here
	if(nCtlColor == CTLCOLOR_DLG)
	{
		static HBRUSH brush=CreateSolidBrush(RGB(221,241,251));
		return brush;
	}
	// TODO: Return a different brush if the default is not desired
	return hbr;
}


void CSMSDialog::SetTraBtnState(bool bGray)
{
	CString strAppPath = GetAppPath()+"\\skin\\sms\\";
	int itemCount = m_smsList.GetItemCount();
	
	if(bGray)
	{
		m_TransmitBtn.EnableWindow(FALSE);
		m_TransmitBtn.SetBitmapPath(strAppPath+"sms_forward_gray.bmp");
		m_TransmitBtn.Draw();
	}
	else
	{
		if(itemCount>0)
		{
			m_TransmitBtn.EnableWindow(TRUE);
			m_TransmitBtn.SetBitmapPath(strAppPath+"sms_forward.bmp",strAppPath+"sms_forward_down.bmp",strAppPath+"sms_forward_over.bmp");
			m_TransmitBtn.Draw();
		}
		else
		{
		m_TransmitBtn.EnableWindow(FALSE);
		m_TransmitBtn.SetBitmapPath(strAppPath+"sms_forward_gray.bmp");
		m_TransmitBtn.Draw();

		}
	}

}

void CSMSDialog::SetReBtnState(bool bGray)
{
	CString strAppPath = GetAppPath()+"\\skin\\sms\\";
	int itemCount = m_smsList.GetItemCount();
	if(bGray)
	{
		m_ReplyBtn.EnableWindow(FALSE);
		m_ReplyBtn.SetBitmapPath(strAppPath+"sms_reply_gray.bmp");
		m_ReplyBtn.Draw();
	}
	else
	{
		if(itemCount>0)
		{
			m_ReplyBtn.EnableWindow(TRUE);
			m_ReplyBtn.SetBitmapPath(strAppPath+"sms_reply.bmp",strAppPath+"sms_reply_down.bmp",strAppPath+"sms_reply_over.bmp");
			m_ReplyBtn.Draw();
		}
		else
		{

		m_ReplyBtn.EnableWindow(FALSE);
		m_ReplyBtn.SetBitmapPath(strAppPath+"sms_reply_gray.bmp");
		m_ReplyBtn.Draw();
		}
	}

}

void CSMSDialog::SetDelBtnState(bool bGray)
{
	CString strAppPath = GetAppPath()+"\\skin\\sms\\";
	int itemCount = m_smsList.GetItemCount();
	if(bGray)
	{
		m_DeleteBtn.EnableWindow(FALSE);
		m_DeleteBtn.SetBitmapPath(strAppPath+"sms_delete_gray.bmp");
		m_DeleteBtn.Draw();
	}
	else
	{
		if(itemCount>0)
		{
			m_DeleteBtn.EnableWindow(TRUE);
			m_DeleteBtn.SetBitmapPath(strAppPath+"sms_delete.bmp",strAppPath+"sms_delete_down.bmp",strAppPath+"sms_delete_over.bmp");
			m_DeleteBtn.Draw();
		}
		else
		{
			m_DeleteBtn.EnableWindow(FALSE);
			m_DeleteBtn.SetBitmapPath(strAppPath+"sms_delete_gray.bmp");
			m_DeleteBtn.Draw();

		}
	}
}
