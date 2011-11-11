// SelectClassDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SunnyCat.h"
#include "SelectClassDlg.h"
#include "GlobalDataCenter.h"
#include "NewCardDlg.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectClassDlg dialog


CSelectClassDlg::CSelectClassDlg(CWnd* pParent /*=NULL*/)
	: CBmpBkDialog(CSelectClassDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectClassDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	g_pTreeImage = new CImageList();
	m_PBClass = new CPhonebookPCClass();
}


void CSelectClassDlg::DoDataExchange(CDataExchange* pDX)
{
	CBmpBkDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectClassDlg)
	DDX_Control(pDX, IDC_TREE_SEL, m_ctrlListSel);
	//}}AFX_DATA_MAP
}

CSelectClassDlg::~CSelectClassDlg()
{
	delete g_pTreeImage;
	g_pTreeImage = NULL;
	delete m_PBClass;
	m_PBClass = NULL;
}

BEGIN_MESSAGE_MAP(CSelectClassDlg, CBmpBkDialog)
	//{{AFX_MSG_MAP(CSelectClassDlg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_SEL, OnSelchangedTreeSel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectClassDlg message handlers

BOOL CSelectClassDlg::OnInitDialog() 
{
	CBmpBkDialog::OnInitDialog();
	SetWindowText(LoadStringEx(IDS_SEL_CLASS));
	GetDlgItem(IDOK)->SetWindowText(LoadStringEx(IDS_OK));
	GetDlgItem(IDCANCEL)->SetWindowText(LoadStringEx(IDS_CANCEL));

	InitTree();
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectClassDlg::InitTree()
{
	//初使化历史记录树	
	m_ctrlListSel.DeleteAllItems();
	LoadImgList(g_pTreeImage, IDB_LINKMANTREE);
	m_ctrlListSel.SetImageList(g_pTreeImage, LVSIL_NORMAL);
	m_PBClass->GetAllRecords();

	int m_pbClassCount = m_PBClass->Size();
//	HTREEITEM m_linkManItem = m_ctrlListSel.InsertItem(m_PBClass->GetAt(0).strClassName,LINKMAN_TREE_PC,LINKMAN_TREE_PC);
//	m_ctrlListSel.SetItemData(m_linkManItem, 1);
	
	
	HTREEITEM m_item;
	for(int i = 1; i < m_pbClassCount; i++)
	{
		if(i < 5)
		m_item = m_ctrlListSel.InsertItem(m_PBClass->GetAt(i).strClassName, 5+i,5+i);//,m_linkManItem);
		else
		m_item = m_ctrlListSel.InsertItem(m_PBClass->GetAt(i).strClassName, 10,10);//,m_linkManItem);
		m_ctrlListSel.SetItemData(m_item, m_PBClass->GetAt(i).nID);
	}
//	m_ctrlListSel.Expand(m_linkManItem, TVE_EXPAND);
/*	m_count = m_ctrlListSel.InsertItem("查找结果",LINKMAN_TREE_COUNT,LINKMAN_TREE_COUNT);
	m_ctrlListSel.SetItemData(m_count, LINKMAN_COUNT);
	HTREEITEM m_linkmanSmItem = m_ctrlListSel.InsertItem("SIM卡",LINKMAN_TREE_SIM,LINKMAN_TREE_SIM);
	m_ctrlListSel.SetItemData(m_linkmanSmItem, 0);
*/

}

void CSelectClassDlg::OnOK() 
{
	// TODO: Add extra validation here
	((CNewCardDlg *)GetParent())->m_classID = m_currTreeItemData;
	CString className = m_ctrlListSel.GetItemText(m_currTreeItem);
	if(className != "")
	((CNewCardDlg *)GetParent())->m_className = m_ctrlListSel.GetItemText(m_currTreeItem);
	else
	{
	((CNewCardDlg *)GetParent())->m_className = LoadStringEx(IDS_LINKMAN_NOCLASS);
	((CNewCardDlg *)GetParent())->m_classID = 6;
	}

	CBmpBkDialog::OnOK();
}

void CSelectClassDlg::OnSelchangedTreeSel(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_currTreeItem = pNMTreeView->itemNew.hItem;

	m_currTreeItemData = m_ctrlListSel.GetItemData(pNMTreeView->itemNew.hItem);
	*pResult = 0;
}
