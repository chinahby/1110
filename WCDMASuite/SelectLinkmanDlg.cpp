// SelectLinkmanDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SunnyCat.h"
#include "SelectLinkmanDlg.h"
#include "SelectClassDlg.h"
#include "GlobalDataCenter.h"
#include "NewSmsDialog.h"
#include "Util.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectLinkmanDlg dialog


CSelectLinkmanDlg::CSelectLinkmanDlg(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CSelectLinkmanDlg::IDD, pParent),m_pbookEmbed("SM")
{
	//{{AFX_DATA_INIT(CSelectLinkmanDlg)
	m_strQuery = _T("");
	//}}AFX_DATA_INIT
	g_pSysImageList = new CImageList();

	m_PBClass = new CPhonebookPCClass();
}

CSelectLinkmanDlg::~CSelectLinkmanDlg()
{
	delete g_pSysImageList;
	g_pSysImageList = NULL;
	delete m_PBClass;
	m_PBClass = NULL;

}

void CSelectLinkmanDlg::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectLinkmanDlg)
	DDX_Control(pDX, IDC_STATIC_SEL, m_ctrlSel);
	DDX_Control(pDX, IDC_LIST_SEL, m_ctrlListSel);
	DDX_Control(pDX, IDC_TREE_SEL_LINKMAN, m_ctrlLinkManTree);
	DDX_Control(pDX, IDC_LIST_FORSEL, m_ctrlLinkmanList);
	DDX_Text(pDX, IDC_EDIT_SMSSEARCHTXT, m_strQuery);
	DDV_MaxChars(pDX, m_strQuery, 32);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectLinkmanDlg, CBmpBkDialog)
	//{{AFX_MSG_MAP(CSelectLinkmanDlg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SEL_LINKMAN, OnSelchangedTreeSelLinkman)
	ON_BN_CLICKED(IDC_BUTTON_ADDTOSEL, OnButtonAddtosel)
	ON_BN_CLICKED(IDC_BUTTON_ADDALLTOSEL, OnButtonAddalltosel)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_REMOVEALL, OnButtonRemoveall)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_FORSEL, OnRclickListForsel)
	ON_COMMAND(IDR_SEL_ALLCARD, OnSelAllcard)
	ON_COMMAND(IDR_RETURN_CARD, OnReturnCard)
	ON_BN_CLICKED(IDC_BUTTON_SMSSEARCH, OnButtonSmssearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectLinkmanDlg message handlers

void CSelectLinkmanDlg::InitUI()
{
	
	LoadImgList(g_pSysImageList, IDB_LINKMANTREE);
	InitTree();
	//初使化LinkMan记录列表

	m_ctrlLinkmanList.SetImageList(g_pSysImageList,LVSIL_SMALL);
	(void)m_ctrlLinkmanList.SetExtendedStyle(LVS_EX_FULLROWSELECT| LVS_EX_INFOTIP);			//隐藏列表线 
	m_ctrlLinkmanList.SetHeadings(LoadStringEx(IDS_SEL_LINKMAN_LISTHEADA));
	m_ctrlLinkmanList.LoadColumnInfo();	
	m_ctrlLinkmanList.SetTextBkColor(RGB(247,247,255));
	m_ctrlLinkmanList.SetTextColor(RGB(100,100,100));

	m_ctrlListSel.SetImageList(g_pSysImageList,LVSIL_SMALL);
	(void)m_ctrlListSel.SetExtendedStyle( LVS_EX_FULLROWSELECT| LVS_EX_INFOTIP);			//隐藏列表线 
	m_ctrlListSel.SetHeadings(LoadStringEx(IDS_SEL_LINKMAN_LISTHEADB));
	m_ctrlListSel.LoadColumnInfo();	
	m_ctrlListSel.SetTextBkColor(RGB(247,247,255));
	m_ctrlListSel.SetTextColor(RGB(100,100,100));
	
}

BOOL CSelectLinkmanDlg::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	
	InitUI();
	SetWindowText(LoadStringEx(IDS_SELECT_CARD));
	GetDlgItem(IDC_SEARCH_CONTENT)->SetWindowText(LoadStringEx(IDS_SEARCH_CONTENT));
	GetDlgItem(IDC_BUTTON_SMSSEARCH)->SetWindowText(LoadStringEx(IDS_BUTTON_SEARCH));
	GetDlgItem(IDC_RECEIVER_LIST)->SetWindowText(LoadStringEx(IDS_RECEIVER_LIST));
	GetDlgItem(IDC_BUTTON_ADDTOSEL)->SetWindowText(LoadStringEx(IDS_ADD));
	GetDlgItem(IDC_BUTTON_ADDALLTOSEL)->SetWindowText(LoadStringEx(IDS_ADD_ALL));
	GetDlgItem(IDC_BUTTON_REMOVE)->SetWindowText(LoadStringEx(IDS_REMOVE));
	GetDlgItem(IDC_BUTTON_REMOVEALL)->SetWindowText(LoadStringEx(IDS_REMOVE_ALL));
	GetDlgItem(IDC_SELECT_COUNT)->SetWindowText(LoadStringEx(IDS_SELECT_COUNT));
	GetDlgItem(IDOK)->SetWindowText(LoadStringEx(IDS_OK));
	GetDlgItem(IDCANCEL)->SetWindowText(LoadStringEx(IDS_CANCEL));


	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectLinkmanDlg::InitTree()
{
	//初使化历史记录树	
	m_ctrlLinkManTree.SetImageList(g_pSysImageList, LVSIL_NORMAL);
	m_PBClass->GetAllRecords();
	int m_pbClassCount = m_PBClass->Size();
	HTREEITEM m_linkManItem = m_ctrlLinkManTree.InsertItem(m_PBClass->GetAt(0).strClassName, LINKMAN_TREE_PC,LINKMAN_TREE_PC);
	m_ctrlLinkManTree.SetItemData(m_linkManItem, 1);
	HTREEITEM m_item;
	for(int i = 1; i < m_pbClassCount; i++)
	{		
			if(i < 5)
		m_item = m_ctrlLinkManTree.InsertItem(m_PBClass->GetAt(i).strClassName, 5+i,5+i,m_linkManItem);
		else
		m_item = m_ctrlLinkManTree.InsertItem(m_PBClass->GetAt(i).strClassName, 10,10,m_linkManItem);
		m_ctrlLinkManTree.SetItemData(m_item, m_PBClass->GetAt(i).nID);
	}
	m_ctrlLinkManTree.Expand(m_linkManItem, TVE_EXPAND);
	HTREEITEM m_linkmanSmItem = m_ctrlLinkManTree.InsertItem(LoadStringEx(IDS_LINKMAN_SIM), LINKMAN_TREE_SIM, LINKMAN_TREE_SIM);
	m_ctrlLinkManTree.SetItemData(m_linkmanSmItem, 0);
	m_count = m_ctrlLinkManTree.InsertItem(LoadStringEx(IDS_LINKMAN_QUERY),LINKMAN_TREE_COUNT,LINKMAN_TREE_COUNT);
	m_ctrlLinkManTree.SetItemData(m_count, LINKMAN_COUNT);
	m_ctrlLinkManTree.SetBkColor(RGB(255,255,255));
}

void CSelectLinkmanDlg::OnSelchangedTreeSelLinkman(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_currTreeItem = pNMTreeView->itemNew.hItem;
	m_currTreeItemData = m_ctrlLinkManTree.GetItemData(pNMTreeView->itemNew.hItem);
	if(m_currTreeItemData != LINKMAN_SIM)
	{
		ShowPBForPC(m_currTreeItemData);
	}
	else
	{
		ShowPBForSIM();
	}
	
	*pResult = 0;
}

void CSelectLinkmanDlg::ShowPBForPC(DWORD currItemData)
{
	m_phBookPC.ClearVector();
	m_phBookPC.GetRecordsByTypeName(currItemData);
	
	int size = m_phBookPC.Size();
	m_ctrlLinkmanList.SetRedraw(false);
	m_ctrlLinkmanList.DeleteAllItems_EX();
	TPhoneBookItemEx phoneItem;
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
	for(int i = 0; i < size; i++)
	{
		phoneItem = m_phBookPC.GetAt(i);
		m_ctrlLinkmanList.AddItem(iMag,TEXT(""), phoneItem.strName, phoneItem.strMobilePhone);
		m_ctrlLinkmanList.SetItemData_EX(i, phoneItem.nID);
	}
	m_ctrlLinkmanList.SetRedraw(true);
}

void CSelectLinkmanDlg::ShowPBForSIM()
{
	int iSize;
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

void CSelectLinkmanDlg::OnButtonAddtosel() 
{
	// TODO: Add your control notification handler code here
	int nCount = m_ctrlLinkmanList.GetItemCount();

//	m_ctrlListSel.DeleteAllItems_EX();
	for(int i = 0; i < nCount; i++)
	{
	//	if(m_ctrlLinkmanList.GetCheck(i))
	//		m_ctrlLinkmanList.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	//	if (m_ctrlLinkmanList.GetItemState(i, LVIS_SELECTED))
		if(m_ctrlLinkmanList.GetItemState(i, LVIS_SELECTED))
		{
			CString name = m_ctrlLinkmanList.GetItemText(i, 1);
			CString phone = m_ctrlLinkmanList.GetItemText(i, 2);
			m_ctrlListSel.AddItem(21,TEXT(""), name, phone);
			m_ctrlListSel.SetItemData_EX(i, i);
		}
		
	}
	CString selInfo;
	selInfo.Format(TEXT("  %d"), m_ctrlListSel.GetItemCount());
	selInfo += LoadStringEx(IDS_SEL_LINKMAN_SUM);
	m_ctrlSel.SetWindowText(selInfo);
	
}

void CSelectLinkmanDlg::OnButtonAddalltosel() 
{
	// TODO: Add your control notification handler code here
	OnSelAllcard();
	OnButtonAddtosel();
}

void CSelectLinkmanDlg::OnButtonRemove() 
{
	// TODO: Add your control notification handler code here

	int nCount = m_ctrlListSel.GetItemCount();	
	for(int i = nCount; i >= 0 ; i--)
	{	
		if (m_ctrlListSel.GetItemState(i, LVIS_SELECTED))
		{
			m_ctrlListSel.DeleteItem_EX(i);			
		}
	}
		
	CString selInfo;
	selInfo.Format(TEXT("  %d"), m_ctrlListSel.GetItemCount());
	selInfo += LoadStringEx(IDS_SEL_LINKMAN_SUM);
	m_ctrlSel.SetWindowText(selInfo);
}

void CSelectLinkmanDlg::OnButtonRemoveall() 
{
	// TODO: Add your control notification handler code here
	m_ctrlListSel.DeleteAllItems_EX();
	CString selInfo;
	selInfo.Format(TEXT("  %d"), m_ctrlListSel.GetItemCount());
	selInfo += LoadStringEx(IDS_SEL_LINKMAN_SUM);
	m_ctrlSel.SetWindowText(selInfo);
}

//全选
void CSelectLinkmanDlg::OnButtonSelall(CSortListCtrl m_listExp) 
{
	// TODO: Add your control notification handler code here

}
//反选
void CSelectLinkmanDlg::OnButtonSelrev(CSortListCtrl m_listExp) 
{

}

void CSelectLinkmanDlg::OnRclickListForsel(NMHDR* pNMHDR, LRESULT* pResult) 
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
		CardMenu.LoadMenu(IDR_MENU_LINKMAN);
		pPopMenu = CardMenu.GetSubMenu(2) ;
		
		if(pPopMenu != NULL)
		{	
			pPopMenu->ModifyMenu(IDR_SEL_ALLCARD, MF_BYCOMMAND|MF_STRING, IDR_SEL_ALLCARD , LoadStringEx(IDS_SMS_ALL));
			pPopMenu->ModifyMenu(IDR_RETURN_CARD, MF_BYCOMMAND|MF_STRING, IDR_RETURN_CARD , LoadStringEx(IDS_SMS_OTHER));
		
			pPopMenu->TrackPopupMenu(TPM_LEFTBUTTON,pt.x,pt.y,this) ;
		}
}

void CSelectLinkmanDlg::OnSelAllcard() 
{
	for(int i=0; i<m_ctrlLinkmanList.GetItemCount(); i++)
	{
		m_ctrlLinkmanList.SetItemState(i,LVIS_SELECTED,LVIS_SELECTED) ;
		m_ctrlLinkmanList.SetCheck(i) ;
	}
}

void CSelectLinkmanDlg::OnReturnCard() 
{
	
/*	for(int i=0; i<m_ctrlLinkmanList.GetItemCount(); i++)
	{
		m_ctrlLinkmanList.SetCheck(i,!m_ctrlLinkmanList.GetCheck(i)) ;
		m_ctrlLinkmanList.SetItemState(i,m_ctrlLinkmanList.GetCheck(i)?LVIS_SELECTED:0,LVIS_SELECTED) ;
	}
	*/
	static DWORD dwStaFlag = LVIS_SELECTED|LVIS_FOCUSED|LVIS_ACTIVATING;
	int nCount = m_ctrlLinkmanList.GetItemCount();
	for(int i = 0; i < nCount; i++)
	{
		if (m_ctrlLinkmanList.GetItemState(i, LVIS_SELECTED))
		{
			m_ctrlLinkmanList.SetItemState(i, 0, LVIS_SELECTED);
		}
		else
		m_ctrlLinkmanList.SetItemState(i,dwStaFlag,dwStaFlag);
	}
}

void CSelectLinkmanDlg::OnOK() 
{
	// TODO: Add extra validation here
	int count = m_ctrlListSel.GetItemCount();
	CString numbers;
	for(int i = 0; i < count; i++)
	{	
		m_numbers.push_back(m_ctrlListSel.GetItemText(i, 2));		
		numbers += m_numbers[i];
		numbers += ";";
	}
	((CNewSmsDialog *)GetParent())->UpdateInfo(numbers);
	
	CBmpBkDialog::OnOK();
}

void CSelectLinkmanDlg::OnButtonSmssearch() 
{
	// TODO: Add your control notification handler code here
		// TODO: Add your control notification handler code here
	UpdateData(true);
	m_ctrlLinkManTree.SelectItem(m_count) ;
	m_ctrlLinkManTree.SelectSetFirstVisible(m_count) ;
	m_ctrlLinkmanList.SetRedraw(false);
	m_phBookPC.ClearVector();
	m_ctrlLinkmanList.DeleteAllItems_EX();	
	Search(m_strQuery);
	SearchInPBSIM(m_pbookEmbed);
	m_ctrlLinkmanList.SetRedraw(true);
}


void CSelectLinkmanDlg::Search(CString queryStr)
{
  	int i=0;
	int size = 0;	
	queryStr.TrimLeft();
	queryStr.TrimRight();
	TPhoneBookItemEx pbItem;
	if(queryStr == "")
	{
		//AfxMessageBox("请输入查找内容！");
		GMessageBox(LoadStringEx(IDS_QUERYCONDITON_NOTEMPTY),LoadStringEx(IDS_PROMPT),MB_ICONINFORMATION) ;
		return;
	}
	if(m_phBookPC.GetRecordsByNumber(queryStr))
	{
		size = m_phBookPC.Size();
		for(i=0; i<size; i++)
		{
			pbItem = m_phBookPC.GetAt(i);
			//int nCount = m_ctrlLinkmanList.GetItemCount();
			int nCount = m_ctrlLinkmanList.AddItem(21,TEXT("PC"),pbItem.strName, pbItem.strMobilePhone, pbItem.strBusiness,pbItem.strOffice);
			
			m_ctrlLinkmanList.SetItemData_EX(nCount, pbItem.nID);
		}
		
	}

	return;  
}

void CSelectLinkmanDlg::SearchInPBSIM(CPhonebookEmbed& pb)
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