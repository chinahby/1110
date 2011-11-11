// LinkManDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SunnyCat.h"
#include "LinkManDialog.h"
#include "NewCardDlg.h"
#include "NewClassDlg.h"
#include <vector>
#include "GlobalDataCenter.h"
#include "SunnyCatDlg.h"
#include "NewCardForSIM.h"
#include "NewSmsDialog.h"
#include "Util.h"
#include "ProgressWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLinkManDialog dialog
extern CCallDialog *pCallDialog;

CLinkManDialog::CLinkManDialog(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CLinkManDialog::IDD, pParent),m_pbookEmbed("SM")
{
	//{{AFX_DATA_INIT(CLinkManDialog)
	m_strQuery = _T("");
	//}}AFX_DATA_INIT
	g_pSysImageList   = new CImageList();
	m_PBClass = new CPhonebookPCClass();
	m_currTreeItemData = LINKMAN_NODIVCLASS;
	m_nCtrlDest = CUR_CTRL_CLSTREE;
	isNewClass = true;
	isNewCard = true;
	m_bLDragging = false;
	m_bMoveable = false;
	m_htiDrag = NULL ;
	m_htiDrop = NULL ;
	m_htiOldDrop = NULL ;
	currSelListItem = 0;
	m_bFirst = TRUE;
	m_bListFirst = TRUE;
	g_pTreeImage = new CImageList();
	m_otherClassID = 6;
	desID = 6;
	m_iRepeat = REPEAT_OK;
	bInputOP = FALSE;
}

CLinkManDialog::~CLinkManDialog()
{
	delete m_PBClass;
	m_PBClass = NULL;
	delete g_pSysImageList;
	g_pSysImageList = NULL;
	delete g_pTreeImage;
	g_pTreeImage = NULL;
}
void CLinkManDialog::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinkManDialog)
	DDX_Control(pDX, IDC_BUTTON_EDIT, m_ctrlEdit);
	DDX_Control(pDX, IDC_BUTTON_SEND, m_ctrlSendCard);
	DDX_Control(pDX, IDC_BUTTON_QUERY_LINKMAN, m_ctrlSearchCard);
	DDX_Control(pDX, IDC_BUTTON_DEL_LINKMAN, m_ctrlDelCard);
	DDX_Control(pDX, IDR_LINKMAN_NEWCARD, m_ctrlNewCard);
	DDX_Control(pDX, IDC_TREE_LINKMAN, m_ctrlLinkManTree);
	DDX_Control(pDX, IDC_LIST_SHOW_LINKMAN, m_ctrlLinkManShow);
	DDX_Control(pDX, IDC_LIST_LINKMAN, m_ctrlLinkmanList);
	DDX_Control(pDX, IDC_EDIT_QUERY_LINKMAN, m_ctrlQuery);
	DDX_Text(pDX, IDC_EDIT_QUERY_LINKMAN, m_strQuery);
	DDV_MaxChars(pDX, m_strQuery, 64);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLinkManDialog, CBmpBkDialog)
	//{{AFX_MSG_MAP(CLinkManDialog)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_LINKMAN, OnRclickListLinkman)
	ON_BN_CLICKED(IDC_BUTTON_NEW_LINKMAN, OnButtonNewLinkman)
	ON_BN_CLICKED(IDC_BUTTON_NEWCLASS_LINKMAN, OnButtonNewclassLinkman)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_LINKMAN, OnSelchangedTreeLinkman)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_LINKMAN, OnRclickTreeLinkman)
	ON_COMMAND(IDR_LINKMAN_SSMS, OnLinkmanSsms)
	ON_COMMAND(IDR_LINKMAN_NEWCLASS, OnLinkmanNewclass)
	ON_COMMAND(IDR_LINKMAN_NEWCARD, OnLinkmanNewcard)
	ON_COMMAND(IDR_LINKMAN_EDITCLASS, OnLinkmanEditclass)
	ON_COMMAND(IDR_LINKMAN_DELCLASS, OnLinkmanDelclass)
	ON_COMMAND(IDR_LINKMAN_CLEARCARD, OnLinkmanClearcard)
	ON_COMMAND(IDR_LINKMAN_SMS, OnLinkmanSms)
	ON_COMMAND(IDR_LINKMAN_SELALL, OnLinkmanSelall)
	ON_COMMAND(IDR_LINKMAN_SELOTHER, OnLinkmanSelother)
	ON_COMMAND(IDR_LINKMAN_DIAL, OnLinkmanDial)
	ON_COMMAND(IDR_LINKMAN_DEL, OnLinkmanDel)
	ON_COMMAND(IDR_LINKMAN_EDITCARD, OnLinkmanEditcard)
	ON_NOTIFY(NM_CLICK, IDC_LIST_LINKMAN, OnClickListLinkman)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_LINKMAN, OnDblclkListLinkman)
	ON_NOTIFY(HDN_ITEMCHANGED, IDC_LIST_LINKMAN, OnItemchangedListLinkman)
	ON_BN_CLICKED(IDC_BUTTON_DEL_LINKMAN, OnButtonDelLinkman)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_LINKMAN, OnButtonClearLinkman)
	ON_BN_CLICKED(IDC_BUTTON_QUERY_LINKMAN, OnButtonQueryLinkman)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_LINKMAN_NOTIFY, OnNotifyList)
	ON_NOTIFY(NM_CLICK, IDC_TREE_LINKMAN, OnClickTreeLinkman)
	ON_NOTIFY(LVN_BEGINRDRAG, IDC_LIST_LINKMAN, OnBeginrdragListLinkman)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_LINKMAN, OnBegindragListLinkman)
ON_WM_SIZE()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_LINKMAN, OnKeydownListLinkman)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_LINKMAN, OnItemchangedListLinkman)
	ON_EN_CHANGE(IDC_EDIT_QUERY_LINKMAN, OnChangeEditQueryLinkman)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MY_MOVE,OnMyMove)
ON_COMMAND_RANGE(IDM_START, IDM_END, OnDialNum)   
END_MESSAGE_MAP()



//void constructeTree( CTreeCtrl& cardTree, HTREEITEM node, int parentId)
//	{
//		std::string filter = "ParentID = " + Util::intToString(parentId);
//		std::vector<CardClass*> cc = CardClass::getCardClasssByFilter(filter); 
//		for (int i = 0; i < cc.size(); ++i)
//		{
//			try{
//				
//				HTREEITEM newNode = cardTree.InsertItem(cc[i]->szClassName.c_str(), node);
//				
//				//保存查找结果目录树
//				if (cc[i]->uiClassID == 1)
//					CCardManagerDialog::m_hRootTreeItem = newNode;
//				else if (cc[i]->uiClassID == 5)
//					CCardManagerDialog::m_hSearchTreeItem = newNode;
//				
//				//cardTree.Expand(newNode, TVGN_ROOT);
//				cardTree.SetItemData(newNode, cc[i]->uiClassID);
//				//if (!cc[i]->isLeaf)
//				if (CardClass::getDataCountByFilter("ParentId = " + Util::intToString(cc[i]->uiClassID)) != 0)
//					constructeTree(cardTree, newNode);
//				
//				delete cc[i];
//			}
//		
//
//		}
//
//	}
/*	m_ctrlCardTree.SetRedraw(false);
  constructeTree(m_ctrlCardTree, m_ctrlCardTree.GetRootItem())
  m_ctrlCardTree.SetRedraw(true);
  m_ctrlCardTree.Expand(hti,TVE_EXPAND);
  */

/////////////////////////////////////////////////////////////////////////////
// CLinkManDialog message handlers

void CLinkManDialog::SetButtonState(BOOL bState)
{
	CWnd* pWnd = NULL;

	pWnd = GetDlgItem(IDC_BUTTON_DEL_LINKMAN);
	pWnd->EnableWindow(bState);
	pWnd = GetDlgItem(IDC_BUTTON_CLEAR_LINKMAN);
	pWnd->EnableWindow(bState);
}



BOOL CLinkManDialog::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();

	/**************************************************/// modify ui
	CRect temprect(110, 0, 431, 251);
	CWnd::SetWindowPos(NULL, 0, 0, temprect.Width(), temprect.Height(), SWP_NOZORDER|SWP_NOMOVE);
	/**************************************************/
	
	SetButtonState(FALSE);
//	m_ctrlQuery.SetTxtColor(RGB(62,137,190),RGB(255,255,255)) ;//RGB(228,135,255) ;
//	m_ctrlQuery.SubclassDlgItem(IDC_EDIT_QUERY_LINKMAN,this) ;
//	m_ctrlQuery.SetFontSize(12);
//	CFont newFont;
//	newFont.CreateFont(20,0,0,0, FW_NORMAL,FALSE,FALSE,0,ANSI_CHARSET, OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
//	DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS,"仿宋");
//	m_ctrlQueryString.SetFont(&newFont);
	GetDlgItem(IDR_LINKMAN_NEWCARD)->SetWindowText(LoadStringEx(IDS_LINKMAN_NEWCARD));
	GetDlgItem(IDC_BUTTON_DEL_LINKMAN)->SetWindowText(LoadStringEx(IDS_LINKMAN_DEL));
	GetDlgItem(IDC_BUTTON_CLEAR_LINKMAN)->SetWindowText(LoadStringEx(IDS_LINKMAN_CLEAR));
	GetDlgItem(IDC_BUTTON_QUERY_LINKMAN)->SetWindowText(LoadStringEx(IDS_LINKMAN_SEARCH));

	InitUI();

	// TODO: Add extra initialization here
//	m_resize.Create( this, TRUE, 400);
//	m_resize.SetGripEnabled( TRUE );
	
//	m_resize.Add(IDC_TREE_LINKMAN,0,0,0,400);
	//m_resize.Add(IDC_LIST_LINKMAN,0,0,400,400);
	//m_resize.Add(IDC_STATIC_1,0,0,400,0);
	//m_resize.Add(IDC_LIST_SHOW_LINKMAN,0,0,400,400);

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
	
	if(m_ctrlLinkmanList.GetItemCount() > 0)
	{
		m_ctrlLinkmanList.SelectItem(0, true);
		m_ctrlLinkmanList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		SetButtonState(TRUE);
		SetEditBtnState(false);
		SetSendBtnState(false);
		SetDelBtnState(false);
	}
	else
	{
		SetButtonState(FALSE);
		SetEditBtnState(true);
		SetSendBtnState(true);
		SetDelBtnState(true);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLinkManDialog::InitUI()
{
	
	LoadImgList(g_pTreeImage, IDB_LINKMANTREE);
	InitTree();
	//初使化LinkMan记录列表
	CBitmap IconBitmap;
	IconBitmap.LoadBitmap(IDB_LONGBAR);
//	g_pSysImageList->Create(20,20,ILC_MASK|ILC_COLOR24,1,1) ;
	g_pSysImageList->Create(30,30,ILC_MASK|ILC_COLOR24,1,1) ;
	g_pSysImageList->Add( &IconBitmap,(COLORREF)0xFFFFFF );
	IconBitmap.DeleteObject();
	
	m_ctrlLinkmanList.SetImageList(g_pSysImageList,LVSIL_SMALL);
	(void)m_ctrlLinkmanList.SetExtendedStyle( LVS_EX_FULLROWSELECT| LVS_EX_INFOTIP);			//隐藏列表线 
//	m_ctrlLinkmanList.SetHeadings( _T("标识,40; 姓名, 40;电话,60;职务,58;单位,100") );
	m_ctrlLinkmanList.SetHeadings(LoadStringEx(IDS_LINKMAN_LISTHEAD));
	m_ctrlLinkmanList.LoadColumnInfo();	
//	m_ctrlLinkmanList.SetTextBkColor(RGB(247,247,255));
	m_ctrlLinkmanList.SetTextColor(RGB(100,100,100));
	
	m_ctrlLinkmanList.SetRedraw(false);
	m_phBookPC.ClearVector();
	m_phBookPC.GetRecordsByTypeName(LINKMAN_NODIVCLASS);
	int size = m_phBookPC.Size();
	TPhoneBookItemEx phoneItem;
	for(int i = 0; i < size; i++)
	{	
		phoneItem = m_phBookPC.GetAt(i);
		m_ctrlLinkmanList.AddItem(6,TEXT(""), phoneItem.strName, phoneItem.strMobilePhone, phoneItem.strBusiness,phoneItem.strOffice);
		m_ctrlLinkmanList.SetItemData_EX(i, phoneItem.nID);
	}
	
	m_ctrlLinkManTree.SelectItem(m_undivItem) ;
	m_ctrlLinkManTree.SelectSetFirstVisible(m_undivItem) ;

	m_ctrlLinkmanList.SetRedraw(true);


	CString	strAppPath = GetAppPath();
	strAppPath += "skin\\phonebook\\";
	m_ctrlNewCard.SetBitmapPath(strAppPath+"new_phonebook.bmp",strAppPath+"new_phonebook_down.bmp",strAppPath+"new_phonebook_over.bmp");
	m_ctrlEdit.SetBitmapPath(strAppPath+"edit_phonebook.bmp",strAppPath+"edit_phonebook_down.bmp",strAppPath+"edit_phonebook_over.bmp");
	m_ctrlDelCard.SetBitmapPath(strAppPath+"delete.bmp",strAppPath+"delete_down.bmp",strAppPath+"delete_over.bmp");
	m_ctrlSearchCard.SetBitmapPath(strAppPath+"search.bmp",strAppPath+"search_down.bmp",strAppPath+"search_over.bmp");
	m_ctrlSendCard.SetBitmapPath(strAppPath+"sms_send.bmp",strAppPath+"sms_send_down.bmp",strAppPath+"sms_send_over.bmp");
	
	m_ctrlNewCard.SetHitOver();
	m_ctrlDelCard.SetHitOver();
	m_ctrlSearchCard.SetHitOver();
	m_ctrlSendCard.SetHitOver();
	m_ctrlEdit.SetHitOver();

	m_ctrlNewCard.SetToolTipTxt(LoadStringEx(214));
	m_ctrlDelCard.SetToolTipTxt(LoadStringEx(216));
	m_ctrlEdit.SetToolTipTxt(LoadStringEx(215));
	m_ctrlSearchCard.SetToolTipTxt(LoadStringEx(749));
	m_ctrlSendCard.SetToolTipTxt(LoadStringEx(128));
	SetQueryBtnState(true);
}


void CLinkManDialog::InitTree()
{
	//初使化历史记录树	
	m_ctrlLinkManTree.SetRedraw(false);
	m_ctrlLinkManTree.DeleteAllItems();
	m_ctrlLinkManTree.SetImageList(g_pTreeImage, LVSIL_NORMAL);
	m_PBClass->ClearVector();
	m_PBClass->GetAllRecords();
	m_ctrlLinkManTree.SetTextColor(RGB(100,100,100));
	m_ctrlLinkManTree.SetBkColor(RGB(255,255,255));
	int m_pbClassCount = m_PBClass->Size();
	

//	if (((CSunnyCatDlg *)AfxGetMainWnd())->m_strLanguage == "-En" )
//	{
		m_linkManItem = m_ctrlLinkManTree.InsertItem(m_PBClass->GetAt(0).strClassName,LINKMAN_TREE_PC,LINKMAN_TREE_PC);
		m_ctrlLinkManTree.SetItemData(m_linkManItem, 1);
	
		HTREEITEM m_item;
		for(int i = 1; i < m_pbClassCount; i++)
		{
			int id = m_PBClass->GetAt(i).nID;
			m_phBookPC.ClearVector();
			m_phBookPC.GetRecordsByTypeName(id);
			int size = 0;
			size = m_phBookPC.Size();
			CString className = "";
			className += m_PBClass->GetAt(i).strClassName + "(" + Util::intToString(size) + ")";
			if(i < 5)
			{
				m_item = m_ctrlLinkManTree.InsertItem(className, 5+i,5+i,m_linkManItem);
			}
			else
			{
				m_item = m_ctrlLinkManTree.InsertItem(className, 10,10,m_linkManItem);
			}
			if(id == 6)
				m_undivItem = m_item;
			m_ctrlLinkManTree.SetItemData(m_item, id);
		}
//	}
#if 0
	else if (((CSunnyCatDlg *)AfxGetMainWnd())->m_strLanguage == "-Ru" )
	{
		HTREEITEM m_item;
		CString russian_name = "";
		CString className = "";
		russian_name = "Компьютер";
		m_linkManItem = m_ctrlLinkManTree.InsertItem(russian_name,LINKMAN_TREE_PC,LINKMAN_TREE_PC);
		m_ctrlLinkManTree.SetItemData(m_linkManItem, 1);

		int size = 0;
		size = m_phBookPC.Size();

		russian_name = "";
		className = "";
		russian_name = "Другие";
		className += russian_name + "(" + Util::intToString(size) + ")";
		m_item = m_ctrlLinkManTree.InsertItem(className, 6,6,m_linkManItem);
		m_ctrlLinkManTree.SetItemData(m_item, 1);
		russian_name = "";
		className = "";
		russian_name = "Друг";
		className += russian_name + "(" + Util::intToString(size) + ")";
		m_item = m_ctrlLinkManTree.InsertItem(className, 7,7,m_linkManItem);
		m_ctrlLinkManTree.SetItemData(m_item, 2);
		russian_name = "";
		className = "";
		russian_name = "Бизнес";
		className += russian_name + "(" + Util::intToString(size) + ")";
		m_item = m_ctrlLinkManTree.InsertItem(className, 8,8,m_linkManItem);
		m_ctrlLinkManTree.SetItemData(m_item, 3);
		russian_name = "";
		className = "";
		russian_name = "Другие";
		className += russian_name + "(" + Util::intToString(size) + ")";
		m_item = m_ctrlLinkManTree.InsertItem(className, 9,9,m_linkManItem);
		m_ctrlLinkManTree.SetItemData(m_item, 4);
	}
#endif

	m_ctrlLinkManTree.Expand(m_linkManItem, TVE_EXPAND);
	m_count = m_ctrlLinkManTree.InsertItem(LoadStringEx(IDS_LINKMAN_QUERYZERO),LINKMAN_TREE_COUNT,LINKMAN_TREE_COUNT);
	m_ctrlLinkManTree.SetItemData(m_count, LINKMAN_COUNT);
	m_linkmanSmItem = m_ctrlLinkManTree.InsertItem(LoadStringEx(IDS_LINKMAN_SIMZERO),LINKMAN_TREE_SIM,LINKMAN_TREE_SIM);
	m_ctrlLinkManTree.SetItemData(m_linkmanSmItem, 0);
	m_ctrlLinkManTree.SetRedraw(true);
}


void CLinkManDialog::OnRclickListLinkman(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW *pListV = (NM_LISTVIEW *)pNMHDR;
	
	*pResult = 0;
	m_nCtrlDest = CUR_CTRL_CARDLIST;
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
	if(m_ctrlLinkmanList.GetItemCount() < 1)
	{
		return;
	}
	int times = 0;	
	
	
	CString countStr = "";
	int nCount = m_ctrlLinkmanList.GetItemCount();
	
	for(int i = 0; i < nCount ; i++)
	{
		
		if (m_ctrlLinkmanList.GetItemState(i, LVIS_SELECTED))
		{				
			times++;
			if(m_currTreeItemData != LINKMAN_SIM )//&& m_currTreeItemData != LINKMAN_COUNT)
			{
				
				countStr = m_ctrlLinkmanList.GetItemText(i, 0);
				int id = m_ctrlLinkmanList.GetItemData_EX(i);
				if(m_currTreeItemData == LINKMAN_COUNT)
				{
//					if (((CSunnyCatDlg *)AfxGetMainWnd())->m_strLanguage == "-En" )
//					{
						if(countStr == "PC")
						{					
							int index = m_phBookPC.FindIndexByID(id);
							m_item = m_phBookPC.GetAt(index);
						}
//					}
#if 0
					else if (((CSunnyCatDlg *)AfxGetMainWnd())->m_strLanguage == "-Ru" )
					{
						if(countStr == "Компьютер")
						{					
							int index = m_phBookPC.FindIndexByID(id);
							m_item = m_phBookPC.GetAt(index);
						}
					}
#endif					
				}
				else
				{
					int index = m_phBookPC.FindIndexByID(id);
					m_item = m_phBookPC.GetAt(index);
				}
			}
			else
			{
				
			}
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
	CMenu *pSkinMenu = NULL;
	pSkinMenu = new   CMenu; 
	GetCursorPos(&pt) ;
	CardMenu.LoadMenu(IDR_MENU_LINKMAN);
	pPopMenu = CardMenu.GetSubMenu(0) ;
	
	if(pPopMenu != NULL)
	{
		pPopMenu->ModifyMenu(IDR_LINKMAN_SMS, MF_BYCOMMAND|MF_STRING, IDR_LINKMAN_SMS , LoadStringEx(IDS_RECORD_SMS));
		pPopMenu->ModifyMenu(IDR_LINKMAN_DIAL, MF_BYCOMMAND|MF_STRING, IDR_LINKMAN_DIAL , LoadStringEx(IDS_SMS_DAIL));
		pPopMenu->ModifyMenu(IDR_LINKMAN_DEL, MF_BYCOMMAND|MF_STRING, IDR_LINKMAN_DEL , LoadStringEx(IDS_SMS_DEL));
		pPopMenu->ModifyMenu(IDR_LINKMAN_EDITCARD, MF_BYCOMMAND|MF_STRING, IDR_LINKMAN_EDITCARD , LoadStringEx(IDS_LINKMAN_EDITCARD));
		pPopMenu->ModifyMenu(IDR_LINKMAN_SELOTHER, MF_BYCOMMAND|MF_STRING, IDR_LINKMAN_SELOTHER , LoadStringEx(IDS_SMS_OTHER));
		pPopMenu->ModifyMenu(IDR_LINKMAN_SELALL, MF_BYCOMMAND|MF_STRING, IDR_LINKMAN_SELALL , LoadStringEx(IDS_SMS_ALL));
		
	}
	
	if(m_currTreeItemData != LINKMAN_SIM )//&& m_currTreeItemData != LINKMAN_COUNT
	{
//		if (((CSunnyCatDlg *)AfxGetMainWnd())->m_strLanguage == "-En" )
//		{
			if((m_currTreeItemData == LINKMAN_COUNT && countStr != "PC"))
			{
			}
			else
			{
				pSkinMenu->CreatePopupMenu();
				if(!m_item.strMobilePhone.IsEmpty())
					pSkinMenu->AppendMenu(MF_STRING,IDM_START,m_item.strMobilePhone);  
				if(!m_item.strOfficePhone.IsEmpty())
					pSkinMenu->AppendMenu(MF_STRING,IDM_START+1,m_item.strOfficePhone); 
				if(!m_item.strHomePhone.IsEmpty())
					pSkinMenu->AppendMenu(MF_STRING,IDM_START+2,m_item.strHomePhone);
				if(!m_item.strFax.IsEmpty())
					pSkinMenu->AppendMenu(MF_STRING,IDM_START+3,m_item.strFax);
			}
//		}
#if 0		
		else if (((CSunnyCatDlg *)AfxGetMainWnd())->m_strLanguage == "-Ru" )
		{
			if((m_currTreeItemData == LINKMAN_COUNT && countStr != "Компьютер"))
			{
			}
			else
			{
				pSkinMenu->CreatePopupMenu();
				if(!m_item.strMobilePhone.IsEmpty())
					pSkinMenu->AppendMenu(MF_STRING,IDM_START,m_item.strMobilePhone);  
				if(!m_item.strOfficePhone.IsEmpty())
					pSkinMenu->AppendMenu(MF_STRING,IDM_START+1,m_item.strOfficePhone); 
				if(!m_item.strHomePhone.IsEmpty())
					pSkinMenu->AppendMenu(MF_STRING,IDM_START+2,m_item.strHomePhone);
				if(!m_item.strFax.IsEmpty())
					pSkinMenu->AppendMenu(MF_STRING,IDM_START+3,m_item.strFax);
			}
		}	
#endif		
	}
	
	if(pPopMenu != NULL)
	{			
		if(m_currTreeItemData != LINKMAN_SIM )//&& m_currTreeItemData != LINKMAN_COUNT
		{
//			if (((CSunnyCatDlg *)AfxGetMainWnd())->m_strLanguage == "-En" )
//			{
				if(m_currTreeItemData == LINKMAN_COUNT && countStr != "PC")
				{
				}
				else
				{
					pPopMenu->ModifyMenu(IDR_LINKMAN_DIAL, MF_BYCOMMAND | MF_POPUP | MF_STRING, (UINT)pSkinMenu->m_hMenu, LoadStringEx(IDS_LINKMAN_DIAL)); 
				}
//			}
#if 0
			else if (((CSunnyCatDlg *)AfxGetMainWnd())->m_strLanguage == "-Ru" )
			{
				if(m_currTreeItemData == LINKMAN_COUNT && countStr != "Компьютер")
				{
				}
				else
				{
					pPopMenu->ModifyMenu(IDR_LINKMAN_DIAL, MF_BYCOMMAND | MF_POPUP | MF_STRING, (UINT)pSkinMenu->m_hMenu, LoadStringEx(IDS_LINKMAN_DIAL)); 
				}
			}
#endif			
		}
		
		int nCount = m_ctrlLinkmanList.GetItemCount();
		int selCount = 0;
		for(int j = 0; j < nCount; j++)
		{
			if (m_ctrlLinkmanList.GetItemState(j, LVIS_SELECTED))
			{				
				selCount++;
				if(selCount > 1)
				{
					SetEditBtnState(true);
					//SetSendBtnState(true);				
					pPopMenu->EnableMenuItem(1,MF_BYPOSITION|MF_GRAYED);					
					pPopMenu->EnableMenuItem(4,MF_BYPOSITION|MF_GRAYED);
					break;
				}
			}		
		}
		/******************************************************************
		if(((CSunnyCatDlg *)AfxGetMainWnd())->m_isConnect)
		{
			//SetSendBtnState(true);
			pPopMenu->EnableMenuItem(1,MF_BYPOSITION|MF_GRAYED);					
			//pPopMenu->EnableMenuItem(0,MF_BYPOSITION|MF_GRAYED);
		}
		else
		/******************************************************************/
		{
			if(!((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.m_bSimExist)
			{			
				//SetSendBtnState(true);			
				pPopMenu->EnableMenuItem(1,MF_BYPOSITION|MF_GRAYED);					
				//pPopMenu->EnableMenuItem(0,MF_BYPOSITION|MF_GRAYED);
			}
		}
		
		pPopMenu->TrackPopupMenu(TPM_LEFTBUTTON,pt.x,pt.y,this) ;
	}
	if(m_currTreeItemData != LINKMAN_SIM )//&& m_currTreeItemData != LINKMAN_COUNT
		pSkinMenu->DestroyMenu();
	delete pSkinMenu;
}

LONG CLinkManDialog::OnNotifyList(WPARAM wParam, LPARAM lParam)
{
	Invalidate(FALSE);
		
	m_ctrlLinkmanList.SetRedraw(false);

	if(m_currTreeItemData != LINKMAN_SIM && m_currTreeItemData != LINKMAN_COUNT)
	{
		ShowPBForPC(m_currTreeItemData);
	}

	if(m_currTreeItemData == LINKMAN_SIM)
	{
		if(!((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.isAbleCom)
		{
//			ShowMessageBox(IDS_NO_SC700_CARD);			
			GMessageBox(LoadStringEx(IDS_NO_SC700_CARD));
			UpdateWindow();
			return -1;
		}
		//==================================================070625he
		/**************************************************
		if(((CSunnyCatDlg *)AfxGetMainWnd())->m_isConnect)
		{					
			GMessageBox(LoadStringEx(IDS_PROMPT_ONLINE));
			UpdateWindow();
			return -1;
		}
		/**************************************************/

		if(((CSunnyCatDlg *)AfxGetMainWnd())->m_bResetting)
		{
			GMessageBox(LoadStringEx(IDS_SEARCH_NET));
			UpdateWindow();
			return -1;
		}

		if(((CSunnyCatDlg *)AfxGetMainWnd())->m_InitAT.IsInitRunning())
		{					
			GMessageBox(LoadStringEx(IDS_INIT_RUNNING));
			UpdateWindow();
			return -1;
		}

		//===================================================
		if(m_currTreeItemData != LINKMAN_COUNT)
			ShowPBForSIM();
	}



	if(m_currTreeItemData == LINKMAN_COUNT)
	{		
		int itemSum = m_ctrlLinkmanList.GetItemCount();
		CString strText = LoadStringEx(IDS_LINKMAN_QUERY);
		CString strSum ="";
		strSum.Format(TEXT("(%d)"), itemSum);
		strText += strSum;
		m_ctrlLinkManTree.SetItemText(m_count,strText);
	}

//	if(m_currTreeItemData == LINKMAN_SIM)
//	{
		CString strText = LoadStringEx(IDS_LINKMAN_SIM);
	//	int strSimSum = m_ctrlLinkmanList.GetItemCount();
		int strSimSum = m_pbookEmbed.Size();			//070625he
		CString str; 
		str.Format(TEXT("(%d)"), strSimSum);
		strText += str;
		m_ctrlLinkManTree.SetItemText(m_linkmanSmItem, strText);
//	}
	
	if(m_currTreeItemData != LINKMAN_SIM && m_currTreeItemData != LINKMAN_COUNT)
	{
		ShowClassText(m_currTreeItem , m_currTreeItemData);
	}

	m_ctrlLinkmanList.SetRedraw(true);
	((CSunnyCatDlg *)AfxGetMainWnd())->m_smsDlg.PostMessage(WM_SMSLIST_NOTIFY,0,1);	
	if(m_ctrlLinkmanList.GetItemCount() > 0)
	{
		SetEditBtnState(false);
	    SetSendBtnState(false);
	    SetDelBtnState(false);
	}
	else
	{
		SetEditBtnState(true);
	    SetSendBtnState(true);
	    SetDelBtnState(true);

	}
	Invalidate(TRUE);

	return 0;
}

void CLinkManDialog::SetTreeText(DWORD nID)
{
	HTREEITEM treeItem = m_ctrlLinkManTree.GetRootItem();
	HTREEITEM Item = treeItem;
	HTREEITEM newItem = NULL;
	while(1)
	{			
		Item = m_ctrlLinkManTree.GetNextItem(Item, TVGN_NEXTVISIBLE);
		DWORD id = m_ctrlLinkManTree.GetItemData(Item);
		if(id == nID)
		{
			newItem = Item;
			break;
		}
	}
	if(newItem != NULL)
	{
		int id = m_PBClass->FindIndexByID(nID);
		CString strText = m_PBClass->GetAt(id).strClassName;
		CString strSQL = "";
		strSQL.Format(TEXT("Select count(phoneid) from sc700_phonebook where parentid =  %d"), m_PBClass->GetAt(id).nID);
		int countNum = m_phBookPC.GetColCount(strSQL);
		CString strNum;
		strNum.Format(TEXT("(%d)"), countNum);
		strText += strNum;
		m_ctrlLinkManTree.SetItemText(newItem, strText);
	}

}


void CLinkManDialog::OnButtonNewLinkman() 
{
	// TODO: Add your control notification handler code here
	if(m_currTreeItemData == LINKMAN_COUNT)
		return;
	if(m_currTreeItemData == LINKMAN_SIM)
	{
	/***********************************************************
		if(((CSunnyCatDlg *)AfxGetMainWnd())->m_isConnect)
		{			
			GMessageBox(LoadStringEx(IDS_PROMPT_ONLINE));
			return;
		}
	/***********************************************************/
		if(((CSunnyCatDlg *)AfxGetMainWnd())->m_bResetting)
		{
			GMessageBox(LoadStringEx(IDS_SEARCH_NET));
			return;
	}
		if(((CSunnyCatDlg *)AfxGetMainWnd())->m_InitAT.IsInitRunning())
		{					
			GMessageBox(LoadStringEx(IDS_INIT_RUNNING));
			return ;
		}
		CNewCardForSIM m_newCardSim;
		m_newCardSim.DoModal();
		return;
	}
	CNewCardDlg m_newCard;
	m_newCard.m_classID = m_currTreeItemData;
	int index = m_PBClass->FindIndexByID(m_currTreeItemData);	
	m_newCard.m_className = m_PBClass->GetAt(index).strClassName;
	if(m_newCard.DoModal() == IDOK)
	{
		return;
	}
	
}

void CLinkManDialog::ShowClassText(HTREEITEM treeItem, DWORD treeData)
{
//	int countNum = m_ctrlLinkmanList.GetItemCount();

	if(treeData == LINKMAN_COUNT)
	{
		treeData = 6;
	}
	if(treeData == LINKMAN_PC)
		return;

	int id = m_PBClass->FindIndexByID(treeData);
	CString strText = m_PBClass->GetAt(id).strClassName;
	m_phBookPC.ClearVector();
	m_phBookPC.GetRecordsByTypeName(m_PBClass->GetAt(id).nID);
	int countNum = m_phBookPC.Size();

	CString strNum;
	strNum.Format(TEXT("(%d)"), countNum);
	strText += strNum;
	m_ctrlLinkManTree.SetItemText(treeItem, strText);
}

void CLinkManDialog::OnButtonNewclassLinkman() 
{
	// TODO: Add your control notification handler code here
	CNewClassDlg m_newClass;
	if(m_newClass.DoModal() == IDOK)
	{
		SetInputFlag(TRUE);
		m_ctrlLinkManTree.SetRedraw(false);
		m_ctrlLinkManTree.DeleteAllItems();
		m_PBClass->ClearVector();		
		InitTree();
		m_ctrlLinkManTree.SetRedraw(true);
		SetInputFlag(FALSE);
	}
	
}

void CLinkManDialog::OnButtonDelLinkman() 
{
	// TODO: Add your control notification handler code here
	int sum = m_ctrlLinkmanList.GetItemCount();
	int times = 0;
	if(sum < 1)
		return;
	for(int i=0; i<sum; i++)
	{
		if (m_ctrlLinkmanList.GetItemState(i, LVIS_SELECTED))
		{
			times++;
			break;
		}
	}
	if(times < 1)
	{
		GMessageBox(LoadStringEx(IDS_LINKMAN_INPUTNOTICE));
		return;
	}
	OnLinkmanDel(); 
}

void CLinkManDialog::OnButtonClearLinkman() 
{
	// TODO: Add your control notification handler code here
	OnLinkmanSelall();

	if(GMessageBox(LoadStringEx(IDS_LINKMAN_ISCLEAR), NULL, MB_OKCANCEL|MB_ICONASTERISK) == IDOK)
	{
		int nCount = m_ctrlLinkmanList.GetItemCount();
		m_ctrlLinkmanList.SetRedraw(false);
	
		if(m_currTreeItemData != LINKMAN_COUNT && m_currTreeItemData != LINKMAN_SIM)
		{
			m_ctrlLinkmanList.DeleteAllItems_EX();
			m_phBookPC.ClearCurrClassCard(m_currTreeItemData);
		}

		if(m_currTreeItemData == LINKMAN_SIM)
		{
			int nCount = m_ctrlLinkmanList.GetItemCount();
			OnLinkmanSelall();
			for(int i = nCount-1; i >= 0; i--)
			{
			
				if (m_ctrlLinkmanList.GetItemState(i, LVIS_SELECTED))
				{
					int id = m_ctrlLinkmanList.GetItemData_EX(i);
					//	m_pbookEmbed.Delete(id);
					m_pbookEmbed.Remove(id);
					m_ctrlLinkmanList.DeleteItem_EX(i);
				}
			}
		}

		if(m_currTreeItemData == LINKMAN_COUNT)
		{
			
			int nCount = m_ctrlLinkmanList.GetItemCount();
			
			for(int i = 0; i < nCount; i++)
			{
//				if (((CSunnyCatDlg *)AfxGetMainWnd())->m_strLanguage == "-En" )
//				{
					if(m_ctrlLinkmanList.GetItemText(i, 0) == "PC")
					{
						DWORD id = m_ctrlLinkmanList.GetItemData_EX(i);					
						m_phBookPC.Delete(id);					
					}
					else if(m_ctrlLinkmanList.GetItemText(i, 0) == "SIM")
					{
						DWORD id = m_ctrlLinkmanList.GetItemData_EX(i);				
						m_pbookEmbed.Remove(id);
					}
//				}
#if 0				
				else if (((CSunnyCatDlg *)AfxGetMainWnd())->m_strLanguage == "-Ru" )
				{	
					if(m_ctrlLinkmanList.GetItemText(i, 0) == "Компьютер")
					{
						DWORD id = m_ctrlLinkmanList.GetItemData_EX(i);					
						m_phBookPC.Delete(id);					
					}
					else if(m_ctrlLinkmanList.GetItemText(i, 0) == "SIM")
					{
						DWORD id = m_ctrlLinkmanList.GetItemData_EX(i);				
						m_pbookEmbed.Remove(id);
					}
				}
#endif				
			}
			m_ctrlLinkmanList.DeleteAllItems_EX();
			
			int classNum = m_PBClass->Size();
			
			for(i = 0; i < classNum; i++)
			{
				int id = m_PBClass->GetAt(i).nID;
				if(id > 1)
				{
					SetTreeText(id);
				}
			}
			
			CString strText = LoadStringEx(IDS_LINKMAN_QUERY);		
			m_ctrlLinkManTree.SetItemText(m_count,strText);
			
			strText = LoadStringEx(IDS_LINKMAN_SIM);
			int strSimSum = m_pbookEmbed.Size();			
			CString str; 
			str.Format(TEXT("(%d)"), strSimSum);
			strText += str;
			m_ctrlLinkManTree.SetItemText(m_linkmanSmItem, strText);
			
		}
		m_ctrlLinkmanList.SetRedraw(true);
	}
	
	PostMessage(WM_LINKMAN_NOTIFY,0,0);
//	OnLinkmanDel(); 
}

void CLinkManDialog::OnSelchangedTreeLinkman(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_currTreeItem = pNMTreeView->itemNew.hItem;
	m_currTreeItemData = m_ctrlLinkManTree.GetItemData(pNMTreeView->itemNew.hItem);	
	m_nCtrlDest = CUR_CTRL_CLSTREE;
	TRACE(TEXT("classid: %d,\n"),m_currTreeItemData);
	OnClickTreeItem();
	if(m_currTreeItemData != LINKMAN_PC && m_currTreeItemData != LINKMAN_COUNT
		&& m_currTreeItemData != LINKMAN_SIM)
	{
		SetNewBtnState(false);
		int sum = m_ctrlLinkmanList.GetItemCount();
		int index = m_PBClass->FindIndexByID(m_currTreeItemData);
		CString classShow = m_PBClass->GetAt(index).strClassName;
		classShow += "(" + Util::intToString(sum) + ")";
		m_ctrlLinkManTree.SetItemText(m_currTreeItem, classShow);
	}
	if(m_currTreeItemData != LINKMAN_COUNT)
	{
		int sum = m_ctrlLinkmanList.GetItemCount();	
		CString classShow = "";
		classShow = LoadStringEx(IDS_LINKMAN_QUERYZERO);		
		m_ctrlLinkManTree.SetItemText(m_count, classShow);
	}
	if(m_currTreeItemData == LINKMAN_PC)
	{
		SetNewBtnState(true);
	}

	if(m_currTreeItemData == LINKMAN_SIM)
	{
		SetNewBtnState(false,true);
	}

	if(m_currTreeItemData == LINKMAN_COUNT)
	{
		SetNewBtnState(true);
		int sum = m_ctrlLinkmanList.GetItemCount();	
		CString classShow = "";
		classShow = LoadStringEx(IDS_LINKMAN_QUERY);
		classShow += "(" + Util::intToString(sum) + ")";
		m_ctrlLinkManTree.SetItemText(m_currTreeItem, classShow);
	}
	
//	if(m_currTreeItemData == LINKMAN_SIM)
//	{
		
	//	int sum = m_ctrlLinkmanList.GetItemCount();	
		int sum = m_pbookEmbed.Size();   //070625he
		if(!((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.isAbleCom)
		{
			sum = 0;
		}
		CString classShow = "";
		classShow = LoadStringEx(IDS_LINKMAN_SIM);
		classShow += "(" + Util::intToString(sum) + ")";
		/*************************************************************************************
		if(((CSunnyCatDlg *)AfxGetMainWnd())->m_isConnect)
		{
			m_ctrlLinkManTree.SetItemText(m_linkmanSmItem, LoadStringEx(IDS_LINKMAN_SIMZERO));
		}
		else
		/*************************************************************************************/
			m_ctrlLinkManTree.SetItemText(m_linkmanSmItem, classShow);
//	}

	if(m_ctrlLinkmanList.GetItemCount() > 0)
	{
		m_ctrlLinkmanList.SelectItem(0, true);
		m_ctrlLinkmanList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		SetButtonState(TRUE);
		SetEditBtnState(false);
		SetSendBtnState(false);
		SetDelBtnState(false);
	}
	else
	{
		SetButtonState(FALSE);
		SetEditBtnState(true);
		SetSendBtnState(true);
		SetDelBtnState(true);
	}
	*pResult = 0;
}

void CLinkManDialog::OnRclickTreeLinkman(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
		m_nCtrlDest = CUR_CTRL_CLSTREE;		

//		if(m_currTreeItemData == LINKMAN_COUNT)
//			return;
		CPoint pt;
		GetCursorPos(&pt);
		m_ctrlLinkManTree.ScreenToClient(&pt);
		
		HTREEITEM hHitItem = m_ctrlLinkManTree.HitTest(pt);
		if(hHitItem == NULL)
			return;
		DWORD selItem = m_ctrlLinkManTree.GetItemData(hHitItem);		
		
		
		if(hHitItem != NULL)
		{
			m_ctrlLinkManTree.SelectItem(hHitItem);
			GetCursorPos(&pt);
			CMenu CardMenu,*pPopMenu = NULL;
			CardMenu.LoadMenu(IDR_MENU_LINKMAN);			
			pPopMenu = CardMenu.GetSubMenu(1);
			if(pPopMenu != NULL)
			{
				pPopMenu->ModifyMenu(IDR_LINKMAN_NEWCLASS, MF_BYCOMMAND|MF_STRING, IDR_LINKMAN_NEWCLASS , LoadStringEx(IDS_LINKMAN_NEWCLASS));
				pPopMenu->ModifyMenu(IDR_LINKMAN_NEWCARD, MF_BYCOMMAND|MF_STRING, IDR_LINKMAN_NEWCARD , LoadStringEx(IDS_NEWCARD));
				pPopMenu->ModifyMenu(IDR_LINKMAN_SSMS, MF_BYCOMMAND|MF_STRING, IDR_LINKMAN_SSMS , LoadStringEx(IDS_LINKMAN_SSMS));
				pPopMenu->ModifyMenu(IDR_LINKMAN_EDITCLASS, MF_BYCOMMAND|MF_STRING, IDR_LINKMAN_EDITCLASS , LoadStringEx(IDS_LINKMAN_EDITCLASS));
				pPopMenu->ModifyMenu(IDR_LINKMAN_DELCLASS, MF_BYCOMMAND|MF_STRING, IDR_LINKMAN_DELCLASS , LoadStringEx(IDS_LINKMAN_DELCLASS));
				pPopMenu->ModifyMenu(IDR_LINKMAN_CLEARCARD, MF_BYCOMMAND|MF_STRING, IDR_LINKMAN_CLEARCARD , LoadStringEx(IDS_LINKMAN_CLEARCARD));

			}
			if(m_currTreeItemData == LINKMAN_PC)
			{
				pPopMenu->EnableMenuItem(1, MF_BYPOSITION|MF_GRAYED);
				pPopMenu->EnableMenuItem(2, MF_BYPOSITION|MF_GRAYED);
				pPopMenu->EnableMenuItem(3, MF_BYPOSITION|MF_GRAYED);
				pPopMenu->EnableMenuItem(5, MF_BYPOSITION|MF_GRAYED);
				pPopMenu->EnableMenuItem(6, MF_BYPOSITION|MF_GRAYED);
			}

			m_phBookPC.ClearVector();
			m_phBookPC.GetRecordsByTypeName(m_currTreeItemData);			
			int size = m_phBookPC.Size();
			if(size < 1)
			{
				pPopMenu->EnableMenuItem(2, MF_BYPOSITION|MF_GRAYED);
				pPopMenu->EnableMenuItem(6, MF_BYPOSITION|MF_GRAYED);
			}

			if(m_currTreeItemData < 7)
			{
				pPopMenu->EnableMenuItem(5, MF_BYPOSITION|MF_GRAYED);
			}
		//	if(m_currTreeItemData == 6)
		//	{
				//pPopMenu->EnableMenuItem(3, MF_BYPOSITION|MF_GRAYED);
		//	}
			if(m_currTreeItemData == LINKMAN_SIM) 
			{
				pPopMenu->EnableMenuItem(0, MF_BYPOSITION|MF_GRAYED);
				if(!m_pbookEmbed.IsLoadSim())
					pPopMenu->EnableMenuItem(1,MF_BYPOSITION|MF_GRAYED);
				pPopMenu->EnableMenuItem(4, MF_BYPOSITION|MF_GRAYED);
				pPopMenu->EnableMenuItem(5, MF_BYPOSITION|MF_GRAYED);
				pPopMenu->EnableMenuItem(3, MF_BYPOSITION|MF_GRAYED);
			}
	
			if(m_currTreeItemData == LINKMAN_COUNT)
			{
				pPopMenu->EnableMenuItem(0, MF_BYPOSITION|MF_GRAYED);
				pPopMenu->EnableMenuItem(1, MF_BYPOSITION|MF_GRAYED);
				pPopMenu->EnableMenuItem(3, MF_BYPOSITION|MF_GRAYED);
				pPopMenu->EnableMenuItem(5, MF_BYPOSITION|MF_GRAYED);
			}
			pPopMenu->TrackPopupMenu(TPM_LEFTBUTTON,pt.x,pt.y,this) ;
		}
}

//===========================================================
//						Tree右键功能菜单
//===========================================================


void CLinkManDialog::OnLinkmanSsms() 
{
	// TODO: Add your command handler code here
	CString strPhone = "";
	int iSize = 0;
	if(m_currTreeItemData != LINKMAN_SIM)
	{
		iSize = m_phBookPC.Size();
//	m_phBookPC.GetRecordsByTypeName(m_currTreeItemData);
	
	strPhone.Empty();
	//std::vector<CString> numbers;
	iSize = m_phBookPC.Size();
	for(int i = 0; i < iSize; i++)
	{
		if(m_phBookPC.GetAt(i).strMobilePhone == "")
			continue;
		strPhone += m_phBookPC.GetAt(i).strMobilePhone;
		strPhone += ";";
			
	}
	}
	else
	{
		iSize = m_pbookEmbed.Size();
		for(int i = 0; i < iSize; i++)
		{
			if(m_pbookEmbed.GetAt(i).strMobilePhone == "")
				continue;
			strPhone += m_pbookEmbed.GetAt(i).strMobilePhone;
			strPhone += ";";
	
		}
	}
	if(iSize < 1)
		return;
	CNewSmsDialog newSmsDlg(this,strPhone,iSize );
	newSmsDlg.DoModal();
}

void CLinkManDialog::OnLinkmanNewclass() 
{
	// TODO: Add your command handler code here
	OnButtonNewclassLinkman();
	return;
}

void CLinkManDialog::OnLinkmanNewcard() 
{
	// TODO: Add your command handler code here
	if(m_currTreeItemData == LINKMAN_PC)
		return;
	if(m_currTreeItemData == LINKMAN_COUNT)
		return;
	OnButtonNewLinkman();
}

void CLinkManDialog::OnLinkmanEditclass() 
{
	// TODO: Add your command handler code here	
	TPBClass pbClass;
	isNewClass = false;
	int index = m_PBClass->FindIndexByID(m_currTreeItemData);
	pbClass = m_PBClass->GetAt(index);
	//UpdateData(false);
	CNewClassDlg newClassDlg;
	int id = 0;
	id = pbClass.nID;
//	newClassDlg.m_strClassName = pbClass.strClassName;
//	newClassDlg.m_strClassVoicePath = pbClass.strVoicePath;
	
	newClassDlg.m_strClassName = pbClass.strClassName;
	newClassDlg.m_strClassVoicePath = pbClass.strVoicePath;
	if(newClassDlg.DoModal() == IDOK)
	{

		pbClass.strClassName = newClassDlg.m_strClassName;
		
		m_phBookPC.ClearVector();
		m_phBookPC.GetRecordsByTypeName(pbClass.nID);
		int size = 0;
		size = m_phBookPC.Size();
		CString className = "";
		className += pbClass.strClassName + "(" + Util::intToString(size) + ")";
		
		m_ctrlLinkManTree.SetItemText(m_currTreeItem, className);
		pbClass.strVoicePath = newClassDlg.m_strClassVoicePath;
		pbClass.nID = id;
		pbClass.nParentID = 0;

	//	m_PBClass->Delete(m_currTreeItemData);
	//	m_PBClass->Add(pbClass);

		m_PBClass->Modify(pbClass);

	}

}

void CLinkManDialog::OnLinkmanDelclass() 
{
	// TODO: Add your command handler code here
	CString info;
	info = LoadStringEx(IDS_DEL_INFO);

	if(GMessageBox(info, NULL, MB_OKCANCEL|MB_ICONASTERISK) != IDOK)
	 return;

	m_PBClass->Delete(m_currTreeItemData);
	if(m_currTreeItemData != LINKMAN_SIM && m_currTreeItemData != LINKMAN_COUNT)
	{
		m_phBookPC.ClearCurrClassCard(m_currTreeItemData);
	}
	
	m_ctrlLinkManTree.DeleteItem(m_currTreeItem);
	
}

void CLinkManDialog::OnLinkmanClearcard() 
{
	// TODO: Add your command handler code here
	OnLinkmanSelall(); 
	OnLinkmanDel(); 
}

//==================================================================


//====================================================================
//						Lish右键功能菜单
//====================================================================
//短信
void CLinkManDialog::OnLinkmanSms() 
{
	// TODO: Add your command handler code here
	int nCount = m_ctrlLinkmanList.GetItemCount() ;
	CString strPhone;
	int nNum = 0; 

	strPhone.Empty();
	
	for(int i = 0; i < nCount; i++)
	{
	
		if (m_ctrlLinkmanList.GetItemState(i, LVIS_SELECTED))
		{
			strPhone += m_ctrlLinkmanList.GetItemText(i,CALLRECORD_PHONE);
			strPhone += ';';

			nNum++;
		}
	}	
	
	CNewSmsDialog smsDlg(&((CSunnyCatDlg*)AfxGetMainWnd())->m_smsDlg,strPhone,nNum);
	smsDlg.DoModal();
}
//全选
void CLinkManDialog::OnLinkmanSelall() 
{
	// TODO: Add your command handler code here
	static DWORD dwStaFlag = LVIS_SELECTED|LVIS_FOCUSED|LVIS_ACTIVATING ;
	int nCount = m_ctrlLinkmanList.GetItemCount() ;
	int selCount = 0;
	for(int i = 0; i < nCount; i++)
	{
		m_ctrlLinkmanList.SetItemState(i,dwStaFlag,dwStaFlag);
		selCount++;
	}

	if(selCount > 1)
	{
		SetEditBtnState(true);
		SetSendBtnState(true);		
	}
}
//反选
void CLinkManDialog::OnLinkmanSelother() 
{
	// TODO: Add your command handler code here
	static DWORD dwStaFlag = LVIS_SELECTED|LVIS_FOCUSED|LVIS_ACTIVATING;
	int nCount = m_ctrlLinkmanList.GetItemCount();
	int selCount = 0;
	for(int i = 0; i < nCount; i++)
	{
		if (m_ctrlLinkmanList.GetItemState(i, LVIS_SELECTED))
		{
			m_ctrlLinkmanList.SetItemState(i, 0, LVIS_SELECTED);
		}
		else
		{
		m_ctrlLinkmanList.SetItemState(i,dwStaFlag,dwStaFlag);
			selCount++;
		}
	}

	if(selCount > 1)
	{
		SetEditBtnState(true);
		SetSendBtnState(true);		
	}
}

void CLinkManDialog::OnLinkmanDial() 
{
	// TODO: Add your command handler code here
	((CSunnyCatDlg *)AfxGetMainWnd())->ShowPage(2);
	int nCount = m_ctrlLinkmanList.GetItemCount();
	for(int i = nCount-1; i >= 0; i--)
	{
		
		if (m_ctrlLinkmanList.GetItemState(i, LVIS_SELECTED))
		{	
			CString number = m_ctrlLinkmanList.GetItemText(i, 2);
			pCallDialog->Call(number);
			return;
		}
	}
	

}

void CLinkManDialog::OnLinkmanDel() 
{
	// TODO: Add your command handler code here
	//判断所选联系人的存在位置（ＰＣ、ＳＩＭ）
	//先从ＰＣ和ＳＩＭ卡中删除，如成功再从内存中删除
	//从内存中删除
	CString info;
	info = LoadStringEx(IDS_DEL_INFO);

	((CSunnyCatDlg *)AfxGetMainWnd())->KillTimer(TIMER_CHECK_SIGNAL);

	if(GMessageBox(info, NULL, MB_OKCANCEL|MB_ICONASTERISK) == IDOK)
	{
	
	int nCount = m_ctrlLinkmanList.GetItemCount();
	//进度显示
	CProgressWnd wndProgress(this, LoadStringEx(IDS_WAITFORDELETE));
	
	wndProgress.GoModal();
	wndProgress.SetRange(0,nCount-1);
	wndProgress.SetText(LoadStringEx(IDS_WAITFORDELETE));
	CString strText;

	for(int i = nCount-1; i >= 0; i--)
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

		m_ctrlLinkmanList.SetRedraw(false);
		if (m_ctrlLinkmanList.GetItemState(i, LVIS_SELECTED))
		{
			CString strText = m_ctrlLinkmanList.GetItemText(i, 0);
			if(m_ctrlLinkManTree.GetItemData(m_currTreeItem) != LINKMAN_COUNT)
			{
				if(m_ctrlLinkManTree.GetItemData(m_currTreeItem) != LINKMAN_SIM)
				{
					int id = m_ctrlLinkmanList.GetItemData_EX(i);
					m_phBookPC.Delete(id);
					m_ctrlLinkmanList.DeleteItem_EX(i);
				}
				else
				{
					//==================================================2007.6.18he
					/*********************************************************
					if(((CSunnyCatDlg *)AfxGetMainWnd())->m_isConnect)
					{
						m_ctrlLinkmanList.SetRedraw(true);
						GMessageBox(LoadStringEx(IDS_PROMPT_ONLINE));
						return;
					}
					/*********************************************************/
					if(((CSunnyCatDlg *)AfxGetMainWnd())->m_bResetting)
					{
						m_ctrlLinkmanList.SetRedraw(true);
						GMessageBox(LoadStringEx(IDS_SEARCH_NET));
						return;
	}
					if(((CSunnyCatDlg *)AfxGetMainWnd())->m_InitAT.IsInitRunning())
					{					
						m_ctrlLinkmanList.SetRedraw(true);
						GMessageBox(LoadStringEx(IDS_INIT_RUNNING));
						return ;
					}
					//===================================================
					int id = m_ctrlLinkmanList.GetItemData_EX(i);
				//	m_pbookEmbed.Delete(id);
					m_pbookEmbed.Remove(id);
					m_ctrlLinkmanList.DeleteItem_EX(i);
				}
			}
			else
			{
//				if (((CSunnyCatDlg *)AfxGetMainWnd())->m_strLanguage == "-En" )
//				{	
					if(strText == "PC")
					{
						int id = m_ctrlLinkmanList.GetItemData_EX(i);
						DWORD index = m_phBookPC.GetAt(i).nParentID;
						m_phBookPC.Delete(id);					
						SetTreeText(index);
						m_ctrlLinkmanList.DeleteItem_EX(i);
					}
					else if(strText == "SIM")
					{
						//==================================================2007.6.18he
						/*********************************************************
						if(((CSunnyCatDlg *)AfxGetMainWnd())->m_isConnect)
						{
							m_ctrlLinkmanList.SetRedraw(true);
							GMessageBox(LoadStringEx(IDS_PROMPT_ONLINE));
							continue;
						}
						/*********************************************************/
						if(((CSunnyCatDlg *)AfxGetMainWnd())->m_bResetting)
						{
							m_ctrlLinkmanList.SetRedraw(true);
							GMessageBox(LoadStringEx(IDS_SEARCH_NET));
							continue;
						}
						if(((CSunnyCatDlg *)AfxGetMainWnd())->m_InitAT.IsInitRunning())
						{
							m_ctrlLinkmanList.SetRedraw(true);
							GMessageBox(LoadStringEx(IDS_INIT_RUNNING));
							continue;
						}
						//===================================================
						
						int id = m_ctrlLinkmanList.GetItemData_EX(i);
						m_pbookEmbed.Remove(id);					
						m_ctrlLinkmanList.DeleteItem_EX(i);
						CString	strText(LoadStringEx(IDS_LINKMAN_SIM));
						int strSimSum = m_pbookEmbed.Size();			
						CString str;  
						str.Format(TEXT("(%d)"), strSimSum);
						strText += str;
						m_ctrlLinkManTree.SetItemText(m_linkmanSmItem, strText);
					}
//				}
#if 0
				else if (((CSunnyCatDlg *)AfxGetMainWnd())->m_strLanguage == "-Ru" )
				{
					if(strText == "Компьютер")
					{
						int id = m_ctrlLinkmanList.GetItemData_EX(i);
						DWORD index = m_phBookPC.GetAt(i).nParentID;
						m_phBookPC.Delete(id);					
						SetTreeText(index);
						m_ctrlLinkmanList.DeleteItem_EX(i);
					}
					else if(strText == "SIM")
					{
						//==================================================2007.6.18he
						/*********************************************************
						if(((CSunnyCatDlg *)AfxGetMainWnd())->m_isConnect)
						{
							m_ctrlLinkmanList.SetRedraw(true);
							GMessageBox(LoadStringEx(IDS_PROMPT_ONLINE));
							continue;
						}
						/*********************************************************/
						if(((CSunnyCatDlg *)AfxGetMainWnd())->m_bResetting)
						{
							m_ctrlLinkmanList.SetRedraw(true);
							GMessageBox(LoadStringEx(IDS_SEARCH_NET));
							continue;
						}
						if(((CSunnyCatDlg *)AfxGetMainWnd())->m_InitAT.IsInitRunning())
						{
							m_ctrlLinkmanList.SetRedraw(true);
							GMessageBox(LoadStringEx(IDS_INIT_RUNNING));
							continue;
						}
						//===================================================
						
						int id = m_ctrlLinkmanList.GetItemData_EX(i);
						m_pbookEmbed.Remove(id);					
						m_ctrlLinkmanList.DeleteItem_EX(i);
						CString	strText(LoadStringEx(IDS_LINKMAN_SIM));
						int strSimSum = m_pbookEmbed.Size();			
						CString str;  
						str.Format(TEXT("(%d)"), strSimSum);
						strText += str;
						m_ctrlLinkManTree.SetItemText(m_linkmanSmItem, strText);
					}
				}
#endif				
			}
		}
		m_ctrlLinkmanList.SetRedraw(true);
	}
	wndProgress.m_bCancelled = true;
	wndProgress.Hide();
	}

	((CSunnyCatDlg *)AfxGetMainWnd())->SetTimer(TIMER_CHECK_SIGNAL, 8000, 0);
	SendMessage(WM_LINKMAN_NOTIFY,0,0);
	int countNum = m_ctrlLinkmanList.GetItemCount();
	if(countNum <=0)
	{
		SetButtonState(FALSE);
		SetEditBtnState(true);
		SetSendBtnState(true);
		SetDelBtnState(true);
	}
}

//编辑名片
void CLinkManDialog::OnLinkmanEditcard() 
{
	// TODO: Add your command handler code here
	
	//编辑ＳＩＭ名片
	TPhoneBookItemEx items;
	TPhoneBookItem simItems;
	if(m_currTreeItemData == LINKMAN_COUNT)
	{
		if(m_currTreeItemData == LINKMAN_COUNT)
		{
			CString str = m_ctrlLinkmanList.GetItemText(currSelListItem, 0);
			if(str == "SIM")
			{
				//==================================================2007.6.18he
				/***********************************************************
				if(((CSunnyCatDlg *)AfxGetMainWnd())->m_isConnect)
				{				
					GMessageBox(LoadStringEx(IDS_PROMPT_ONLINE));
					return;
				}
				/***********************************************************/
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
				//===================================================
				CNewCardForSIM newCardDlg;
				int id = m_ctrlLinkmanList.GetItemData_EX(currSelListItem);
				int index = m_pbookEmbed.FindIndexByID(id);
				simItems = m_pbookEmbed.GetAt(index);
				
				newCardDlg.m_strNameSIM = simItems.strName;
				newCardDlg.m_strNumSIM = simItems.strMobilePhone;

				if(newCardDlg.DoModal() == IDOK)
				{
					simItems.strName = newCardDlg.m_strNameSIM;
					simItems.strMobilePhone = newCardDlg.m_strNumSIM;			
					m_pbookEmbed.Modify(simItems, id);
					((CSunnyCatDlg *)AfxGetMainWnd())->m_linkManDlg.PostMessage(WM_LINKMAN_NOTIFY, 0 ,1);
					
				}
				m_ctrlLinkmanList.DeleteItem_EX(currSelListItem);
				CString strNum ;
				CString strText(LoadStringEx(IDS_LINKMAN_QUERY));
				int countNum = m_ctrlLinkmanList.GetItemCount();
				strNum.Format(TEXT("(%d)"), countNum);
				strText += strNum;
				m_ctrlLinkManTree.SetItemText(m_count, strText);
				return;
				
			}
			else
			{
				CNewCardDlg newCardDlg;		
			
				int id = m_phBookPC.FindIndexByID(m_ctrlLinkmanList.GetItemData_EX(currSelListItem));
				//newCardDlg.m_className = m_ctrlLinkManTree.GetItemText(m_currTreeItem);
				items = m_phBookPC.GetAt(id);
				int parentID = items.nParentID;
				newCardDlg.m_classID = items.nParentID;
				int index = m_PBClass->FindIndexByID(parentID);
				newCardDlg.m_className = m_PBClass->GetAt(index).strClassName;
				newCardDlg.m_strName = items.strName;
				newCardDlg.m_strMobilePhone = items.strMobilePhone;
				newCardDlg.m_strHomePhone = items.strHomePhone;
				newCardDlg.m_strFaxPhone = items.strFax;
				newCardDlg.m_strOfficePhone = items.strOfficePhone;
				newCardDlg.m_strHeadShip = items.strBusiness;
				newCardDlg.m_strCompany = items.strOffice;
				newCardDlg.m_strCountry = items.strCountry;
				newCardDlg.m_strEmail = items.strEmail;		
				if(newCardDlg.DoModal() == IDOK)
				{
					//UpdateData(true);
					items.nParentID = newCardDlg.m_classID;
					//int parentID = ((CSunnyCatDlg *)AfxGetMainWnd())->m_linkManDlg.m_currTreeItemData;	
					items.nParentID = parentID;
					items.strName = newCardDlg.m_strName;
					items.strHomePhone = newCardDlg.m_strHomePhone;
					items.strEmail = newCardDlg.m_strEmail;
					items.strFax = newCardDlg.m_strFaxPhone;	
					items.strMobilePhone = newCardDlg.m_strMobilePhone;	
					items.strOfficePhone = newCardDlg.m_strOfficePhone;
					items.strBusiness = newCardDlg.m_strHeadShip;
					items.strOffice = newCardDlg.m_strCompany;
					items.strCountry = newCardDlg.m_strCountry;	
					items.strTypeName = m_PBClass->FindNameByID(parentID);
					CPhonebookPC phBookPC;
					int repeat = REPEAT_OK;
					if(repeat != REPEAT_OK_ALL && repeat != REPEAT_NO_ALL)
					{
					
						if(phBookPC.IsExitRecord(items, false))
						{
							CRepeatDialog repeatDlg;
							repeatDlg.DoModal();
							repeat = repeatDlg.m_repeat;
						}
					}
					
					if(repeat == REPEAT_OK || repeat == REPEAT_OK_ALL)
					{
						bool bOk = phBookPC.Modify(items);
						if(!bOk)
						{
							GMessageBox(LoadStringEx(IDS_EDIT_CARD_FAIL));
						}
					}
					else if(repeat == REPEAT_NO_ALL || repeat == REPEAT_NO)
					{
					
					}

					//m_phBookPC.Modify(items);
					PostMessage(WM_LINKMAN_NOTIFY, 0 ,1);
					SetTreeText(newCardDlg.m_classID);	
				}
				m_ctrlLinkmanList.DeleteItem_EX(currSelListItem);
				return;
			}
			
		
	}

		return;
	}

	if(m_currTreeItemData == LINKMAN_SIM)
	{
		//==================================================2007.6.18he
		/**********************************************************
		if(((CSunnyCatDlg *)AfxGetMainWnd())->m_isConnect)
		{			
			GMessageBox(LoadStringEx(IDS_PROMPT_ONLINE));
			return;
		}
		/**********************************************************/
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
		//===================================================
		CNewCardForSIM newCardDlg;
		int id = m_ctrlLinkmanList.GetItemData_EX(currSelListItem);
		int index = m_pbookEmbed.FindIndexByID(id);
		simItems = m_pbookEmbed.GetAt(index);
		
		
		newCardDlg.m_strNameSIM = simItems.strName;
		newCardDlg.m_strNumSIM = simItems.strMobilePhone;

		if(newCardDlg.DoModal() == IDOK)
		{
			UpdateData(true);	
			simItems.strName = newCardDlg.m_strNameSIM;
			simItems.strMobilePhone = newCardDlg.m_strNumSIM;			
			bool bOk = m_pbookEmbed.Modify(simItems, id);
			if(!bOk)
			{
				GMessageBox(LoadStringEx(IDS_EDIT_CARD_FAIL));
			}
			PostMessage(WM_LINKMAN_NOTIFY, 0 ,1);
			
		}

		return;
	}
	//编辑ＰＣ名片
	else
	{
		CNewCardDlg newCardDlg;		
		int id = m_phBookPC.FindIndexByID(m_ctrlLinkmanList.GetItemData_EX(currSelListItem));
		items = m_phBookPC.GetAt(id);
		int index = m_PBClass->FindIndexByID(m_currTreeItemData);	
		newCardDlg.m_className = m_PBClass->GetAt(index).strClassName;		
		newCardDlg.m_classID = items.nParentID;
		newCardDlg.m_strName = items.strName;
		newCardDlg.m_strMobilePhone = items.strMobilePhone;
		newCardDlg.m_strHomePhone = items.strHomePhone;
		newCardDlg.m_strFaxPhone = items.strFax;
		newCardDlg.m_strOfficePhone = items.strOfficePhone;
		newCardDlg.m_strHeadShip = items.strBusiness;
		newCardDlg.m_strCompany = items.strOffice;
		newCardDlg.m_strCountry = items.strCountry;
		newCardDlg.m_strEmail = items.strEmail;		
	
		if(newCardDlg.DoModal() == IDOK)
		{
			
		//	UpdateData(true);
			//int parentID = ((CSunnyCatDlg *)AfxGetMainWnd())->m_linkManDlg.m_currTreeItemData;	
			items.nParentID = newCardDlg.m_classID;
			m_otherClassID = newCardDlg.m_classID;
			items.strName = newCardDlg.m_strName;
			items.strHomePhone = newCardDlg.m_strHomePhone;
			items.strEmail = newCardDlg.m_strEmail;
			items.strFax = newCardDlg.m_strFaxPhone;	
			items.strMobilePhone = newCardDlg.m_strMobilePhone;	
			items.strOfficePhone = newCardDlg.m_strOfficePhone;
			items.strBusiness = newCardDlg.m_strHeadShip;
			items.strOffice = newCardDlg.m_strCompany;
			items.strCountry = newCardDlg.m_strCountry;	
			items.strTypeName = m_PBClass->FindNameByID(items.nParentID);
			CPhonebookPC phBookPC;
			int repeat2 = REPEAT_OK;
			if(repeat2 != REPEAT_OK_ALL && repeat2 != REPEAT_NO_ALL)
			{
				
				if(phBookPC.IsExitRecord(items, false))
				{
					CRepeatDialog repeatDlg;
					repeatDlg.DoModal();
					repeat2 = repeatDlg.m_repeat;
				}
			}
			
			if(repeat2 == REPEAT_OK || repeat2 == REPEAT_OK_ALL)
			{
				bool bOk = phBookPC.Modify(items);
				if(!bOk)
				{
					GMessageBox(LoadStringEx(IDS_EDIT_CARD_FAIL));
					return;
				}
			}
			else if(repeat2 == REPEAT_NO_ALL || repeat2 == REPEAT_NO)
			{
			
			}

		//	m_phBookPC.Modify(items);
			Invalidate(FALSE);
			PostMessage(WM_LINKMAN_NOTIFY, 0 ,1);
			SetTreeText(newCardDlg.m_classID);
			Invalidate(TRUE);
			
		}
		return;
	}
	//currSelListItem
}

//====================================================================

//====================================================================

void CLinkManDialog::OnClickListLinkman(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	m_nCtrlDest = CUR_CTRL_CARDLIST;
	int sizeNum = m_ctrlLinkmanList.GetItemCount();
	int selNum = 0;
	if(sizeNum < 1)
		return;
	for(int i = 0; i < sizeNum; i++)
	{
		if(m_ctrlLinkmanList.GetItemState(i, LVIS_SELECTED))
		{
			selNum++;
		}
	}
	if(selNum < 1)
		return;
	SetButtonState(TRUE);
	*pResult = 0;
}

void CLinkManDialog::OnDblclkListLinkman(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	m_nCtrlDest = CUR_CTRL_CARDLIST;
	int sizeNum = m_ctrlLinkmanList.GetItemCount();
	int selNum = 0;
	if(sizeNum < 1)
		return;
	for(int i = 0; i < sizeNum; i++)
	{
		if(m_ctrlLinkmanList.GetItemState(i, LVIS_SELECTED))
		{
			selNum++;
		}
	}
	if(selNum < 1)
		return;
	SetButtonState(TRUE);
		//编辑ＳＩＭ名片
	TPhoneBookItemEx items;
	TPhoneBookItem simItems;
	if(m_currTreeItemData == LINKMAN_COUNT)
	{
		CString str = m_ctrlLinkmanList.GetItemText(currSelListItem, 0);
		if(str == "SIM")
		{
			//==================================================2007.6.18he
			/*********************************************************
			if(((CSunnyCatDlg *)AfxGetMainWnd())->m_isConnect)
			{				
				GMessageBox(LoadStringEx(IDS_PROMPT_ONLINE));
				return;
			}
			/*********************************************************/
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
			//===================================================
			CNewCardForSIM newCardDlg;
			int id = m_ctrlLinkmanList.GetItemData_EX(currSelListItem);
			int index = m_pbookEmbed.FindIndexByID(id);
			simItems = m_pbookEmbed.GetAt(index);
			
			newCardDlg.m_strNameSIM = simItems.strName;
			newCardDlg.m_strNumSIM = simItems.strMobilePhone;

			if(newCardDlg.DoModal() == IDOK)
			{
				simItems.strName = newCardDlg.m_strNameSIM;
				simItems.strMobilePhone = newCardDlg.m_strNumSIM;			
				m_pbookEmbed.Modify(simItems, id);
				((CSunnyCatDlg *)AfxGetMainWnd())->m_linkManDlg.PostMessage(WM_LINKMAN_NOTIFY, 0 ,1);
				m_ctrlLinkmanList.DeleteItem_EX(currSelListItem);
			}
				
			CString strNum ;
			CString strText(LoadStringEx(IDS_LINKMAN_QUERY));
			int countNum = m_ctrlLinkmanList.GetItemCount();
			strNum.Format(TEXT("(%d)"), countNum);
			strText += strNum;
			m_ctrlLinkManTree.SetItemText(m_count, strText);
			return;
			
		}
		else
		{
			CNewCardDlg newCardDlg;		
		
			int id = m_phBookPC.FindIndexByID(m_ctrlLinkmanList.GetItemData_EX(currSelListItem));
			//newCardDlg.m_className = m_ctrlLinkManTree.GetItemText(m_currTreeItem);
			items = m_phBookPC.GetAt(id);
			int parentID = items.nParentID;
			newCardDlg.m_classID = items.nParentID;
			int index = m_PBClass->FindIndexByID(parentID);
			newCardDlg.m_className = m_PBClass->GetAt(index).strClassName;
			newCardDlg.m_strName = items.strName;
			newCardDlg.m_strMobilePhone = items.strMobilePhone;
			newCardDlg.m_strHomePhone = items.strHomePhone;
			newCardDlg.m_strFaxPhone = items.strFax;
			newCardDlg.m_strOfficePhone = items.strOfficePhone;
			newCardDlg.m_strHeadShip = items.strBusiness;
			newCardDlg.m_strCompany = items.strOffice;
			newCardDlg.m_strCountry = items.strCountry;
			newCardDlg.m_strEmail = items.strEmail;		
			if(newCardDlg.DoModal() == IDOK)
			{
				//UpdateData(true);
				items.nParentID = newCardDlg.m_classID;
				//int parentID = ((CSunnyCatDlg *)AfxGetMainWnd())->m_linkManDlg.m_currTreeItemData;	
				//items.nParentID = parentID;
				items.strName = newCardDlg.m_strName;
				items.strHomePhone = newCardDlg.m_strHomePhone;
				items.strEmail = newCardDlg.m_strEmail;
				items.strFax = newCardDlg.m_strFaxPhone;	
				items.strMobilePhone = newCardDlg.m_strMobilePhone;	
				items.strOfficePhone = newCardDlg.m_strOfficePhone;
				items.strBusiness = newCardDlg.m_strHeadShip;
				items.strOffice = newCardDlg.m_strCompany;
				items.strCountry = newCardDlg.m_strCountry;	
				items.strTypeName = m_PBClass->FindNameByID(items.nParentID);

				CPhonebookPC phBookPC;
				int iRepeat = REPEAT_OK;
				if(iRepeat != REPEAT_OK_ALL && iRepeat != REPEAT_NO_ALL)
				{
					
					if(phBookPC.IsExitRecord(items, false))
					{
						CRepeatDialog repeatDlg;
						repeatDlg.DoModal();
						iRepeat = repeatDlg.m_repeat;
					}
				}
				
				if(iRepeat == REPEAT_OK || iRepeat == REPEAT_OK_ALL)
				{
					bool bOk = phBookPC.Modify(items);
					if(!bOk)
					{
						GMessageBox(LoadStringEx(IDS_EDIT_CARD_FAIL));
						return;
					}
				}
				else if(iRepeat == REPEAT_NO_ALL || iRepeat == REPEAT_NO)
				{
				
				}

				PostMessage(WM_LINKMAN_NOTIFY, 0 ,1);
				SetTreeText(parentID);	
				SetTreeText(newCardDlg.m_classID);	
				m_ctrlLinkmanList.DeleteItem_EX(currSelListItem);
			}
				
			return;
		}
		
		
	}

	OnLinkmanEditcard();
	*pResult = 0;
}

void CLinkManDialog::OnItemchangedListLinkman(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;
	// TODO: Add your control notification handler code here
	currSelListItem = phdn->iItem;

	m_nCtrlDest = CUR_CTRL_CARDLIST;

	if(m_ctrlLinkmanList.GetItemCount() > 0)
	{		
		SetEditBtnState(false);
		SetSendBtnState(false);
		SetDelBtnState(false);
	}
	else
	{	
		SetEditBtnState(true);
		SetSendBtnState(true);
		SetDelBtnState(true);
	}

	int selCount = 0;
	for(int i = 0; i < m_ctrlLinkmanList.GetItemCount(); i++)
	{
		if(m_ctrlLinkmanList.GetItemState(i, LVIS_SELECTED))	
		{
			selCount++;
			if(selCount > 1)
				break;
		}
	}

	if(selCount > 1)
	{
		SetEditBtnState(true);
		
	}
	else
	{
		SetEditBtnState(false);
	}


	*pResult = 0;
}


void CLinkManDialog::ShowPBForPC(DWORD currItemData)
{
	m_phBookPC.ClearVector();
	m_phBookPC.GetRecordsByTypeName(currItemData);
	
	int size = m_phBookPC.Size();
	m_ctrlLinkmanList.SetRedraw(false);
	m_ctrlLinkmanList.DeleteAllItems_EX();
	
	int iMag = 0;
	switch(currItemData)
	{
		case 2:
			iMag = 6;
			break;
		case 3:
			iMag = 7;
			break;
		case 5:
			iMag = 8;
			break;
		case 6:
			iMag = 9;
			break;
		default:
			iMag = 10;
			break;
	}

	TPhoneBookItemEx phoneItem;
	
	for(int i = 0; i < size; i++)
	{
		phoneItem = m_phBookPC.GetAt(i);
		m_ctrlLinkmanList.AddItem(iMag,TEXT(""), phoneItem.strName, phoneItem.strMobilePhone, phoneItem.strBusiness,phoneItem.strOffice);		
		m_ctrlLinkmanList.SetItemData_EX(i, phoneItem.nID);
	}
	m_ctrlLinkmanList.SetRedraw(true);
}

void CLinkManDialog::ShowPBForSIM()
{
	int iSize;
//	
	m_pbookEmbed.Load();
	iSize = m_pbookEmbed.Size();
	m_ctrlLinkmanList.SetRedraw(false);
	m_ctrlLinkmanList.DeleteAllItems_EX();
	TPhoneBookItemEmbed *phoneItem;
	
	for(int i = 0; i < iSize; i++)
	{
		phoneItem = (TPhoneBookItemEmbed *)&m_pbookEmbed.GetAt(i);
		m_ctrlLinkmanList.AddItem(5,TEXT(""), phoneItem->strName, phoneItem->strMobilePhone, "","");
		m_ctrlLinkmanList.SetItemData_EX(i, phoneItem->ID);
	}
	m_ctrlLinkmanList.SetRedraw(true);
}

void CLinkManDialog::OnButtonQueryLinkman() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	m_ctrlLinkManTree.SelectItem(m_count) ;
	m_ctrlLinkManTree.SelectSetFirstVisible(m_count) ;
	m_ctrlLinkmanList.SetRedraw(false);
	m_phBookPC.ClearVector();
	m_ctrlLinkmanList.DeleteAllItems_EX();
	SearchInPB(m_strQuery);
	SearchInPBSIM(m_pbookEmbed);
	int countNum = m_ctrlLinkmanList.GetItemCount();
	CString strText = LoadStringEx(IDS_LINKMAN_QUERY);
	CString strNum;
	strNum.Format(TEXT("(%d)"), countNum);
	strText += strNum;
	m_ctrlLinkManTree.SetItemText(m_count, strText);
	m_ctrlLinkmanList.SetRedraw(true);
	if(m_ctrlLinkmanList.GetItemCount() > 0)
	{
		m_ctrlLinkmanList.SelectItem(0, true);
		m_ctrlLinkmanList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		SetButtonState(TRUE);
		SetEditBtnState(false);
		SetSendBtnState(false);
		SetDelBtnState(false);
	}
	else
	{
		SetButtonState(FALSE);
		SetEditBtnState(true);
		SetSendBtnState(true);
		SetDelBtnState(true);
	}
//	SetNewBtnState(true,false);
//	UpdateData(true);
}

void CLinkManDialog::SelectCountItem()
{
	m_ctrlLinkManTree.SelectItem(m_undivItem) ;
	m_ctrlLinkManTree.SelectSetFirstVisible(m_undivItem) ;
	return;
}
void CLinkManDialog::SearchInPB(CString queryStr)
{
	int i=0;
	int size = 0;	
	queryStr.TrimLeft();
	queryStr.TrimRight();
	TPhoneBookItemEx pbItem;
	if(queryStr == "")
	{
		//AfxMessageBox("请输入查找内容！");
		GMessageBox(LoadStringEx(IDS_LINKMAN_QUERYNOTICE), LoadStringEx(IDS_LINKMAN_NOTICE),MB_ICONINFORMATION) ;
		return;
	}
	if(m_phBookPC.GetRecordsByNumber(queryStr))
	{
		size = m_phBookPC.Size();
		for(i=0; i<size; i++)
		{
			pbItem = m_phBookPC.GetAt(i);
			int nCount;
			//int nCount = m_ctrlLinkmanList.GetItemCount();
//			if (((CSunnyCatDlg *)AfxGetMainWnd())->m_strLanguage == "-En" )
//			{
				nCount = m_ctrlLinkmanList.AddItem(21,TEXT("PC"),pbItem.strName, pbItem.strMobilePhone, pbItem.strBusiness,pbItem.strOffice);
//			}
//			else if (((CSunnyCatDlg *)AfxGetMainWnd())->m_strLanguage == "-Ru" )
//			{
//				nCount = m_ctrlLinkmanList.AddItem(21,TEXT("Компьютер"),pbItem.strName, pbItem.strMobilePhone, pbItem.strBusiness,pbItem.strOffice);
//			}
			m_ctrlLinkmanList.SetItemData_EX(nCount, pbItem.nID);
		}
		
	}	

	return;
}


//

void CLinkManDialog::SearchInPBSIM(CPhonebookEmbed& pb)
{
	CString strQuery;
	UpdateData(true);
	strQuery = m_strQuery;
	strQuery.MakeLower();//姓名忽略大小写
	
	//去掉待搜索字符串两端的空格
	strQuery.TrimLeft();
	strQuery.TrimRight();
	if(strQuery == "")
	{
		//AfxMessageBox("请输入查找内容！");
		return;
	}
	
	for(size_t i=0; i<pb.Size(); ++i)
	{
		const TPhoneBookItemEmbed* item = (TPhoneBookItemEmbed *)&pb.GetAt(i);
		CString number = item->strMobilePhone;
		
		CString name = item->strName;
		name.MakeLower(); //姓名忽略大小写
		
		
		//匹配姓名
		if( name.Find(strQuery) != -1 )
		{	
			//AddFindItem(NUM_MOBILE, pb, i, item.name, item.mobile);
		//	int itemID = m_ctrlLinkmanList.GetItemCount();
			int nCount = m_ctrlLinkmanList.AddItem(21,TEXT("SIM"),item->strName, item->strMobilePhone,"","");				
			m_ctrlLinkmanList.SetItemData_EX(nCount, item->ID);					
		}		
		else if(number.Find(strQuery) != -1)//匹配号码
		{
			//AddFindItem(NUM_MOBILE, pb, i, item.name, item.mobile);
		//	int itemID = m_ctrlLinkmanList.GetItemCount();
		int nCount = m_ctrlLinkmanList.AddItem(21,TEXT("SIM"),item->strName, item->strMobilePhone,"","");	
			m_ctrlLinkmanList.SetItemData_EX(nCount, item->ID);
		}							
	}	
}


//


//鼠标移动
void CLinkManDialog::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	HTREEITEM hti;
	UINT  flags;
	
	
	if(m_bLDragging)
	{
		CRect rcTemp;
		CRect rc;
		m_ctrlLinkManTree.GetWindowRect(&rc);
		rcTemp.left = rc.left+145;
		rcTemp.top = rc.top;
		rcTemp.bottom = rc.bottom;
		m_ctrlLinkmanList.GetWindowRect(&rc);
		rcTemp.right = rc.right;

		ClipCursor(&rcTemp);
		POINT pt = point;
		ClientToScreen(&pt);
		pt.y -= 20 ;
		CImageList::DragMove(pt);
		POINT pt1 = point;
		pt1.y -= 10 ;	
		hti = m_ctrlLinkManTree.HitTest(pt1,&flags);
		m_bMoveable = false ;
		DWORD dwDesID = 0 ;
		
	//	m_ctrlLinkManTree.Invalidate() ;
		m_ctrlLinkManTree.SelectDropTarget(hti);
		
		CImageList::DragShowNolock(TRUE);
		if( hti != NULL )
		{
			//CImageList::DragShowNolock(FALSE);
			if( m_htiOldDrop == NULL )
				m_htiOldDrop = m_ctrlLinkManTree.GetDropHilightItem();
			m_htiDrop = hti;
			dwDesID = m_ctrlLinkManTree.GetItemData(hti) ;
			desID = dwDesID;
			m_bMoveable = true ;
			if(m_nCtrlDest == CUR_CTRL_CLSTREE)
				m_bMoveable = true ;//g_pCardClassCenter->ClassMoveable(m_dwCurSelClsID,dwDesID) ;
			else
				m_bMoveable = (dwDesID == LINKMAN_SIM || dwDesID == m_currTreeItemData || dwDesID == LINKMAN_COUNT )?false:true ;
		}

		if(!m_bMoveable)
		{
			SetCursor( LoadCursor(NULL,IDC_NO)) ;
		}
		else
		{
			SetCursor( LoadCursor(NULL,IDC_ARROW)) ;
		}
		ClipCursor(NULL);
	}
	
	CBmpBkDialog::OnMouseMove(nFlags, point);
}

void CLinkManDialog::OnUpdateCard()
{
	
}

//释放鼠标
void CLinkManDialog::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	ReleaseCapture();
	bool bPost = false;
	if(m_bLDragging)
	{
		bPost = true;
		m_bLDragging = false;
		CImageList::DragLeave(this);
		CImageList::EndDrag();
		m_ctrlLinkManTree.Invalidate() ;
		m_ctrlLinkManTree.SelectDropTarget(NULL);
		
		if(m_pDragImage)
		{
			delete m_pDragImage;
			m_pDragImage=NULL;
		}
		
		CRect rc;
		m_ctrlLinkManTree.GetWindowRect(rc) ;
		ScreenToClient(rc) ;
		
		if(!rc.PtInRect(point) || m_htiDrop==NULL)
		{
			return ;
		}
		
		DWORD dwTarGetClsID = m_ctrlLinkManTree.GetItemData(m_htiDrop);
		
		if(!m_bMoveable)
			return ;
		
		if(dwTarGetClsID != LINKMAN_SIM || dwTarGetClsID != LINKMAN_COUNT || dwTarGetClsID != LINKMAN_PC )
		{
			if(m_currTreeItemData != LINKMAN_SIM)
				OnUpdateCard();
			else
			{
				//return;
			}
			
		}
		else
		{
			return;
		}
		
		//把选择的内容移动到树上
		
		if(m_nCtrlDest == CUR_CTRL_CLSTREE)
		{
			
		}
		else
		{	
			//取得被选中的项
			
			int count = m_ctrlLinkmanList.GetItemCount();
			CArray< DWORD, DWORD >itemData;
			CArray< int, int >item;
			for (int i = 0; i < count; i++)
			{
				if (m_ctrlLinkmanList.GetItemState(i, LVIS_SELECTED))
				{
					itemData.Add(m_ctrlLinkmanList.GetItemData_EX(i));
					item.Add(i);
					
				}
			} 
			int selectCount = itemData.GetSize();
			int nRepeat = REPEAT_OK;
			bool bIsMoveToSubCls = false;
				
			//进度显示
			CProgressWnd wndProgress(this, LoadStringEx(IDS_WAITFORMOVEPB));
			
			wndProgress.GoModal();
			wndProgress.SetRange(0,selectCount-1);
			wndProgress.SetText(LoadStringEx(IDS_WAITFORMOVEPB));
			CString strText;

			for (int j = selectCount - 1; j >= 0; j--)
			{
				strText.Format(TEXT("%s(%d)..."),LoadStringEx(IDS_WAITFORMOVEPB), j);
				wndProgress.StepIt();
				wndProgress.PeekAndPump();
				wndProgress.SetText(strText);

				if (wndProgress.Cancelled()) 
				{
					GMessageBox(LoadStringEx(IDS_MOVEPBCANCEL),LoadStringEx(IDS_PROMPT),MB_ICONEXCLAMATION);
					break;
				}

				//	CardInfo* pCard = CardInfo::getCardInfoById(itemData.GetAt(j));		//从数据库中获取选择移动结点的信息
				TPhoneBookItemEx items;
				if(m_currTreeItemData != LINKMAN_SIM)
				{
					int index = m_phBookPC.FindIndexByID(itemData.GetAt(j));
					items = m_phBookPC.GetAt(index);
					if (items.nParentID == m_currTreeItemData)
						bIsMoveToSubCls = true;	
					if (dwTarGetClsID == LINKMAN_SIM || dwTarGetClsID == LINKMAN_PC || dwTarGetClsID == LINKMAN_COUNT) // dwTarGetClsID == LINKMAN_SIM || 
						return;	
					else													//名片移动到其它类
					{
						CPhonebookPC phBookPC;
						items.nParentID = (WORD)dwTarGetClsID;
						items.strTypeName = m_PBClass->FindNameByID(dwTarGetClsID);							
						if(nRepeat != REPEAT_OK_ALL && nRepeat != REPEAT_NO_ALL)
						{
							
							if(phBookPC.IsExitRecord(items, false))
							{
								CRepeatDialog repeatDlg;
								repeatDlg.DoModal();
								nRepeat = repeatDlg.m_repeat;
							}
						}
						
						if(nRepeat == REPEAT_OK || nRepeat == REPEAT_OK_ALL)
						{
							phBookPC.Modify(items);
							m_ctrlLinkmanList.DeleteItem_EX(item.GetAt(j));
						}
						else if(nRepeat == REPEAT_NO_ALL || nRepeat == REPEAT_NO)
						{
							
						}
						
						//	m_phBookPC.Modify(items);
						
						
					}
				}
				
				if(m_currTreeItemData == LINKMAN_SIM)
				{
					//==================================================2007.6.18he
					/*****************************************************
					if(((CSunnyCatDlg *)AfxGetMainWnd())->m_isConnect)
					{					
						GMessageBox(LoadStringEx(IDS_PROMPT_ONLINE));
						return;
					}
					/*****************************************************/
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
					//===================================================
					bIsMoveToSubCls = true;	
					if (dwTarGetClsID == LINKMAN_SIM || dwTarGetClsID == LINKMAN_PC || dwTarGetClsID == LINKMAN_COUNT) // dwTarGetClsID == LINKMAN_SIM || 
						return;	
					int index = m_pbookEmbed.FindIndexByID(itemData.GetAt(j));
					m_pbookEmbed.GetAt(index);
					items.nParentID = (WORD)dwTarGetClsID;
					int nID = m_phBookPC.GetMaxID("Select max(PhoneID) from SC700_PhoneBook"); 
					if(nID<0)
						nID = 0;
					items.nID = nID+1;
					items.strName = m_pbookEmbed.GetAt(index).strName;
					items.strMobilePhone = m_pbookEmbed.GetAt(index).strMobilePhone;
					items.strHomePhone = "";
					items.strEmail = "";
					items.strFax = "";							
					items.strOfficePhone = "";
					items.strBusiness = "";
					items.strOffice = "";
					items.strCountry = "";
					items.strTypeName = m_PBClass->FindNameByID(dwTarGetClsID);	
					CPhonebookPC phBookPC;
					if(m_iRepeat != REPEAT_OK_ALL && m_iRepeat != REPEAT_NO_ALL)
					{
						
						if(phBookPC.IsExitRecord(items, false))
						{
							CRepeatDialog repeatDlg;
							repeatDlg.DoModal();
							m_iRepeat = repeatDlg.m_repeat;
						}
					}
					
					if(m_iRepeat == REPEAT_OK || m_iRepeat == REPEAT_OK_ALL)
					{
						phBookPC.Add(items);;
						m_ctrlLinkmanList.DeleteItem_EX(item.GetAt(j));
					}
					else if(m_iRepeat == REPEAT_NO_ALL || m_iRepeat == REPEAT_NO)
					{
						
					}
					//	m_phBookPC.Add(items);
					//	m_ctrlLinkmanList.DeleteItem_EX(item.GetAt(j));
				}
				
			}
			wndProgress.m_bCancelled = true;
			wndProgress.Hide();
			if(!bIsMoveToSubCls)
			{
				m_ctrlLinkmanList.DeleteItem_EX(item.GetAt(j));
			}				
			
				}									 
				
				
	}
	
	m_iRepeat = REPEAT_OK;
	
	int id = m_PBClass->FindIndexByID(desID);
	CString strText = "";
	strText = m_PBClass->GetAt(id).strClassName;
	CString strNum = "";
	m_phBookPC.ClearVector();
	m_phBookPC.GetRecordsByTypeName(desID);
	int countNum = m_phBookPC.Size();
	strNum.Format(TEXT("(%d)"), countNum);
	strText += strNum;
	m_ctrlLinkManTree.SetItemText(m_htiDrop, strText);
	if(bPost)
		PostMessage(WM_LINKMAN_NOTIFY,0,0);
	CBmpBkDialog::OnLButtonUp(nFlags, point);
}


void CLinkManDialog::OnClickTreeLinkman(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	m_ctrlLinkManTree.SelectItem(m_currTreeItem) ;
	m_ctrlLinkManTree.SelectSetFirstVisible(m_currTreeItem) ;
	*pResult = 0;
}
void CLinkManDialog::SetInputFlag(BOOL isInput)
{
	bInputOP = isInput;
}

void CLinkManDialog::OnClickTreeItem()
{
	//070625he 
	int iCount = m_ctrlLinkmanList.GetItemCount();	
	if(m_currTreeItemData != LINKMAN_SIM)
	{
		ShowPBForPC(m_currTreeItemData);
	}
	else
	{
		if(bInputOP)
		{
			return;		//导入操作不进行以下操作
		}
		if(!((CSunnyCatDlg *)AfxGetMainWnd())->m_MobileManager.isAbleCom)
		{
//			ShowMessageBox(IDS_NO_SC700_CARD);
			GMessageBox(LoadStringEx(IDS_NO_SC700_CARD));
			if(iCount > 0)
				m_ctrlLinkmanList.DeleteAllItems_EX();			//070625he
			return;
		}
		/***************************************************************
		if(((CSunnyCatDlg *)AfxGetMainWnd())->m_isConnect)
		{			
			GMessageBox(LoadStringEx(IDS_PROMPT_ONLINE));
			if(iCount > 0)
				m_ctrlLinkmanList.DeleteAllItems_EX();			//070625he
			return;
		}
		/***************************************************************/
		if(((CSunnyCatDlg *)AfxGetMainWnd())->m_bResetting)
		{
			GMessageBox(LoadStringEx(IDS_SEARCH_NET));
			if(iCount > 0)
				m_ctrlLinkmanList.DeleteAllItems_EX();			//070625he
			return;
		}
		if(((CSunnyCatDlg *)AfxGetMainWnd())->m_InitAT.IsInitRunning())
		{
			GMessageBox(LoadStringEx(IDS_INIT_RUNNING));
			if(iCount > 0)
				m_ctrlLinkmanList.DeleteAllItems_EX();			//070625he
			return;
		}
		
		if(m_currTreeItemData != LINKMAN_COUNT)
			ShowPBForSIM();
	}

}



/*	
	*/

void CLinkManDialog::OnBeginrdragListLinkman(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CLinkManDialog::OnBegindragListLinkman(NMHDR* pNMHDR, LRESULT* pResult) 
{
// TODO: Add your control notification handler code here
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	//	if(m_currTreeItemData == LINKMAN_SIM)
	//		return;
		if(m_currTreeItemData == LINKMAN_COUNT)
			return;
		
			*pResult = 0;
			CPoint pt;
			int count = m_ctrlLinkmanList.GetItemCount();
			CArray< DWORD, DWORD >itemData;
			CArray< int, int >item;
			for (int i = 0; i < count; i++)
			{
				if (m_ctrlLinkmanList.GetItemState(i, LVIS_SELECTED))
				{
					itemData.Add(m_ctrlLinkmanList.GetItemData_EX(i));
					item.Add(i);
					
				}
			} 
			int selectCount = itemData.GetSize();
			//CArray<int,int>arrIndex ;
			//GetListCtrlSelectItems(&m_ctrlLinkmanList,arrIndex) ;
			//int nItemCount = arrIndex.GetSize() ;
			if(selectCount <= 0)
				return ;
			m_ctrlLinkmanList.SetHotItem(-1) ;
			m_ctrlLinkManTree.SetFocus() ;
		#if 0
			m_pDragImage = m_ctrlLinkmanList.CreateDragImage( itemData[0], &pt );
			if( m_pDragImage == NULL )
				return;
			pt.x = 30;
			pt.y = -10;
		#else
			m_pDragImage = new CImageList();//
			if(m_DragBTM.GetSafeHandle())
				m_DragBTM.DeleteObject() ;
			if( m_pDragImage == NULL )
				return;
			
			CRect rc ;
			m_ctrlLinkmanList.GetWindowRect(&rc) ;
			ScreenToClient(rc) ;
			
			int nWidth  = 0 ;
			int nHeight = rc.Height()/14*selectCount ;
			nHeight = 32*selectCount ;
			nWidth   = rc.Width() ;
			CPaintDC dc(this);
			CDC *pDC = &dc;
			CDC dcMem;
			dcMem.CreateCompatibleDC(pDC) ;
			m_DragBTM.DeleteObject() ;
			m_DragBTM.CreateCompatibleBitmap(pDC,nWidth,nHeight);
			CBitmap *pOldBit = dcMem.SelectObject(&m_DragBTM) ;
			dcMem.SelectObject(GetFont()) ;
			//nHeight = 35;
			CRect bitRc(0,0,150,nHeight) ;
			TRACE(TEXT("height:%d\n"),nHeight);
			dcMem.FillSolidRect(&bitRc,RGB(255,255,255)) ;
			dcMem.SetTextColor(RGB(255, 0, 0)) ;
			dcMem.SetBkMode(TRANSPARENT) ;
			CRect txtRc(30, 5, 150, 20);
			//CardInfo *pTmp = NULL ;
			//m_phBookPC
			//m_pbookEmbed
			TPhoneBookItemEx pTmp;
			CString strInfo ;
			LVITEM LItem ;
			CImageList *pTmpList = m_ctrlLinkmanList.GetImageList(LVSIL_SMALL) ;
			CPoint IconPt(5, 0) ;
			static COLORREF arrTypeClr[] = {RGB(255, 0, 0), RGB(0, 0, 255), RGB(0, 255, 0)};
			for(i = 0; i < selectCount; i++)
			{
				txtRc.top = 5 + (32*i) ;
				txtRc.bottom = txtRc.top + 32 ;
				IconPt.y = 32*i ;
				if(m_currTreeItemData != LINKMAN_SIM)
				{
				int id = m_phBookPC.FindIndexByID(itemData.GetAt(i));
				pTmp = m_phBookPC.GetAt(id);
				if(_tcslen(pTmp.strOffice) > 0)
				{	
					strInfo += "(";
					strInfo += pTmp.strOffice;
					strInfo += ")";
				}
if(pTmp.strName.GetLength()>12)
						pTmp.strName = pTmp.strName.Left(12)+"...";
					strInfo.Format(TEXT("%02d.%s"), i + 1, pTmp.strName);
				}
				else
				{
					int id = m_pbookEmbed.FindIndexByID(itemData.GetAt(i));					
					strInfo.Format(TEXT("%02d.%s"), i + 1, m_pbookEmbed.GetAt(id).strName);
				}
				

				dcMem.SetTextColor(RGB(100,100,100));					
				memset(&LItem,0,sizeof(LVITEM)) ;
				LItem.iItem =itemData[i] ;
				LItem.mask  =LVIF_IMAGE  ; 
				m_ctrlLinkmanList.GetItem(&LItem);
			//	pTmpList->Draw(&dcMem,LItem.iImage,IconPt,ILD_TRANSPARENT);
				int iMag = 0;
				switch(m_currTreeItemData)
				{
					case 0:
						iMag = 5;
						break;
					case 2:
						iMag = 6;
						break;
					case 3:
						iMag = 7;
						break;
					case 5:
						iMag = 8;
						break;
					case 6:
						iMag = 9;
						break;
					default:
						iMag = 10;
						break;
				}

				pTmpList->Draw(&dcMem, iMag, IconPt, ILD_TRANSPARENT);
			//	dcMem.DrawText(strInfo,&txtRc,DT_LEFT|DT_VCENTER);

				txtRc.left = bitRc.left+40 ;				
				dcMem.DrawText(strInfo,&txtRc,DT_LEFT|DT_VCENTER);
				txtRc.left = 30 ;
			}
			dcMem.SelectObject(pOldBit);
			m_pDragImage->Create(150,nHeight,ILC_COLORDDB,1,1);
			m_pDragImage->Add(&m_DragBTM,&m_DragBTM);
			dcMem.DeleteDC()  ;
			ReleaseDC(&dcMem) ;
			pt.x = -1;
			pt.y = -20+nHeight/2;
		#endif

			m_nCtrlDest = CUR_CTRL_CARDLIST ;
			m_bLDragging = true;
			m_pDragImage->BeginDrag( 0, pt );
			pt = pNMListView->ptAction;
			ClientToScreen( &pt );
			m_pDragImage->DragEnter(NULL,pt);

			SetCapture();
				
	*pResult = 0;
}


void CLinkManDialog::DoResize1(int delta)
{
	CSplitterControl::ChangeWidth(&m_ctrlLinkManTree, delta);
	CSplitterControl::ChangeWidth(&m_ctrlLinkmanList, -delta, CW_RIGHTALIGN);
	//CSplitterControl::ChangeWidth(&m_ctrlNewCard, -delta, CW_RIGHTALIGN);
	//CSplitterControl::ChangeWidth(&m_ctrlDelCard, -delta, CW_RIGHTALIGN);
	//CSplitterControl::ChangeWidth(&m_ctrlSendCard, -delta, CW_RIGHTALIGN);
	//CSplitterControl::ChangeWidth(&m_ctrlEdit, -delta, CW_RIGHTALIGN);
	//CSplitterControl::ChangeWidth(&m_ctrlSearchCard, -delta, CW_RIGHTALIGN);
	//CSplitterControl::ChangeWidth(&m_ctrlQuery, -delta, CW_RIGHTALIGN);

	CRect rc;
	if(m_ctrlQuery.GetSafeHwnd() != NULL)
	{		
		m_ctrlQuery.GetWindowRect(&rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
		int nWidth = rc.Width();
		rc.left = rc.left+delta;
		rc.right = rc.left+nWidth;
		m_ctrlQuery.MoveWindow(&rc);
	}
	if(m_ctrlNewCard.GetSafeHwnd() != NULL)
	{		
		m_ctrlNewCard.GetWindowRect(&rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
		int nWidth = rc.Width();
		rc.left = rc.left+delta;
		rc.right = rc.left+nWidth;
		m_ctrlNewCard.MoveWindow(&rc);
	}
	if(m_ctrlDelCard.GetSafeHwnd() != NULL)
	{		
		m_ctrlDelCard.GetWindowRect(&rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
		int nWidth = rc.Width();
		rc.left = rc.left+delta;
		rc.right = rc.left+nWidth;
		m_ctrlDelCard.MoveWindow(&rc);
	}
	if(m_ctrlSendCard.GetSafeHwnd() != NULL)
	{		
		m_ctrlSendCard.GetWindowRect(&rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
		int nWidth = rc.Width();
		rc.left = rc.left+delta;
		rc.right = rc.left+nWidth;
		m_ctrlSendCard.MoveWindow(&rc);
	}
	if(m_ctrlSearchCard.GetSafeHwnd() != NULL)
	{		
		m_ctrlSearchCard.GetWindowRect(&rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
		int nWidth = rc.Width();
		rc.left = rc.left+delta;
		rc.right = rc.left+nWidth;
		m_ctrlSearchCard.MoveWindow(&rc);
	}
	if(m_ctrlEdit.GetSafeHwnd() != NULL)
	{		
		m_ctrlEdit.GetWindowRect(&rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
		int nWidth = rc.Width();
		rc.left = rc.left+delta;
		rc.right = rc.left+nWidth;
		m_ctrlEdit.MoveWindow(&rc);
	}
//	CSplitterControl::ChangeWidth(&m_ctrlLinkManShow, -delta, CW_RIGHTALIGN);
	Invalidate();
	UpdateWindow();
}

LRESULT CLinkManDialog::OnMyMove(WPARAM wParam,LPARAM lParam)
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
			rcTab.bottom += 6;
			m_bFirst = FALSE;
		}
		else
			rcTab.bottom = rc.bottom-rc.top-170;

	
		m_wndSplitter1.MoveWindow(&rcTab,TRUE);
	}
	
	return TRUE;
}

LRESULT CLinkManDialog::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
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

void CLinkManDialog::OnSize(UINT nType, int cx, int cy) 
{
	CBmpBkDialog::OnSize(nType, cx, cy);
	CRect rcTree;
	CRect rcList;
	CRect rcEdit;
	if(m_ctrlLinkManTree.GetSafeHwnd() != NULL)
	{
		m_ctrlLinkManTree.GetWindowRect(&rcTree);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcTree, 2 );

		rcTree.bottom = cy;
		m_ctrlLinkManTree.MoveWindow(&rcTree);
	}

	if(m_ctrlLinkmanList.GetSafeHwnd() != NULL)
	{
		m_ctrlLinkmanList.GetWindowRect(&rcList);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rcList, 2 );

		rcList.right = cx;
		rcList.bottom = cy;//rcEdit.top-5;
		m_ctrlLinkmanList.MoveWindow(&rcList);
	}
	if(m_wndSplitter1.GetSafeHwnd() != NULL)
	{	
		CRect rc;
		m_wndSplitter1.GetWindowRect(&rc);
		::MapWindowPoints( HWND_DESKTOP, GetSafeHwnd(), (LPPOINT)(RECT*)&rc, 2 );
		rc.bottom = cy;
		rc.right = rc.left+1;
		m_wndSplitter1.MoveWindow(&rc);	
	}
	
}

void   CLinkManDialog::OnDialNum(UINT   nID)   
{   
	CString strPhone;
	CMenu * pMenu = NULL;
	CallStatus m_status;
	m_status = ((CSunnyCatDlg*)AfxGetMainWnd())->m_MobileManager.m_pCallMgr->GetCallStatus();
	if(m_status != CALLSTATE_IDLE)
		return;

	((CSunnyCatDlg *)AfxGetMainWnd())->ShowPage(2);

	switch(nID)   
	{   
	case   IDM_START:   
		((CSunnyCatDlg*)AfxGetMainWnd())->m_callDlg.Call(m_item.strMobilePhone );  
		break;
	case   IDM_START+1:   
		((CSunnyCatDlg*)AfxGetMainWnd())->m_callDlg.Call(m_item.strOfficePhone);
		break;
	case IDM_START+2:
		((CSunnyCatDlg*)AfxGetMainWnd())->m_callDlg.Call(m_item.strHomePhone);
		break;
	case IDM_START+3:
		((CSunnyCatDlg*)AfxGetMainWnd())->m_callDlg.Call(m_item.strFax);
		break;
	}   
}   

HBRUSH CLinkManDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CBmpBkDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
	if(nCtlColor == CTLCOLOR_DLG)
	{
		static HBRUSH brush=CreateSolidBrush(RGB(221,241,251));
		return brush;
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

void CLinkManDialog::OnButtonSend() 
{
	// TODO: Add your control notification handler code here
	OnLinkmanSms();
}

void CLinkManDialog::OnButtonEdit() 
{
	// TODO: Add your control notification handler code here
	OnLinkmanEditcard();
}


void CLinkManDialog::SetEditBtnState(bool bGray)
{
	CString strAppPath = GetAppPath()+"\\skin\\phonebook\\";
	int itemCount = m_ctrlLinkmanList.GetItemCount();
	
	if(bGray)
	{
		m_ctrlEdit.EnableWindow(FALSE);
		m_ctrlEdit.SetBitmapPath(strAppPath+"edit_phonebook_gray.bmp");
		m_ctrlEdit.Draw();
	}
	else
	{
		if(itemCount>0)
		{
			m_ctrlEdit.EnableWindow(TRUE);
			m_ctrlEdit.SetBitmapPath(strAppPath+"edit_phonebook.bmp",strAppPath+"edit_phonebook_down.bmp",strAppPath+"edit_phonebook_over.bmp");
			m_ctrlEdit.Draw();
		}
		else
		{
		m_ctrlEdit.EnableWindow(FALSE);
		m_ctrlEdit.SetBitmapPath(strAppPath+"edit_phonebook_gray.bmp");
		m_ctrlEdit.Draw();

		}
	}

}

void CLinkManDialog::SetSendBtnState(bool bGray)
{
	CString strAppPath = GetAppPath()+"\\skin\\phonebook\\";
	int itemCount = m_ctrlLinkmanList.GetItemCount();
	if(bGray)
	{
		m_ctrlSendCard.EnableWindow(FALSE);
		m_ctrlSendCard.SetBitmapPath(strAppPath+"sms_send_gray.bmp");
		m_ctrlSendCard.Draw();
	}
	else
	{
		if(itemCount>0)
		{
			m_ctrlSendCard.EnableWindow(TRUE);
			m_ctrlSendCard.SetBitmapPath(strAppPath+"sms_send.bmp",strAppPath+"sms_send_down.bmp",strAppPath+"sms_send_over.bmp");
			m_ctrlSendCard.Draw();
		}
		else
		{

		m_ctrlSendCard.EnableWindow(FALSE);
		m_ctrlSendCard.SetBitmapPath(strAppPath+"sms_send_gray.bmp");
		m_ctrlSendCard.Draw();
		}
	}

}



void CLinkManDialog::SetDelBtnState(bool bGray)
{
	CString strAppPath = GetAppPath()+"\\skin\\phonebook\\";
	int itemCount = m_ctrlLinkmanList.GetItemCount();
	if(bGray)
	{
		m_ctrlDelCard.EnableWindow(FALSE);
		m_ctrlDelCard.SetBitmapPath(strAppPath+"delete_gray.bmp");
		m_ctrlDelCard.Draw();
	}
	else
	{
		if(itemCount>0)
		{
			m_ctrlDelCard.EnableWindow(TRUE);
			m_ctrlDelCard.SetBitmapPath(strAppPath+"delete.bmp",strAppPath+"delete_down.bmp",strAppPath+"delete_over.bmp");
			m_ctrlDelCard.Draw();
		}
		else
		{
			m_ctrlDelCard.EnableWindow(FALSE);
			m_ctrlDelCard.SetBitmapPath(strAppPath+"delete_gray.bmp");
			m_ctrlDelCard.Draw();

		}
	}
}

void CLinkManDialog::SetNewBtnState(bool bGray,bool bSim)
{
	CString strAppPath = GetAppPath()+"\\skin\\phonebook\\";
	if(!bSim)
	{
		if(bGray)
		{
			m_ctrlNewCard.EnableWindow(FALSE);
			m_ctrlNewCard.SetBitmapPath(strAppPath+"new_phonebook_gray.bmp");
			m_ctrlNewCard.Draw();
		}
		else
		{
			m_ctrlNewCard.EnableWindow(TRUE);
			m_ctrlNewCard.SetBitmapPath(strAppPath+"new_phonebook.bmp",strAppPath+"new_phonebook_down.bmp",strAppPath+"new_phonebook_over.bmp");
			m_ctrlNewCard.Draw();
		}
	}
	else
	{
		if(bGray || !m_pbookEmbed.IsLoadSim())
		{
			m_ctrlNewCard.EnableWindow(FALSE);
			m_ctrlNewCard.SetBitmapPath(strAppPath+"new_phonebook_gray.bmp");
			m_ctrlNewCard.Draw();
		}
		else
		{
			m_ctrlNewCard.EnableWindow(TRUE);
			m_ctrlNewCard.SetBitmapPath(strAppPath+"new_phonebook.bmp",strAppPath+"new_phonebook_down.bmp",strAppPath+"new_phonebook_over.bmp");
			m_ctrlNewCard.Draw();
		}
	}
}

void CLinkManDialog::SetQueryBtnState(bool bGray)
{
	CString strAppPath = GetAppPath()+"\\skin\\phonebook\\";
	if(bGray)
	{
		m_ctrlSearchCard.EnableWindow(FALSE);
		m_ctrlSearchCard.SetBitmapPath(strAppPath+"search_gray.bmp");
		m_ctrlSearchCard.Draw();
	}
	else
	{
		m_ctrlSearchCard.EnableWindow(TRUE);
		m_ctrlSearchCard.SetBitmapPath(strAppPath+"search.bmp",strAppPath+"search_down.bmp",strAppPath+"search_over.bmp");
		m_ctrlSearchCard.Draw();
	}
}

void CLinkManDialog::OnKeydownListLinkman(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	if(pLVKeyDow->wVKey == VK_DELETE)
	{
		OnButtonDelLinkman();	
	}
	*pResult = 0;
}

void CLinkManDialog::OnChangeEditQueryLinkman() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CBmpBkDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	UpdateData();
	if(m_strQuery.GetLength()>0)
	{
		SetQueryBtnState(false);
	}
	else
	{
		SetQueryBtnState(true);
	}
	// TODO: Add your control notification handler code here
	
}
